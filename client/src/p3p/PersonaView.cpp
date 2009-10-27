/******************************************************************************\
*  client/src/PersonaView.cpp                                                    *
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

#include "PersonaView.h"

#include <QtGui/QMessageBox>

PersonaView::PersonaView(QWidget *parent_widget) : AjaxView(parent_widget)
{
	ui.setupUi(this);

	// View Labels
	darkenWidget(ui.nameLabel);
	darkenWidget(ui.englishNameLabel);
	darkenWidget(ui.japaneseNameLabel);

	// Edit labels
	darkenWidget(ui.nameLabel2);
	darkenWidget(ui.englishNameLabel2);
	darkenWidget(ui.japaneseNameLabel2);

	// Bind the data
	bindText("name_en", ui.englishName, ui.englishNameEdit);
	bindText("name_ja", ui.japaneseName, ui.japaneseNameEdit);

	// Init the view
	initView(ui.stackedWidget, ui.editButton, ui.cancelButton,
		ui.refreshButton, ui.updateButton);
}

bool PersonaView::checkValues()
{
	// Check name_ja and name_en
	if( ui.japaneseNameEdit->text().isEmpty() ||
		ui.englishNameEdit->text().isEmpty() )
	{
		QMessageBox::warning(this, tr("Update Error"), tr("You must both a"
			"valid Japanese and English persona name."));

		return false;
	}

	return true;
}

QString PersonaView::table() const
{
	return QString("db_skills");
}

QString PersonaView::addWarningTitle() const
{
	return tr("Add Persona");
}

QString PersonaView::addWarningMessage() const
{
	return tr("You are attemping to add another persona while editing a "
		"persona. If you continue you will lose any changes you made to this "
		"persona. Are you sure you want to continue?");
}
