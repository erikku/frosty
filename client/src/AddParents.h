/******************************************************************************\
*  client/src/AddParents.h                                                     *
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

#ifndef __AddParents_h__
#define __AddParents_h__

#include "ui_AddParents.h"

#include "GenusPair.h"

#include <QtGui/QDialog>
#include <QtCore/QSharedDataPointer>

class FusionComboData : public QSharedData
{
public:
	bool baseLevel;
	int id_a, id_b;
	QVariantMap a, b;
	GenusPair pair;
	QString name_a, name_b;
	int level_a, level_b;
};

class FusionCombo
{
public:
	FusionCombo(const QVariantMap& devil1, const QVariantMap& devil2,
		int id1, int id2, bool canUseBaseLevel, const GenusPair& pair,
		const QString& name_a, const QString& name_b,
		int level_a, int level_b);

	int id_a() const;
	int id_b() const;

	int level_a() const;
	int level_b() const;

	GenusPair pair() const;

	QVariantMap a() const;
	QVariantMap b() const;

	bool isBaseLevel() const;

	QString name_a() const;
	QString name_b() const;

private:
	QSharedDataPointer<FusionComboData> d;
};

class Range
{
public:
	Range() : min(0), max(0) { }

	bool inRange(int val) { return ((val >= min) && (val <= max)); }

	int min, max;
};

class AddParents : public QDialog
{
	Q_OBJECT

public:
	AddParents(int target_id, QWidget *parent = 0);

signals:
	void children(const QList<QVariantMap>& list);

private slots:
	void genusChangedA();
	void genusChangedB();

	void devilsChangedA();
	void devilsChangedB();

	void updateLevels();
	void updateCombo();
	void finish();

protected:
	void processDevils();

	bool updateCombo(QListWidget *list, QLabel *icon, QLabel *name,
		QSpinBox *level, const QList<QLabel*> skills);

	void genusChanged(QListWidget *a, QListWidget *b);
	void devilsChanged(QListWidget *a, QListWidget *b);

	QHash< GenusPair, QList<FusionCombo> > mCombos;
	QHash< int, QList<FusionCombo> > mCombosByGenus;

	QList<QLabel*> mSkillsA, mSkillsB;

	QVariantMap mParentA, mParentB;

	int mTargetGenus;

	Range mFuseRange;
	Ui::AddParents ui;
};

#endif // __AddParents_h__
