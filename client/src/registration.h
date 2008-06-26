/******************************************************************************\
*  client/src/registration.h                                                   *
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

#ifndef __registration_h__
#define __registration_h__

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QUrl>

#include <QtGui/QPixmap>

#include <QtNetwork/QHttpResponseHeader>

class registration : public QObject
{
	Q_OBJECT

public:
	registration(QObject *parent = 0);

	QString salt() const;

public slots:
	void loadValidationImage(const QUrl& url);
	void registerUser(const QUrl& url, const QString& email, const QString& name,
		const QString& pass, const QString& code);

signals:
	void validationImage(const QPixmap& image, const QString& error);
	void registrationComplete();

protected slots:
	void readyRead(const QHttpResponseHeader& resp);

protected:
	QString mCookie, mSalt;
};

#endif // __registration_h__
