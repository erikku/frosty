/******************************************************************************\
*  server/src/Config.cpp                                                       *
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

#include "Config.h"
#include "DomUtils.h"
#include "Log.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QFileSystemWatcher>
#include <QtXml/QDomDocument>

static Config *g_config_inst = 0;

Config::Config(QObject *parent) : QObject(parent)
{
	mWatcher = new QFileSystemWatcher;

	connect(mWatcher, SIGNAL(fileChanged(const QString&)),
		this, SLOT(loadConfig(const QString&)));

	loadDefaults();
};

Config* Config::getSingletonPtr()
{
	if(!g_config_inst)
		g_config_inst = new Config;

	Q_ASSERT(g_config_inst);

	return g_config_inst;
};

void Config::loadDefaults()
{
	mAddress = "localhost";
	mPort = 8080;

	mLogPath = "megatendb.log";

	mLogCritical = true;
	mLogError = true;
	mLogWarning = true;
	mLogInfo = true;
	mLogDebug = true;

	mAuthAdmin = false;
	mAuthViewDB = true;
	mAuthModifyDB = false;
	mAuthAdminDB = false;

	mDBType = "sqlite";
	mDBPath = "master.db";

	mSslEnabled = false;
};

void Config::loadConfig(const QString& path)
{
	if( mWatcher->files().isEmpty() )
	{
		mWatcher->addPath(path);
	}
	else if(mWatcher->files().first() != path)
	{
		mWatcher->removePath( mWatcher->files().first() );
		mWatcher->addPath(path);
	}

	QFileInfo info(path);

	if( !info.exists() )
	{
		LOG_ERROR( tr("Config file '%1' does not exist.\n").arg(path) );

		return;
	}

	if( !info.isReadable() )
	{
		LOG_ERROR( tr("Can not read config file '%1'.\n").arg(path) );

		return;
	}

	QFile configFile(path);
	if( !configFile.open(QIODevice::ReadOnly) )
	{
		LOG_ERROR( tr("Failed to open config file '%1'.\n").arg(path) );

		return;
	}

	QString errorMsg;
	int errorLine, errorColumn;

	QDomDocument doc;
	if( !doc.setContent(&configFile, false, &errorMsg, &errorLine,
		&errorColumn) )
	{
		LOG_ERROR( tr("Failed to parse config file:\n%1:%2: %3\n").arg(
			path).arg(errorLine).arg(errorMsg) );

		configFile.close();

		return;
	}

	QList<QDomNode> nodes;

	// <log><path>
	nodes = elementsByXPath(doc, "/log/path");
	if( nodes.count() && nodes.first().isElement() )
		mLogPath = nodes.first().toElement().text().trimmed();
	else
		LOG_WARNING( tr("Failed to find log path in config file\n") );

	// <log><levels><critical>
	nodes = elementsByXPath(doc, "/log/levels/critical");
	if( nodes.isEmpty() )
		LOG_WARNING( tr("Error parsing <critical>, default will be used\n") );
	else
		mLogCritical = nodeToBool(nodes.first(), mLogCritical);

	// <log><levels><error>
	nodes = elementsByXPath(doc, "/log/levels/error");
	if( nodes.isEmpty() )
		LOG_WARNING( tr("Error parsing <error>, default will be used\n") );
	else
		mLogError = nodeToBool(nodes.first(), mLogInfo);

	// <log><levels><warning>
	nodes = elementsByXPath(doc, "/log/levels/warning");
	if( nodes.isEmpty() )
		LOG_WARNING( tr("Error parsing <warning>, default will be used\n") );
	else
		mLogWarning = nodeToBool(nodes.first(), mLogWarning);

	// <log><levels><info>
	nodes = elementsByXPath(doc, "/log/levels/info");
	if( nodes.isEmpty() )
		LOG_WARNING( tr("Error parsing <info>, default will be used\n") );
	else
		mLogInfo = nodeToBool(nodes.first(), mLogInfo);

	// <log><levels><debug>
	nodes = elementsByXPath(doc, "/log/levels/debug");
	if( nodes.isEmpty() )
		LOG_WARNING( tr("Error parsing <debug>, default will be used\n") );
	else
		mLogDebug = nodeToBool(nodes.first(), mLogDebug);

	Log::getSingletonPtr()->loadConfig();

	// <connection><address>
	nodes = elementsByXPath(doc, "/connection/address");
	if( nodes.count() && nodes.first().isElement() )
		mAddress = nodes.first().toElement().text().trimmed().toLower();
	else
		LOG_WARNING( tr("Failed to find address in config file\n") );

	// <connection><port>
	nodes = elementsByXPath(doc, "/connection/port");
	if( nodes.count() && nodes.first().isElement() )
		mPort = nodes.first().toElement().text().trimmed().toInt();
	else
		LOG_WARNING( tr("Failed to find port in config file\n") );

	// <auth><admin>
	nodes = elementsByXPath(doc, "/auth/admin");
	if( nodes.count() && nodes.first().isElement() )
		mAuthAdminUser = nodes.first().toElement().text().trimmed();
	else
		LOG_WARNING( tr("Failed to find auth admin in config file\n") );

	// <auth><defaults><admin>
	nodes = elementsByXPath(doc, "/auth/defaults/admin");
	if( nodes.isEmpty() )
		LOG_WARNING( tr("Error parsing <admin>, default will be used\n") );
	else
		mAuthAdmin = nodeToBool(nodes.first(), mAuthAdmin);

	// <auth><defaults><view_db>
	nodes = elementsByXPath(doc, "/auth/defaults/view_db");
	if( nodes.isEmpty() )
		LOG_WARNING( tr("Error parsing <view_db>, default will be used\n") );
	else
		mAuthViewDB = nodeToBool(nodes.first(), mAuthViewDB);

	// <auth><defaults><modify_db>
	nodes = elementsByXPath(doc, "/auth/defaults/modify_db");
	if( nodes.isEmpty() )
		LOG_WARNING( tr("Error parsing <modify_db>, default will be used\n") );
	else
		mAuthModifyDB = nodeToBool(nodes.first(), mAuthModifyDB);

	// <auth><defaults><admin_db>
	nodes = elementsByXPath(doc, "/auth/defaults/admin_db");
	if( nodes.isEmpty() )
		LOG_WARNING( tr("Error parsing <admin_db>, default will be used\n") );
	else
		mAuthAdminDB = nodeToBool(nodes.first(), mAuthAdminDB);

	// <database><type>
	nodes = elementsByXPath(doc, "/database/type");
	if( nodes.count() && nodes.first().isElement() )
		mDBType = nodes.first().toElement().text().trimmed().toLower();
	else
		LOG_WARNING( tr("Failed to find database type in config file\n") );

	// <database><path>
	nodes = elementsByXPath(doc, "/database/path");
	if( nodes.count() && nodes.first().isElement() )
		mDBPath = nodes.first().toElement().text().trimmed();
	else if(mDBType == "sqlite")
		LOG_WARNING( tr("Failed to find database path in config file\n") );

	// <database><host>
	nodes = elementsByXPath(doc, "/database/host");
	if( nodes.count() && nodes.first().isElement() )
		mDBPath = nodes.first().toElement().text().trimmed();
	else if(mDBType == "mysql")
		LOG_WARNING( tr("Failed to find database host in config file\n") );

	// <database><user>
	nodes = elementsByXPath(doc, "/database/user");
	if( nodes.count() && nodes.first().isElement() )
		mDBUser = nodes.first().toElement().text().trimmed();
	else if(mDBType == "mysql")
		LOG_WARNING( tr("Failed to find database user in config file\n") );

	// <database><pass>
	nodes = elementsByXPath(doc, "/database/pass");
	if( nodes.count() && nodes.first().isElement() )
		mDBPass = nodes.first().toElement().text().trimmed();
	else if(mDBType == "mysql")
		LOG_WARNING( tr("Failed to find database password in config file\n") );

	// <database><db>
	nodes = elementsByXPath(doc, "/database/db");
	if( nodes.count() && nodes.first().isElement() )
		mDBName = nodes.first().toElement().text().trimmed();
	else if(mDBType == "mysql")
		LOG_WARNING( tr("Failed to find database name in config file\n") );

	// <auth><database><type>
	nodes = elementsByXPath(doc, "/auth/database/type");
	if( nodes.count() && nodes.first().isElement() )
		mAuthDBType = nodes.first().toElement().text().trimmed().toLower();
	else
		LOG_WARNING( tr("Failed to find auth database type in config file\n") );

	// <auth><database><path>
	nodes = elementsByXPath(doc, "/auth/database/path");
	if( nodes.count() && nodes.first().isElement() )
		mAuthDBPath = nodes.first().toElement().text().trimmed();
	else if(mAuthDBType == "sqlite")
		LOG_WARNING( tr("Failed to find auth database path in config file\n") );

	// <auth><database><host>
	nodes = elementsByXPath(doc, "/auth/database/host");
	if( nodes.count() && nodes.first().isElement() )
		mAuthDBPath = nodes.first().toElement().text().trimmed();
	else if(mAuthDBType == "mysql")
		LOG_WARNING( tr("Failed to find auth database host in config file\n") );

	// <auth><database><user>
	nodes = elementsByXPath(doc, "/auth/database/user");
	if( nodes.count() && nodes.first().isElement() )
		mAuthDBUser = nodes.first().toElement().text().trimmed();
	else if(mAuthDBType == "mysql")
		LOG_WARNING( tr("Failed to find auth database user in config file\n") );

	// <auth><database><pass>
	nodes = elementsByXPath(doc, "/auth/database/pass");
	if( nodes.count() && nodes.first().isElement() )
		mAuthDBPass = nodes.first().toElement().text().trimmed();
	else if(mAuthDBType == "mysql")
		LOG_WARNING( tr("Failed to find auth database pass in config file\n") );

	// <auth><database><db>
	nodes = elementsByXPath(doc, "/auth/database/db");
	if( nodes.count() && nodes.first().isElement() )
		mAuthDBName = nodes.first().toElement().text().trimmed();
	else if(mAuthDBType == "mysql")
		LOG_WARNING( tr("Failed to find auth database name in config file\n") );

	// <salts><pass>
	nodes = elementsByXPath(doc, "/salts/pass");
	if( nodes.count() && nodes.first().isElement() )
		mSaltPass = nodes.first().toElement().text().trimmed();
	else if(mDBType == "sqlite")
		LOG_WARNING( tr("Failed to find password salt in config file\n") );

	// <salts><img>
	nodes = elementsByXPath(doc, "/salts/img");
	if( nodes.count() && nodes.first().isElement() )
		mSaltImg = nodes.first().toElement().text().trimmed();
	else if(mDBType == "sqlite")
		LOG_WARNING( tr("Failed to find image salt in config file\n") );

	// <signature>
	nodes = elementsByXPath(doc, "/signature");
	if( nodes.count() && nodes.first().isElement() )
		mSignature = nodes.first().toElement().text().trimmed();
	else if(mDBType == "sqlite")
		LOG_WARNING( tr("Failed to find server signature in config file\n") );

	// <captcha><letters>
	nodes = elementsByXPath(doc, "/captcha/letters");
	if( nodes.count() && nodes.first().isElement() )
		mCaptchaLetters = nodes.first().toElement().text().trimmed().split(",");
	else if(mDBType == "sqlite")
		LOG_WARNING( tr("Failed to find captcha letters in config file\n") );

	// <captcha><font>
	nodes = elementsByXPath(doc, "/captcha/font");
	if( nodes.count() && nodes.first().isElement() )
		mCaptchaFont = nodes.first().toElement().text().trimmed();
	else if(mDBType == "sqlite")
		LOG_WARNING( tr("Failed to find captcha font in config file\n") );

	// <ssl><enabled>
	nodes = elementsByXPath(doc, "/ssl/enabled");
	if( nodes.isEmpty() )
		LOG_WARNING( tr("Error parsing <enabled>, default will be used\n") );
	else
		mSslEnabled = nodeToBool(nodes.first(), mSslEnabled);

	// <ssl><cert>
	nodes = elementsByXPath(doc, "/ssl/cert");
	if( nodes.count() && nodes.first().isElement() )
	{
		mCertPath = nodes.first().toElement().text().trimmed();

		QFile cert_file(mCertPath);
	    cert_file.open(QIODevice::ReadOnly);

		QSslCertificate cert(&cert_file);
		if( cert.isNull() )
			LOG_ERROR("Error loading SSL certificate!\n");

		mCert = cert;
	}
	else
		LOG_WARNING( tr("Failed to find SSL cert path in config file\n") );

	// <ssl><key>
	nodes = elementsByXPath(doc, "/ssl/key");
	if( nodes.count() && nodes.first().isElement() )
	{
		mKeyPath = nodes.first().toElement().text().trimmed();

		QFile key_file(mKeyPath);
	    key_file.open(QIODevice::ReadOnly);

		QSslKey key(&key_file, QSsl::Rsa);
		if( key.isNull() )
			LOG_ERROR("Error loading SSL key!\n");

		mKey = key;
	}
	else
		LOG_WARNING( tr("Failed to find SSL key path in config file\n") );

	LOG_INFO( tr("Config file loaded\n") );
};

bool Config::nodeToBool(const QDomNode& node, bool def)
{
	QString val = node.toElement().text().trimmed();

	if(val.toLower() == "true" || val == "1")
		return true;
	else if(val.toLower() == "false" || val == "0")
		return false;
	else
		LOG_WARNING( tr("Error parsing <%1>, default will be used\n").arg(
			node.nodeName()) );

	return def;
};

void Config::saveConfig(const QString& path)
{
	QFile configFile(path);
	if( !configFile.open(QIODevice::ReadOnly) )
	{
		LOG_ERROR( tr("Failed to open config file '%1'.\n").arg(path) );

		return;
	}

	QString errorMsg;
	int errorLine, errorColumn;

	QDomDocument doc;
	if( !doc.setContent(&configFile, false, &errorMsg, &errorLine,
		&errorColumn) )
	{
		LOG_ERROR( tr("Failed to parse config file:\n%1:%2: %3\n").arg(
			path).arg(errorLine).arg(errorMsg) );

		configFile.close();

		return;
	}

	configFile.close();

	writeXPath(doc, "/connection/address", mAddress);
	writeXPath(doc, "/connection/port", QString::number(mPort));

	writeXPath(doc, "/database/type", mDBType);
	writeXPath(doc, "/database/path", mDBPath);
	writeXPath(doc, "/database/host", mDBHost);
	writeXPath(doc, "/database/user", mDBUser);
	writeXPath(doc, "/database/pass", mDBPass);
	writeXPath(doc, "/database/name", mDBName);

	writeXPath(doc, "/log/levels/debug", mLogDebug ? "true" : "false");
	writeXPath(doc, "/log/levels/info", mLogInfo ? "true" : "false");
	writeXPath(doc, "/log/levels/warning", mLogWarning ? "true" : "false");
	writeXPath(doc, "/log/levels/error", mLogError ? "true" : "false");
	writeXPath(doc, "/log/levels/critical", mLogCritical ? "true" : "false");
	writeXPath(doc, "/log/path", mLogPath);

	writeXPath(doc, "/signature", mSignature);

	writeXPath(doc, "/salt/img", mSaltImg);
	writeXPath(doc, "/salt/pass", mSaltPass);

	writeXPath(doc, "/captcha/letters", mCaptchaLetters.join(","));
	writeXPath(doc, "/captcha/font", mCaptchaFont);

	writeXPath(doc, "/auth/admin", mAuthAdminUser);
	writeXPath(doc, "/auth/defaults/admin", mAuthAdmin ? "true" : "false");
	writeXPath(doc, "/auth/defaults/view_db", mAuthViewDB ? "true" : "false");
	writeXPath(doc, "/auth/defaults/modify_db",
		mAuthModifyDB ? "true" : "false");
	writeXPath(doc, "/auth/defaults/admin_db", mAuthAdminDB ? "true" : "false");
	writeXPath(doc, "/auth/database/type", mAuthDBType);
	writeXPath(doc, "/auth/database/path", mAuthDBPath);
	writeXPath(doc, "/auth/database/host", mAuthDBHost);
	writeXPath(doc, "/auth/database/user", mAuthDBUser);
	writeXPath(doc, "/auth/database/pass", mAuthDBPass);
	writeXPath(doc, "/auth/database/name", mAuthDBName);

	writeXPath(doc, "/ssl/enabled", mSslEnabled ? "true" : "false");
	writeXPath(doc, "/ssl/cert", mCertPath);
	writeXPath(doc, "/ssl/key", mKeyPath);

	if( !configFile.open(QIODevice::WriteOnly | QIODevice::Truncate) )
	{
		LOG_ERROR( tr("Failed to open '%1' for writing.\n").arg(path) );

		return;
	}

	configFile.write( doc.toString(4).toUtf8() );
};

QString Config::address() const
{
	return mAddress;
};

void Config::setAddress(const QString& address)
{
	mAddress = address;
};

int Config::port() const
{
	return mPort;
};

void Config::setPort(int port)
{
	mPort = port;
};

QString Config::logPath() const
{
	return mLogPath;
};

void Config::setLogPath(const QString& path)
{
	mLogPath = path;
};

QString Config::authAdminUser() const
{
	return mAuthAdminUser;
};

void Config::setAuthAdminUser(const QString& user)
{
	mAuthAdminUser = user;
};

bool Config::logCritical() const
{
	return mLogCritical;
};

void Config::setLogCritical(bool log)
{
	mLogCritical = log;
};

bool Config::logError() const
{
	return mLogError;
};

void Config::setLogError(bool log)
{
	mLogError = log;
};

bool Config::logWarning() const
{
	return mLogWarning;
};

void Config::setLogWarning(bool log)
{
	mLogWarning = log;
};

bool Config::logInfo() const
{
	return mLogInfo;
};

void Config::setLogInfo(bool log)
{
	mLogInfo = log;
};

bool Config::logDebug() const
{
	return mLogDebug;
};

void Config::setLogDebug(bool log)
{
	mLogDebug = log;
};

bool Config::authAdmin() const
{
	return mAuthAdmin;
};

void Config::setAuthAdmin(bool ok)
{
	mAuthAdmin = ok;
};

bool Config::authViewDB() const
{
	return mAuthViewDB;
};

void Config::setAuthViewDB(bool ok)
{
	mAuthViewDB = ok;
};

bool Config::authModifyDB() const
{
	return mAuthModifyDB;
};

void Config::setAuthModifyDB(bool ok)
{
	mAuthModifyDB = ok;
};

bool Config::authAdminDB() const
{
	return mAuthAdminDB;
};

void Config::setAuthAdminDB(bool ok)
{
	mAuthAdminDB = ok;
};

QString Config::dbType() const
{
	return mDBType;
};

void Config::setDBType(const QString& type)
{
	mDBType = type;
};

QString Config::dbPath() const
{
	return mDBPath;
};

void Config::setDBPath(const QString& path)
{
	mDBPath = path;
};

QString Config::dbHost() const
{
	return mDBHost;
};

void Config::setDBHost(const QString& host)
{
	mDBHost = host;
};

QString Config::dbUser() const
{
	return mDBUser;
};

void Config::setDBUser(const QString& user)
{
	mDBUser = user;
};

QString Config::dbPass() const
{
	return mDBPass;
};

void Config::setDBPass(const QString& pass)
{
	mDBPass = pass;
};

QString Config::dbName() const
{
	return mDBName;
};

void Config::setDBName(const QString& name)
{
	mDBName = name;
};

QString Config::authDBType() const
{
	return mAuthDBType;
};

void Config::setAuthDBType(const QString& type)
{
	mAuthDBType = type;
};

QString Config::authDBPath() const
{
	return mAuthDBPath;
};

void Config::setAuthDBPath(const QString& path)
{
	mAuthDBPath = path;
};

QString Config::authDBHost() const
{
	return mAuthDBHost;
};

void Config::setAuthDBHost(const QString& host)
{
	mAuthDBHost = host;
};

QString Config::authDBUser() const
{
	return mAuthDBUser;
};

void Config::setAuthDBUser(const QString& user)
{
	mAuthDBUser = user;
};

QString Config::authDBPass() const
{
	return mAuthDBPass;
};

void Config::setAuthDBPass(const QString& pass)
{
	mAuthDBPass = pass;
};

QString Config::authDBName() const
{
	return mAuthDBName;
};

void Config::setAuthDBName(const QString& name)
{
	mAuthDBName = name;
};

QString Config::saltPass() const
{
	return mSaltPass;
};

void Config::setSaltPass(const QString& salt)
{
	mSaltPass = salt;
};

QString Config::saltImg() const
{
	return mSaltImg;
};

void Config::setSaltImg(const QString& salt)
{
	mSaltImg = salt;
};

QString Config::signature() const
{
	return mSignature;
};

void Config::setSignature(const QString& sig)
{
	mSignature = sig;
};

QStringList Config::captchaLetters() const
{
	return mCaptchaLetters;
};

void Config::setCaptchaLetters(const QStringList& letters)
{
	mCaptchaLetters = letters;
};

QString Config::captchaFont() const
{
	return mCaptchaFont;
};

void Config::setCaptchaFont(const QString& font)
{
	mCaptchaFont = font;
};

bool Config::sslEnabled() const
{
	return mSslEnabled;
};

void Config::setSslEnabled(bool enabled)
{
	mSslEnabled = enabled;
};

QSslCertificate Config::sslCert() const
{
	return mCert;
};

void Config::setSslCert(const QSslCertificate& cert)
{
	mCert = cert;
};

QSslKey Config::sslKey() const
{
	return mKey;
};

void Config::setSslKey(const QSslKey& key)
{
	mKey = key;
};
