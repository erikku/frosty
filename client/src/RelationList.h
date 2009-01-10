/******************************************************************************\
*  client/src/RelationList.h                                                   *
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

#ifndef __RelationList_h__
#define __RelationList_h__

#include <QtCore/QVariantMap>

#include <QtGui/QDialog>

class RelationEdit;

class RelationList : public QDialog
{
	Q_OBJECT

public:
	RelationList(RelationEdit *edit, QWidget *parent = 0);
	virtual ~RelationList();

public slots:
	virtual void refresh() = 0;
	virtual void showList(const QString& windowTitle, const QString& table);

	virtual void addRelation();
	virtual void editRelation(int id = -1);
	virtual void deleteRelation(int id = -1, const QString& name = QString());

signals:
	void relationsUpdated();

protected slots:
	virtual void itemSelectionChanged() = 0;
	virtual void ajaxResponse(const QVariantMap& resp,
		const QString& user_data);

protected:
	QString mTable;
	RelationEdit *mEdit;
};

#endif // __RelationList_h__
