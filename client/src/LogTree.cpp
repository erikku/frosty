/******************************************************************************\
*  client/src/LogTree.cpp                                                      *
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

#include "LogTree.h"
#include "json.h"

#include <QtGui/QTreeWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QSplitter>

LogTree::LogTree(QWidget *parent) : QWidget(parent)
{
	mTree = new QTreeWidget;
	mTree->headerItem()->setHidden(true);

	mValueLabel = new QLabel;
	mValueLabel->setWordWrap(true);
	mValueLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	mValueLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse |
		Qt::TextSelectableByMouse);

	QVBoxLayout *groupLayout = new QVBoxLayout;
	groupLayout->addWidget(mValueLabel);

	QGroupBox *groupBox = new QGroupBox( tr("Value") );
	groupBox->setLayout(groupLayout);

	QSplitter *mainSplitter = new QSplitter;
	mainSplitter->addWidget(mTree);
	mainSplitter->addWidget(groupBox);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(mainSplitter);

	setLayout(mainLayout);

	connect(mTree, SIGNAL(itemSelectionChanged()),
		this, SLOT(updateValueLabel()));

	connect(mTree, SIGNAL(itemExpanded(QTreeWidgetItem*)),
		this, SLOT(resizeToContents()));

	connect(mTree, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
		this, SLOT(resizeToContents()));
};

void LogTree::loadJSON(const QVariant& json)
{
	mTree->clear();
	mValueLabel->clear();

	if( json.isNull() )
		return;

	mTree->addTopLevelItems( json::toTree(json) );
	mTree->resizeColumnToContents(0);
	mTree->expandAll();
};

void LogTree::resizeToContents()
{
	mTree->resizeColumnToContents(0);
};

void LogTree::updateValueLabel()
{
	if( !mTree->selectedItems().count() )
		return;

	QTreeWidgetItem *item = mTree->selectedItems().first();
	mValueLabel->setText( item->data(0, Qt::UserRole).toString() );
};
