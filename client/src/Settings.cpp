/******************************************************************************\
*  client/src/Settings.cpp                                                     *
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

#include "Settings.h"

#include <QtCore/QSettings>
#include <QtCore/QCoreApplication>
#include <QtGui/QInputDialog>

Settings *g_settings = 0;

Settings::Settings(QObject *parent) : QObject(parent)
{
	QCoreApplication::setOrganizationName("MegatenDB");
	QCoreApplication::setOrganizationDomain("troopersklan.jp");
	QCoreApplication::setApplicationName("Megaten DB");

	QSettings qsettings;
	if( !qsettings.contains("backend") )
	{
		// http://www.troopersklan.jp/megaten/backend.php
		QString text = QInputDialog::getText(0, tr("MegaTen Database Location"),
			tr("URL:"), QLineEdit::Normal, "https://gigadelic.homelinux.net:55517/backend.php");

		if( !text.isEmpty() )
			mURL = QUrl(text);

		qsettings.setValue("backend", mURL);
	}
	else
	{
		mURL = qsettings.value("backend").toUrl();
	}

	if( !qsettings.contains("lang") )
		qsettings.setValue("lang", "ja");

	mUpdateURL = qsettings.value("update_url").toUrl();
	mLang = qsettings.value("lang", "ja").toString();
	mEmail = qsettings.value("email").toString();
	mPass = qsettings.value("pass").toString();

	Q_ASSERT(g_settings == 0);
	g_settings = this;
};

Settings* Settings::getSingletonPtr()
{
	if(!g_settings)
		g_settings = new Settings;

	return g_settings;
};

QUrl Settings::url() const
{
	return mURL;
};

QString Settings::lang() const
{
	return mLang;
};

QString Settings::email() const
{
	return mEmail;
};

QString Settings::pass() const
{
	return mPass;
};

QUrl Settings::updateUrl() const
{
	return mUpdateURL;
};

bool Settings::canDelete() const
{
	QStringList args = QCoreApplication::instance()->arguments();

	return (args.contains("-d") || args.contains("--delete"));
};

void Settings::setUrl(const QUrl& url)
{
	mURL = url;

	QSettings qsettings;
	qsettings.setValue("backend", mURL);
};

void Settings::setLang(const QString& lang)
{
	mLang = lang;

	QSettings qsettings;
	qsettings.setValue("lang", mLang);
};

void Settings::setEmail(const QString& email)
{
	mEmail = email;

	QSettings qsettings;
	qsettings.setValue("email", mEmail);
};

void Settings::setPass(const QString& pass)
{
	mPass = pass;

	QSettings qsettings;
	qsettings.setValue("pass", mPass);
};

void Settings::setUpdateUrl(const QUrl& url)
{
	mUpdateURL = url;
};
