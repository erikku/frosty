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

#include <QtGui/QMessageBox>

SkillView::SkillView(QWidget *parent) : AjaxView(parent)
{
	ui.setupUi(this);

	// View Labels
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

	// Bind the data
	bindText("name_en", ui.englishName, ui.englishNameEdit);
	bindText("name_ja", ui.japaneseName, ui.japaneseNameEdit);
	bindIcon("icon", ui.icon, ui.iconEdit, "icons/skills/icon_%1.png",
		"icons/skills");
	bindNumber("class", ui.skill_class, ui.classEdit);
	bindNumber("rank", ui.rank, ui.rankEdit);
	bindBool("player_only", ui.playerOnly, ui.playerOnlyEdit);
	bindTextBox("desc_en", ui.desc_en, ui.desc_enEdit);
	bindTextBox("desc_ja", ui.desc_ja, ui.desc_jaEdit);
	bindRelation("action_type", ui.type, ui.typeEdit, "db_action_type",
		ui.typeEditButton, tr("Action Type"));
	bindRelation("category", ui.category, ui.categoryEdit, "db_category",
		ui.categoryEditButton, tr("Category"));
	bindRelation("affinity", ui.affinity, ui.affinityEdit, "db_affinity",
		ui.affinityEditButton, tr("Affinity"));
	bindRelation("expert", ui.expert, ui.expertEdit, "db_expert",
		ui.expertEditButton, tr("Expert"));
	bindNumberSet("cost_set", ui.cost, ui.costEdit, ui.costEditCombo,
		QStringList() << "hp_cost" << "mp_cost" << "mag_cost",
		QStringList() << tr("HP%1") << tr("MP%1") << tr("MAG%1"), tr("+"));

	// Give the relation buttons an icon
	ui.typeEditButton->setIcon( QIcon(":/edit.png") );
	ui.categoryEditButton->setIcon( QIcon(":/edit.png") );
	ui.affinityEditButton->setIcon( QIcon(":/edit.png") );
	ui.expertEditButton->setIcon( QIcon(":/edit.png") );

	// Init the view
	initView(ui.stackedWidget, ui.editButton, ui.cancelButton,
		ui.refreshButton, ui.updateButton);
};

bool SkillView::checkValues()
{
	// Check name_ja and name_en
	if( ui.japaneseNameEdit->text().isEmpty() || ui.englishNameEdit->text().isEmpty() )
	{
		QMessageBox::warning(this, tr("Update Error"), tr("You must both a valid "
			"Japanese and English skill name."));

		return false;
	}

	// Check expert
	if( !ui.expertEdit->count() || ui.expertEdit->itemData(
		ui.expertEdit->currentIndex() ).toInt() < 1 )
	{
		QMessageBox::warning(this, tr("Update Error"), tr("You must set a valid "
			"expert."));

		return false;
	}

	// Check affinity
	if( !ui.affinityEdit->count() || ui.affinityEdit->itemData(
		ui.affinityEdit->currentIndex() ).toInt() < 1 )
	{
		QMessageBox::warning(this, tr("Update Error"), tr("You must set a valid "
			"affinity."));

		return false;
	}

	// Check category
	if( !ui.categoryEdit->count() || ui.categoryEdit->itemData(
		ui.categoryEdit->currentIndex() ).toInt() < 1 )
	{
		QMessageBox::warning(this, tr("Update Error"), tr("You must set a valid "
			"category."));

		return false;
	}

	// Check affinity
	if( !ui.typeEdit->count() || ui.typeEdit->itemData(
		ui.typeEdit->currentIndex() ).toInt() < 1 )
	{
		QMessageBox::warning(this, tr("Update Error"), tr("You must set a valid "
			"type."));

		return false;
	}

	// Check icon
	if( ui.iconEdit->value().isEmpty() )
	{
		QMessageBox::warning(this, tr("Update Error"), tr("You must set a valid "
			"icon."));

		return false;
	}

	return true;
};

QString SkillView::table() const
{
	return QString("db_skills");
};

QString SkillView::addWarningTitle() const
{
	return tr("Add Skill");
};

QString SkillView::addWarningMessage() const
{
	return tr("You are attemping to add another skill while editing a skill. "
			"If you continue you will lose any changes you made to the skill. "
			"Are you sure you want to continue?");
};
