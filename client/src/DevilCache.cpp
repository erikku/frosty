/******************************************************************************\
*  client/src/DevilCache.cpp                                                   *
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

#include "DevilCache.h"
#include "Settings.h"
#include "ajax.h"
#include "json.h"

#include <QtCore/QFile>

static DevilCache *g_devilcache_inst = 0;

DevilCache* DevilCache::getSingletonPtr()
{
	if(!g_devilcache_inst)
		g_devilcache_inst = new DevilCache;

	Q_ASSERT(g_devilcache_inst != 0);

	return g_devilcache_inst;
}

DevilCache::DevilCache(QObject *parent_object) : QObject(parent_object),
	mCacheFull(false), mSeireiGenus(-1)
{
	ajax::getSingletonPtr()->subscribe(this);

	{
		QVariantMap action;
		action["action"] = "simulator_cache";
		action["user_data"] = "simulator_cache";
		action["lang"] = settings->lang();

		ajax::getSingletonPtr()->request(action);
	}
}

void DevilCache::ajaxResponse(const QVariantMap& resp, const QString& user_data)
{
	// TODO: Clear the pre-existing cache first

	if(user_data == "simulator_cache")
	{
		// Skills
		{
			QVariantList skills = resp.value("skills").toList();

			QList<QVariant>::const_iterator it;
			for(it = skills.begin(); it != skills.end(); it++)
			{
				QVariantMap skill = (*it).toMap();

				mSkills[skill.value("id").toInt()] = skill;
			}
		}

		// Traits
		{
			QVariantList traits = resp.value("traits").toList();

			QList<QVariant>::const_iterator it;
			for(it = traits.begin(); it != traits.end(); it++)
			{
				QVariantMap trait = (*it).toMap();

				mTraits[trait.value("id").toInt()] = trait;
			}
		}

		// Devils
		{
			QVariantList devils = resp.value("devils").toList();

			QListIterator<QVariant> it(devils);
			while( it.hasNext() )
			{
				QVariantMap devil = it.next().toMap();

				mDevils[devil.value("id").toInt()] = devil;

				int genus = devil.value("genus_id").toInt();

				QList<QVariantMap> genus_list = mDevilsByGenus.value(genus);
				genus_list << devil;

				// Save the genus list back
				mDevilsByGenus[genus] = genus_list;
			}
		}

		// Genus
		{
			QVariantMap genus = resp.value("genus").toMap();

			QMapIterator<QString, QVariant> it(genus);
			while( it.hasNext() )
			{
				it.next();

				mGenusByName[it.key()] = it.value().toInt();
			}

			// Add short-hand names
			Q_ASSERT( mGenusByName.contains(QString::fromUtf8("国津神")) );
			mGenusByName[QString::fromUtf8("国津")] = mGenusByName.value(
				QString::fromUtf8("国津神") );
			Q_ASSERT( mGenusByName.contains(QString::fromUtf8("地母神")) );
			mGenusByName[QString::fromUtf8("地母")] = mGenusByName.value(
				QString::fromUtf8("地母神") );
			Q_ASSERT( mGenusByName.contains(QString::fromUtf8("堕天使")) );
			mGenusByName[QString::fromUtf8("堕天")] = mGenusByName.value(
				QString::fromUtf8("堕天使") );
			Q_ASSERT( mGenusByName.contains(QString::fromUtf8("天津神")) );
			mGenusByName[QString::fromUtf8("天津")] = mGenusByName.value(
				QString::fromUtf8("天津神") );
			Q_ASSERT( mGenusByName.contains(QString::fromUtf8("大天使")) );
			mGenusByName[QString::fromUtf8("大天")] = mGenusByName.value(
				QString::fromUtf8("大天使") );
		}

		// Seirei
		{
			QVariantMap seirei = resp.value("seirei").toMap();

			QMapIterator<QString, QVariant> it(seirei);
			while( it.hasNext() )
			{
				it.next();

				mSeireiByName[it.key()] = it.value().toInt();
			}
		}

		loadSimulatorData();

		mCacheFull = true;
		emit cacheReady();
	}
}

void DevilCache::fillCache()
{
	if(mCacheFull)
		emit cacheReady();
}

QVariantMap DevilCache::skillByID(int id) const
{
	return mSkills.value(id);
}

QVariantMap DevilCache::devilByID(int id) const
{
	return mDevils.value(id);
}

QVariantMap DevilCache::seireiByGenus(int genus) const
{
	if( !mSeireiCombos.contains(genus) )
		return QVariantMap();

	return mDevils.value( mSeireiCombos.value(genus) );
}

QList<QVariantMap> DevilCache::devilsByGenus(int genus) const
{
	return mDevilsByGenus.value(genus);
}

QString DevilCache::skillToolTip(int id) const
{
	QVariantMap skill = mSkills.value(id);

	QString tooltip = tr("%1").arg( skill.value("name").toString() );

	return tooltip;
}

QString DevilCache::devilToolTip(const QVariantMap& mix_data) const
{
	int devil_id = mix_data.value("id").toInt();

	QVariantMap devil = mDevils.value(devil_id);

	QString tooltip = tr("%1 - %2\n%3").arg(
		devil.value("name").toString() ).arg(
		devil.value("lvl").toInt() ).arg(
		devil.value("genus").toString() );

	return tooltip;
}

void DevilCache::loadSimulatorData()
{
	//
	// Fusion Combos
	//
	QFile combo_file(":/fusion_combos.json");
	combo_file.open(QIODevice::ReadOnly);

	QVariantMap combos = json::parse( QString::fromUtf8(
		combo_file.readLine().trimmed() ) ).toMap();
	combo_file.close();

	QMapIterator<QString, QVariant> it(combos);
	while( it.hasNext() )
	{
		it.next();

		Q_ASSERT( mGenusByName.contains( it.key() ) );
		int key = mGenusByName.value( it.key() );

		QVariantMap values = it.value().toMap();

		QMapIterator<QString, QVariant> inside_it(values);
		while( inside_it.hasNext() )
		{
			inside_it.next();

			Q_ASSERT( mGenusByName.contains( inside_it.key() ) );
			Q_ASSERT( mGenusByName.contains( inside_it.value().toString() ) );

			int inside_key = mGenusByName.value( inside_it.key() );
			int inside_value = mGenusByName.value(
				inside_it.value().toString() );

#ifndef QT_NO_DEBUG
			GenusPair genus_key(key, inside_key);
			if( mFusionCombos.contains(genus_key) )
				Q_ASSERT(mFusionCombos.value(genus_key) == inside_value);
#endif // QT_NO_DEBUG

			mFusionCombos[GenusPair(key, inside_key)] = inside_value;
		}
	}

	//
	// Seirei Combos
	//
	QFile seirei_file(":/seirei.json");
	seirei_file.open(QIODevice::ReadOnly);

	QVariantMap seirei_combos = json::parse( QString::fromUtf8(
		seirei_file.readLine().trimmed() ) ).toMap();
	seirei_file.close();

	it = seirei_combos;
	while( it.hasNext() )
	{
		it.next();

		Q_ASSERT( mSeireiByName.contains( it.key() ) );
		int seirei = mSeireiByName.value( it.key() );

		QVariantList genus = it.value().toList();
		QListIterator<QVariant> genus_it(genus);
		while( genus_it.hasNext() )
		{
			QString genus_name = genus_it.next().toString();

			Q_ASSERT( mGenusByName.contains(genus_name) );
			mSeireiCombos[mGenusByName.value(genus_name)] = seirei;
		}
	}

	//
	// Seirei Modifiers
	//
	QFile modifiers_file(":/seirei_modifiers.json");
	modifiers_file.open(QIODevice::ReadOnly);

	QVariantMap modifiers = json::parse( QString::fromUtf8(
		modifiers_file.readLine().trimmed() ) ).toMap();
	modifiers_file.close();

	it = modifiers;
	while( it.hasNext() )
	{
		it.next();

		Q_ASSERT( mGenusByName.contains( it.key() ) );
		int genus = mGenusByName.value( it.key() );

		QVariantMap modifier_set = it.value().toMap();
		QMapIterator<QString, QVariant> modifier_it(modifier_set);

		QMap<int, int> set;
		while( modifier_it.hasNext() )
		{
			modifier_it.next();

			Q_ASSERT( mSeireiByName.contains( modifier_it.key() ) );
			int seirei = mSeireiByName.value( modifier_it.key() );

			// UP - 0 | DOWN - 2
			set[seirei] = modifier_it.value().toInt();
		}

		mSeireiModifiers[genus] = set;
	}

	Q_UNUSED( mGenusByName.contains(QString::fromUtf8("精霊")) );
	mSeireiGenus = mGenusByName.value(QString::fromUtf8("精霊"));
}

int DevilCache::seireiGenus() const
{
	return mSeireiGenus;
}

int DevilCache::resultGenus(int first, int second) const
{
	if( !mFusionCombos.contains( GenusPair(first, second) ) )
		return -1;

	return mFusionCombos.value( GenusPair(first, second) );
}

int DevilCache::seireiModifier(int genus, int seirei) const
{
	if( !mSeireiModifiers.contains(genus) )
		return -1;

	QMap<int, int> inside = mSeireiModifiers.value(genus);
	Q_ASSERT( inside.contains(seirei) );

	return inside.value(seirei);
}

QVariantList DevilCache::calcInheritedSkills(const QVariantMap& devil) const
{
	QList<int> skills;
	{
		QVariantList skills_list = devil.value("skills").toList();
		QListIterator<QVariant> it(skills_list);
		while( it.hasNext() )
			skills << it.next().toInt();
	}

	QVariantList parents = devil.value("parents").toList();
	QListIterator<QVariant> parent_it(parents);

	QVariantMap devil_info = devilByID( devil.value("id").toInt() );

	QMap<int, bool> canInherit;
	canInherit[-2] = false;
	canInherit[-1] = false;
	canInherit[0] = devil_info.value("breath").toBool();
	canInherit[1] = devil_info.value("wing").toBool();
	canInherit[2] = devil_info.value("pierce").toBool();
	canInherit[3] = devil_info.value("fang").toBool();
	canInherit[4] = devil_info.value("claw").toBool();
	canInherit[5] = devil_info.value("needle").toBool();
	canInherit[6] = devil_info.value("sword").toBool();
	canInherit[7] = devil_info.value("strange").toBool();
	canInherit[8] = devil_info.value("eye").toBool();
	canInherit[99] = true;

	QVariantList inherited_skills;

	while( parent_it.hasNext() )
	{
		QVariantMap parent = parent_it.next().toMap();

		QVariantList parent_skills = parent.value("skills").toList();
		QListIterator<QVariant> skills_it(parent_skills);
		while( skills_it.hasNext() )
		{
			int skill = skills_it.next().toInt();
			if( skill < 0 || skills.contains(skill) )
				continue;

			QVariantMap skill_info = skillByID(skill);
			Q_ASSERT( !skill_info.isEmpty() );

			int inherit = skill_info.value("inheritance").toInt();

			Q_ASSERT( canInherit.contains(inherit) );
			if( !canInherit.contains(inherit) )
				inherit = -1;

			if( !canInherit.value(inherit) )
				continue;

			inherited_skills << skill;
			skills << skill;
		}
	}

	return inherited_skills;
}

QHash<GenusPair, int> DevilCache::fusionCombos() const
{
	return mFusionCombos;
}
