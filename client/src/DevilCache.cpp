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

		ajax::getSingletonPtr()->request(settings->url(), action);
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

			QListIterator<QVariant> it(skills);
			while( it.hasNext() )
			{
				QVariantMap skill = it.next().toMap();

				mSkills[skill.value("id").toInt()] = skill;
			}
		}

		// Traits
		{
			QVariantList traits = resp.value("traits").toList();

			QListIterator<QVariant> it(traits);
			while( it.hasNext() )
			{
				QVariantMap trait = it.next().toMap();

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
		combo_file.readLine() ) ).toMap();
	combo_file.close();

	QMapIterator<QString, QVariant> it(combos);
	while( it.hasNext() )
	{
		it.next();

		Q_ASSERT( mGenusByName.contains( it.key() ) );
		int key = mGenusByName.value( it.key() );

		QVariantMap values = it.value().toMap();

		QHash<int, int> inside;
		QMapIterator<QString, QVariant> inside_it(values);
		while( inside_it.hasNext() )
		{
			inside_it.next();

			Q_ASSERT( mGenusByName.contains( inside_it.key() ) );
			Q_ASSERT( mGenusByName.contains( inside_it.value().toString() ) );

			int inside_key = mGenusByName.value( inside_it.key() );
			int inside_value = mGenusByName.value(
				inside_it.value().toString() );

			inside[inside_key] = inside_value;
		}

		mFusionCombos[key] = inside;
	}

	//
	// Seirei Combos
	//
	QFile seirei_file(":/seirei.json");
	seirei_file.open(QIODevice::ReadOnly);

	QVariantMap seirei_combos = json::parse( QString::fromUtf8(
		seirei_file.readLine() ) ).toMap();
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
		modifiers_file.readLine() ) ).toMap();
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
	if( !mFusionCombos.contains(first) )
		return -1;

	QHash<int, int> inside = mFusionCombos.value(first);
	if( !inside.contains(second) )
		return -1;

	return inside.value(second);
}

int DevilCache::seireiModifier(int genus, int seirei) const
{
	if( !mSeireiModifiers.contains(genus) )
		return -1;

	QMap<int, int> inside = mSeireiModifiers.value(genus);
	Q_ASSERT( inside.contains(seirei) );

	return inside.value(seirei);
}
