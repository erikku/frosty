/******************************************************************************\
*  server/src/Auth.cpp                                                         *
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

#include "Auth.h"
#include "Config.h"
#include "Utils.h"
#include "Log.h"

#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

static Auth *g_auth_inst = 0;

Auth::Auth()
{
	// Nothing to see here
}

Auth* Auth::getSingletonPtr()
{
	if(!g_auth_inst)
		g_auth_inst = new Auth;

	Q_ASSERT(g_auth_inst);

	return g_auth_inst;
}

void Auth::start(const QMap<QString, BackendActionHandler>& actionHandlers)
{
	// Standard Actions
	mActionPermissions["salt"] = "any";
	mActionPermissions["delete"] = "admin_db";
	mActionPermissions["insert"] = "modify_db";
	mActionPermissions["update"] = "modify_db";
	mActionPermissions["select"] = "view_db";

	// Database Actions
	mActionPermissions["db_export"] = "admin";
	mActionPermissions["db_import"] = "admin";

	// Auth Actions
	mActionPermissions["auth_query_perms"] = "any";
	mActionPermissions["auth_query_users"] = "admin";
	mActionPermissions["auth_query_user"] = "admin";
	mActionPermissions["auth_make_inactive"] = "admin";
	mActionPermissions["auth_make_active"] = "admin";
	mActionPermissions["auth_modify_user"] = "admin";

	// Server Actions
	mActionPermissions["server_updates"] = "any";

	mDefaultPerms["any"] = true;
	mDefaultPerms["admin"] = conf->authAdmin();
	mDefaultPerms["view_db"] = conf->authViewDB();
	mDefaultPerms["modify_db"] = conf->authModifyDB();
	mDefaultPerms["admin_db"] = conf->authAdminDB();

	QMapIterator<QString, BackendActionHandler> i = actionHandlers;
	while( i.hasNext() )
	{
		i.next();
		if( !mActionPermissions.contains( i.key() ) )
		{
			LOG_CRITICAL( QObject::tr("Action '%1' is not in the auth "
				"permissions list.\n").arg( i.key() ) );
			Q_ASSERT(!"auth permissions list action missing");
		}
	}

	if(conf->authDBType() == "sqlite")
	{
		mAuthDB = QSqlDatabase::addDatabase("QSQLITE", "auth");
		mAuthDB.setDatabaseName( conf->authDBPath() );
		mAuthDB.open();
	}
	else // mysql
	{
		mAuthDB = QSqlDatabase::addDatabase("QMYSQL", "auth");
		mAuthDB.setHostName( conf->authDBHost() );
		mAuthDB.setDatabaseName( conf->authDBName() );
		mAuthDB.setUserName( conf->authDBUser() );
		mAuthDB.setPassword( conf->authDBPass() );
		mAuthDB.open();

		QSqlQuery query("SET CHARSET utf8", mAuthDB);
		query.exec();
	}
}

bool Auth::authenticate(const QString& email, const QString& data,
	const QString& hash) const
{
	QString sql = "SELECT pass, active FROM users WHERE email = ?";

	QSqlQuery query(sql, mAuthDB);
	query.bindValue(0, email);

	if( !query.exec() )
	{
		LOG_ERROR( query.lastError().text() );
		return false;
	}

	if( !query.next() )
	{
		LOG_ERROR( query.lastError().text() );
		return false;
	}

	if( !query.record().value("active").toBool() )
	{
		LOG_ERROR("User is not active");
		return false;
	}

	QString pass = query.record().value("pass").toString();
	pass = sha1FromString(pass + data);

	if(pass == hash)
		return true;

	LOG_ERROR("Passwords do not match\n");

	return false;
}

bool Auth::validateRequest(const QString& email, const QVariant& action) const
{
	QVariantMap perms = queryPerms(email).toMap();
	QString act = action.toMap().value("action").toString();
	QStringList groups = mActionPermissions.value(act).split(",");

	if( !mActionPermissions.contains(act) )
		return false;

	bool can_perform = false;
	foreach(QString group, groups)
	{
		if( perms.value(group).toBool() )
		{
			can_perform = true;
			break;
		}
	}

	return can_perform | perms.value("admin").toBool();
}

QVariant Auth::queryPerms(const QString& email) const
{
	if( email.isEmpty() )
		return mDefaultPerms;

	if(conf->authAdminUser() == email)
	{
		QVariantMap map;
		map["any"] = true;
		map["admin"] = true;
		map["view_db"] = true;
		map["modify_db"] = true;
		map["admin_db"] = true;

		return map;
	}

	QString sql = "SELECT admin, view_db, modify_db, admin_db FROM "
		"users WHERE email = ?";

	QSqlQuery query(sql, mAuthDB);
	query.bindValue(0, email);

	if( !query.exec() )
	{
		LOG_DEBUG( query.lastError().text() );
		return mDefaultPerms;
	}

	if( !query.next() )
	{
		LOG_DEBUG( query.lastError().text() );
		return mDefaultPerms;
	}

	QSqlRecord record = query.record();

	QVariantMap perms;
	for(int j = 0; j < record.count(); j++)
		perms[record.fieldName(j)] = record.value(j);

	perms["any"] = true;

	return perms;
}

QVariant Auth::queryUsers(const QString& email) const
{
	QVariantMap perms = queryPerms(email).toMap();

	if(perms.value("admin").toBool() != true)
		return QVariantList();

	QString sql = "SELECT email, name, active, type, last_login FROM users";

	QSqlQuery query(sql, mAuthDB);
	if( !query.exec() )
	{
		LOG_DEBUG( query.lastError().text() );
		return QVariantList();
	}

	QVariantList users;

	while( query.next() )
	{
		QSqlRecord record = query.record();
		QVariantMap user;

		for(int j = 0; j < record.count(); j++)
			user[record.fieldName(j)] = record.value(j);

		users << user;
	}

	return users;
}

QString Auth::registerUser(const QString& email, const QString& name,
	const QString& pass) const
{
	{
		QSqlQuery query(mAuthDB);
		query.prepare("SELECT id FROM users WHERE email = :email");
		query.bindValue(":email", email);

		if( !query.exec() )
			return "Database error";

		if( query.next() )
			return "Email address already in use";
	}

	{
		QSqlQuery query(mAuthDB);
		query.prepare("SELECT id FROM users WHERE name = :name");
		query.bindValue(":name", name);

		if( !query.exec() )
			return "Database error";

		if( query.next() )
			return "User name already in use";
	}

	// Insert the account
	{
		QSqlQuery query(mAuthDB);
		query.prepare("INSERT INTO users(email, name, pass, admin, view_db, "
			"modify_db, admin_db, type) VALUES(:email, :name, :pass, :admin, "
			":view_db, :modify_db, :admin_db, :type)");
		query.bindValue(":email", email);
		query.bindValue(":name", name);
		query.bindValue(":pass", pass);
		query.bindValue(":admin", conf->authAdmin());
		query.bindValue(":view_db", conf->authViewDB());
		query.bindValue(":modify_db", conf->authModifyDB());
		query.bindValue(":admin_db", conf->authAdminDB());
		query.bindValue(":type", 1);

		if( !query.exec() )
			return "Database error";
	}

	return QString();
}

QVariant Auth::queryUser(const QString& email, const QString& target) const
{
	QVariantMap perms = queryPerms(email).toMap();
	if( !perms.value("admin").toBool() )
		return QVariant();

	QString sql = "SELECT email, name, active, type, last_login FROM users "
		"WHERE email = :email";

	QSqlQuery query(mAuthDB);
	query.prepare(sql);
	query.bindValue(":email", target);

	if( !query.exec() )
		return QVariant();

	if( !query.next() )
		return QVariant();

	QSqlRecord record = query.record();

	QVariantMap map;
	map["email"] = record.value("email");
	map["name"] = record.value("name");
	map["active"] = record.value("active");
	map["type"] = record.value("type");
	map["last_login"] = record.value("last_login");

	return map;
}

bool Auth::makeInactive(const QString& email,
	const QString& target_email) const
{
	QVariantMap perms = queryPerms(email).toMap();
	if( !perms.value("admin").toBool() )
		return false;

	QString sql = "UPDATE users SET active = :active WHERE email = :email";

	QSqlQuery query(mAuthDB);
	query.prepare(sql);
	query.bindValue(":active", false);
	query.bindValue(":email", target_email);

	if( !query.exec() )
		return false;

	if( !query.next() )
		return false;

	return true;
}

bool Auth::makeActive(const QString& email, const QString& target_email) const
{
	QVariantMap perms = queryPerms(email).toMap();
	if( !perms.value("admin").toBool() )
		return false;

	QString sql = "UPDATE users SET active = :active WHERE email = :email";

	QSqlQuery query(mAuthDB);
	query.prepare(sql);
	query.bindValue(":active", true);
	query.bindValue(":email", target_email);

	if( !query.exec() )
		return false;

	if( !query.next() )
		return false;

	return true;
}

bool Auth::modifyUser(const QString& email, const QString& target_email,
	const QString& new_email, const QString& name, const QString& pass,
	const QVariantMap& perms) const
{
	QVariantMap admin_perms = queryPerms(email).toMap();
	if( !admin_perms.value("admin").toBool() )
		return false;

	QString sql;
	if( pass.isEmpty() )
	{
		sql = "UPDATE users SET name = :name, email = :new_email, "
			"admin = :admin, view_db = :view_db, modify_db = :modify_db, "
			"admin_db = :admin_db WHERE email = :email";
	}
	else
	{
		sql = "UPDATE users SET name = :name, email = :new_email, "
			"admin = :admin, view_db = :view_db, modify_db = :modify_db, "
			"admin_db = :admin_db, pass = :pass WHERE email = :email";
	}

	QSqlQuery query(mAuthDB);
	query.prepare(sql);
	query.bindValue(":name", name);
	query.bindValue(":email", target_email);
	query.bindValue(":new_email", new_email);
	query.bindValue(":admin", perms.value("admin"));
	query.bindValue(":view_db", perms.value("view_db"));
	query.bindValue(":modify_db", perms.value("modify_db"));
	query.bindValue(":admin_db", perms.value("admin_db"));
	if( !pass.isEmpty() )
		query.bindValue(":email", target_email);

	if( !query.exec() )
		return false;

	if( !query.next() )
		return false;

	return true;
}
