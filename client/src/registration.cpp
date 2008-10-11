/******************************************************************************\
*  client/src/registration.cpp                                                 *
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

#include "registration.h"

#include <QtGui/QMessageBox>
#include <QtNetwork/QHttp>
#include <iostream>
registration::registration(QObject *parent) : QObject(parent)
{
};

void registration::loadValidationImage(const QUrl& url)
{
	QHttpRequestHeader header("GET", url.path());

	header.addValue("User-Agent", "Qt4 JSON");
	header.addValue("Host", url.host());

	QHttp *http = new QHttp(url.host(),
		(url.scheme() == "https") ? QHttp::ConnectionModeHttps :
		QHttp::ConnectionModeHttp, url.port(80), 0);

	connect( http, SIGNAL(sslErrors(const QList<QSslError>&)),
		this, SLOT(sslErrors(const QList<QSslError>&)) );
	connect( http, SIGNAL(readyRead(const QHttpResponseHeader&)),
		this, SLOT(readyRead(const QHttpResponseHeader&)) );

	http->request(header);
};

QString registration::salt() const
{
	return mSalt;
};

void registration::registerUser(const QUrl& url, const QString& email, const QString& name,
	const QString& pass, const QString& code)
{
	QString p_email = QString( QUrl::toPercentEncoding(email) );
	QString p_name = QString( QUrl::toPercentEncoding(name) );
	QString p_pass = QString( QUrl::toPercentEncoding(pass) );
	QString p_code = QString( QUrl::toPercentEncoding(code) );

	QByteArray data = QString( QString("email=") + p_email +
		QString("&code=") + p_code + QString("&name=") + p_name +
		QString("&pass=") + p_pass).toAscii();

	QHttpRequestHeader header("POST", url.path());

	header.addValue("User-Agent", "Qt4 JSON");
	header.addValue("Host", url.host());
	header.addValue("Cookie", mCookie);

	header.setContentType("application/x-www-form-urlencoded; charset=UTF-8");
	header.setContentLength( data.length() );

	QHttp *http = new QHttp(url.host(),
		(url.scheme() == "https") ? QHttp::ConnectionModeHttps :
		QHttp::ConnectionModeHttp, url.port(80), 0);

	connect( http, SIGNAL(sslErrors(const QList<QSslError>&)),
		this, SLOT(sslErrors(const QList<QSslError>&)) );
	connect( http, SIGNAL(readyRead(const QHttpResponseHeader&)),
		this, SLOT(readyRead(const QHttpResponseHeader&)) );

	http->request(header, data);
};

void registration::readyRead(const QHttpResponseHeader& resp)
{
	QHttp *http = qobject_cast<QHttp*>( sender() );
	if(!http)
		return;

	if(resp.statusCode() != 200)
	{
		QMessageBox::critical(0, tr("Registration Error"),
			tr("Server returned code %1").arg(resp.statusCode()));

		http->deleteLater();

		return;
	}

	if(resp.contentType() == "image/png")
	{
		mCookie = resp.value("set-cookie");
		mSalt = resp.value("x-registration-salt");

		QPixmap image;
		image.loadFromData( http->readAll() );

		emit validationImage(image, resp.value("x-registration-error"));
	}
	else if(resp.contentType() == "text/html")
	{
		std::cout << http->readAll().data() << std::endl;
		emit registrationComplete();
	}

	http->deleteLater();
};

void registration::sslErrors(const QList<QSslError>& errors)
{
	QStringList list;
	foreach(QSslError err, errors)
		list << err.errorString();

	QMessageBox::critical(0, tr("AJAX Error"), tr("The AJAX backend has "
		"encountered the following SSL errors:\n%1").arg( list.join("\n") ));
};
