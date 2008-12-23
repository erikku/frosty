/******************************************************************************\
*  client/src/TraitView.cpp                                                    *
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

#include "TraitView.h"

#include <QtGui/QMessageBox>

TraitView::TraitView(QWidget *parent_widget) : AjaxView(parent_widget)
{
	ui.setupUi(this);

	// View Labels
	darkenWidget(ui.nameLabel);
	darkenWidget(ui.englishNameLabel);
	darkenWidget(ui.japaneseNameLabel);

	darkenWidget(ui.desc_jaLabel);
	darkenWidget(ui.desc_enLabel);

	// Edit labels
	darkenWidget(ui.nameLabel2);
	darkenWidget(ui.englishNameLabel2);
	darkenWidget(ui.japaneseNameLabel2);

	darkenWidget(ui.desc_jaLabel2);
	darkenWidget(ui.desc_enLabel2);

	// Bind the data
	bindText("name_en", ui.englishName, ui.englishNameEdit);
	bindText("name_ja", ui.japaneseName, ui.japaneseNameEdit);
	bindTextBox("desc_en", ui.desc_en, ui.desc_enEdit);
	bindTextBox("desc_ja", ui.desc_ja, ui.desc_jaEdit);

	// Init the view
	initView(ui.stackedWidget, ui.editButton, ui.cancelButton,
		ui.refreshButton, ui.updateButton);
}

bool TraitView::checkValues()
{
	// Check name_ja and name_en
	if( ui.japaneseNameEdit->text().isEmpty() ||
		ui.englishNameEdit->text().isEmpty() )
	{
		QMessageBox::warning(this, tr("Update Error"), tr("You must both a "
			"valid Japanese and English trait name."));

		return false;
	}

	return true;
}

QString TraitView::table() const
{
	return QString("db_traits");
}

QString TraitView::addWarningTitle() const
{
	return tr("Add Trait");
}

QString TraitView::addWarningMessage() const
{
	return tr("You are attemping to add another trait while editing a trait. "
			"If you continue you will lose any changes you made to the trait. "
			"Are you sure you want to continue?");
}
