/******************************************************************************\
*  client/src/MashouView.cpp                                                   *
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

#include "MashouView.h"

#include <QtGui/QMessageBox>

MashouView::MashouView(QWidget *parent_widget) : AjaxView(parent_widget)
{
	ui.setupUi(this);

	// View Labels
	darkenWidget(ui.nameLabel);
	darkenWidget(ui.nameModifierLabel);
	darkenWidget(ui.englishNameLabel);
	darkenWidget(ui.japaneseNameLabel);
	darkenWidget(ui.englishModifierLabel);
	darkenWidget(ui.japaneseModifierLabel);
	darkenWidget(ui.nameDivider);
	darkenWidget(ui.equipTypeLabel);
	darkenWidget(ui.difficultyLabel);
	darkenWidget(ui.fusion1Label);
	darkenWidget(ui.fusion2Label);
	darkenWidget(ui.desc_enFusion1Label);
	darkenWidget(ui.desc_jaFusion1Label);
	darkenWidget(ui.desc_enFusion2Label);
	darkenWidget(ui.desc_jaFusion2Label);

	// Edit labels
	darkenWidget(ui.nameLabel2);
	darkenWidget(ui.nameModifierLabel2);
	darkenWidget(ui.englishNameLabel2);
	darkenWidget(ui.japaneseNameLabel2);
	darkenWidget(ui.englishModifierLabel2);
	darkenWidget(ui.japaneseModifierLabel2);
	darkenWidget(ui.nameEditDivider);
	darkenWidget(ui.equipTypeLabel2);
	darkenWidget(ui.difficultyLabel2);
	darkenWidget(ui.fusion1Label2);
	darkenWidget(ui.fusion2Label2);
	darkenWidget(ui.desc_enFusion1Label2);
	darkenWidget(ui.desc_jaFusion1Label2);
	darkenWidget(ui.desc_enFusion2Label2);
	darkenWidget(ui.desc_jaFusion2Label2);
	darkenWidget(ui.fusionEditDivider);

	// Bind the data
	bindText("name_en", ui.englishName, ui.englishNameEdit);
	bindText("name_ja", ui.japaneseName, ui.japaneseNameEdit);
	bindText("name_modifier_en", ui.englishModifier, ui.englishModifierEdit);
	bindText("name_modifier_ja", ui.japaneseModifier, ui.japaneseModifierEdit);
	bindRelation("equip_type", ui.equipType, ui.equipTypeEdit, "db_equip_type",
		QString(), ui.equipTypeEditButton, tr("Equip Type"));
	bindNumber("difficulty", ui.difficulty, ui.difficultyEdit);
	bindNumberRange("fusion1", ui.fusion1, ui.fusion1MinEdit,
		ui.fusion1MaxEdit, "fusion1_min", "fusion1_max", -1, -1, true);
	bindNumberRange("fusion2", ui.fusion2, ui.fusion2MinEdit,
		ui.fusion2MaxEdit, "fusion2_min", "fusion2_max", -1, -1, true);
	bindIcon("icon", ui.icon, ui.iconEdit, "icons/mashou/icon_%1.png",
		"icons/mashou");
	bindTextBox("fusion1_desc_en", ui.desc_enFusion1, ui.desc_enFusion1Edit);
	bindTextBox("fusion1_desc_ja", ui.desc_jaFusion1, ui.desc_jaFusion1Edit);
	bindTextBox("fusion2_desc_en", ui.desc_enFusion2, ui.desc_enFusion2Edit);
	bindTextBox("fusion2_desc_ja", ui.desc_jaFusion2, ui.desc_jaFusion2Edit);

	// Give the relation buttons an icon
	QIcon edit_icon(":/edit.png");
	ui.equipTypeEditButton->setIcon(edit_icon);

	// Init the view
	initView(ui.stackedWidget, ui.editButton, ui.cancelButton,
		ui.refreshButton, ui.updateButton);
}

bool MashouView::checkValues()
{
	// Check name_ja and name_en
	if( ui.japaneseNameEdit->text().isEmpty() ||
		ui.englishNameEdit->text().isEmpty() )
	{
		QMessageBox::warning(this, tr("Update Error"), tr("You must have both "
			"a valid Japanese and English mashou name."));

		return false;
	}

	return true;
}

QString MashouView::table() const
{
	return QString("db_mashou");
}

QString MashouView::addWarningTitle() const
{
	return tr("Add Mashou");
}

QString MashouView::addWarningMessage() const
{
	return tr("You are attemping to add another mashou while editing a mashou. "
		"If you continue you will lose any changes you made to the mashou. "
		"Are you sure you want to continue?");
}
