/******************************************************************************\
*  server/src/SimulatorActions.cpp                                             *
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

#include "SimulatorActions.h"
#include "Utils.h"
#include "Auth.h"
#include "Log.h"

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#define tr(msg) QObject::tr(msg)

#define SLOT_COUNT_COMP    (8)
#define SLOT_COUNT_STORAGE (50)

QMap<QString, QVariantMap> simulator_cache;

void simulator_reset_cache()
{
	simulator_cache.clear();
}

QVariantMap simulatorCache(int i, QIODevice *connection,
	const QSqlDatabase& db, const QSqlDatabase& user_db,
	const QVariantMap& action, const QString& email)
{
	Q_UNUSED(email);
	Q_UNUSED(user_db);
	Q_UNUSED(connection);

	QVariantMap map;

	// Check for the 'lang' paramater
	if( !action.contains("lang") )
		return param_error(tr("simulator_cache action"), tr("lang"),
			tr("action %1").arg(i));

	bool reload = false;
	if( action.contains("reload") )
	{
		if( !action.value("reload").canConvert(QVariant::Bool) )
			return herror("simulator_cache action", tr("reload must be a "
				"boolean value for action %1").arg(i));

		reload = action.value("reload").toBool();
	}

	QString lang = action.value("lang").toString().toLower();
	if(lang != "en" && lang != "ja")
		return herror("simulator_cache action", tr("parmater 'lang' for action "
			"%1 contains an illegal value").arg(i));

	if(simulator_cache.contains(lang) && !reload)
		return simulator_cache.value(lang);

	QString sql = "SELECT db_skills.id id, db_skills.name_{$lang} name, "
		"db_skills.icon icon, db_affinity.name_{$lang} affinity, "
		"db_expert.name_{$lang} expert, db_category.name_{$lang} category, "
		"db_skills.mp_cost mp_cost, db_skills.hp_cost hp_cost, "
		"db_skills.mag_cost mag_cost, db_action_type.name_{$lang} action_type, "
		"db_skills.desc_{$lang} desc, db_skills.class class, "
		"db_skills.rank rank, db_skills.inheritance inheritance, "
		"db_related_stat.name_{$lang} related_stat FROM db_skills LEFT JOIN "
		"db_affinity ON db_skills.affinity = db_affinity.id LEFT JOIN "
		"db_expert ON db_skills.expert = db_expert.id LEFT JOIN db_category ON "
		"db_skills.category = db_category.id LEFT JOIN db_action_type ON "
		"db_skills.action_type = db_action_type.id LEFT JOIN db_related_stat "
		"ON db_skills.related_stat = db_related_stat.id "
		"WHERE db_skills.player_only = 0 OR db_skills.player_only = 'false'";
	sql = sql.replace("{$lang}", lang);

	QSqlQuery query(db);
	if( !query.prepare(sql) )
		return herror_sql(db, "simulator_cache action", tr("SQL error for "
			"action %1: %2").arg(i).arg( query.lastError().text() ));

	if( !query.exec() )
		return herror_sql(db, "simulator_cache action", tr("SQL error for "
			"action %1: %2").arg(i).arg( query.lastError().text() ));

	QVariantList skills;
	while( query.next() )
	{
		QSqlRecord record = query.record();

		QVariantMap skill;
		for(int j = 0; j < record.count(); j++)
			skill[record.fieldName(j)] = record.value(j);

		skills << skill;
	}

	map["skills"] = skills;

	sql = "SELECT id, name_{$lang} name, desc_{$lang} desc FROM db_traits";
	sql = sql.replace("{$lang}", lang);

	if( !query.prepare(sql) )
		return herror_sql(db, "simulator_cache action", tr("SQL error for "
			"action %1: %2").arg(i).arg( query.lastError().text() ));

	if( !query.exec() )
		return herror_sql(db, "simulator_cache action", tr("SQL error for "
			"action %1: %2").arg(i).arg( query.lastError().text() ));

	QVariantList traits;
	while( query.next() )
	{
		QSqlRecord record = query.record();

		QVariantMap trait;
		for(int j = 0; j < record.count(); j++)
			trait[record.fieldName(j)] = record.value(j);

		traits << trait;
	}

	map["traits"] = traits;

	sql = "SELECT db_devils.id id, db_devils.name_{$lang} name, "
		"db_devils.icon icon, db_devils.lvl lvl, db_genus.name_{$lang} genus, "
		"db_growth_type.name_{$lang} growth_type, db_devils.lnc lnc, "
		"db_devils.hp hp, db_devils.mp mp, db_devils.mag mag, "
		"db_mashou.fusion1_desc_{$lang} mashou_desc1, "
		"db_mashou.fusion2_desc_{$lang} mashou_desc2, "
		"db_equip_type.name_{$lang} mashou_equip_type, "
		"db_devils.fusion2 fusion2, db_devils.fusion3 fusion3, "
		"db_devils.fuse_min_lvl fuse_min_lvl, db_devils.parent parent_id, "
		"db_devils.fuse_max_lvl fuse_max_lvl, db_devils.genus genus_id, "
		"db_devils.fuse_chance fuse_chance, db_devils.breath breath, "
		"db_devils.wing wing, db_devils.pierce pierce, db_devils.fang fang, "
		"db_devils.claw claw, db_devils.needle needle, db_devils.sword sword, "
		"db_devils.strange strange, db_devils.eye eye, db_devils.notes notes, "
		"db_devils.fusion_cost fuse_cost, db_devils.mashou_gem mashou_gem "
		"FROM db_devils LEFT JOIN db_genus ON db_devils.genus = db_genus.id "
		"LEFT JOIN db_growth_type ON db_devils.growth = db_growth_type.id "
		"LEFT JOIN db_mashou ON db_devils.mashou = db_mashou.id LEFT JOIN "
		"db_equip_type ON db_mashou.equip_type = db_equip_type.id ORDER BY "
		"db_devils.genus, db_devils.lvl";
	sql = sql.replace("{$lang}", lang);

	if( !query.prepare(sql) )
		return herror_sql(db, "simulator_cache action", tr("SQL error for "
			"action %1: %2").arg(i).arg( query.lastError().text() ));

	if( !query.exec() )
		return herror_sql(db, "simulator_cache action", tr("SQL error for "
			"action %1: %2").arg(i).arg( query.lastError().text() ));

	QVariantList devils;
	while( query.next() )
	{
		QSqlRecord record = query.record();

		QVariantMap devil;
		for(int j = 0; j < record.count(); j++)
			devil[record.fieldName(j)] = record.value(j);

		sql = "SELECT skill, lvl FROM db_devil_skills WHERE devil = :devil";

		QSqlQuery query2(db);
		if( !query2.prepare(sql) )
			return herror_sql(db, "simulator_cache action", tr("SQL error for "
				"action %1: %2").arg(i).arg( query2.lastError().text() ));

		query2.bindValue(":devil", devil.value("id"), QSql::In);

		if( !query2.exec() )
			return herror_sql(db, "simulator_cache action", tr("SQL error for "
				"action %1: %2").arg(i).arg( query2.lastError().text() ));

		QVariantList devil_skills;
		while( query2.next() )
		{
			QSqlRecord record2 = query2.record();

			QVariantMap devil_skill;
			for(int j = 0; j < record2.count(); j++)
				devil_skill[record2.fieldName(j)] = record2.value(j);

			devil_skills << devil_skill;
		}

		devil["skills"] = devil_skills;

		sql = "SELECT trait FROM db_devil_traits WHERE devil = :devil";

		if( !query2.prepare(sql) )
			return herror_sql(db, "simulator_cache action", tr("SQL error for "
				"action %1: %2").arg(i).arg( query2.lastError().text() ));

		query2.bindValue(":devil", devil.value("id"), QSql::In);

		if( !query2.exec() )
			return herror_sql(db, "simulator_cache action", tr("SQL error for "
				"action %1: %2").arg(i).arg( query2.lastError().text() ));

		QVariantList devil_traits;
		while( query2.next() )
			devil_traits << query2.value(0);

		devil["traits"] = devil_traits;

		devils << devil;
	}

	map["devils"] = devils;


	sql = "SELECT id, name_ja FROM db_genus";

	if( !query.prepare(sql) )
		return herror_sql(db, "simulator_cache action", tr("SQL error for "
			"action %1: %2").arg(i).arg( query.lastError().text() ));

	if( !query.exec() )
		return herror_sql(db, "simulator_cache action", tr("SQL error for "
			"action %1: %2").arg(i).arg( query.lastError().text() ));

	QVariantMap genus;
	while( query.next() )
		genus[query.value(1).toString()] = query.value(0);

	map["genus"] = genus;

	Q_ASSERT( genus.contains(QString::fromUtf8("精霊")) );

	sql = "SELECT id, name_ja FROM db_devils WHERE genus = :genus";

	if( !query.prepare(sql) )
		return herror_sql(db, "simulator_cache action", tr("SQL error for "
			"action %1: %2").arg(i).arg( query.lastError().text() ));

	query.bindValue(":genus", genus.value(QString::fromUtf8("精霊")), QSql::In);

	if( !query.exec() )
		return herror_sql(db, "simulator_cache action", tr("SQL error for "
			"action %1: %2").arg(i).arg( query.lastError().text() ));

	QVariantMap seirei;
	while( query.next() )
		seirei[query.value(1).toString()] = query.value(0);

	map["seirei"] = seirei;

	// Update the cache
	simulator_cache[lang] = map;

	return map;
}

QVariantMap simulatorLoadStorage(int i, QIODevice *connection,
	const QSqlDatabase& db, const QSqlDatabase& user_db,
	const QVariantMap& action, const QString& email)
{
	Q_UNUSED(db);
	Q_UNUSED(action);
	Q_UNUSED(connection);

	int user_id = Auth::getSingletonPtr()->queryUserID(email);
	if(user_id < 1)
		return herror("simulator_load_storage action", tr("unknown user for "
			"action %1").arg(i));

	QString sql = "SELECT devil FROM db_storage WHERE user_id = :user_id "
		"ORDER BY slot";

	QSqlQuery query(user_db);
	if( !query.prepare(sql) )
		return herror_sql(user_db, "simulator_load_storage action", tr("SQL error "
			"for action %1: %2").arg(i).arg( query.lastError().text() ));

	query.bindValue(":user_id", user_id, QSql::In);

	if( !query.exec() )
		return herror_sql(user_db, "simulator_load_storage action", tr("SQL error "
			"for action %1: %2").arg(i).arg( query.lastError().text() ));

	QVariantList devils;
	while( query.next() )
		devils << query.value(0);

	if( devils.isEmpty() )
	{
		QSqlDatabase m_db = user_db;
		m_db.transaction();

		for(int j = 0; j < SLOT_COUNT_STORAGE; j++)
		{
			sql = "INSERT INTO db_storage(user_id, slot, devil) "
				"VALUES(:user_id, :slot, :devil)";

			if( !query.prepare(sql) )
				return herror_sql(m_db, "simulator_load_storage action", tr("SQL "
					"error for action %1: %2").arg(i).arg(
					query.lastError().text() ));

			query.bindValue(":user_id", user_id, QSql::In);
			query.bindValue(":slot", j + 1, QSql::In);
			query.bindValue(":devil", "{}", QSql::In);

			if( !query.exec() )
				return herror_sql(m_db, "simulator_load_storage action", tr("SQL "
					"error for action %1: %2").arg(i).arg(
					query.lastError().text() ));

			devils << "{}";
		}

		m_db.commit();
	}

	QVariantMap map;
	map["devils"] = devils;

	return map;
}

QVariantMap simulatorSyncStorage(int i, QIODevice *connection,
	const QSqlDatabase& db, const QSqlDatabase& user_db,
	const QVariantMap& action, const QString& email)
{
	Q_UNUSED(db);
	Q_UNUSED(connection);

	// Check for the 'devils' paramater
	if( !action.contains("devils") )
		return param_error(tr("simulator_sync_storage action"), tr("devils"),
			tr("action %1").arg(i));

	int user_id = Auth::getSingletonPtr()->queryUserID(email);
	if(user_id < 1)
		return herror("simulator_sync_storage action", tr("unknown user for "
			"action %1").arg(i));

	QVariantList devils = action.value("devils").toList();
	if(devils.count() != SLOT_COUNT_STORAGE)
		return herror("simulator_sync_storage action", tr("unexpected number "
			"of devil storage slots for action %1").arg(i));

	QSqlDatabase m_db = user_db;
	m_db.transaction();

	QString sql = "UPDATE db_storage SET devil = :devil WHERE "
		"user_id = :user_id AND slot = :slot";

	QSqlQuery query(m_db);
	if( !query.prepare(sql) )
		return herror_sql(m_db, "simulator_sync_storage action", tr("SQL "
			"error for action %1: %2").arg(i).arg(
			query.lastError().text() ));

	for(int j = 0; j < SLOT_COUNT_STORAGE; j++)
	{
		query.bindValue(":user_id", user_id, QSql::In);
		query.bindValue(":slot", j + 1, QSql::In);
		query.bindValue(":devil", devils.at(j), QSql::In);

		if( !query.exec() )
			return herror_sql(m_db, "simulator_sync_storage action", tr("SQL "
				"error for action %1: %2").arg(i).arg(
				query.lastError().text() ));
	}

	m_db.commit();

	return QVariantMap();
}

QVariantMap simulatorLoadCOMP(int i, QIODevice *connection,
	const QSqlDatabase& db, const QSqlDatabase& user_db,
	const QVariantMap& action, const QString& email)
{
	Q_UNUSED(db);
	Q_UNUSED(action);
	Q_UNUSED(connection);

	int user_id = Auth::getSingletonPtr()->queryUserID(email);
	if(user_id < 1)
		return herror("simulator_load_comp action", tr("unknown user for "
			"action %1").arg(i));

	QString sql = "SELECT devil FROM db_comp WHERE user_id = :user_id "
		"ORDER BY slot";

	QSqlQuery query(user_db);
	if( !query.prepare(sql) )
		return herror_sql(user_db, "simulator_load_comp action", tr("SQL error "
			"for action %1: %2").arg(i).arg( query.lastError().text() ));

	query.bindValue(":user_id", user_id, QSql::In);

	if( !query.exec() )
		return herror_sql(user_db, "simulator_load_comp action", tr("SQL error "
			"for action %1: %2").arg(i).arg( query.lastError().text() ));

	QVariantList devils;
	while( query.next() )
		devils << query.value(0);

	if( devils.isEmpty() )
	{
		QSqlDatabase m_db = user_db;
		m_db.transaction();

		for(int j = 0; j < SLOT_COUNT_COMP; j++)
		{
			sql = "INSERT INTO db_comp(user_id, slot, devil) "
				"VALUES(:user_id, :slot, :devil)";

			if( !query.prepare(sql) )
				return herror_sql(m_db, "simulator_load_comp action", tr("SQL "
					"error for action %1: %2").arg(i).arg(
					query.lastError().text() ));

			query.bindValue(":user_id", user_id, QSql::In);
			query.bindValue(":slot", j + 1, QSql::In);
			query.bindValue(":devil", "{}", QSql::In);

			if( !query.exec() )
				return herror_sql(m_db, "simulator_load_comp action", tr("SQL "
					"error for action %1: %2").arg(i).arg(
					query.lastError().text() ));

			devils << "{}";
		}

		m_db.commit();
	}

	QVariantMap map;
	map["devils"] = devils;

	return map;
}

QVariantMap simulatorSyncCOMP(int i, QIODevice *connection,
	const QSqlDatabase& db, const QSqlDatabase& user_db,
	const QVariantMap& action, const QString& email)
{
	Q_UNUSED(db);
	Q_UNUSED(connection);

	// Check for the 'devils' paramater
	if( !action.contains("devils") )
		return param_error(tr("simulator_sync_comp action"), tr("devils"),
			tr("action %1").arg(i));

	int user_id = Auth::getSingletonPtr()->queryUserID(email);
	if(user_id < 1)
		return herror("simulator_sync_comp action", tr("unknown user for "
			"action %1").arg(i));

	QVariantList devils = action.value("devils").toList();
	if(devils.count() != SLOT_COUNT_COMP)
		return herror("simulator_sync_comp action", tr("unexpected number "
			"of devil COMP slots for action %1").arg(i));

	QSqlDatabase m_db = user_db;
	m_db.transaction();

	QString sql = "UPDATE db_comp SET devil = :devil WHERE "
		"user_id = :user_id AND slot = :slot";

	QSqlQuery query(m_db);
	if( !query.prepare(sql) )
		return herror_sql(m_db, "simulator_sync_comp action", tr("SQL "
			"error for action %1: %2").arg(i).arg(
			query.lastError().text() ));

	for(int j = 0; j < SLOT_COUNT_COMP; j++)
	{
		query.bindValue(":user_id", user_id, QSql::In);
		query.bindValue(":slot", j + 1, QSql::In);
		query.bindValue(":devil", devils.at(j), QSql::In);

		if( !query.exec() )
			return herror_sql(m_db, "simulator_sync_comp action", tr("SQL "
				"error for action %1: %2").arg(i).arg(
				query.lastError().text() ));
	}

	m_db.commit();

	return QVariantMap();
}
