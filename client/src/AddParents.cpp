/******************************************************************************\
*  client/src/AddParents.cpp                                                   *
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

#include "AddParents.h"
#include "DevilCache.h"

FusionCombo::FusionCombo(const QVariantMap& devil1, const QVariantMap& devil2,
	int id1, int id2, bool canUseBaseLevel, const GenusPair& pair,
	const QString& name_a, const QString& name_b,
	int level_a, int level_b)
{
	d = new FusionComboData;

	d->a = devil1;
	d->b = devil2;
	d->id_a = id1;
	d->id_b = id2;
	d->baseLevel = canUseBaseLevel;
	d->pair = pair;
	d->name_a = name_a;
	d->name_b = name_b;
	d->level_a = level_a;
	d->level_b = level_b;
}

QVariantMap FusionCombo::a() const
{
	return d->a;
}

QVariantMap FusionCombo::b() const
{
	return d->b;
}

int FusionCombo::id_a() const
{
	return d->id_a;
}

int FusionCombo::id_b() const
{
	return d->id_b;
}

int FusionCombo::level_a() const
{
	return d->level_a;
}

int FusionCombo::level_b() const
{
	return d->level_b;
}

bool FusionCombo::isBaseLevel() const
{
	return d->baseLevel;
}

GenusPair FusionCombo::pair() const
{
	return d->pair;
}

QString FusionCombo::name_a() const
{
	return d->name_a;
}

QString FusionCombo::name_b() const
{
	return d->name_b;
}

AddParents::AddParents(int target_id, QWidget *parent) : QDialog(parent)
{
	QHash<GenusPair, QString> test;
	test[GenusPair(1, 2)] = tr("lol");

	ui.setupUi(this);

	setWindowTitle( tr("%1 - Add Parents").arg( tr("Absolutely Frosty") ) );

	setWindowModality(Qt::WindowModal);
	setAttribute(Qt::WA_DeleteOnClose);

	connect(ui.genusA, SIGNAL(itemSelectionChanged()),
		this, SLOT(genusChangedA()));
	connect(ui.genusB, SIGNAL(itemSelectionChanged()),
		this, SLOT(genusChangedB()));

	connect(ui.devilsA, SIGNAL(itemSelectionChanged()),
		this, SLOT(devilsChangedA()));
	connect(ui.devilsB, SIGNAL(itemSelectionChanged()),
		this, SLOT(devilsChangedB()));

	connect(ui.levelA, SIGNAL(valueChanged(int)), this, SLOT(updateLevels()));
	connect(ui.levelB, SIGNAL(valueChanged(int)), this, SLOT(updateLevels()));

	connect(ui.cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));
	connect(ui.addButton, SIGNAL(clicked(bool)), this, SLOT(finish()));

	connect(ui.limitLevels, SIGNAL(stateChanged(int)),
		this, SLOT(genusChangedA()));

	mSkillsA << ui.skillA1 << ui.skillA2 << ui.skillA3 << ui.skillA4
		<< ui.skillA5 << ui.skillA6 << ui.skillA7 << ui.skillA8;
	mSkillsB << ui.skillB1 << ui.skillB2 << ui.skillB3 << ui.skillB4
		<< ui.skillB5 << ui.skillB6 << ui.skillB7 << ui.skillB8;

	DevilCache *cache = DevilCache::getSingletonPtr();

	{
		QVariantMap info = cache->devilByID(target_id);

		mTargetGenus = info.value("genus_id").toInt();
		mFuseRange.min = info.value("fuse_min_lvl").toInt();
		mFuseRange.max = info.value("fuse_max_lvl").toInt();

		QPixmap devil_icon( QString("icons/devils/icon_%1.png").arg(
			info.value("icon").toString() ) );

		ui.iconC->setPixmap(devil_icon);
		ui.nameC->setText( info.value("name").toString() );
		ui.levelC->setText( tr("%1 (%2-%3)").arg(
			info.value("lvl").toInt() ).arg(
			mFuseRange.min).arg(mFuseRange.max) );
	}

	QList<int> genusList;

	int seireiGenus = cache->seireiGenus();

	QHash<GenusPair, int> genus_combos = cache->fusionCombos();
	QHashIterator<GenusPair, int> it(genus_combos);
	while( it.hasNext() )
	{
		it.next();

		int genus_a = it.key().a;
		int genus_b = it.key().b;
		int result = it.value();

		if(result != mTargetGenus)
			continue;

		if(seireiGenus == genus_a || seireiGenus == genus_b)
			continue;

		QList<QVariantMap> devils_a = cache->devilsByGenus(genus_a);
		QList<QVariantMap> devils_b = cache->devilsByGenus(genus_b);
		if( devils_a.isEmpty() || devils_b.isEmpty() )
			continue;

		GenusPair pair(genus_a, genus_b);

		QList<FusionCombo> combos;

		QList<FusionCombo> byGenusA = mCombosByGenus.value(genus_a);
		QList<FusionCombo> byGenusB = mCombosByGenus.value(genus_b);

		QListIterator<QVariantMap> it_a(devils_a);
		while( it_a.hasNext() )
		{
			QVariantMap devil_a = it_a.next();

			int id_a = devil_a.value("id").toInt();
			int level_a = devil_a.value("lvl").toInt();

			QString name_a = devil_a.value("name").toString();

			QListIterator<QVariantMap> it_b(devils_b);
			while( it_b.hasNext() )
			{
				QVariantMap devil_b = it_b.next();

				int id_b = devil_b.value("id").toInt();
				int level_b = devil_b.value("lvl").toInt();
				int level_total = level_a + level_b;

				QString name_b = devil_b.value("name").toString();

				bool baseLevel = false;
				if(level_total > mFuseRange.max)
					continue; // Too high of a level to fuse this one
				else if(level_total >= mFuseRange.min)
					baseLevel = true; // This combo works w/o leveling up

				// Save the combo
				FusionCombo combo(devil_a, devil_b, id_a, id_b,
					baseLevel, pair, name_a, name_b, level_a, level_b);

				combos << combo;
				byGenusA << combo;
				byGenusB << FusionCombo(devil_b, devil_a, id_b, id_a,
					baseLevel, GenusPair(pair.b, pair.a),
					name_b, name_a, level_b, level_a);
			}
		}

		if( !combos.isEmpty() )
		{
			mCombos[pair] = combos;
			mCombosByGenus[genus_a] = byGenusA;
			mCombosByGenus[genus_b] = byGenusB;

			if(genusList.indexOf(genus_a) < 0)
				genusList << genus_a;
			if(genusList.indexOf(genus_b) < 0)
				genusList << genus_b;
		}
	}

	{
		QListWidgetItem *item = new QListWidgetItem( tr("All") );
		item->setData(Qt::UserRole, -1);

		ui.genusA->clear();
		ui.genusB->clear();
		ui.genusA->addItem(item);
		ui.genusB->addItem(item->clone());
	}

	// Using a QMap, our genus list will be sorted by name
	QMap<QString, QListWidgetItem*> items;
	foreach(int genus, genusList)
	{
		QList<QVariantMap> devils = cache->devilsByGenus(genus);
		QString genus_name = devils.first().value("genus").toString();

		QListWidgetItem *item = new QListWidgetItem(genus_name);
		item->setData(Qt::UserRole, genus);

		items[genus_name] = item;
	}

	foreach(QListWidgetItem *item, items.values())
	{
		ui.genusA->addItem(item);
		ui.genusB->addItem(item->clone());
	}

	// Select 'All'
	ui.genusA->item(0)->setSelected(true);

	// Clear the right side
	updateCombo();
}

void AddParents::genusChangedA()
{
	genusChanged(ui.genusA, ui.genusB);
	processDevils();
}

void AddParents::genusChangedB()
{
	genusChanged(ui.genusB, ui.genusA);
	processDevils();
}

void AddParents::genusChanged(QListWidget *a, QListWidget *b)
{
	QList<QListWidgetItem*> items = a->selectedItems();
	if( items.isEmpty() )
		return;

	QListWidgetItem *item = items.first();
	if(!item)
		return;

	if(a->item(0) == item)
	{
		for(int i = 1; i < a->count(); i++)
			a->item(i)->setHidden(false);
		for(int i = 1; i < b->count(); i++)
			b->item(i)->setHidden(false);

		bool blocked = b->blockSignals(true);
		b->setCurrentRow(0);
		b->blockSignals(blocked);

		return;
	}

	DevilCache *cache = DevilCache::getSingletonPtr();

	int genus = item->data(Qt::UserRole).toInt();
	for(int i = 1; i < b->count(); i++)
	{
		int other = b->item(i)->data(Qt::UserRole).toInt();
		bool valid = (cache->resultGenus(genus, other) == mTargetGenus);
		b->item(i)->setHidden(!valid);
	}

	items = b->selectedItems();
	if( items.isEmpty() )
		return;

	item = items.first();
	if(!item)
		return;

	if( item->isHidden() )
	{
		bool blocked = b->blockSignals(true);
		b->setCurrentRow(0);
		b->blockSignals(blocked);
	}
}

void AddParents::processDevils()
{
	int genus_a = -1, genus_b = -1;

	{
		QList<QListWidgetItem*> items = ui.genusA->selectedItems();
		if( !items.isEmpty() )
		{
			QListWidgetItem *item = items.first();
			if(item)
				genus_a = item->data(Qt::UserRole).toInt();
		}
	}

	{
		QList<QListWidgetItem*> items = ui.genusB->selectedItems();
		if( !items.isEmpty() )
		{
			QListWidgetItem *item = items.first();
			if(item)
				genus_b = item->data(Qt::UserRole).toInt();
		}
	}

	// Save selection
	int id_a = -1, id_b = -1;
	{
		QList<QListWidgetItem*> items = ui.devilsA->selectedItems();
		if( !items.isEmpty() )
		{
			QListWidgetItem *item = items.first();
			if(item)
				id_a = item->data(Qt::UserRole).toMap().value("id").toInt();
		}
	}

	{
		QList<QListWidgetItem*> items = ui.devilsB->selectedItems();
		if( !items.isEmpty() )
		{
			QListWidgetItem *item = items.first();
			if(item)
				id_b = item->data(Qt::UserRole).toMap().value("id").toInt();
		}
	}

	// Block everything
	bool block_a = ui.devilsA->blockSignals(true);
	bool block_b = ui.devilsB->blockSignals(true);

	{
		QListWidgetItem *item = new QListWidgetItem( tr("All") );
		item->setData(Qt::UserRole, -1);

		ui.devilsA->clear();
		ui.devilsB->clear();
		ui.devilsA->addItem(item);
		ui.devilsB->addItem(item->clone());
	}

	bool limitLevels = ui.limitLevels->checkState() == Qt::Checked;
	bool flip = false;

	QList<FusionCombo> combos;

	if(genus_a < 0 && genus_b < 0)
	{
		// Show all demons
		QHashIterator< GenusPair, QList<FusionCombo> > it(mCombos);
		while( it.hasNext() )
		{
			it.next();

			QList<FusionCombo> set = it.value();
			QListIterator<FusionCombo> inside(set);
			while( inside.hasNext() )
			{
				FusionCombo combo = inside.next();
				combos << combo;

				GenusPair pair(combo.pair().b, combo.pair().a);

				// Create a combo of the same thing but flipped
				combos << FusionCombo(combo.b(), combo.a(), combo.id_b(),
					combo.id_a(), combo.isBaseLevel(), pair,
					combo.name_b(), combo.name_a(),
					combo.level_b(), combo.level_a());
			}
		}
	}
	else if(genus_a < 0 || genus_b < 0)
	{
		// Only one genus is known
		combos = mCombosByGenus.value(
			genus_a < 0 ? genus_b : genus_a);

		if(genus_a < 0)
			flip = combos.first().pair().b != genus_b;
		else
			flip = combos.first().pair().a != genus_a;
	}
	else
	{
		// Show both
		Q_ASSERT( mCombos.contains( GenusPair(genus_a, genus_b) ) );

		combos = mCombos.value( GenusPair(genus_a, genus_b) );
		Q_ASSERT( !combos.isEmpty() );

		flip = combos.first().pair().a == genus_b;
	}

	if( !combos.isEmpty() )
	{
		QListWidget *widgetA = ui.devilsA, *widgetB = ui.devilsB;
		if(flip)
		{
			int temp = id_a;
			id_a = id_b;
			id_b = temp;
			widgetA = ui.devilsB;
			widgetB = ui.devilsA;
		}

		QMap<QString, QVariantMap> listA, listB;
		QListIterator<FusionCombo> it(combos);
		while( it.hasNext() )
		{
			FusionCombo combo = it.next();
			if( limitLevels && !combo.isBaseLevel() )
				continue;

			QString keyA = tr("%1_%2").arg(combo.level_a(), 3, 10,
				QLatin1Char('0')).arg(combo.name_a());
			QString keyB = tr("%1_%2").arg(combo.level_b(), 3, 10,
				QLatin1Char('0')).arg(combo.name_b());

			if( !listA.contains(keyA) )
				listA[keyA] = combo.a();
			if( !listB.contains(keyB) )
				listB[keyB] = combo.b();
		}

		QMapIterator<QString, QVariantMap> it_a(listA);
		while( it_a.hasNext() )
		{
			it_a.next();

			QVariantMap data = it_a.value();

			QListWidgetItem *item = new QListWidgetItem( tr("%1 (%2)").arg(
				data.value("name").toString() ).arg(
				data.value("lvl").toInt() ) );
			item->setData(Qt::UserRole, data);

			widgetA->addItem(item);

			int id = data.value("id").toInt();
			if(id == id_a)
				item->setSelected(true);
		}

		QMapIterator<QString, QVariantMap> it_b(listB);
		while( it_b.hasNext() )
		{
			it_b.next();

			QVariantMap data = it_b.value();

			QListWidgetItem *item = new QListWidgetItem( tr("%1 (%2)").arg(
				data.value("name").toString() ).arg(
				data.value("lvl").toInt() ) );
			item->setData(Qt::UserRole, data);

			widgetB->addItem(item);

			int id = data.value("id").toInt();
			if(id == id_b)
				item->setSelected(true);
		}
	}

	if( ui.devilsA->selectedItems().isEmpty() ||
		ui.devilsA->selectedItems().first()->isHidden() )
			ui.devilsA->item(0)->setSelected(true);
	if( ui.devilsB->selectedItems().isEmpty() ||
		ui.devilsB->selectedItems().first()->isHidden() )
			ui.devilsB->item(0)->setSelected(true);

	ui.devilsA->blockSignals(block_a);
	ui.devilsB->blockSignals(block_b);

	// Check the combo
	devilsChangedA();
}

void AddParents::devilsChangedA()
{
	devilsChanged(ui.devilsA, ui.devilsB);
	updateCombo();
}

void AddParents::devilsChangedB()
{
	devilsChanged(ui.devilsB, ui.devilsA);
	updateCombo();
}

void AddParents::devilsChanged(QListWidget *a, QListWidget *b)
{
	QList<QListWidgetItem*> items = a->selectedItems();
	if( items.isEmpty() )
		return;

	QListWidgetItem *item = items.first();
	if(!item)
		return;

	if(a->item(0) == item)
	{
		for(int i = 1; i < a->count(); i++)
			a->item(i)->setHidden(false);
		for(int i = 1; i < b->count(); i++)
			b->item(i)->setHidden(false);

		bool blocked = b->blockSignals(true);
		b->setCurrentRow(0);
		b->blockSignals(blocked);

		return;
	}

	DevilCache *cache = DevilCache::getSingletonPtr();

	QVariantMap devil = item->data(Qt::UserRole).toMap();

	int level_a = devil.value("lvl").toInt();
	int id_a = devil.value("genus_id").toInt();

	for(int i = 1; i < b->count(); i++)
	{
		QVariantMap other = b->item(i)->data(Qt::UserRole).toMap();
		int level_b = other.value("lvl").toInt();
		int level_total = level_a + level_b;

		bool valid = level_total <= mFuseRange.max;
		if(ui.limitLevels->checkState() == Qt::Checked)
			valid &= level_total >= mFuseRange.min;

		int id_b = other.value("genus_id").toInt();
		if(cache->resultGenus(id_a, id_b) != mTargetGenus)
			valid = false;

		b->item(i)->setHidden(!valid);
	}

	items = b->selectedItems();
	if( items.isEmpty() )
		return;

	item = items.first();
	if(!item)
		return;

	if( item->isHidden() )
	{
		bool blocked = b->blockSignals(true);
		b->setCurrentRow(0);
		b->blockSignals(blocked);
	}
}

void AddParents::updateCombo()
{
	bool good = true;
	good &= updateCombo(ui.devilsA, ui.iconA, ui.nameA, ui.levelA, mSkillsA);
	good &= updateCombo(ui.devilsB, ui.iconB, ui.nameB, ui.levelB, mSkillsB);

	// Adjust level if need be
	int total_level = ui.levelA->value() + ui.levelB->value();
	if( good && !mFuseRange.inRange(total_level) &&
		ui.limitLevels->checkState() != Qt::Checked )
	{
		int extra = mFuseRange.min - total_level;
		int half = (extra / 2) + (extra % 2);

		if( (ui.levelA->maximum() - ui.levelA->value()) < half ||
			(ui.levelB->maximum() - ui.levelB->value()) < half )
		{
			ui.levelA->setValue( ui.levelA->maximum() );
			ui.levelB->setValue( mFuseRange.min - ui.levelA->value() );
		}
		else
		{
			ui.levelA->setValue(ui.levelA->value() + half);
			ui.levelB->setValue(ui.levelB->value() +
				((half * 2) == extra ? half : (half - 1)));
		}
	}

	if(good)
	{
		QVariantMap a = ui.devilsA->selectedItems().first()->data(Qt::UserRole).toMap();

		QVariantList skills, default_skills = a.value("skills").toList();
		foreach(QVariant default_skill, default_skills)
		{
			QVariantMap skill = default_skill.toMap();
			if(skill.value("lvl").toInt() == -1)
				skills << skill.value("skill");
		}

		while(skills.count() < 8)
			skills << -1;

		mParentA["id"] = a.value("id");
		mParentA["lvl"] = ui.levelA->value();
		mParentA["skills"] = skills;
		mParentA["parents"] = QVariantList();

		QVariantMap b = ui.devilsB->selectedItems().first()->data(Qt::UserRole).toMap();

		skills.clear();
		default_skills = b.value("skills").toList();
		foreach(QVariant default_skill, default_skills)
		{
			QVariantMap skill = default_skill.toMap();
			if(skill.value("lvl").toInt() == -1)
				skills << skill.value("skill");
		}

		while(skills.count() < 8)
			skills << -1;

		mParentB["id"] = b.value("id");
		mParentB["lvl"] = ui.levelB->value();
		mParentB["skills"] = skills;
		mParentB["parents"] = QVariantList();
	}

	updateLevels();
}

void AddParents::finish()
{
	emit children(QList<QVariantMap>() << mParentA << mParentB);
	close();
}

void AddParents::updateLevels()
{
	int total_level = ui.levelA->value() + ui.levelB->value();
	if( mFuseRange.inRange(total_level) && ui.levelA->value() > 0 &&
		ui.levelB->value() > 0)
	{
		ui.levelC->setStyleSheet(QString());
		ui.addButton->setEnabled(true);
	}
	else
	{
		ui.levelC->setStyleSheet("color: red");
		ui.addButton->setEnabled(false);
	}
}

bool AddParents::updateCombo(QListWidget *list, QLabel *icon, QLabel *name,
	QSpinBox *level, const QList<QLabel*> skills)
{
	QPixmap blank(":/border.png");

	// Clear existing combo
	icon->setPixmap(blank);
	name->clear();
	level->setMinimum(0);
	level->setMaximum(0);
	level->setEnabled(false);
	foreach(QLabel *skill, skills)
	{
		skill->setPixmap(blank);
		skill->setToolTip(QString());
	}

	QList<QListWidgetItem*> items = list->selectedItems();
	if( items.isEmpty() )
		return false;

	QListWidgetItem *item = items.first();
	if(!item)
		return false;

	// Set the new combo
	QVariantMap data = item->data(Qt::UserRole).toMap();

	int id = data.value("id").toInt();
	if(id <= 0)
		return false;

	int lvl = data.value("lvl").toInt();

	QPixmap devil_icon( QString("icons/devils/icon_%1.png").arg(
		data.value("icon").toString() ) );

	icon->setPixmap(devil_icon);
	name->setText( data.value("name").toString() );
	level->setMinimum(lvl);
	level->setMaximum(90);
	level->setValue(lvl);

	level->setEnabled(ui.limitLevels->checkState() != Qt::Checked);

	DevilCache *cache = DevilCache::getSingletonPtr();

	// Show skills
	QVariantList skill_ids = data.value("skills").toList();
	QListIterator<QVariant> it(skill_ids);

	int i = 0;
	while( it.hasNext() )
	{
		QVariantMap map = it.next().toMap();
		int skill_id = map.value("skill").toInt();
		if(skill_id < 0)
			continue;

		if( map.value("lvl").toInt() > lvl )
			continue;

		QVariantMap skill_info = cache->skillByID(skill_id);

		QPixmap skill_icon( QString("icons/skills/icon_%1.png").arg(
			skill_info.value("icon").toString() ) );

		skills.at(i)->setPixmap(skill_icon);
		skills.at(i)->setToolTip( cache->skillToolTip(skill_id) );

		i++;
	}

	return true;
}
