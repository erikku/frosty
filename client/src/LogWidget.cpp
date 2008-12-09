/******************************************************************************\
*  client/src/LogWidget.cpp                                                    *
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

#include "LogWidget.h"
#include "Settings.h"
#include "LogView.h"

#include "ajaxTransfer.h"
#include "ajax.h"

#include <QtCore/QDateTime>
#include <QtGui/QListWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QSplitter>
#include <QtGui/QAction>

class RequestSet
{
public:
	QString content;
	QVariant request, response;
};

static LogWidget *g_log_inst = 0;

LogWidget::LogWidget(QWidget *parent_widget) :
	QWidget(parent_widget, Qt::Dialog)
{
	mLogList = new QListWidget;
	mLogView = new LogView;

	QSplitter *mainSplitter = new QSplitter;
	mainSplitter->addWidget(mLogList);
	mainSplitter->addWidget(mLogView);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(mainSplitter);

	setLayout(mainLayout);

	setWindowTitle( tr("%1 - AJAX Log").arg(
		tr("Absolutely Frosty") ) );

	connect(mLogList, SIGNAL(itemSelectionChanged()),
		this, SLOT(updateCurrentRequest()));

	QAction *action = new QAction(tr("Resend Request"), this);

	mLogList->addAction(action);
	mLogList->setContextMenuPolicy(Qt::ActionsContextMenu);

	connect(action, SIGNAL(triggered(bool)), this, SLOT(resendRequest()));
}

LogWidget* LogWidget::getSingletonPtr()
{
	if(!g_log_inst)
		g_log_inst = new LogWidget;

	Q_ASSERT(g_log_inst != 0);

	return g_log_inst;
}

void LogWidget::updateCurrentRequest()
{
	if( !mLogList->selectedItems().count() )
		return;

	QListWidgetItem *item = mLogList->selectedItems().first();
	mLogView->loadRequest( mRequests.value(item)->request );
	mLogView->loadResponse( mRequests.value(item)->response );
	mLogView->loadContent( mRequests.value(item)->content );
}

void LogWidget::resendRequest()
{
	if( !mLogList->selectedItems().count() )
		return;

	QListWidgetItem *item = mLogList->selectedItems().first();

	QVariantMap request = mRequests.value(item)->request.toMap();
	QVariantList acts = request.value("actions").toList();

	foreach(QVariant act, acts)
		ajax::getSingletonPtr()->request(settings->url(), act);
}

void LogWidget::registerRequest(ajaxTransfer *transfer, const QVariant& request)
{
	connect(transfer, SIGNAL(transferInfo(const QString&, const QVariant&)),
		this, SLOT(transferInfo(const QString&, const QVariant&)));

	QListWidgetItem *item = new QListWidgetItem( tr("%1 - %2").arg(
		mLogList->count(), 4, 10, QLatin1Char('0')).arg(
		QDateTime::currentDateTime().toString() ) );

	RequestSet *set = new RequestSet;
	set->request = request;

	mRequests[item] = set;
	mRequestMap[transfer] = item;

	mLogList->addItem(item);
}

void LogWidget::transferInfo(const QString& content, const QVariant& response)
{
	ajaxTransfer *transfer = qobject_cast<ajaxTransfer*>( sender() );
	if(!transfer)
		return;

	if( !mRequestMap.contains(transfer) )
		return;

	QListWidgetItem *item = mRequestMap.take(transfer);

	mRequests.value(item)->content = content;
	mRequests.value(item)->response = response;
}
