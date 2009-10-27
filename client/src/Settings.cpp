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

#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QCoreApplication>
#include <QtGui/QInputDialog>

Settings *g_settings = 0;

Settings::Settings(QObject *parent_object) : QObject(parent_object)
{
	QCoreApplication::setOrganizationName("Absolutely Frosty Team");
	QCoreApplication::setOrganizationDomain("troopersklan.jp");
	QCoreApplication::setApplicationName("Absolutely Frosty");

	QSettings qsettings;
	if( !qsettings.contains("backend") )
	{
		setUrl( QUrl("https://gigadelic.homelinux.net:55517/backend.php") );
		setOffline(true);
	}
	else
	{
		foreach(QVariant url, qsettings.value("backend_urls").toList())
			mURLs.append( url.toUrl() );
	}

	if( !qsettings.contains("lang") )
		qsettings.setValue("lang", "en");

	mRemindTrayIcon = qsettings.value("remind_tray", true).toBool();
	mTaskbarClosePolicy = qsettings.value("close_taskbar", 0).toInt();

	foreach(QVariant url, qsettings.value("update_urls").toList())
		mUpdateURLs.append( url.toUrl() );

	mOfflineMode = qsettings.value("offline", true).toBool();
	mLang = qsettings.value("lang", "en").toString();
	mEmail = qsettings.value("email").toString();
	mPass = qsettings.value("pass").toString();
	mMode = qsettings.value("mode", "imagine").toString();

	if( mURLs.isEmpty() )
	{
		QUrl url = qsettings.value("backend").toUrl();
		if( url == QUrl("offline") )
		{
			url = QUrl("https://gigadelic.homelinux.net:55517/backend.php");

			setOffline(true);
		}
		else
		{
			setOffline(false);
		}

		setUrl(url);
	}

	if( mUpdateURLs.isEmpty() )
	{
		mUpdateURLs.append( QUrl("http://gigadelic.homelinux.net:10900"
			"/frosty/updates") );
		mUpdateURLs.append( QUrl("http://gigadelic.homelinux.net:10900"
			"/frosty/updates_jp") );

		if( qsettings.contains("update_url") )
			setUpdateUrl( qsettings.value("update_url").toUrl() );
		else
			setUpdateUrl( QUrl(mUpdateURLs.first()) );
	}

	Q_ASSERT(g_settings == 0);
	g_settings = this;

#ifdef Q_OS_WIN32
	QSettings magick("HKEY_LOCAL_MACHINE\\SOFTWARE\\ImageMagick\\6.4.7\\Q:16",
		QSettings::NativeFormat);

	QString appPath = QDir::toNativeSeparators( qApp->applicationDirPath() );

	magick.setValue("BinPath", appPath);
	magick.setValue("ConfigurePath", appPath);
	magick.setValue("LibPath", appPath);
#endif // Q_OS_WIN32
}

Settings* Settings::getSingletonPtr()
{
	if(!g_settings)
		g_settings = new Settings;

	return g_settings;
}

QUrl Settings::url() const
{
	if( mURLs.isEmpty() )
		return QUrl();

	return mURLs.first();
}

QList<QUrl> Settings::urls() const
{
	return mURLs;
}

QString Settings::lang() const
{
	return mLang;
}

QString Settings::email() const
{
	return mEmail;
}

QString Settings::pass() const
{
	return mPass;
}

QString Settings::mode() const
{
	return mMode.toLower();
}

QUrl Settings::updateUrl() const
{
	if( mUpdateURLs.isEmpty() )
		return QUrl();

	return mUpdateURLs.first();
}

QList<QUrl> Settings::updateUrls() const
{
	return mUpdateURLs;
}

bool Settings::canDelete() const
{
	QStringList args = QCoreApplication::instance()->arguments();

	return (args.contains("-d") || args.contains("--delete"));
}

bool Settings::remindTrayIcon() const
{
	return mRemindTrayIcon;
}

int Settings::taskbarClosePolicy() const
{
	return mTaskbarClosePolicy;
}

bool Settings::offline() const
{
	return mOfflineMode;
}

void Settings::setUrl(const QUrl& u)
{
	if( mURLs.contains(u) )
		mURLs.removeAt( mURLs.indexOf(u) );

	mURLs.prepend(u);

	while(mURLs.count() > 5)
		mURLs.removeLast();

	QSettings qsettings;
	qsettings.setValue("backend", u);

	{
		QVariantList urls;
		foreach(QUrl url, mURLs)
			urls.append(url);

		qsettings.setValue("backend_urls", urls);
	}
}

void Settings::setMode(const QString& m)
{
	mMode = m.toLower();

	QSettings qsettings;
	qsettings.setValue("mode", mMode);
}

void Settings::setLang(const QString& l)
{
	mLang = l;

	QSettings qsettings;
	qsettings.setValue("lang", l);
}

void Settings::setEmail(const QString& e)
{
	mEmail = e;

	QSettings qsettings;
	qsettings.setValue("email", e);
}

void Settings::setPass(const QString& p)
{
	mPass = p;

	QSettings qsettings;
	qsettings.setValue("pass", p);
}

void Settings::setUpdateUrl(const QUrl& u)
{
	if( mUpdateURLs.contains(u) )
		mUpdateURLs.removeAt( mUpdateURLs.indexOf(u) );

	mUpdateURLs.prepend(u);

	while(mUpdateURLs.count() > 5)
		mUpdateURLs.removeLast();

	QSettings qsettings;
	qsettings.setValue("update_url", u);

	{
		QVariantList urls;
		foreach(QUrl url, mUpdateURLs)
			urls.append(url);

		qsettings.setValue("update_urls", urls);
	}
}

void Settings::setRemindTrayIcon(bool remind)
{
	mRemindTrayIcon = remind;

	QSettings qsettings;
	qsettings.setValue("remind_tray", remind);
}

void Settings::setTaskbarClosePolicy(int policy)
{
	mTaskbarClosePolicy = policy;

	QSettings qsettings;
	qsettings.setValue("close_taskbar", policy);
}

void Settings::setOffline(bool is_offline)
{
	mOfflineMode = is_offline;

	QSettings qsettings;
	qsettings.setValue("offline", is_offline);
}
