/******************************************************************************\
*  client/src/TraitList.h                                                      *
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

#ifndef __TraitList_h__
#define __TraitList_h__

#include "AjaxList.h"

#include <QtCore/QMap>
#include <QtCore/QVariant>

class TraitView;
class QListWidgetItem;

class TraitList : public AjaxList
{
	Q_OBJECT

public:
	TraitList(TraitView *view = 0, QWidget *parent = 0);

protected:
	virtual QVariant filterAction() const;
	virtual QVariant filterUserData() const;

	virtual QVariant listAction() const;
	virtual QVariant listUserData() const;

	virtual QString switchTitle() const;
	virtual QString switchMessage() const;

	virtual QString deleteTitle() const;
	virtual QString deleteMessage() const;
	virtual QVariant deleteAction(int id) const;
	virtual QVariant deleteUserData() const;

	virtual int filterID(const QVariantMap& map) const;
	virtual QString itemIcon(const QVariantMap& map) const;
};

#endif // __TraitList_h__
