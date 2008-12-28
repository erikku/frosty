/******************************************************************************\
*  client/src/AddDevil.h                                                       *
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

#ifndef __AddDevil_h__
#define __AddDevil_h__

#include "ui_AddDevil.h"

#include <QtGui/QDialog>

class AddDevil : public QDialog
{
	Q_OBJECT

public:
	AddDevil(QWidget *parent = 0);

signals:
	void devilSelected(int index, const QVariantMap& devil);

public slots:
	void add(int slot);

protected slots:
	void addDevil();
	void updateSearch();
	void handleItemSelected();
	void ajaxResponse(const QVariant& resp);

protected:
	bool itemMatches(const QVariantMap& map, const QString& search);

	int mSlot;

	Ui::AddDevil ui;
};

#endif // __AddDevil_h__
