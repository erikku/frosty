/******************************************************************************\
*  client/src/Settings.h                                                       *
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

#ifndef __Settings_h__
#define __Settings_h__

#include <QtCore/QUrl>
#include <QtCore/QObject>
#include <QtCore/QString>

class Settings : public QObject
{
	Q_OBJECT

public:
	Settings(QObject *parent = 0);

	QUrl url() const;
	QString lang() const;

	QString email() const;
	QString pass() const;

	bool canDelete() const;

	static Settings* getSingletonPtr();

public slots:
	void setUrl(const QUrl& url);
	void setLang(const QString& lang);

	void setEmail(const QString& email);
	void setPass(const QString& pass);

protected:
	QUrl mURL;
	QString mLang, mEmail, mPass;
};

#define settings ( Settings::getSingletonPtr() )

#endif // __Settings_h__
