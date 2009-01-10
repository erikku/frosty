/******************************************************************************\
*  client/src/BasicRelationEdit.h                                              *
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

#ifndef __BasicRelationEdit_h__
#define __BasicRelationEdit_h__

#include "RelationEdit.h"
#include "ui_RelationEdit.h"

class BasicRelationEdit : public RelationEdit
{
	Q_OBJECT

public:
	BasicRelationEdit(QWidget *parent = 0);

public slots:
	virtual void submitRelation();

protected slots:
	virtual void refresh();
	virtual void ajaxResponse(const QVariantMap& resp,
		const QString& user_data);

protected:
	Ui::RelationEdit ui;
};

#endif // __BasicRelationEdit_h__
