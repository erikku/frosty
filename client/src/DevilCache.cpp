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

static DevilCache *g_devilcache_inst = 0;

DevilCache* DevilCache::getSingletonPtr()
{
	if(!g_devilcache_inst)
		g_devilcache_inst = new DevilCache;

	Q_ASSERT(g_devilcache_inst != 0);

	return g_devilcache_inst;
}

DevilCache::DevilCache(QObject *parent_object) : QObject(parent_object),
	mCacheFull(false)
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

void DevilCache::ajaxResponse(const QVariant& resp)
{
	QVariantMap result = resp.toMap();

	if(result.value("user_data").toString() == "simulator_cache")
	{
		// Skills
		{
			QVariantList skills = result.value("skills").toList();

			QListIterator<QVariant> it(skills);
			while( it.hasNext() )
			{
				QVariantMap skill = it.next().toMap();

				mSkills[skill.value("id").toInt()] = skill;
			}
		}

		// Traits
		{
			QVariantList traits = result.value("traits").toList();

			QListIterator<QVariant> it(traits);
			while( it.hasNext() )
			{
				QVariantMap trait = it.next().toMap();

				mTraits[trait.value("id").toInt()] = trait;
			}
		}

		// Devils
		{
			QVariantList devils = result.value("devils").toList();

			QListIterator<QVariant> it(devils);
			while( it.hasNext() )
			{
				QVariantMap devil = it.next().toMap();

				mDevils[devil.value("id").toInt()] = devil;

				QString genus = devil.value("genus").toString();

				QList<QVariantMap> genus_list = mDevilsByGenus.value(genus);
				genus_list << devil;

				// Save the genus list back
				mDevilsByGenus[genus] = genus_list;
			}
		}

		mCacheFull = true;
		emit cacheReady();
	}
}

void DevilCache::fillCache()
{
	if(mCacheFull)
		emit cacheReady();
}

QVariantMap DevilCache::devilByID(int id) const
{
	return mDevils.value(id);
}

QString DevilCache::generateToolTip(const QVariantMap& mix_data) const
{
	int devil_id = mix_data.value("id").toInt();

	QVariantMap devil = mDevils.value(devil_id);

	QString tooltip = tr("%1 - %2\n%3").arg(
		devil.value("name").toString() ).arg(
		devil.value("lvl").toInt() ).arg(
		devil.value("genus").toString() );

	return tooltip;
}
