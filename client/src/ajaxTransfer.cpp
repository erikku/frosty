/******************************************************************************\
*  client/src/ajaxTransfer.cpp                                                 *
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

#include "ajaxTransfer.h"
#include "ajax.h"
#include "json.h"

#include <QtCore/QStringList>
#include <QtGui/QMessageBox>

ajaxTransfer::ajaxTransfer(QObject *parent_object) : QObject(parent_object)
{
	// Nothing to see here
}

ajaxTransfer::~ajaxTransfer()
{
	if(mHttp)
	{
		delete mHttp;
		mHttp = 0;
	}
}

void ajaxTransfer::sslErrors(const QList<QSslError>& errors)
{
	QStringList list;
	foreach(QSslError err, errors)
		list << err.errorString();

	QMessageBox::critical(0, tr("AJAX Error"), tr("The AJAX backend has "
		"encountered the following SSL errors:\n%1").arg( list.join("\n") ));
}

ajaxTransfer* ajaxTransfer::start(const QUrl& url,
	const QMap<QString, QString>& post)
{
	ajaxTransfer *transfer = new ajaxTransfer;

	transfer->mHttp = new QHttp(url.host(),
		(url.scheme() == "https") ? QHttp::ConnectionModeHttps :
		QHttp::ConnectionModeHttp, url.port(80), 0);

	connect(transfer->mHttp, SIGNAL(sslErrors(const QList<QSslError>&)),
		transfer, SLOT(sslErrors(const QList<QSslError>&)));

	transfer->mBackendURL = url;

	QHttpRequestHeader header(post.isEmpty() ? "GET" : "POST", url.path());

	QByteArray data;
	if( !post.isEmpty() )
	{
		QStringList post_vars;
		QMapIterator<QString, QString> i(post);
		while( i.hasNext() )
		{
			i.next();
			QString post_var = i.key();
			post_var += "=";
			post_var += QString( QUrl::toPercentEncoding( i.value() ) );

			post_vars << post_var;
		}

		data = post_vars.join("&").toAscii();

		header.setContentType("application/x-www-form-urlencoded; charset=UTF-8");
		header.setContentLength( data.length() );
	}

	header.addValue("User-Agent", "Qt4 JSON");
	header.addValue("Host", url.host());

	connect( transfer->mHttp, SIGNAL(requestFinished(int, bool)),
		transfer, SLOT(requestFinished(int, bool)), Qt::QueuedConnection );

	connect( transfer->mHttp, SIGNAL(readyRead(const QHttpResponseHeader&)),
		transfer, SLOT(readyRead(const QHttpResponseHeader&)) );

	connect( transfer->mHttp,
		SIGNAL(responseHeaderReceived(const QHttpResponseHeader&)),
		transfer, SLOT(responseHeaderReceived(const QHttpResponseHeader&)) );

	transfer->mRequestID = transfer->mHttp->request(header, data);

	return transfer;
}

void ajaxTransfer::requestFinished(int id, bool error)
{
	emit transferInfo(QString::fromUtf8(mContent), mResponse);

	if(mRequestID != id)
		return;

	QString content_error = QString::fromUtf8(mContent);
	if( !content_error.isEmpty() )
	{
		QMessageBox::StandardButton button = QMessageBox::critical(0,
			tr("AJAX Error"), tr("The AJAX backend has encountered one or "
			"more errors. Would you like to view the AJAX Log?"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		if(button == QMessageBox::Yes)
			ajax::getSingletonPtr()->showLog();

		emit transferFailed();
		deleteLater();

		return;
	}

	if(error)
	{
		QMessageBox::StandardButton button = QMessageBox::critical(0,
			tr("AJAX Error"), tr("The AJAX backend has encountered one or "
			"more errors. Would you like to view the AJAX Log?"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		if(button == QMessageBox::Yes)
			ajax::getSingletonPtr()->showLog();

		emit transferFailed();
		deleteLater();

		return;
	}

	foreach(QVariant action, mResponse.toList())
		emit transferFinished(action);
}

void ajaxTransfer::readyRead(const QHttpResponseHeader& resp)
{
	Q_UNUSED(resp);

	mContent += mHttp->readAll();
}

void ajaxTransfer::responseHeaderReceived(const QHttpResponseHeader& resp)
{
	if(resp.statusCode() != 200)
	{
		QMessageBox::critical(0, tr("AJAX Error"),
			tr("Server returned code %1").arg(resp.statusCode()));

		ajax::getSingletonPtr()->showLog();

		emit transferFailed();
		deleteLater();

		return;
	}

	if( !resp.hasKey("X-JSON") )
	{
		QMessageBox::critical(0, tr("AJAX Error"),
			tr("Response missing X-JSON header"));

		ajax::getSingletonPtr()->showLog();

		emit transferFailed();
		deleteLater();

		return;
	}

	QRegExp jsonResponse("\\((.*)\\)");
	QString result = QUrl::fromPercentEncoding( resp.value("X-JSON").toAscii() );

	if( jsonResponse.exactMatch(result) )
		result = jsonResponse.cap(1);

	// TODO: Add error checking here
	mResponse = json::parse(result);

	QVariantList e_res = mResponse.toList();
	for(int i = 0; i < e_res.count(); i++)
	{
		QVariantMap map = e_res.at(i).toMap();
		if( map.contains("error") )
		{
			QMessageBox::StandardButton button = QMessageBox::critical(0,
				tr("AJAX Error"), tr("The AJAX backend has encountered one or "
				"more errors. Would you like to view the AJAX Log?"),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

			if(button == QMessageBox::Yes)
				ajax::getSingletonPtr()->showLog();

			emit transferFailed();
			deleteLater();

			return;
		}
	}
}
