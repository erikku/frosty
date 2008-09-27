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

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

QSqlDatabase auth_db;

QVariantMap default_perms()
{
	QVariantMap map;
	map["admin"] = conf->authAdmin();
	map["view_db"] = conf->authViewDB();
	map["modify_db"] = conf->authModifyDB();
	map["admin_db"] = conf->authAdminDB();

	return map;
};

void auth_start()
{
	if(conf->authDBType() == "sqlite")
	{
		auth_db = QSqlDatabase::addDatabase("QSQLITE", "auth");
		auth_db.setDatabaseName( conf->authDBPath() );
		auth_db.open();
	}
	else // mysql
	{
		auth_db = QSqlDatabase::addDatabase("QMYSQL", "auth");
		auth_db.setHostName( conf->authDBHost() );
		auth_db.setDatabaseName( conf->authDBName() );
		auth_db.setUserName( conf->authDBUser() );
		auth_db.setPassword( conf->authDBPass() );
		auth_db.open();

		QSqlQuery query("SET CHARSET utf8", auth_db);
		query.exec();
	}
};

bool auth_authenticate(const QString& email, const QString& data,
	const QString& hash)
{
	QString sql = "SELECT pass, active FROM users WHERE email = ?";

	QSqlQuery query(sql, auth_db);
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
};

bool auth_validate_request(const QString& email, const QVariant& action)
{
	QVariantMap perms = auth_query_perms(email).toMap();
	QVariantMap action_map = action.toMap();

	if(action_map.value("action").toString() == "select")
		return perms.value("view_db").toBool();
	if(action_map.value("action").toString() == "delete")
		return perms.value("admin_db").toBool();
	else
		return perms.value("modify_db").toBool();

	return false;
};

QVariant auth_query_perms(const QString& email)
{
	if( email.isEmpty() )
		return default_perms();

	if(conf->authAdminUser() == email)
	{
		QVariantMap map;
		map["admin"] = true;
		map["view_db"] = true;
		map["modify_db"] = true;
		map["admin_db"] = true;

		return map;
	}

	QString sql = "SELECT admin, view_db, modify_db, admin_db FROM "
		"users WHERE email = ?";

	QSqlQuery query(sql, auth_db);
	query.bindValue(0, email);

	if( !query.exec() )
	{
		LOG_DEBUG( query.lastError().text() );
		return default_perms();
	}

	if( !query.next() )
	{
		LOG_DEBUG( query.lastError().text() );
		return default_perms();
	}

	QSqlRecord record = query.record();

	QVariantMap perms;
	for(int j = 0; j < record.count(); j++)
		perms[record.fieldName(j)] = record.value(j);

	return perms;
};

QVariant auth_query_users(const QString& email)
{
	QVariantMap perms = auth_query_perms(email).toMap();

	if(perms.value("admin").toBool() != true)
		return QVariantList();

	QString sql = "SELECT email, name, active, type, last_login FROM users";

	QSqlQuery query(sql, auth_db);
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
};

QString auth_register(const QString& email, const QString& name,
	const QString& pass)
{
	{
		QSqlQuery query(auth_db);
		query.prepare("SELECT id FROM users WHERE email = :email");
		query.bindValue(":email", email);

		if( !query.exec() )
			return "Database error";

		if( query.next() )
			return "Email address already in use";
	}

	{
		QSqlQuery query(auth_db);
		query.prepare("SELECT id FROM users WHERE name = :name");
		query.bindValue(":name", name);

		if( !query.exec() )
			return "Database error";

		if( query.next() )
			return "User name already in use";
	}

	// Insert the account
	{
		QSqlQuery query(auth_db);
		query.prepare("INSERT INTO users(email, name, pass, admin, view_db, "
			"modify_db, admin_db) VALUES(:email, :name, :pass, :admin, "
			":view_db, :modify_db, :admin_db)");
		query.bindValue(":email", email);
		query.bindValue(":name", name);
		query.bindValue(":pass", pass);
		query.bindValue(":admin", conf->authAdmin());
		query.bindValue(":view_db", conf->authViewDB());
		query.bindValue(":modify_db", conf->authModifyDB());
		query.bindValue(":admin_db", conf->authAdminDB());

		if( !query.exec() )
			return "Database error";
	}

	return QString();
};

QVariant auth_query_user(const QString& email, const QString& target_email)
{
	QVariantMap perms = auth_query_perms(email).toMap();
	if( !perms.value("admin").toBool() )
		return QVariant();

	QString sql = "SELECT email, name, active, type, last_login FROM users "
		"WHERE email = :email";

	QSqlQuery query(auth_db);
	query.prepare(sql);
	query.bindValue(":email", target_email);

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
};

bool auth_make_inactive(const QString& email, const QString& target_email)
{
	QVariantMap perms = auth_query_perms(email).toMap();
	if( !perms.value("admin").toBool() )
		return false;

	QString sql = "UPDATE users SET active = :active WHERE email = :email";

	QSqlQuery query(auth_db);
	query.prepare(sql);
	query.bindValue(":active", false);
	query.bindValue(":email", target_email);

	if( !query.exec() )
		return false;

	if( !query.next() )
		return false;

	return true;
};

bool auth_make_active(const QString& email, const QString& target_email)
{
	QVariantMap perms = auth_query_perms(email).toMap();
	if( !perms.value("admin").toBool() )
		return false;

	QString sql = "UPDATE users SET active = :active WHERE email = :email";

	QSqlQuery query(auth_db);
	query.prepare(sql);
	query.bindValue(":active", true);
	query.bindValue(":email", target_email);

	if( !query.exec() )
		return false;

	if( !query.next() )
		return false;

	return true;
};

bool auth_modify_user(const QString& email, const QString& target_email,
	const QString& new_email, const QString& name, const QString& pass,
	const QVariantMap& perms)
{
	QVariantMap admin_perms = auth_query_perms(email).toMap();
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

	QSqlQuery query(auth_db);
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
};
