/******************************************************************************\
*  client/src/TraitWindow.h                                                    *
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

#ifndef __TraitWindow_h__
#define __TraitWindow_h__

#include <QtGui/QWidget>
#include <QtCore/QVariant>
#include <QtCore/QMap>

class TraitList;
class TraitView;
class QListWidgetItem;

class TraitWindow : public QWidget
{
	Q_OBJECT

public:
	TraitWindow(QWidget *parent = 0);

public slots:
	void refresh();

protected:
	TraitList *mTraitList;
	TraitView *mTraitView;
};

#endif // __TraitWindow_h__
