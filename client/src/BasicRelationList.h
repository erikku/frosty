/******************************************************************************\
*  client/src/BasicRelationList.h                                              *
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

#ifndef __BasicRelationList_h__
#define __BasicRelationList_h__

#include "RelationList.h"
#include "ui_RelationList.h"

class BasicRelationList : public RelationList
{
	Q_OBJECT

public:
	BasicRelationList(RelationEdit *edit, QWidget *parent = 0);

public slots:
	virtual void refresh();

	virtual void editRelation(int id = -1);
	virtual void deleteRelation(int id = -1, const QString& name = QString());

protected slots:
	virtual void itemSelectionChanged();
	virtual void ajaxResponse(const QVariant& resp);

protected:
	Ui::RelationList ui;
};

#endif // __BasicRelationList_h__
