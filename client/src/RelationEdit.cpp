/******************************************************************************\
*  client/src/RelationEdit.cpp                                                 *
*  Copyright (C) 2008 John Eric Martin <john.eric.martin@gmail.com>            *
*                                                                              *
*  This program is free software; you can redistribute it and/or modify        *
*  it under the terms of the GNU General Public License version 2 as           *
*  published by the Free Software Foundation.                                  *
*                                                                              *
*  This program is distributed in the hope that it will be useful,             *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
*  GNU General Public License for more details.                                *
*                                                                              *
*  You should have received a copy of the GNU General Public License           *
*  along with this program; if not, write to the                               *
*  Free Software Foundation, Inc.,                                             *
*  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                   *
\******************************************************************************/

#include "RelationEdit.h"
#include "Settings.h"
#include "ajax.h"

#include <QtGui/QMessageBox>

RelationEdit::RelationEdit(QWidget *parent) : QWidget(parent), mID(-1)
{
	ui.setupUi(this);

	darkenWidget(ui.nameLabel);
	darkenWidget(ui.japaneseNameLabel);
	darkenWidget(ui.englishNameLabel);

	setWindowModality(Qt::ApplicationModal);

	connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(submitRelation()));

	ajax::getSingletonPtr()->subscribe(this);
};

void RelationEdit::darkenWidget(QWidget *widget)
{
	QPalette dark_palette = palette();
	dark_palette.setBrush(QPalette::Window,
		dark_palette.brush(QPalette::Base));

	qobject_cast<QLabel*>(widget)->setMargin(5);
	widget->setAutoFillBackground(true);
	widget->setPalette(dark_palette);
};

void RelationEdit::submitRelation()
{
	setEnabled(false);

	QVariantList columns;
	columns << QString("name_en") << QString("name_ja");

	QVariantMap where;
	where["type"] = "equals";
	where["column"] = "id";
	where["value"] = QVariant(mID);

	QVariantMap row;
	row["name_ja"] = ui.japaneseNameEdit->text();
	row["name_en"] = ui.englishNameEdit->text();

	QVariantMap action;

	if(mID < 0)
		action["action"] = QString("insert");
	else
		action["action"] = QString("update");

	action["table"] = mTable;
	action["columns"] = columns;
	action["user_data"] = QString("relation_updated");
	action["rows"] = QVariantList() << row;

	if(mID > 0)
		action["where"] = QVariantList() << where;

	ajax::getSingletonPtr()->request(settings->url(), action);
};

void RelationEdit::showEdit(const QString& windowTitle, const QString& table, int id)
{
	mID = id;
	mTable = table;

	ui.japaneseNameEdit->clear();
	ui.englishNameEdit->clear();

	setWindowTitle(windowTitle);
	setEnabled(true);
	show();

	if(mID < 0)
		return;

	setEnabled(false);

	QVariantList columns;
	columns << QString("id") << QString("name_en") << QString("name_ja");

	QVariantMap where;
	where["type"] = "equals";
	where["column"] = "id";
	where["value"] = QVariant(mID);

	QVariantMap action;
	action["action"] = QString("select");
	action["table"] = mTable;
	action["columns"] = columns;
	action["user_data"] = QString("%1_entry").arg(mTable);
	action["where"] = QVariantList() << where;

	ajax::getSingletonPtr()->request(settings->url(), action);
};

void RelationEdit::ajaxResponse(const QVariant& resp)
{
	QVariantMap result = resp.toMap();

	if(result.value("user_data").toString() == "relation_updated")
	{
		setWindowModality(Qt::NonModal);
		emit relationsUpdated();

		close();
	}
	else if(result.value("user_data").toString() == QString("%1_entry").arg(mTable))
	{
		QVariantMap map = result.value("rows").toList().first().toMap();
		if(map["id"].toInt() != mID)
			return;

		ui.japaneseNameEdit->setText(
			map["name_ja"].toString() );
		ui.englishNameEdit->setText(
			map["name_en"].toString() );

		setEnabled(true);
	}
};
