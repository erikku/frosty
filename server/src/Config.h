/******************************************************************************\
*  server/src/Config.h                                                         *
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

#ifndef __Config_h__
#define __Config_h__

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtXml/QDomNode>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>

class QFileSystemWatcher;

class Config : public QObject
{
	Q_OBJECT

public:
	static Config* getSingletonPtr();

public slots:
	void loadDefaults();
	void loadConfig(const QString& path);
	void saveConfig(const QString& path);

public:
	QString address() const;
	void setAddress(const QString& address);

	Q_PROPERTY(QString mAddress READ address WRITE setAddress)

	int port() const;
	void setPort(int port);

	Q_PROPERTY(int mPort READ port WRITE setPort)

	QString logPath() const;
	void setLogPath(const QString& path);

	Q_PROPERTY(QString mLogPath READ logPath WRITE setLogPath)

	QString authAdminUser() const;
	void setAuthAdminUser(const QString& user);

	Q_PROPERTY(QString mAuthAdminUser READ authAdminUser WRITE setAuthAdminUser)

	bool logCritical() const;
	void setLogCritical(bool log);

	Q_PROPERTY(bool mLogCritical READ logCritical WRITE setLogCritical)

	bool logError() const;
	void setLogError(bool log);

	Q_PROPERTY(bool mLogError READ logError WRITE setLogError)

	bool logWarning() const;
	void setLogWarning(bool log);

	Q_PROPERTY(bool mLogWarning READ logWarning WRITE setLogWarning)

	bool logInfo() const;
	void setLogInfo(bool log);

	Q_PROPERTY(bool mLogInfo READ logInfo WRITE setLogInfo)

	bool logDebug() const;
	void setLogDebug(bool log);

	Q_PROPERTY(bool mLogDebug READ logDebug WRITE setLogDebug)

	bool authAdmin() const;
	void setAuthAdmin(bool ok);

	Q_PROPERTY(bool mAuthAdmin READ authAdmin WRITE setAuthAdmin)

	bool authViewDB() const;
	void setAuthViewDB(bool ok);

	Q_PROPERTY(bool mAuthViewDB READ authViewDB WRITE setAuthViewDB)

	bool authModifyDB() const;
	void setAuthModifyDB(bool ok);

	Q_PROPERTY(bool mAuthModifyDB READ authModifyDB WRITE setAuthModifyDB)

	bool authAdminDB() const;
	void setAuthAdminDB(bool ok);

	Q_PROPERTY(bool mAuthAdminDB READ authAdminDB WRITE setAuthAdminDB)

	QString dbType() const;
	void setDBType(const QString& type);

	Q_PROPERTY(QString mDBType READ dbType WRITE setDBType)

	QString dbPath() const;
	void setDBPath(const QString& path);

	Q_PROPERTY(QString mDBPath READ dbPath WRITE setDBPath)

	QString dbHost() const;
	void setDBHost(const QString& host);

	Q_PROPERTY(QString mDBHost READ dbHost WRITE setDBHost)

	QString dbUser() const;
	void setDBUser(const QString& user);

	Q_PROPERTY(QString mDBUser READ dbUser WRITE setDBUser)

	QString dbPass() const;
	void setDBPass(const QString& pass);

	Q_PROPERTY(QString mDBPass READ dbPass WRITE setDBPass)

	QString dbName() const;
	void setDBName(const QString& name);

	Q_PROPERTY(QString mDBName READ dbName WRITE setDBName)

	QString authDBType() const;
	void setAuthDBType(const QString& type);

	Q_PROPERTY(QString mAuthType READ authDBType WRITE setAuthDBType)

	QString authDBPath() const;
	void setAuthDBPath(const QString& path);

	Q_PROPERTY(QString mAuthPath READ authDBPath WRITE setAuthDBPath)

	QString authDBHost() const;
	void setAuthDBHost(const QString& host);

	Q_PROPERTY(QString mAuthHost READ authDBHost WRITE setAuthDBHost)

	QString authDBUser() const;
	void setAuthDBUser(const QString& user);

	Q_PROPERTY(QString mAuthUser READ authDBUser WRITE setAuthDBUser)

	QString authDBPass() const;
	void setAuthDBPass(const QString& pass);

	Q_PROPERTY(QString mAuthPass READ authDBPass WRITE setAuthDBPass)

	QString authDBName() const;
	void setAuthDBName(const QString& name);

	Q_PROPERTY(QString mAuthName READ authDBName WRITE setAuthDBName)

	QString saltPass() const;
	void setSaltPass(const QString& salt);

	Q_PROPERTY(QString mSaltPass READ saltPass WRITE setSaltPass)

	QString saltImg() const;
	void setSaltImg(const QString& salt);

	Q_PROPERTY(QString mSaltImg READ saltImg WRITE setSaltImg)

	QString signature() const;
	void setSignature(const QString& sig);

	Q_PROPERTY(QString mSignature READ signature WRITE setSignature)

	QStringList captchaLetters() const;
	void setCaptchaLetters(const QStringList& letters);

	Q_PROPERTY(QStringList mCaptchaLetters READ captchaLetters
		WRITE setCaptchaLetters)

	QString captchaFont() const;
	void setCaptchaFont(const QString& font);

	Q_PROPERTY(QString mCaptchaFont READ captchaFont WRITE setCaptchaFont)

	bool sslEnabled() const;
	void setSslEnabled(bool enabled);

	Q_PROPERTY(bool mSslEnabled READ sslEnabled WRITE setSslEnabled)

	QSslCertificate sslCert() const;
	void setSslCert(const QSslCertificate& cert);

	Q_PROPERTY(QSslCertificate mCert READ sslCert WRITE setSslCert)

	QSslKey sslKey() const;
	void setSslKey(const QSslKey& key);

	Q_PROPERTY(QSslKey mKey READ sslKey WRITE setSslKey)

	QString clientWin32() const;
	void setClientWin32(const QString& sha1);

	Q_PROPERTY(QString mClientWin32 READ clientWin32 WRITE setClientWin32)

	QString clientMacOSX() const;
	void setClientMacOSX(const QString& sha1);

	Q_PROPERTY(QString mClientMacOSX READ clientMacOSX WRITE setClientMacOSX)

	QString clientLinux() const;
	void setClientLinux(const QString& sha1);

	Q_PROPERTY(QString mClientLinux READ clientLinux WRITE setClientLinux)

	QString updaterWin32() const;
	void setUpdaterWin32(const QString& sha1);

	Q_PROPERTY(QString mUpdaterWin32 READ updaterWin32 WRITE setUpdaterWin32)

	QString updaterMacOSX() const;
	void setUpdaterMacOSX(const QString& sha1);

	Q_PROPERTY(QString mUpdaterMacOSX READ updaterMacOSX WRITE setUpdaterMacOSX)

	QString updaterLinux() const;
	void setUpdaterLinux(const QString& sha1);

	Q_PROPERTY(QString mUpdaterLinux READ updaterLinux WRITE setUpdaterLinux)

protected:
	Config(QObject *parent = 0);
	bool nodeToBool(const QDomNode& node, bool def);

	QString mAddress;
	int mPort;

	QString mLogPath;
	QString mAuthAdminUser;

	bool mLogCritical;
	bool mLogError;
	bool mLogWarning;
	bool mLogInfo;
	bool mLogDebug;

	bool mAuthAdmin;
	bool mAuthViewDB;
	bool mAuthModifyDB;
	bool mAuthAdminDB;

	QString mDBType;
	QString mDBPath;
	QString mDBHost;
	QString mDBUser;
	QString mDBPass;
	QString mDBName;

	QString mAuthDBType;
	QString mAuthDBPath;
	QString mAuthDBHost;
	QString mAuthDBUser;
	QString mAuthDBPass;
	QString mAuthDBName;

	QString mSaltPass;
	QString mSaltImg;

	QString mSignature;

	QStringList mCaptchaLetters;
	QString mCaptchaFont;

	QSslKey mKey;
	QSslCertificate mCert;
	QString mKeyPath, mCertPath;

	bool mSslEnabled;

	QString mClientWin32, mClientMacOSX, mClientLinux;
	QString mUpdaterWin32, mUpdaterMacOSX, mUpdaterLinux;

	QFileSystemWatcher *mWatcher;
};

#define conf ( Config::getSingletonPtr() )

#endif // __Config_h__
