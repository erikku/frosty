/******************************************************************************\
*  client/src/SkillView.cpp                                                    *
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

#include "SkillView.h"
#include "IconSelect.h"
#include "RelationList.h"
#include "Settings.h"
#include "ajax.h"

#include <QtCore/QFileInfo>
#include <QtGui/QMessageBox>
#include <QtGui/QPalette>

SkillView::SkillView(QWidget *parent) : QWidget(parent), mID(-1), mPreserveEdit(false),
	mExpertGood(false), mAffinityGood(false), mCategoryGood(false), mActionTypeGood(false),
	mLastExpertID(-1), mLastActionTypeID(-1), mLastCategoryID(-1), mLastAffinityID(-1)
{
	ui.setupUi(this);

	mIconSelect = new IconSelect;
	mRelationList = new RelationList;

	connect(ui.editButton, SIGNAL(clicked(bool)), this, SLOT(edit()));
	connect(ui.cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancel()));
	connect(ui.refreshButton, SIGNAL(clicked(bool)), this, SLOT(refresh()));
	connect(ui.updateButton, SIGNAL(clicked(bool)), this, SLOT(updateSkill()));

	connect(ui.costEditCombo, SIGNAL(currentIndexChanged(int)),
		this, SLOT(costEditUpdate()));

	connect(ui.costEdit, SIGNAL(valueChanged(int)), this, SLOT(costEditChanged()));

	connect(ui.iconEdit, SIGNAL(iconDoubleClicked()), this, SLOT(showIconSelect()));

	connect(mIconSelect, SIGNAL(iconReady(const QString&, const QString&)),
		this, SLOT(updateIcon(const QString&, const QString&)));

	connect(ui.typeEditButton, SIGNAL(clicked(bool)), this, SLOT(editType()));
	connect(ui.categoryEditButton, SIGNAL(clicked(bool)), this, SLOT(editCategory()));
	connect(ui.affinityEditButton, SIGNAL(clicked(bool)), this, SLOT(editAffinity()));
	connect(ui.expertEditButton, SIGNAL(clicked(bool)), this, SLOT(editExpert()));

	darkenWidget(ui.nameLabel);
	darkenWidget(ui.englishNameLabel);
	darkenWidget(ui.japaneseNameLabel);

	darkenWidget(ui.classLabel);
	darkenWidget(ui.rankLabel);
	darkenWidget(ui.costLabel);
	darkenWidget(ui.playerOnlyLabel);

	darkenWidget(ui.typeLabel);
	darkenWidget(ui.categoryLabel);
	darkenWidget(ui.affinityLabel);
	darkenWidget(ui.expertLabel);

	darkenWidget(ui.desc_jaLabel);
	darkenWidget(ui.desc_enLabel);

	// Edit labels
	darkenWidget(ui.nameLabel2);
	darkenWidget(ui.englishNameLabel2);
	darkenWidget(ui.japaneseNameLabel2);

	darkenWidget(ui.classLabel2);
	darkenWidget(ui.rankLabel2);
	darkenWidget(ui.costLabel2);
	darkenWidget(ui.playerOnlyLabel2);

	darkenWidget(ui.typeLabel2);
	darkenWidget(ui.categoryLabel2);
	darkenWidget(ui.affinityLabel2);
	darkenWidget(ui.expertLabel2);

	darkenWidget(ui.desc_jaLabel2);
	darkenWidget(ui.desc_enLabel2);

	ui.typeEditButton->setIcon( QIcon(":/edit.png") );
	ui.categoryEditButton->setIcon( QIcon(":/edit.png") );
	ui.affinityEditButton->setIcon( QIcon(":/edit.png") );
	ui.expertEditButton->setIcon( QIcon(":/edit.png") );

	ui.updateButton->setVisible(false);
	ui.cancelButton->setVisible(false);

	// Hide the edit button unless you have permissions to edit
	ui.editButton->setVisible(false);

	ajax::getSingletonPtr()->subscribe(this);
	setEnabled(false);
	refresh();

	connect(ui.typeEdit, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCombo()));
	connect(ui.categoryEdit, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCombo()));
	connect(ui.affinityEdit, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCombo()));
	connect(ui.expertEdit, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCombo()));
};

void SkillView::updateCombo()
{
	if(sender() == ui.typeEdit && mActionTypeGood)
	{
		mLastActionTypeID = ui.typeEdit->itemData(
			ui.typeEdit->currentIndex() ).toInt();
	}
	else if(sender() == ui.categoryEdit && mCategoryGood)
	{
		mLastCategoryID = ui.categoryEdit->itemData(
			ui.categoryEdit->currentIndex() ).toInt();
	}
	else if(sender() == ui.affinityEdit && mAffinityGood)
	{
		mLastAffinityID = ui.affinityEdit->itemData(
			ui.affinityEdit->currentIndex() ).toInt();
	}
	else if(sender() == ui.expertEdit && mExpertGood)
	{
		mLastExpertID = ui.expertEdit->itemData(
			ui.expertEdit->currentIndex() ).toInt();
	}
};

void SkillView::editType()
{
	mRelationList->showList(tr("Action Type"), "action_type");
};

void SkillView::editCategory()
{
	mRelationList->showList(tr("Category"), "category");
};

void SkillView::editAffinity()
{
	mRelationList->showList(tr("Affinity"), "affinity");
};

void SkillView::editExpert()
{
	mRelationList->showList(tr("Expert"), "expert");
};

void SkillView::updateIcon(const QString& path, const QString& value)
{
	ui.iconEdit->setValue(value);
	ui.iconEdit->setPixmap(path);
};

void SkillView::showIconSelect()
{
	mIconSelect->selectIcon("skills");
};

void SkillView::costEditChanged()
{
	switch( ui.costEditCombo->currentIndex() )
	{
		case 0:
		default:
			mCostHP = ui.costEdit->value();
			break;
		case 1:
			mCostMP = ui.costEdit->value();
			break;
		case 2:
			mCostMAG = ui.costEdit->value();
			break;
	}
};

void SkillView::costEditUpdate()
{
	switch( ui.costEditCombo->currentIndex() )
	{
		case 0:
		default:
			ui.costEdit->setValue(mCostHP);
			break;
		case 1:
			ui.costEdit->setValue(mCostMP);
			break;
		case 2:
			ui.costEdit->setValue(mCostMAG);
			break;
	}
};

bool SkillView::isEditing() const
{
	return (ui.stackedWidget->currentIndex() == 1);
};

void SkillView::edit()
{
	refresh();
	ui.editButton->setVisible(false);
	ui.refreshButton->setVisible(false);
	ui.updateButton->setVisible(true);
	ui.cancelButton->setVisible(true);
	ui.stackedWidget->setCurrentIndex(1);
};

void SkillView::addSkill()
{
	if( isEditing() )
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this,
			tr("Add Skill"),
			tr("You are attemping to add another skill while editing a skill. "
			"If you continue you will lose any changes you made to the skill. "
			"Are you sure you want to continue?"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

		if(button == QMessageBox::No)
			return;
	}

	mID = -1;

	clear();
	ui.editButton->setVisible(false);
	ui.refreshButton->setVisible(false);
	ui.updateButton->setVisible(true);
	ui.cancelButton->setVisible(true);
	ui.stackedWidget->setCurrentIndex(1);
	setEnabled(true);
};

void SkillView::cancel()
{
	ui.editButton->setVisible(true);
	ui.refreshButton->setVisible(true);
	ui.updateButton->setVisible(false);
	ui.cancelButton->setVisible(false);
	ui.stackedWidget->setCurrentIndex(0);

	if(mID < 0)
		setEnabled(false);
};

void SkillView::clear()
{
	ui.icon->setPixmap( QPixmap(":/blank.png") );
	ui.japaneseName->clear();
	ui.englishName->clear();

	ui.skill_class->clear();
	ui.rank->clear();
	ui.cost->clear();
	ui.playerOnly->clear();

	ui.type->clear();
	ui.category->clear();
	ui.affinity->clear();
	ui.expert->clear();

	ui.desc_ja->clear();
	ui.desc_en->clear();

	if(!mPreserveEdit)
	{
		mCostHP  = 0;
		mCostMP  = 0;
		mCostMAG = 0;

		ui.iconEdit->setPixmap( QPixmap(":/blank.png") );
		ui.iconEdit->setValue( QString() );
		ui.japaneseNameEdit->clear();
		ui.englishNameEdit->clear();

		ui.classEdit->setValue(-1);
		ui.rankEdit->setValue(-1);
		ui.costEdit->setValue(0);
		ui.playerOnlyEdit->setCheckState(Qt::Unchecked);

		ui.typeEdit->setCurrentIndex(0);
		ui.categoryEdit->setCurrentIndex(0);
		ui.affinityEdit->setCurrentIndex(0);
		ui.expertEdit->setCurrentIndex(0);

		ui.desc_jaEdit->clear();
		ui.desc_enEdit->clear();
	}
};

void SkillView::updateSkill()
{
	// Check name_ja and name_en
	if( ui.japaneseNameEdit->text().isEmpty() || ui.englishNameEdit->text().isEmpty() )
	{
		QMessageBox::warning(this, tr("Update Error"), tr("You must both a valid "
			"Japanese and English skill name."));

		return;
	}

	// Checkexpert
	if( !ui.expertEdit->count() || ui.expertEdit->itemData(
		ui.expertEdit->currentIndex() ).toInt() < 1 )
	{
		QMessageBox::warning(this, tr("Update Error"), tr("You must set a valid "
			"expert."));

		return;
	}

	// Check affinity
	if( !ui.affinityEdit->count() || ui.affinityEdit->itemData(
		ui.affinityEdit->currentIndex() ).toInt() < 1 )
	{
		QMessageBox::warning(this, tr("Update Error"), tr("You must set a valid "
			"affinity."));

		return;
	}

	// Check category
	if( !ui.categoryEdit->count() || ui.categoryEdit->itemData(
		ui.categoryEdit->currentIndex() ).toInt() < 1 )
	{
		QMessageBox::warning(this, tr("Update Error"), tr("You must set a valid "
			"category."));

		return;
	}

	// Check affinity
	if( !ui.typeEdit->count() || ui.typeEdit->itemData(
		ui.typeEdit->currentIndex() ).toInt() < 1 )
	{
		QMessageBox::warning(this, tr("Update Error"), tr("You must set a valid "
			"type."));

		return;
	}

	// Check icon
	if( ui.iconEdit->value().isEmpty() )
	{
		QMessageBox::warning(this, tr("Update Error"), tr("You must set a valid "
			"icon."));

		return;
	}

	setEnabled(false);

	QVariantList columns;
	columns << QString("name_en") << QString("name_ja")
		<< QString("icon") << QString("category") << QString("mp_cost")
		<< QString("affinity") << QString("action_type") << QString("hp_cost")
		<< QString("mag_cost") << QString("class") << QString("rank") << QString("expert")
		<< QString("player_only") << QString("desc_en") << QString("desc_ja");

	QVariantMap where;
	where["type"] = "equals";
	where["column"] = "id";
	where["value"] = QVariant(mID);

	QVariantMap row;
	row["name_en"] = ui.englishNameEdit->text();
	row["name_ja"] = ui.japaneseNameEdit->text();
	row["icon"] = ui.iconEdit->value();
	row["hp_cost"] = QVariant(mCostHP);
	row["mp_cost"] = QVariant(mCostMP);
	row["mag_cost"] = QVariant(mCostMAG);
	row["category"] = ui.categoryEdit->itemData( ui.categoryEdit->currentIndex() ).toInt();
	row["affinity"] = ui.affinityEdit->itemData( ui.affinityEdit->currentIndex() ).toInt();
	row["action_type"] = ui.typeEdit->itemData( ui.typeEdit->currentIndex() ).toInt();
	row["expert"] = ui.expertEdit->itemData( ui.expertEdit->currentIndex() ).toInt();
	row["class"] = ui.classEdit->value();
	row["rank"] = ui.rankEdit->value();
	row["desc_en"] = ui.desc_enEdit->toPlainText();
	row["desc_ja"] = ui.desc_jaEdit->toPlainText();

	row["player_only"] = QVariant(
		ui.playerOnlyEdit->checkState() == Qt::Checked ? true : false);

	QVariantMap action;

	if(mID > 0)
		action["action"] = QString("update");
	else
		action["action"] = QString("insert");

	action["table"] = QString("db_skills");
	action["columns"] = columns;
	action["user_data"] = QString("skill_update");

	if(mID > 0)
		action["where"] = QVariantList() << where;

	action["rows"] = QVariantList() << row;

	ajax::getSingletonPtr()->request(settings->url(), action);

	// Clear it waiting for the update to finish (and then a refresh)
	cancel();
	clear();
};

void SkillView::refresh()
{
	setEnabled(false);
	clear();

	// Set the lists as bad
	mAffinityGood = false;
	mCategoryGood = false;
	mActionTypeGood = false;
	mExpertGood = false;

	// Clear the old lists
	mAffinityCache.clear();
	mCategoryCache.clear();
	mActionTypeCache.clear();
	mExpertCache.clear();

	QVariantMap orderby_name;
	orderby_name["column"] = QString("name_%1").arg(settings->lang());
	orderby_name["direction"] = "asc";

	QVariantMap orderby_id;
	orderby_id["column"] = "id";
	orderby_id["direction"] = "asc";

	// Categories
	{
		QVariantList columns;
		columns << QString("id") << QString("name_en") << QString("name_ja");

		QVariantMap action;
		action["action"] = QString("select");
		action["table"] = QString("db_category");
		action["columns"] = columns;
		action["order_by"] = QVariantList() << orderby_name << orderby_id;
		action["user_data"] = QString("category_cache");

		ajax::getSingletonPtr()->request(settings->url(), action);
	}

	// Affinities
	{
		QVariantList columns;
		columns << QString("id") << QString("name_en") << QString("name_ja");

		QVariantMap action;
		action["action"] = QString("select");
		action["table"] = QString("db_affinity");
		action["columns"] = columns;
		action["order_by"] = QVariantList() << orderby_name << orderby_id;
		action["user_data"] = QString("affinity_cache");

		ajax::getSingletonPtr()->request(settings->url(), action);
	}

	// Action Types
	{
		QVariantList columns;
		columns << QString("id") << QString("name_en") << QString("name_ja");

		QVariantMap action;
		action["action"] = QString("select");
		action["table"] = QString("db_action_type");
		action["columns"] = columns;
		action["order_by"] = QVariantList() << orderby_name << orderby_id;
		action["user_data"] = QString("action_type_cache");

		ajax::getSingletonPtr()->request(settings->url(), action);
	}

	// Experts
	{
		QVariantList columns;
		columns << QString("id") << QString("name_en") << QString("name_ja");

		QVariantMap action;
		action["action"] = QString("select");
		action["table"] = QString("db_expert");
		action["columns"] = columns;
		action["order_by"] = QVariantList() << orderby_name << orderby_id;
		action["user_data"] = QString("expert_cache");

		ajax::getSingletonPtr()->request(settings->url(), action);
	}

	if(mID > 0)
		view(mID, false);
};

void SkillView::darkenWidget(QWidget *widget)
{
	QPalette dark_palette = palette();
	dark_palette.setBrush(QPalette::Window,
		dark_palette.brush(QPalette::Base));

	qobject_cast<QLabel*>(widget)->setMargin(5);
	widget->setAutoFillBackground(true);
	widget->setPalette(dark_palette);
};

void SkillView::view(int id, bool switchView)
{
	if(switchView)
		cancel();

	clear();
	setEnabled(true);

	if(id <= 0)
		return;

	mID = id;

	QVariantList columns;
	columns << QString("id") << QString("name_en") << QString("name_ja")
		<< QString("icon") << QString("category") << QString("mp_cost")
		<< QString("affinity") << QString("action_type") << QString("hp_cost")
		<< QString("mag_cost") << QString("class") << QString("rank") << QString("expert")
		<< QString("player_only") << QString("desc_en") << QString("desc_ja");

	QVariantMap where;
	where["type"] = "equals";
	where["column"] = "id";
	where["value"] = QVariant(id);

	QVariantMap action;
	action["action"] = QString("select");
	action["table"] = QString("db_skills");
	action["columns"] = columns;
	action["user_data"] = QString("skill_view");
	action["where"] = QVariantList() << where;

	ajax::getSingletonPtr()->request(settings->url(), action);

	setEnabled(false);
};

void SkillView::ajaxResponse(const QVariant& resp)
{
	QVariantMap result = resp.toMap();

	if(result.value("user_data").toString() == "skill_view")
	{
		if( result.value("rows").toList().first().toMap().value("id") == mID )
			mSleepingResponse = result;
	}
	else if(result.value("user_data").toString() == "skill_update")
	{
		refresh();

		emit skillChanged();

		return;
	}
	else if(result.value("user_data").toString() == "category_cache")
	{
		mCategoryGood = false;
		mCategoryCache.clear();
		QVariantList rows = result.value("rows").toList();
		ui.categoryEdit->clear();

		int index = -1;

		for(int i = 0; i < rows.count(); i++)
		{
			QVariantMap map = rows.at(i).toMap();

			mCategoryCache[map["id"].toInt()] = map;
			ui.categoryEdit->addItem(
				map[QString("name_%1").arg(settings->lang())].toString() );
			ui.categoryEdit->setItemData(i, map["id"]);

			if(map["id"].toInt() == mLastCategoryID)
				index = i;
		}

		if(index != -1)
			ui.categoryEdit->setCurrentIndex(index);

		mCategoryGood = true;
	}
	else if(result.value("user_data").toString() == "affinity_cache")
	{
		mAffinityGood = false;
		mAffinityCache.clear();
		QVariantList rows = result.value("rows").toList();
		ui.affinityEdit->clear();

		int index = -1;

		for(int i = 0; i < rows.count(); i++)
		{
			QVariantMap map = rows.at(i).toMap();

			mAffinityCache[map["id"].toInt()] = map;
			ui.affinityEdit->addItem(
				map[QString("name_%1").arg(settings->lang())].toString() );
			ui.affinityEdit->setItemData(i, map["id"]);

			if(map["id"].toInt() == mLastAffinityID)
				index = i;
		}

		if(index != -1)
			ui.affinityEdit->setCurrentIndex(index);

		mAffinityGood = true;
	}
	else if(result.value("user_data").toString() == "action_type_cache")
	{
		mActionTypeGood = false;
		mActionTypeCache.clear();
		QVariantList rows = result.value("rows").toList();
		ui.typeEdit->clear();

		int index = -1;

		for(int i = 0; i < rows.count(); i++)
		{
			QVariantMap map = rows.at(i).toMap();

			mActionTypeCache[map["id"].toInt()] = map;
			ui.typeEdit->addItem(
				map[QString("name_%1").arg(settings->lang())].toString() );
			ui.typeEdit->setItemData(i, map["id"]);

			if(map["id"].toInt() == mLastActionTypeID)
				index = i;
		}

		if(index != -1)
			ui.typeEdit->setCurrentIndex(index);

		mActionTypeGood = true;
	}
	else if(result.value("user_data").toString() == "expert_cache")
	{
		mExpertGood = false;
		mExpertCache.clear();
		QVariantList rows = result.value("rows").toList();
		ui.expertEdit->clear();

		int index = -1;

		for(int i = 0; i < rows.count(); i++)
		{
			QVariantMap map = rows.at(i).toMap();

			mExpertCache[map["id"].toInt()] = map;
			ui.expertEdit->addItem(
				map[QString("name_%1").arg(settings->lang())].toString() );
			ui.expertEdit->setItemData(i, map["id"]);

			if(map["id"].toInt() == mLastExpertID)
				index = i;
		}

		if(index != -1)
			ui.expertEdit->setCurrentIndex(index);

		mExpertGood = true;
	}
	else if(result.value("user_data").toString() == "auth_query_perms")
	{
		if( result.value("perms").toMap().value("modify_db").toBool() )
			ui.editButton->setVisible(true);
	}

	result = mSleepingResponse; // Check if we can now load the skill
	if(result.value("user_data").toString() == "skill_view")
	{
		// Someone clicked too fast, ignore this request
		if( result.value("rows").toList().first().toMap().value("id") != mID )
		{
			mSleepingResponse = QVariantMap();

			return;
		}

		// Wait for all the needed data to become avaliable
		if( !mAffinityGood || !mCategoryGood || !mActionTypeGood || !mExpertGood )
			return;

		// Clear out the sleeping response so we don't do this again
		mSleepingResponse = QVariantMap();

		QVariantList rows = result.value("rows").toList();
		QVariantMap map = rows.at(0).toMap();

		QString icon_path = QString("icons/skills/icon_%1.png").arg(
			map["icon"].toString() );

		if( QFileInfo(icon_path).exists() )
			ui.icon->setPixmap( QPixmap(icon_path) );
		else
			ui.iconEdit->setPixmap( QPixmap(":/blank.png") );

		ui.englishName->setText( map["name_en"].toString() );
		ui.japaneseName->setText( map["name_ja"].toString() );

		QStringList cost;
		if(map["hp_cost"].toInt() > 0)
			cost << QString("HP%1%").arg(map["hp_cost"].toInt());
		if(map["mp_cost"].toInt() > 0)
			cost << QString("MP%1").arg(map["mp_cost"].toInt());
		if(map["mag_cost"].toInt() > 0)
			cost << QString("MAG%2").arg(map["mag_cost"].toInt());

		if( cost.isEmpty() )
			cost << tr("N/A");

		QString category = mCategoryCache[map["category"].toInt()
			].value( QString("name_%1").arg(settings->lang()) ).toString();

		QString affinity = mAffinityCache[map["affinity"].toInt()
			].value( QString("name_%1").arg(settings->lang()) ).toString();

		QString expert = mExpertCache[map["expert"].toInt()
			].value( QString("name_%1").arg(settings->lang()) ).toString();

		QString type = mActionTypeCache[map["action_type"].toInt()
			].value( QString("name_%1").arg(settings->lang()) ).toString();

		ui.affinity->setText(affinity);
		ui.category->setText(category);
		ui.expert->setText(expert);
		ui.type->setText(type);

		if( map["class"].toInt() < 0 )
			ui.skill_class->setText( tr("N/A") );
		else
			ui.skill_class->setText( map["class"].toString() );

		if( map["rank"].toInt() < 0 )
			ui.rank->setText( tr("N/A") );
		else
			ui.rank->setText( map["rank"].toString() );

		ui.cost->setText( cost.join("+") );
		ui.playerOnly->setText( map["player_only"].toBool() ? tr("YES") : tr("NO") );

		ui.desc_ja->setText( map["desc_ja"].toString() );
		ui.desc_en->setText( map["desc_en"].toString() );

		if(!mPreserveEdit)
		{
			if( QFileInfo(icon_path).exists() )
			{
				ui.iconEdit->setValue( map["icon"].toString() );
				ui.iconEdit->setPixmap( QPixmap(icon_path) );
			}
			else
			{
				ui.iconEdit->setValue( QString() );
				ui.iconEdit->setPixmap( QPixmap(":/blank.png") );
			}

			ui.englishNameEdit->setText( map["name_en"].toString() );
			ui.japaneseNameEdit->setText( map["name_ja"].toString() );

			mCostHP  = map["hp_cost"].toInt();
			mCostMP  = map["mp_cost"].toInt();
			mCostMAG = map["mag_cost"].toInt();

			if(mCostHP)
			{
				ui.costEditCombo->setCurrentIndex(0);
				ui.costEdit->setValue(mCostHP);
			}
			else if(mCostMP)
			{
				ui.costEditCombo->setCurrentIndex(1);
				ui.costEdit->setValue(mCostMP);
			}
			else if(mCostMAG)
			{
				ui.costEditCombo->setCurrentIndex(2);
				ui.costEdit->setValue(mCostMAG);
			}
			else
			{
				ui.costEditCombo->setCurrentIndex(0);
				ui.costEdit->setValue(mCostHP);
			}

			int categoryID = ui.categoryEdit->findText(category);
			if(categoryID != -1)
				ui.categoryEdit->setCurrentIndex(categoryID);

			int affinityID = ui.affinityEdit->findText(affinity);
			if(affinityID != -1)
				ui.affinityEdit->setCurrentIndex(affinityID);

			int typeID = ui.typeEdit->findText(type);
			if(typeID != -1)
				ui.typeEdit->setCurrentIndex(typeID);

			int expertID = ui.expertEdit->findText(expert);
			if(expertID != -1)
				ui.expertEdit->setCurrentIndex(expertID);

			ui.playerOnlyEdit->setCheckState( map["player_only"].toBool() ?
				Qt::Checked : Qt::Unchecked );

			ui.classEdit->setValue( map["class"].toInt() );
			ui.rankEdit->setValue( map["rank"].toInt() );

			ui.desc_jaEdit->setText( map["desc_ja"].toString() );
			ui.desc_enEdit->setText( map["desc_en"].toString() );
		}

		mPreserveEdit = false;
		setEnabled(true);
	}
};

void SkillView::setPreserveEdit(bool preserve)
{
	mPreserveEdit = preserve;
};
