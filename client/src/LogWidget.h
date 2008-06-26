/******************************************************************************\
*  client/src/LogWidget.h                                                      *
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

#ifndef __LogWidget_h__
#define __LogWidget_h__

#include <QtCore/QMap>
#include <QtCore/QVariant>
#include <QtGui/QWidget>

class LogView;
class QListWidget;
class QListWidgetItem;
class ajaxTransfer;
class RequestSet;

class LogWidget : public QWidget
{
	Q_OBJECT

public:
	LogWidget(QWidget *parent = 0);

public slots:
	void updateCurrentRequest();
	void registerRequest(ajaxTransfer *transfer, const QVariant& request);
	void transferInfo(const QString& content, const QVariant& response);

protected:
	LogView *mLogView;
	QListWidget *mLogList;

	QMap<QListWidgetItem*, RequestSet*> mRequests;
	QMap<ajaxTransfer*, QListWidgetItem*> mRequestMap;
};

#endif // ___h__
