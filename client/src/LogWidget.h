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
#include <QtCore/QVariantList>

#include <QtGui/QWidget>

class LogView;
class QListWidget;
class QListWidgetItem;
class baseTransfer;
class RequestSet;

class LogWidget : public QWidget
{
	Q_OBJECT

public:
	static LogWidget* getSingletonPtr();

public slots:
	void resendRequest();
	void updateCurrentRequest();
	void registerRequest(baseTransfer *transfer, const QVariant& request);
	void transferInfo(const QString& content, const QVariantList& response);

protected:
	LogWidget(QWidget *parent = 0);

	LogView *mLogView;
	QListWidget *mLogList;

	QMap<QListWidgetItem*, RequestSet*> mRequests;
	QMap<baseTransfer*, QListWidgetItem*> mRequestMap;
};

#endif // ___h__
