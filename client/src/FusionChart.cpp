/******************************************************************************\
*  client/src/FusionChart.cpp                                                  *
*  Copyright (C) 2009 John Eric Martin <john.eric.martin@gmail.com>            *
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

#include "FusionChart.h"
#include "DevilCache.h"
#include "COMP.h"

#include <QtGui/QMouseEvent>

FusionChart::FusionChart(QWidget *parent_widget) : QDialog(parent_widget),
	mStorage(0)
{
	ui.setupUi(this);

	setWindowTitle( tr("%1 - Fuse").arg(
		tr("Absolutely Frosty") ) );

	setWindowModality(Qt::WindowModal);

	mDevilLabels << ui.demonLabel1 << ui.demonLabel2 << ui.demonLabel3
		<< ui.demonLabel4 << ui.demonLabel5 << ui.demonLabel6 << ui.demonLabel7
		<< ui.demonLabel8;

	mDevilIcons << ui.demonIcon1 << ui.demonIcon2 << ui.demonIcon3
		<< ui.demonIcon4 << ui.demonIcon5 << ui.demonIcon6 << ui.demonIcon7
		<< ui.demonIcon8;

	mDevilIcons2 << ui.demonIconA << ui.demonIconB << ui.demonIconC
		<< ui.demonIconD << ui.demonIconE << ui.demonIconF << ui.demonIconG
		<< ui.demonIconH;

	mDevilIconGrid
		<< ui.demonIcon1A << ui.demonIcon1B << ui.demonIcon1C << ui.demonIcon1D
		<< ui.demonIcon1E << ui.demonIcon1F << ui.demonIcon1G << ui.demonIcon1H
		<< ui.demonIcon2A << ui.demonIcon2B << ui.demonIcon2C << ui.demonIcon2D
		<< ui.demonIcon2E << ui.demonIcon2F << ui.demonIcon2G << ui.demonIcon2H
		<< ui.demonIcon3A << ui.demonIcon3B << ui.demonIcon3C << ui.demonIcon3D
		<< ui.demonIcon3E << ui.demonIcon3F << ui.demonIcon3G << ui.demonIcon3H
		<< ui.demonIcon4A << ui.demonIcon4B << ui.demonIcon4C << ui.demonIcon4D
		<< ui.demonIcon4E << ui.demonIcon4F << ui.demonIcon4G << ui.demonIcon4H
		<< ui.demonIcon5A << ui.demonIcon5B << ui.demonIcon5C << ui.demonIcon5D
		<< ui.demonIcon5E << ui.demonIcon5F << ui.demonIcon5G << ui.demonIcon5H
		<< ui.demonIcon6A << ui.demonIcon6B << ui.demonIcon6C << ui.demonIcon6D
		<< ui.demonIcon6E << ui.demonIcon6F << ui.demonIcon6G << ui.demonIcon6H
		<< ui.demonIcon7A << ui.demonIcon7B << ui.demonIcon7C << ui.demonIcon7D
		<< ui.demonIcon7E << ui.demonIcon7F << ui.demonIcon7G << ui.demonIcon7H
		<< ui.demonIcon8A << ui.demonIcon8B << ui.demonIcon8C << ui.demonIcon8D
		<< ui.demonIcon8E << ui.demonIcon8F << ui.demonIcon8G << ui.demonIcon8H;

	int max = mDevilIconGrid.count();
	for(int i = 0; i < max; i++)
		mFusionData << QVariantMap();

	connect(ui.closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));
}

void FusionChart::loadDevils(StorageBase *storage,
	const QList<DevilData>& devils)
{
	mStorage = storage;

	Q_ASSERT( mDevilLabels.count() == devils.count() );

	mDevilSlots.clear();
	mDevilData.clear();

	int i = -1;
	foreach(QVariantMap devil, devils)
	{
		i++;
		if( devil.isEmpty() )
			continue;

		mDevilData << devil;
		mDevilSlots << i;
	}
	clear();

	DevilCache *cache = DevilCache::getSingletonPtr();

	int max = mDevilData.count();

	i = 0;
	foreach(QVariantMap devil, mDevilData)
	{
		if( devil.isEmpty() )
			continue;

		QVariantMap devil_info = cache->devilByID( devil.value("id").toInt() );

		QString lbl = tr("lv%2 %1").arg( devil_info.value("name").toString()
			).arg(devil.value("lvl").toInt() );

		QPixmap icon( QString("icons/devils/icon_%1.png").arg(
			devil_info.value("icon").toString() ) );

		QString tooltip = cache->devilToolTip(devil);

		mDevilLabels.at(i)->setText(lbl);
		mDevilLabels.at(i)->setToolTip(tooltip);
		mDevilIcons.at(i)->setPixmap(icon);
		mDevilIcons.at(i)->setToolTip(tooltip);
		mDevilIcons2.at(i)->setPixmap(icon);
		mDevilIcons2.at(i)->setToolTip(tooltip);

		for(int j = 0; j < max; j++)
		{
			if(j <= i)
				continue;

			if( mDevilData.at(j).isEmpty() )
				continue;

			calculateFusion(i, j);
		}

		i++;
	}

	show();
	resize(minimumSizeHint());
}

void FusionChart::clear()
{
	QPixmap border(":/border.png");

	foreach(QLabel *label, mDevilLabels)
	{
		label->clear();
		label->setToolTip(QString());
	}

	foreach(QLabel *label, mDevilIcons)
	{
		label->setPixmap(border);
		label->setToolTip(QString());
	}

	foreach(QLabel *label, mDevilIcons2)
	{
		label->setPixmap(border);
		label->setToolTip(QString());
	}

	foreach(QLabel *label, mDevilIconGrid)
	{
		label->setPixmap(border);
		label->setToolTip(QString());
	}
}

void FusionChart::calculateFusion(int first, int second)
{
	DevilCache *cache = DevilCache::getSingletonPtr();

	QVariantMap devilA = mDevilData.at(first);
	QVariantMap devilB = mDevilData.at(second);

	QVariantMap devilInfoA = cache->devilByID( devilA.value("id").toInt() );
	QVariantMap devilInfoB = cache->devilByID( devilB.value("id").toInt() );

	int devilGenusA = devilInfoA.value("genus_id").toInt();
	int devilGenusB = devilInfoB.value("genus_id").toInt();
	int seireiGenus = cache->seireiGenus();

	QVariantMap result_devil;

	if(devilGenusA == devilGenusB)
	{
		// The result is a seirei
		result_devil = cache->seireiByGenus(devilGenusA);
	}
	else if(devilGenusA == seireiGenus || devilGenusB == seireiGenus)
	{
		// Seirei Modifier Fusion
		int devilGenus;
		QVariantMap devil, seirei, info;
		if(devilGenusA == seireiGenus)
		{
			devil = devilB;
			seirei = devilA;
			info = devilInfoB;
			devilGenus = devilGenusB;
		}
		else
		{
			devil = devilA;
			seirei = devilB;
			info = devilInfoA;
			devilGenus = devilGenusA;
		}

		QVariantMap seirei_info = cache->devilByID(
			seirei.value("id").toInt() );

		int seirei_id = seirei_info.value("id").toInt();
		if(seirei_info.value("parent_id").toInt() > 0)
			seirei_id = seirei_info.value("parent_id").toInt();
		int parent_id = info.value("id").toInt();
		if(info.value("parent_id").toInt() > 0)
			parent_id = info.value("parent_id").toInt();

		int modifier = cache->seireiModifier(devilGenus, seirei_id);
		if(modifier != 0 && modifier != 2)
			return;

		QMap<int, QVariantMap> devils;
		QMap<int, int> level_map;

		{
			QList<QVariantMap> genus_devils = cache->devilsByGenus(devilGenus);
			foreach(QVariantMap d, genus_devils)
			{
				if( !d.value("fusion2").toBool() )
					continue;

				int id = d.value("id").toInt();

				devils[id] = d;
				level_map[d.value("lvl").toInt()] = id;
			}
		}

		QList<int> levels = level_map.keys();
		qSort(levels);

		Q_ASSERT( level_map.values().contains(parent_id) );
		int parent_lvl = devils.value(parent_id).value("lvl").toInt();

		int index = levels.indexOf(parent_lvl);
		index += modifier == 0 ? 1 : -1;
		if(index < 0)
			index = 0;
		else if( index >= levels.count() )
			index = levels.count() - 1;

		index = levels.at(index);
		result_devil = devils.value( level_map.value(index) );
	}
	else
	{
		// Normal Fusion
		int result_genus = cache->resultGenus(devilGenusA, devilGenusB);
		if(result_genus < 1)
			return;

		int level_sum = devilA.value("lvl").toInt() +
			devilB.value("lvl").toInt();

		QList<QVariantMap> choices = cache->devilsByGenus(result_genus);
		foreach(QVariantMap choice, choices)
		{
			if( !checkLevelRange(level_sum, choice) )
				continue;

			result_devil = choice;
			break;
		}
	}

	if( result_devil.isEmpty() )
		return;

	QVariantMap result_data = fuseDevils(devilA, devilB, result_devil);

	QVariantMap devil_info = cache->devilByID(
		result_devil.value("id").toInt() );

	QPixmap icon( QString("icons/devils/icon_%1.png").arg(
		devil_info.value("icon").toString() ) );

	QString tooltip = cache->devilToolTip(result_devil);

	int max = mDevilLabels.count();

	int index = first * max + second;
	mDevilIconGrid.at(index)->setPixmap(icon);
	mDevilIconGrid.at(index)->setToolTip(tooltip);
	mFusionData[index] = result_data;

	index = second * max + first;
	mDevilIconGrid.at(index)->setPixmap(icon);
	mDevilIconGrid.at(index)->setToolTip(tooltip);
	mFusionData[index] = result_data;
}

bool FusionChart::checkLevelRange(int level, const QVariantMap& devil)
{
	if( !devil.value("fusion2").toBool() )
		return false;

	if(devil.value("fuse_min_lvl").toInt() > level)
		return false;

	int max_level = devil.value("fuse_max_lvl").toInt();
	if(max_level < level && max_level > 0)
		return false;

	return true;
}

QVariantMap FusionChart::fuseDevils(const QVariantMap& devilA,
	const QVariantMap& devilB, const QVariantMap& resultDevil)
{
	QVariantList default_skills = resultDevil.value("skills").toList();

	QVariantList skills;
	QList<int> skills_lookup;
	foreach(QVariant default_skill, default_skills)
	{
		QVariantMap skill = default_skill.toMap();
		if(skill.value("lvl").toInt() == -1)
		{
			skills << skill.value("skill");
			skills_lookup << skill.value("skill").toInt();
		}
	}

	while(skills.count() < 8)
		skills << -1;

	QVariantList inherited_skills = devilA.value("skills").toList();
	inherited_skills << devilB.value("skills").toList();

	QMutableListIterator<QVariant> it(inherited_skills);
	while( it.hasNext() )
	{
		int skill = it.next().toInt();
		if(skills_lookup.contains(skill) || skill <= 0)
			it.remove();

		skills_lookup << skill;
	}

	QVariantMap devil;
	devil["id"] = resultDevil.value("id").toInt();
	devil["lvl"] = resultDevil.value("lvl").toInt();
	devil["skills"] = skills;
	devil["inherited_skills"] = inherited_skills;
	devil["parents"] = QVariantList() << devilA << devilB;

	return devil;
}

void FusionChart::mouseReleaseEvent(QMouseEvent *evt)
{
	QLabel *item = qobject_cast<QLabel*>( childAt( evt->pos() ) );
	if(!item)
		return;

	int index = mDevilIconGrid.indexOf(item);
	if(index < 0)
		return;

	QVariantMap devil_data = mFusionData.at(index);

	int slot1 = index % mDevilLabels.count();
	int slot2 = (index - slot1) / mDevilLabels.count();
	slot1 = mDevilSlots.at(slot1);
	slot2 = mDevilSlots.at(slot2);

	mStorage->setAt(slot1 < slot2 ? slot1 : slot2, devil_data);
	mStorage->clearAt(slot1 < slot2 ? slot2 : slot1);

	close();
}
