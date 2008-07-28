/******************************************************************************\
*  client/src/SkillView.h                                                      *
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

#ifndef __SkillView_h__
#define __SkillView_h__

#include "AjaxView.h"
#include "ui_SkillView.h"

#include <QtCore/QVariant>
#include <QtCore/QMap>

class SkillView : public AjaxView
{
	Q_OBJECT

public:
	SkillView(QWidget *parent = 0);

protected:
	virtual bool checkValues();
	virtual QString table() const;

	virtual QString addWarningTitle() const;
	virtual QString addWarningMessage() const;

	Ui::SkillView ui;
};

#endif // __SkillView_h__
