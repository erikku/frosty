/******************************************************************************\
*  client/src/LogView.h                                                        *
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

#ifndef __LogView_h__
#define __LogView_h__

#include "ui_LogView.h"

#include <QtCore/QVariant>

class LogView : public QWidget
{
	Q_OBJECT

public:
	LogView(QWidget *parent = 0);

public slots:
	void loadRequest(const QVariant& request);
	void loadResponse(const QVariant& response);
	void loadContent(const QString& content);

protected:
	void darkenWidget(QWidget *widget);

	Ui::LogView ui;
};

#endif // __LogView_h__
