/******************************************************************************\
*  client/src/DevilProperties.cpp                                              *
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

#include "DevilProperties.h"
#include "DevilCache.h"

#include <QtGui/QMouseEvent>
#include <QtGui/QDrag>

#ifndef QT_NO_DEBUG
#include <iostream>
#endif // QT_NO_DEBUG

static DevilProperties *g_devilproperties_inst = 0;

DevilProperties* DevilProperties::getSingletonPtr()
{
	if(!g_devilproperties_inst)
		g_devilproperties_inst = new DevilProperties;

	Q_ASSERT(g_devilproperties_inst != 0);

	return g_devilproperties_inst;
}

DevilProperties::DevilProperties(QWidget *parent_widget) :
	QWidget(parent_widget), mActiveSlot(-1), mActiveStorage(0)
{
	ui.setupUi(this);

	darkenWidget(ui.activeLabel);
	darkenWidget(ui.learnedLabel);
	darkenWidget(ui.inheritedLabel);

	mActiveSkills << ui.active1 << ui.active2 << ui.active3 << ui.active4
		<< ui.active5 << ui.active6 << ui.active7 << ui.active8;

	int max = mActiveSkills.count();
	for(int i = 0; i < max; i++)
		mActiveIDs << -1;

	mLearnedSkills << ui.learned1 << ui.learned2 << ui.learned3 << ui.learned4
		<< ui.learned5 << ui.learned6 << ui.learned7 << ui.learned8
		<< ui.learned9 << ui.learned10 << ui.learned11 << ui.learned12
		<< ui.learned13 << ui.learned14 << ui.learned15 << ui.learned16
		<< ui.learned17 << ui.learned18 << ui.learned19 << ui.learned20
		<< ui.learned21 << ui.learned22 << ui.learned23 << ui.learned24;

	max = mLearnedSkills.count();
	for(int i = 0; i < max; i++)
		mLearnedIDs << -1;

	mInheritedSkills  << ui.inherited1 << ui.inherited2 << ui.inherited3
		<< ui.inherited4 << ui.inherited5 << ui.inherited6 << ui.inherited7
		<< ui.inherited8 << ui.inherited9 << ui.inherited10 << ui.inherited11
		<< ui.inherited12 << ui.inherited13 << ui.inherited14 << ui.inherited15
		<< ui.inherited16 << ui.inherited17 << ui.inherited18 << ui.inherited19
		<< ui.inherited20 << ui.inherited21 << ui.inherited22 << ui.inherited23
		<< ui.inherited24;

	max = mInheritedSkills.count();
	for(int i = 0; i < max; i++)
		mInheritedIDs << -1;

	connect(ui.level, SIGNAL(valueChanged(int)),
		this, SLOT(updateLearnedSkills()));

	connect(ui.historyTree, SIGNAL(itemSelectionChanged()),
		this, SLOT(historySelectionChanged()));
	connect(ui.extractButton, SIGNAL(clicked(bool)),
		this, SLOT(extractHistory()));

	ui.historyTree->header()->hide();

	setAcceptDrops(true);
	historySelectionChanged();
}

void DevilProperties::setActiveDevil(StorageBase *storage, int slot,
	const DevilData& devil_data)
{
	mActiveSlot = slot;
	mActiveStorage = storage;
	mData = devil_data;

	DevilCache *cache = DevilCache::getSingletonPtr();

	QVariantMap info = cache->devilByID(
		mData.value("id").toInt() );

	QPixmap icon( QString("icons/devils/icon_%1.png").arg(
		info.value("icon").toString() ) );

	int lvl = mData.value("lvl").toInt();

	QMap<int, QString> lncEnum;
	lncEnum[0] = tr("Law");
	lncEnum[1] = tr("Neutral");
	lncEnum[2] = tr("Chaos");

	ui.icon->setPixmap(icon);
	ui.name->setText( info.value("name").toString() );
	ui.genus->setText( info.value("genus").toString() );
	ui.lnc->setText( lncEnum.value( info.value("lnc").toInt() ) );
	bool blocked = ui.level->blockSignals(true);
	ui.level->setMinimum(lvl);
	ui.level->setValue(lvl);
	ui.level->blockSignals(blocked);

	setWindowTitle( tr("%1 - %2 Properties").arg(
		tr("Absolutely Frosty") ).arg( info.value("name").toString() ) );

	QVariantList skills = mData.value("skills").toList();
	QListIterator<QVariant> it(skills);

	QPixmap blank(":/border.png");

	// Clear out the skill icons first
	int max = mActiveSkills.count();
	for(int i = 0; i < max; i++)
	{
		mActiveSkills.at(i)->setPixmap(blank);
		mActiveSkills.at(i)->setToolTip(QString());
		mActiveIDs[i] = -1;
	}

	max = mInheritedSkills.count();
	for(int i = 0; i < max; i++)
	{
		mInheritedSkills.at(i)->setPixmap(blank);
		mInheritedSkills.at(i)->setToolTip(QString());
		mInheritedIDs[i] = -1;
	}

	int i = 0;
	while( it.hasNext() )
	{
		int skill_id = it.next().toInt();

		QVariantMap skill_info = cache->skillByID(skill_id);
		if( !skill_info.isEmpty() )
		{
			QPixmap skill_icon( QString("icons/skills/icon_%1.png").arg(
				skill_info.value("icon").toString() ) );

			mActiveSkills.at(i)->setPixmap(skill_icon);
			mActiveSkills.at(i)->setToolTip( cache->skillToolTip(skill_id) );
			mActiveIDs[i] = skill_id;
		}

		i++;
	}

	skills = mData.value("inherited_skills").toList();
	it = skills;

	i = 0;
	while( it.hasNext() )
	{
		int skill_id = it.next().toInt();

		QVariantMap skill_info = cache->skillByID(skill_id);

		QPixmap skill_icon( QString("icons/skills/icon_%1.png").arg(
			skill_info.value("icon").toString() ) );

		mInheritedSkills.at(i)->setPixmap(skill_icon);
		mInheritedSkills.at(i)->setToolTip( cache->skillToolTip(skill_id) );
		mInheritedIDs[i] = skill_id;

		i++;
	}

	updateLearnedSkills();
	updateHistory();
}

void DevilProperties::updateLearnedSkills()
{
	DevilCache *cache = DevilCache::getSingletonPtr();

	QVariantMap info = cache->devilByID(
		mData.value("id").toInt() );

	// Update the demon's level
	int lvl = ui.level->value();

	QPixmap blank(":/border.png");

	int max = mLearnedSkills.count();
	for(int i = 0; i < max; i++)
	{
		mLearnedSkills.at(i)->setPixmap(blank);
		mLearnedSkills.at(i)->setToolTip(QString());
		mLearnedIDs[i] = -1;
	}

	// Learned skills
	QVariantList skills = info.value("skills").toList();
	QListIterator<QVariant> it(skills);

	int i = 0;
	while( it.hasNext() )
	{
		QVariantMap map = it.next().toMap();

		int skill_id = map.value("skill").toInt();
		int skill_lvl = map.value("lvl").toInt();
		if(skill_lvl < 1 || skill_lvl > lvl)
			continue;

		if( mActiveIDs.contains(skill_id) || mInheritedIDs.contains(skill_id) )
			continue;

		QVariantMap skill_info = cache->skillByID(skill_id);

		QPixmap skill_icon( QString("icons/skills/icon_%1.png").arg(
			skill_info.value("icon").toString() ) );

		mLearnedSkills.at(i)->setPixmap(skill_icon);
		mLearnedSkills.at(i)->setToolTip( cache->skillToolTip(skill_id) );
		mLearnedIDs[i] = skill_id;

		i++;
	}

	// If the level changed, we should update the devil info
	if(mData.value("lvl").toInt() != lvl)
	{
		bool blocked = ui.level->blockSignals(true);
		ui.level->setMinimum(lvl);
		ui.level->setValue(lvl);
		ui.level->blockSignals(blocked);
		mData["lvl"] = lvl;

		mActiveStorage->setAt(mActiveSlot, mData);
	}
}

void DevilProperties::updateHistory()
{
	ui.historyTree->clear();

	DevilCache *cache = DevilCache::getSingletonPtr();

	QVariantMap info = cache->devilByID(
		mData.value("id").toInt() );

	QString text = info.value("name").toString();
	int level_base = info.value("lvl").toInt();
	int level_current = mData.value("lvl").toInt();
	if(level_base == level_current)
	{
		text = tr("%1 (%2)").arg(text).arg(level_base);
	}
	else
	{
		text = tr("%1 (%2=>%3)").arg(text).arg(
			level_base).arg(level_current);
	}

	QTreeWidgetItem *parent;
	parent = new QTreeWidgetItem;
	parent->setText(0, text);
	parent->setData(0, Qt::UserRole, mData);
	parent->setToolTip(0, cache->devilToolTip(mData));

	updateHistoryParent(parent, mData);

	ui.historyTree->addTopLevelItem(parent);
	ui.historyTree->expandAll();
}

void DevilProperties::updateHistoryParent(QTreeWidgetItem *parent,
	const DevilData& data)
{
	DevilCache *cache = DevilCache::getSingletonPtr();

	QVariantList list = data.value("parents").toList();
	QListIterator<QVariant> it(list);

	while( it.hasNext() )
	{
		QVariantMap parent_data = it.next().toMap();

		QVariantMap info = cache->devilByID( parent_data.value("id").toInt() );

		QString text = info.value("name").toString();
		int level_base = info.value("lvl").toInt();
		int level_current = parent_data.value("lvl").toInt();
		if(level_base == level_current)
		{
			text = tr("%1 (%2)").arg(text).arg(level_base);
		}
		else
		{
			text = tr("%1 (%2=>%3)").arg(text).arg(
				level_base).arg(level_current);
		}

		QTreeWidgetItem *child;
		child = new QTreeWidgetItem;
		child->setText(0, text);
		child->setData(0, Qt::UserRole, parent_data);
		child->setToolTip(0, cache->devilToolTip(parent_data));

		updateHistoryParent(child, parent_data);

		parent->addChild(child);
	}
}

void DevilProperties::historySelectionChanged()
{
	if(!mActiveStorage || mActiveStorage->count() >= mActiveStorage->capacity() )
	{
		ui.extractButton->setEnabled(false);
		return;
	}

	QList<QTreeWidgetItem*> items = ui.historyTree->selectedItems();
	if( items.isEmpty() )
	{
		ui.extractButton->setEnabled(false);
		return;
	}

	ui.extractButton->setEnabled(true);
}

void DevilProperties::extractHistory()
{
	if(!mActiveStorage || mActiveStorage->count() >= mActiveStorage->capacity() )
		return;

	QList<QTreeWidgetItem*> items = ui.historyTree->selectedItems();
	if( items.isEmpty() )
		return;

	int index = -1;

	// TODO: Make a function to do this for me
	// and update all code that does it this way
	int max = mActiveStorage->capacity();
	for(int i = 0; i < max; i++)
	{
		if( mActiveStorage->devilAt(i).isEmpty() )
		{
			index = i;
			break;
		}
	}

	if(index < 0)
		return;

	mActiveStorage->setAt(index, items.first()->data(0, Qt::UserRole).toMap());

	historySelectionChanged();
}

void DevilProperties::darkenWidget(QWidget *widget)
{
	QPalette dark_palette = palette();
	dark_palette.setBrush(QPalette::Window,
		dark_palette.brush(QPalette::Base));

	QLabel *label = qobject_cast<QLabel*>(widget);
	if(label)
		label->setMargin(5);
#ifndef QT_NO_DEBUG
	else
		std::cout << tr("darkenWidget(%1) is not a QLabel").arg(
			widget->objectName()).toLocal8Bit().data() << std::endl;
#endif // QT_NO_DEBUG

	widget->setAutoFillBackground(true);
	widget->setPalette(dark_palette);
}

void DevilProperties::mousePressEvent(QMouseEvent *evt)
{
	if(evt->buttons() & Qt::LeftButton)
		mDragStartPosition = evt->pos();
}

void DevilProperties::mouseMoveEvent(QMouseEvent *evt)
{
	// TODO: Move the event pos() from here, into the mousePressEvent

	if( !(evt->buttons() & Qt::LeftButton) )
		return;

	if( (evt->pos() - mDragStartPosition).manhattanLength()
		< QApplication::startDragDistance() )
			return;

	QLabel *skill = qobject_cast<QLabel*>( childAt( evt->pos() ) );
	if(!skill)
		return;

	int index = -1;
	int skill_id = -1;
	int pool = -1;

	if( (index = mActiveSkills.indexOf(skill)) >= 0 )
	{
		skill_id = mActiveIDs.at(index);
		pool = 0;
	}
	else if( (index = mLearnedSkills.indexOf(skill)) >= 0 )
	{
		skill_id = mLearnedIDs.at(index);
		pool = 1;
	}
	else if( (index = mInheritedSkills.indexOf(skill)) >= 0 )
	{
		skill_id = mInheritedIDs.at(index);
		pool = 2;
	}
	else
	{
		return;
	}

	if(skill_id < 1)
		return;

	QPixmap pixmap = *skill->pixmap();

	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);
	dataStream << skill_id << index << pool;

	QMimeData *mimeData = new QMimeData;
	mimeData->setData("application/x-frosty-devil-skill", itemData);

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);
	drag->setPixmap(pixmap);
	drag->setHotSpot(evt->pos() - skill->mapTo(this, QPoint(0,0)));
	drag->exec();
}

void DevilProperties::dragEnterEvent(QDragEnterEvent *evt)
{
	if( evt->mimeData()->hasFormat("application/x-frosty-devil-skill") )
		evt->acceptProposedAction();
}

void DevilProperties::dragMoveEvent(QDragMoveEvent *evt)
{
	bool sourceThis = (evt->source() == this);

	QLabel *skill = qobject_cast<QLabel*>( childAt( evt->pos() ) );

	if( !evt->mimeData()->hasFormat("application/x-frosty-devil-skill") ||
		!skill || !sourceThis)
	{
		evt->ignore();
		return;
	}

	if( !mActiveSkills.contains(skill) )
	{
		evt->ignore();
		return;
	}

	QByteArray itemData = evt->mimeData()->data(
		"application/x-frosty-devil-skill");
	QDataStream dataStream(&itemData, QIODevice::ReadOnly);

	int skill_id, index, pool;
	dataStream >> skill_id >> index >> pool;

	if(pool == 0 && mActiveSkills.at(index) == skill)
	{
		evt->ignore();
		return;
	}

	evt->acceptProposedAction();
}

void DevilProperties::dropEvent(QDropEvent *evt)
{
	bool sourceThis = (evt->source() == this);

	QLabel *skill = qobject_cast<QLabel*>( childAt( evt->pos() ) );

	if( !evt->mimeData()->hasFormat("application/x-frosty-devil-skill") ||
		!skill || !sourceThis)
	{
		evt->ignore();
		return;
	}

	if( !mActiveSkills.contains(skill) )
	{
		evt->ignore();
		return;
	}

	QByteArray itemData = evt->mimeData()->data(
		"application/x-frosty-devil-skill");
	QDataStream dataStream(&itemData, QIODevice::ReadOnly);

	int skill_id, index, pool;
	dataStream >> skill_id >> index >> pool;

	if(pool == 0 && mActiveSkills.at(index) == skill)
	{
		evt->ignore();
		return;
	}

	evt->acceptProposedAction();

	QPixmap blank(":/border.png");

	int target_index = mActiveSkills.indexOf(skill);

	QPixmap old_icon = *skill->pixmap();
	QString old_tooltip = skill->toolTip();
	int old_id = mActiveIDs.value(target_index);

	mActiveIDs[target_index] = skill_id;

	QLabel *src = 0;
	switch(pool)
	{
		case 0:
			src = mActiveSkills.at(index);
			mActiveIDs[index] = old_id;
			break;
		case 1:
			src = mLearnedSkills.at(index);
			mLearnedIDs[index] = -1;
			break;
		case 2:
			src = mInheritedSkills.at(index);
			mInheritedIDs[index] = -1;
			break;
	}

	skill->setPixmap( *src->pixmap() );
	skill->setToolTip( src->toolTip() );

	if(pool == 0)
	{
		src->setPixmap(old_icon);
		src->setToolTip(old_tooltip);
	}
	else
	{
		rewindSkills(index, pool == 1 ? &mLearnedSkills : &mInheritedSkills,
			pool == 1 ? &mLearnedIDs : &mInheritedIDs);

		//src->setPixmap(blank);
		//src->setToolTip(QString());
	}

	QVariantList skills;
	foreach(int id, mActiveIDs)
	{
		if(id > 0)
			skills << id;
	}

	mData["skills"] = skills;

	skills.clear();
	foreach(int id, mInheritedIDs)
	{
		if(id > 0)
			skills << id;
	}

	mData["inherited_skills"] = skills;

	mActiveStorage->setAt(mActiveSlot, mData);
}

void DevilProperties::rewindSkills(int start,
	QList<QLabel*> *icons, QList<int> *ids)
{
	int max = icons->count() - 1;

	QPixmap blank(":/border.png");
	QLabel *skill = 0;

	int index;
	for(index = start; index < max; index++)
	{
		if(ids->at(index + 1) == -1)
			break;

		QLabel *src = skill = icons->at(index + 1);
		ids->replace(index, ids->at(index + 1));

		skill = icons->at(index);
		skill->setPixmap( *src->pixmap() );
		skill->setToolTip( src->toolTip() );
	}

	skill = icons->at(index);
	skill->setPixmap(blank);
	skill->setToolTip(QString());
	ids->replace(index, -1);
}
