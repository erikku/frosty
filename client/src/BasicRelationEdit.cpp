/******************************************************************************\
*  client/src/BasicRelationEdit.cpp                                            *
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

#include "BasicRelationEdit.h"
#include "Settings.h"
#include "ajax.h"

#include <QtGui/QMessageBox>

BasicRelationEdit::BasicRelationEdit(QWidget *parent) : RelationEdit(parent)
{
	ui.setupUi(this);

	darkenWidget(ui.nameLabel);
	darkenWidget(ui.japaneseNameLabel);
	darkenWidget(ui.englishNameLabel);

	connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(submitRelation()));
};

void BasicRelationEdit::submitRelation()
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
	action["user_data"] = QString("%1_relation_updated").arg(mTable);
	action["rows"] = QVariantList() << row;

	if(mID > 0)
		action["where"] = QVariantList() << where;

	ajax::getSingletonPtr()->request(settings->url(), action);
};

void BasicRelationEdit::refresh()
{
	ui.japaneseNameEdit->clear();
	ui.englishNameEdit->clear();

	setEnabled(true);

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

void BasicRelationEdit::ajaxResponse(const QVariant& resp)
{
	RelationEdit::ajaxResponse(resp);

	if( mTable.isEmpty() )
		return;

	QVariantMap result = resp.toMap();

	if(result.value("user_data").toString() ==
		QString("%1_entry").arg(mTable))
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
