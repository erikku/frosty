/******************************************************************************\
*  client/src/AjaxView.cpp                                                     *
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

#include "AjaxView.h"

#include <QtCore/QFileInfo>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QLineEdit>
#include <QtGui/QTextEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QStackedWidget>
#include <QtGui/QButtonGroup>
#include <QtGui/QRadioButton>
#include <QtGui/QMessageBox>
#include <QtGui/QPalette>
#include <iostream>

#include "Settings.h"
#include "IconEdit.h"
#include "IconSelect.h"
#include "RelationList.h"
#include "ajax.h"

Q_DECLARE_METATYPE(AjaxView::BindType);
Q_DECLARE_METATYPE(QButtonGroup*);
Q_DECLARE_METATYPE(QPushButton*);
Q_DECLARE_METATYPE(QComboBox*);
Q_DECLARE_METATYPE(QCheckBox*);
Q_DECLARE_METATYPE(QLineEdit*);
Q_DECLARE_METATYPE(QTextEdit*);
Q_DECLARE_METATYPE(QSpinBox*);
Q_DECLARE_METATYPE(QLabel*);
Q_DECLARE_METATYPE(IconEdit*);
Q_DECLARE_METATYPE(IconSelect*);
Q_DECLARE_METATYPE(RelationList*);
Q_DECLARE_METATYPE(RadioButtonMap);

AjaxView::AjaxView(QWidget *parent) : QWidget(parent), mID(-1)
{
	ui.stackedWidget = 0;

	ui.editButton = 0;
	ui.cancelButton = 0;
	ui.refreshButton = 0;
	ui.updateButton = 0;
};

void AjaxView::initView(QStackedWidget *stackedWidget, QPushButton *editButton,
	QPushButton *cancelButton, QPushButton *refreshButton,
	QPushButton *updateButton)
{
	ui.stackedWidget = stackedWidget;

	ui.editButton = editButton;
	ui.cancelButton = cancelButton;
	ui.refreshButton = refreshButton;
	ui.updateButton = updateButton;

	connect(editButton, SIGNAL(clicked(bool)), this, SLOT(edit()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancel()));
	connect(refreshButton, SIGNAL(clicked(bool)), this, SLOT(refresh()));
	connect(updateButton, SIGNAL(clicked(bool)), this, SLOT(update()));

	updateButton->setVisible(false);
	cancelButton->setVisible(false);
	editButton->setVisible(false);

	ajax::getSingletonPtr()->subscribe(this);
	setEnabled(false);
	refresh();
};

void AjaxView::darkenWidget(QWidget *widget)
{
	QPalette dark_palette = palette();
	dark_palette.setBrush(QPalette::Window,
		dark_palette.brush(QPalette::Base));

	QLabel *label = qobject_cast<QLabel*>(widget);
	if(label)
		label->setMargin(5);
	else
		std::cout << tr("darkenWidget(%1) is not a QLabel").arg(
			widget->objectName()).toLocal8Bit().data() << std::endl;

	widget->setAutoFillBackground(true);
	widget->setPalette(dark_palette);
};

bool AjaxView::checkValues()
{
	return true;
};

void AjaxView::update()
{
	if( !checkValues() )
		return;

	setEnabled(false);

	ajax::getSingletonPtr()->request(settings->url(), createUpdateAction());

	cancel();
	clear();
};

void AjaxView::clear()
{
	QMapIterator<QString, QVariantMap> i(mBinds);

	QVariantMap row;
	QVariantList columns;

	mID = -1;

	while( i.hasNext() )
	{
		i.next();

		QString field = i.key();
		QVariantMap map = i.value();

		QLabel *view = map.value("view", 0).value<QLabel*>();
		Q_ASSERT(view != 0);

		view->clear();

		switch( map.value("type", AjaxView::BindText).value<AjaxView::BindType>() )
		{
			case BindText:
			{
				QLineEdit *edit = map.value("edit", 0).value<QLineEdit*>();
				Q_ASSERT(edit != 0);

				edit->clear();

				break;
			}
			case BindTextBox:
			{
				QTextEdit *edit = map.value("edit", 0).value<QTextEdit*>();
				Q_ASSERT(edit != 0);

				edit->clear();

				break;
			}
			case BindIcon:
			{
				IconEdit *edit = map.value("edit", 0).value<IconEdit*>();
				Q_ASSERT(edit != 0);

				view->setPixmap( QPixmap(":/blank.png") );
				edit->setPixmap( QPixmap(":/blank.png") );

				break;
			}
			case BindNumber:
			{
				QSpinBox *edit = map.value("edit", 0).value<QSpinBox*>();
				Q_ASSERT(edit != 0);

				edit->setValue( map.value("default").toInt() );

				break;
			}
			case BindNumberRange:
			{
				QSpinBox *from = map.value("from", 0).value<QSpinBox*>();
				Q_ASSERT(from != 0);

				from->setValue( map.value("min").toInt() );

				QSpinBox *to = map.value("to", 0).value<QSpinBox*>();
				Q_ASSERT(to != 0);

				to->setValue( map.value("to").toInt() );

				break;
			}
			case BindEnum:
			{
				QComboBox *edit = map.value("edit", 0).value<QComboBox*>();
				Q_ASSERT(edit != 0);

				edit->setCurrentIndex( edit->findData( map.value("default") ) );

				break;
			}
			case BindBool:
			{
				QCheckBox *edit = map.value("edit", 0).value<QCheckBox*>();
				Q_ASSERT(edit != 0);

				edit->setCheckState(Qt::Unchecked);

				break;
			}
			case BindRelation:
			{
				// We don't really want to do this because they
				// don't always get re-loaded after calling clear()

				//QComboBox *edit = map.value("edit", 0).value<QComboBox*>();
				//Q_ASSERT(edit != 0);

				// edit->clear();
				//if( map.contains("none_name") )
					//edit->addItem(map.value("none_name").toString(), 0);

				break;
			}
			case BindNumberSet:
			{
				int defaultValue = map.value("default").toInt();

				QSpinBox *edit = map.value("edit", 0).value<QSpinBox*>();
				Q_ASSERT(edit != 0);

				bool block = edit->blockSignals(true);
				edit->setValue(defaultValue);
				edit->blockSignals(block);

				QStringList columns = map.value("columns").toStringList();

				QVariantMap data;
				foreach(QString column, columns)
					data[column] = defaultValue;

				// Save the defaults
				map["data"] = data;
				mBinds[field] = map;

				QLabel *view = map.value("view", 0).value<QLabel*>();
				Q_ASSERT(view != 0);

				view->clear();

				break;
			}
			case BindNumberSelector:
			{
				int defaultValue = map.value("default").toInt();

				QSpinBox *edit = map.value("edit", 0).value<QSpinBox*>();
				Q_ASSERT(edit != 0);

				bool block = edit->blockSignals(true);
				edit->setValue(defaultValue);
				edit->blockSignals(block);

				QStringList columns = map.value("columns").toStringList();

				QVariantMap data;
				foreach(QString column, columns)
					data[column] = defaultValue;

				// Save the defaults
				map["data"] = data;
				map["static_data"] = data;
				mBinds[field] = map;

				QLabel *view = map.value("view", 0).value<QLabel*>();
				Q_ASSERT(view != 0);

				view->clear();

				break;
			}
			default:
			{
				Q_ASSERT("default in bind switch not allowed!" && false);
				break;
			}
		}
	}
};

void AjaxView::refresh()
{
	setEnabled(false);

	// Calling clear will reset mID to -1
	// so we need to save the id first
	int id = mID;

	clear();
	if(id > 0)
		view(id, false);
};

void AjaxView::view(int id, bool switchView)
{
	if(switchView)
		cancel();

	clear();
	setEnabled(true);

	if(id <= 0)
		return;

	mID = id;

	ajax::getSingletonPtr()->request(settings->url(), createViewAction());

	QMapIterator<QString, QVariantMap> i(mBinds);

	while( i.hasNext() )
	{
		i.next();

		QString field = i.key();
		QVariantMap map = i.value();

		if( map.value("type", AjaxView::BindText).value<AjaxView::BindType>()
			!= AjaxView::BindRelation)
				continue;

		refreshRelationCache(field);
	}

	setEnabled(false);
};

void AjaxView::edit()
{
	refresh();
	ui.editButton->setVisible(false);
	ui.refreshButton->setVisible(false);
	ui.updateButton->setVisible(true);
	ui.cancelButton->setVisible(true);
	ui.stackedWidget->setCurrentIndex(1);
};

void AjaxView::cancel()
{
	ui.editButton->setVisible(true);
	ui.refreshButton->setVisible(true);
	ui.updateButton->setVisible(false);
	ui.cancelButton->setVisible(false);
	ui.stackedWidget->setCurrentIndex(0);

	if(mID < 0)
		setEnabled(false);
};

void AjaxView::add()
{
	if( isEditing() )
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this,
			addWarningTitle(), addWarningMessage(),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

		if(button == QMessageBox::No)
			return;
	}

	mID = -1;

	clear();
	ui.editButton->setVisible(false);
	ui.refreshButton->setVisible(false);
	ui.updateButton->setVisible(true);
	ui.cancelButton->setVisible(true);
	ui.stackedWidget->setCurrentIndex(1);
	setEnabled(true);
};

bool AjaxView::isEditing() const
{
	return (ui.stackedWidget->currentIndex() == 1);
};

void AjaxView::updateNumberSetValues()
{
	QSpinBox *edit = qobject_cast<QSpinBox*>( sender() );
	Q_ASSERT(edit != 0);

	// Find the number set bind entry
	QMapIterator<QString, QVariantMap> i(mBinds);

	while( i.hasNext() )
	{
		i.next();

		QString field = i.key();
		QVariantMap map = i.value();

		if( map.value("type", AjaxView::BindText).value<AjaxView::BindType>()
			!= AjaxView::BindNumberSet ||
			map.value("edit", 0).value<QSpinBox*>() != edit )
				continue;

		QVariantMap data = map.value("data").toMap();
		data[map.value("current").toString()] = edit->value();

		// Save the change
		map["data"] = data;
		mBinds[field] = map;
	}
};

void AjaxView::updateNumberSetSelection()
{
	QComboBox *selector = qobject_cast<QComboBox*>( sender() );
	Q_ASSERT(selector != 0);

	if( !selector->count() )
		return;

	// Find the number set bind entry
	QMapIterator<QString, QVariantMap> i(mBinds);

	while( i.hasNext() )
	{
		i.next();

		QString field = i.key();
		QVariantMap map = i.value();

		if( map.value("type", AjaxView::BindText).value<AjaxView::BindType>()
			!= AjaxView::BindNumberSet ||
			map.value("selector", 0).value<QComboBox*>() != selector )
				continue;

		QSpinBox *edit = map.value("edit", 0).value<QSpinBox*>();
		Q_ASSERT(edit != 0);

		QVariantMap data = map.value("data").toMap();

		int index = selector->currentIndex();
		Q_ASSERT(index < data.count());

		map["current"] = map.value("columns").toStringList().at(index);

		// Save the change
		mBinds[field] = map;

		bool block = edit->blockSignals(true);
		edit->setValue( data.value(	map.value("current").toString() ).toInt() );
		edit->blockSignals(block);
	}
};

void AjaxView::browseRelationList()
{
	QPushButton *button = qobject_cast<QPushButton*>( sender() );
	Q_ASSERT(button != 0);

	// Find the relation bind entry
	QMapIterator<QString, QVariantMap> i(mBinds);

	while( i.hasNext() )
	{
		i.next();

		QString field = i.key();
		QVariantMap map = i.value();

		if( map.value("type", AjaxView::BindText).value<AjaxView::BindType>()
			!= AjaxView::BindRelation ||
			map.value("button", 0).value<QPushButton*>() != button )
				continue;

		RelationList *list = map.value("list", 0).value<RelationList*>();
		Q_ASSERT(list != 0);

		list->showList( map.value("title").toString(),
			map.value("table").toString() );
	}
};

void AjaxView::updateIcon(const QString& path, const QString& value)
{
	IconSelect *selector = qobject_cast<IconSelect*>( sender() );
	Q_ASSERT(selector != 0);

	// Find the icon bind entry
	QMapIterator<QString, QVariantMap> i(mBinds);

	while( i.hasNext() )
	{
		i.next();

		QString field = i.key();
		QVariantMap map = i.value();

		if( map.value("type", AjaxView::BindText).value<AjaxView::BindType>()
			!= AjaxView::BindIcon ||
			map.value("selector", 0).value<IconSelect*>() != selector )
				continue;

		IconEdit *edit = map.value("edit", 0).value<IconEdit*>();
		Q_ASSERT(edit != 0);

		edit->setValue(value);
		edit->setPixmap(path);
	}
};

void AjaxView::showIconSelect()
{
	IconEdit *edit = qobject_cast<IconEdit*>( sender() );
	Q_ASSERT(edit != 0);

	// Find the icon bind entry
	QMapIterator<QString, QVariantMap> i(mBinds);

	while( i.hasNext() )
	{
		i.next();

		QString field = i.key();
		QVariantMap map = i.value();

		if( map.value("type", AjaxView::BindText).value<AjaxView::BindType>()
			!= AjaxView::BindIcon || map.value("edit", 0).value<IconEdit*>()
			!= edit )
				continue;

		IconSelect *selector = map.value("selector", 0).value<IconSelect*>();
		Q_ASSERT(selector != 0);

		// TODO: Pass a way to convert the path into a value needed to insert
		// the data into the table
		selector->selectIcon( map.value("searchPath").toString() );
	}
};

void AjaxView::updateRelationCombo()
{
	QComboBox *edit = qobject_cast<QComboBox*>( sender() );
	Q_ASSERT(edit != 0);

	// Find the relation bind entry
	QMapIterator<QString, QVariantMap> i(mBinds);

	while( i.hasNext() )
	{
		i.next();

		QString field = i.key();
		QVariantMap map = i.value();

		if( map.value("type", AjaxView::BindText).value<AjaxView::BindType>()
			!= AjaxView::BindRelation ||
			map.value("edit", 0).value<QComboBox*>() != edit )
				continue;

		map["last_id"] = edit->itemData( edit->currentIndex() );

		// Save the changes
		mBinds[field] = map;
	}
};

void AjaxView::refreshRelationCombo()
{
	RelationList *list = qobject_cast<RelationList*>( sender() );
	Q_ASSERT(list != 0);

	// Find the relation bind entry
	QMapIterator<QString, QVariantMap> i(mBinds);

	while( i.hasNext() )
	{
		i.next();

		QString field = i.key();
		QVariantMap map = i.value();

		if( map.value("type", AjaxView::BindText).value<AjaxView::BindType>()
			!= AjaxView::BindRelation ||
			map.value("list", 0).value<RelationList*>() != list )
				continue;

		refreshRelationCache(field);
	}
};

void AjaxView::bindText(const QString& field, QLabel *view, QLineEdit *edit)
{
	QVariantMap bindInfo;
	bindInfo["view"] = qVariantFromValue(view);
	bindInfo["edit"] = qVariantFromValue(edit);
	bindInfo["type"] = qVariantFromValue(AjaxView::BindText);

	mBinds[field] = bindInfo;
};

void AjaxView::bindTextBox(const QString& field, QLabel *view, QTextEdit *edit)
{
	QVariantMap bindInfo;
	bindInfo["view"] = qVariantFromValue(view);
	bindInfo["edit"] = qVariantFromValue(edit);
	bindInfo["type"] = qVariantFromValue(AjaxView::BindTextBox);

	mBinds[field] = bindInfo;
};

void AjaxView::bindIcon(const QString& field, QLabel *view, IconEdit *edit,
	const QString& path, const QString& searchPath)
{
	IconSelect *selector = new IconSelect;
	Q_ASSERT(selector != 0);

	QVariantMap bindInfo;
	bindInfo["path"] = path;
	bindInfo["searchPath"] = searchPath;
	bindInfo["view"] = qVariantFromValue(view);
	bindInfo["edit"] = qVariantFromValue(edit);
	bindInfo["selector"] = qVariantFromValue(selector);
	bindInfo["type"] = qVariantFromValue(AjaxView::BindIcon);

	mBinds[field] = bindInfo;

	connect(selector, SIGNAL(iconReady(const QString&, const QString&)),
		this, SLOT(updateIcon(const QString&, const QString&)));

	connect(edit, SIGNAL(iconDoubleClicked()), this, SLOT(showIconSelect()));

	view->setPixmap( QPixmap(":/blank.png") );
	edit->setPixmap( QPixmap(":/blank.png") );
};

void AjaxView::bindNumber(const QString& field, QLabel *view, QSpinBox *edit,
	int defaultValue, const QString& pattern)
{
	QVariantMap bindInfo;
	bindInfo["default"] = defaultValue;
	bindInfo["view"] = qVariantFromValue(view);
	bindInfo["edit"] = qVariantFromValue(edit);
	bindInfo["type"] = qVariantFromValue(AjaxView::BindNumber);

	if( !pattern.isEmpty() )
		bindInfo["pattern"] = pattern;

	mBinds[field] = bindInfo;
};

void AjaxView::bindNumberRange(const QString& field, QLabel *view,
	QSpinBox *from, QSpinBox *to, const QString& minColumn,
	const QString& maxColumn, int defaultMin, int defaultMax)
{
	QVariantMap bindInfo;
	bindInfo["view"] = qVariantFromValue(view);
	bindInfo["from"] = qVariantFromValue(from);
	bindInfo["to"] = qVariantFromValue(to);
	bindInfo["min"] = defaultMin;
	bindInfo["max"] = defaultMax;
	bindInfo["min_column"] = minColumn;
	bindInfo["max_column"] = maxColumn;
	bindInfo["type"] = qVariantFromValue(AjaxView::BindNumberRange);

	mBinds[field] = bindInfo;
};

void AjaxView::bindEnum(const QString& field, QLabel *view, QComboBox *edit,
	const QMap<int, QString>& items, int defaultValue)
{
	QVariantMap bindInfo;
	bindInfo["default"] = defaultValue;
	bindInfo["view"] = qVariantFromValue(view);
	bindInfo["edit"] = qVariantFromValue(edit);
	bindInfo["type"] = qVariantFromValue(AjaxView::BindEnum);

	QMapIterator<int, QString> i = items;

	while( i.hasNext() )
	{
		i.next();
		edit->addItem( i.value(), i.key() );
	}

	mBinds[field] = bindInfo;
};

void AjaxView::bindBool(const QString& field, QLabel *view, QCheckBox *edit,
	const QString& yes, const QString& no)
{
	QVariantMap bindInfo;
	bindInfo["view"] = qVariantFromValue(view);
	bindInfo["edit"] = qVariantFromValue(edit);
	bindInfo["type"] = qVariantFromValue(AjaxView::BindBool);

	if( !yes.isEmpty() && !no.isEmpty() )
	{
		bindInfo["yes"] = yes;
		bindInfo["no"] = no;
	}

	mBinds[field] = bindInfo;
};

void AjaxView::bindRelation(const QString& field, QLabel *view, QComboBox *edit,
	const QString& table, const QString& column, QPushButton *browseButton,
	const QString& listTitle, const QString& noneName,
	const QVariantList& filters)
{
	RelationList *list = new RelationList;
	Q_ASSERT(list != 0);

	QVariantMap bindInfo;
	bindInfo["view"] = qVariantFromValue(view);
	bindInfo["edit"] = qVariantFromValue(edit);
	bindInfo["table"] = table;
	bindInfo["last_id"] = 0;
	bindInfo["filled"] = false;
	bindInfo["column"] = column;
	bindInfo["title"] = listTitle;
	bindInfo["cache"] = QVariantMap();

	if( !noneName.isEmpty() )
	{
		bindInfo["none_name"] = noneName;

		edit->addItem(noneName, 0);
	}

	if(browseButton)
	{
		bindInfo["list"] = qVariantFromValue(list);
		bindInfo["button"] = qVariantFromValue(browseButton);
	}

	if( !filters.isEmpty() )
		bindInfo["filters"] = filters;

	bindInfo["type"] = qVariantFromValue(AjaxView::BindRelation);

	mBinds[field] = bindInfo;

	connect(edit, SIGNAL(currentIndexChanged(int)),
		this, SLOT(updateRelationCombo()));

	if(browseButton)
	{
		connect(list, SIGNAL(relationsUpdated()),
			this, SLOT(refreshRelationCombo()));

		connect(browseButton, SIGNAL(clicked(bool)),
			this, SLOT(browseRelationList()));
	}

	refreshRelationCache(field);
};

// For this one the feild value is just used for keeping track of the number set
void AjaxView::bindNumberSet(const QString& field, QLabel *view, QSpinBox *edit,
	QComboBox *selector, const QStringList& columns,
	const QStringList& patterns, const QString& separator, int defaultValue)
{
	Q_ASSERT(selector->count() && !columns.isEmpty());
	Q_ASSERT(selector->count() == columns.count());
	Q_ASSERT(selector->count() == patterns.count());

	QVariantMap data;
	foreach(QString column, columns)
		data[column] = defaultValue;

	QVariantMap bindInfo;
	bindInfo["data"] = data;
	bindInfo["columns"] = columns;
	bindInfo["patterns"] = patterns;
	bindInfo["separator"] = separator;
	bindInfo["default"] = defaultValue;
	bindInfo["view"] = qVariantFromValue(view);
	bindInfo["edit"] = qVariantFromValue(edit);
	bindInfo["selector"] = qVariantFromValue(selector);
	bindInfo["current"] = columns.first();
	bindInfo["type"] = qVariantFromValue(AjaxView::BindNumberSet);

	mBinds[field] = bindInfo;

	connect(edit, SIGNAL(valueChanged(int)),
		this, SLOT(updateNumberSetValues()));

	connect(selector, SIGNAL(currentIndexChanged(int)),
		this, SLOT(updateNumberSetSelection()));
};

// For this one the feild value is just used for keeping track of the number set
void AjaxView::bindNumberSelector(const QString& field, QLabel *view,
	QSpinBox *edit, const RadioButtonMap& selectors_and_columns,
	const RadioButtonMap& selector_edits_and_columns,
	QButtonGroup *group, QButtonGroup *edit_group, int defaultValue,
	QRadioButton *defaultSelector, QRadioButton *defaultEditSelector)
{
	QStringList columns = selectors_and_columns.values();
	QStringList columns_2 = selector_edits_and_columns.values();
	qSort(columns); qSort(columns_2);

	Q_ASSERT(columns == columns_2);
	Q_ASSERT( !columns.isEmpty() );

	// TODO: Make sure none of the radio buttons in the two sets are the same
	// or they will be removed from one group and added to the other, causing
	// chaos and bugs and destruction of the world as we know it!

	QVariantMap data;
	foreach(QString column, columns)
		data[column] = defaultValue;

	RadioButtonMap selectors = selectors_and_columns;
	selectors.unite(selector_edits_and_columns);

	QVariantMap bindInfo;
	bindInfo["data"] = data;
	bindInfo["static_data"] = data;
	bindInfo["columns"] = columns;
	bindInfo["default"] = defaultValue;
	bindInfo["view"] = qVariantFromValue(view);
	bindInfo["edit"] = qVariantFromValue(edit);
	bindInfo["selectors"] = qVariantFromValue(selectors);
	bindInfo["view_selectors"] = qVariantFromValue(selectors_and_columns);
	bindInfo["edit_selectors"] = qVariantFromValue(selector_edits_and_columns);

	bindInfo["view_current"] = defaultSelector ? selectors_and_columns.value(
		defaultSelector) : columns.first();
	bindInfo["edit_current"] = defaultEditSelector ?
		selector_edits_and_columns.value(defaultEditSelector) : columns.first();

	QButtonGroup *bg_group = group, *bg_edit_group = edit_group;
	if(!bg_group)
	{
		bg_group = new QButtonGroup;

		QList<QRadioButton*> buttons = selectors_and_columns.keys();
		foreach(QRadioButton *button, buttons)
			bg_group->addButton(button);
	}
	if(!bg_edit_group)
	{
		bg_edit_group = new QButtonGroup;

		QList<QRadioButton*> buttons = selector_edits_and_columns.keys();
		foreach(QRadioButton *button, buttons)
			bg_edit_group->addButton(button);
	}

	bindInfo["view_group"] = qVariantFromValue(bg_group);
	bindInfo["edit_group"] = qVariantFromValue(bg_edit_group);
	bindInfo["type"] = qVariantFromValue(AjaxView::BindNumberSelector);

	mBinds[field] = bindInfo;

	connect(bg_group, SIGNAL(buttonClicked(QAbstractButton*)),
		this, SLOT(updateNumberSelection()));
	connect(bg_edit_group, SIGNAL(buttonClicked(QAbstractButton*)),
		this, SLOT(updateNumberSelection()));
	connect(edit, SIGNAL(valueChanged(int)),
		this, SLOT(updateNumberSelectionValue()));

	// Set the default radio button to checked
	QString column = bindInfo.value("view_current").toString();
	selectors_and_columns.key(column)->setChecked(true);
	column = bindInfo.value("edit_current").toString();
	selector_edits_and_columns.key(column)->setChecked(true);
};

void AjaxView::updateNumberSelection()
{
	QButtonGroup *group = qobject_cast<QButtonGroup*>( sender() );
	Q_ASSERT(group);

	QMapIterator<QString, QVariantMap> i(mBinds);

	while( i.hasNext() )
	{
		i.next();

		QString field = i.key();
		QVariantMap map = i.value();

		if(map.value("type", AjaxView::BindText).value<AjaxView::BindType>()
			!= AjaxView::BindNumberSelector)
				continue;

		QRadioButton *radio = qobject_cast<QRadioButton*>(
			group->checkedButton() );
		Q_ASSERT(radio);

		if(map.value("view_group", 0).value<QButtonGroup*>() == group)
		{
			QVariantMap data = map.value("static_data").toMap();

			RadioButtonMap buttons = map.value(
				"view_selectors").value<RadioButtonMap>();

			QString column = buttons.value(radio);
			QLabel *view = map.value("view", 0).value<QLabel*>();
			Q_ASSERT(view);

			view->setText( QString::number( data.value(column).toInt() ) );

			map["view_current"] = column;
		}
		else if(map.value("edit_group", 0).value<QButtonGroup*>() == group)
		{
			QVariantMap data = map.value("data").toMap();

			RadioButtonMap buttons = map.value(
				"edit_selectors").value<RadioButtonMap>();

			QString column = buttons.value(radio);
			QSpinBox *edit = map.value("edit", 0).value<QSpinBox*>();
			Q_ASSERT(edit);

			bool block = edit->blockSignals(true);
			edit->setValue( data.value(column).toInt() );
			edit->blockSignals(block);

			map["edit_current"] = column;
		}
		else
		{
			continue;
		}

		// Save the change
		mBinds[field] = map;
	}
};

void AjaxView::updateNumberSelectionValue()
{
	QSpinBox *edit = qobject_cast<QSpinBox*>( sender() );
	Q_ASSERT(edit);

	QMapIterator<QString, QVariantMap> i(mBinds);

	while( i.hasNext() )
	{
		i.next();

		QString field = i.key();
		QVariantMap map = i.value();

		if( map.value("type", AjaxView::BindText).value<AjaxView::BindType>()
			!= AjaxView::BindNumberSelector ||
			map.value("edit", 0).value<QSpinBox*>() != edit )
				continue;

		QString column = map.value("edit_current").toString();
		QVariantMap data = map.value("data").toMap();

		data[column] = edit->value();

		// Save the change
		map["data"] = data;
		mBinds[field] = map;
	}
};

void AjaxView::refreshRelationCache(const QString& field)
{
	if( !mBinds.keys().contains(field) )
		return;

	QVariantMap bindInfo = mBinds.value(field);
	bindInfo["cache"] = QVariantMap();
	bindInfo["filled"] = false;

	QComboBox *edit = bindInfo.value("edit", 0).value<QComboBox*>();
	Q_ASSERT(edit != 0);

	edit->clear();

	{
		QString column_name = bindInfo.value("column").toString();
		if( column_name.isEmpty() )
			column_name = QString("name_%1").arg( settings->lang() );

		QVariantMap orderby_name;
		orderby_name["column"] = column_name;
		orderby_name["direction"] = "asc";

		QVariantMap orderby_id;
		orderby_id["column"] = "id";
		orderby_id["direction"] = "asc";

		QVariantList columns;
		columns << QString("id") << column_name;

		QVariantMap action;
		action["action"] = QString("select");
		action["table"] = bindInfo.value("table");
		action["columns"] = columns;
		action["order_by"] = QVariantList() << orderby_name << orderby_id;
		action["user_data"] = ( bindInfo.value("table").toString()
			+ QString("_relation_cache") );

		if( bindInfo.contains("filters") )
			action["where"] = bindInfo.value("filters");

		ajax::getSingletonPtr()->request(settings->url(), action);
	}

	// Save the data back
	mBinds[field] = bindInfo;
};

QVariantMap AjaxView::createViewAction() const
{
	// TODO: Make sure we don't have duplicates

	QVariantList columns;
	columns << QString("id");

	QMapIterator<QString, QVariantMap> i(mBinds);

	while( i.hasNext() )
	{
		i.next();

		QString field = i.key();
		QVariantMap map = i.value();

		switch( map.value("type", AjaxView::BindText).value<AjaxView::BindType>() )
		{
			case AjaxView::BindNumberRange:
				columns << map.value("min_column") << map.value("max_column");
				break;
			case AjaxView::BindNumberSet:
			case AjaxView::BindNumberSelector:
				columns << map.value("columns").toList();
				break;
			default:
				columns << field;
				break;
		}
	}

	QVariantMap where;
	where["type"] = "equals";
	where["column"] = "id";
	where["value"] = mID;

	QVariantMap action;
	action["action"] = QString("select");
	action["table"] = table();
	action["columns"] = columns;
	action["user_data"] = QString("%1_view").arg( table() );
	action["where"] = QVariantList() << where;

	return action;
};

QVariantMap AjaxView::createUpdateAction() const
{
	// TODO: Make sure we don't have duplicates

	QMapIterator<QString, QVariantMap> i(mBinds);

	QVariantMap row;
	QVariantList columns;

	while( i.hasNext() )
	{
		i.next();

		QString field = i.key();
		QVariantMap map = i.value();

		switch( map.value("type", AjaxView::BindText).value<AjaxView::BindType>() )
		{
			case AjaxView::BindText:
			{
				QLineEdit *edit = map.value("edit", 0).value<QLineEdit*>();
				Q_ASSERT(edit != 0);

				columns << field;
				row[field] = edit->text();

				break;
			}
			case AjaxView::BindTextBox:
			{
				QTextEdit *edit = map.value("edit", 0).value<QTextEdit*>();
				Q_ASSERT(edit != 0);

				columns << field;
				row[field] = edit->toPlainText();

				break;
			}
			case AjaxView::BindIcon:
			{
				IconEdit *edit = map.value("edit", 0).value<IconEdit*>();
				Q_ASSERT(edit != 0);

				columns << field;
				row[field] = edit->value();

				break;
			}
			case AjaxView::BindNumber:
			{
				QSpinBox *edit = map.value("edit", 0).value<QSpinBox*>();
				Q_ASSERT(edit != 0);

				columns << field;
				row[field] = edit->value();

				break;
			}
			case AjaxView::BindNumberRange:
			{
				QSpinBox *from = map.value("from", 0).value<QSpinBox*>();
				Q_ASSERT(from != 0);

				QSpinBox *to = map.value("to", 0).value<QSpinBox*>();
				Q_ASSERT(to != 0);

				columns << map.value("min_column") << map.value("max_column");
				row[map.value("min_column").toString()] = from->value();
				row[map.value("max_column").toString()] = to->value();

				break;
			}
			case AjaxView::BindEnum:
			{
				QComboBox *edit = map.value("edit", 0).value<QComboBox*>();
				Q_ASSERT(edit != 0);

				columns << field;
				row[field] = edit->itemData( edit->currentIndex() );

				break;
			}
			case AjaxView::BindBool:
			{
				QCheckBox *edit = map.value("edit", 0).value<QCheckBox*>();
				Q_ASSERT(edit != 0);

				columns << field;
				row[field] = edit->checkState() == Qt::Checked ? true : false;

				break;
			}
			case AjaxView::BindRelation:
			{
				QComboBox *edit = map.value("edit", 0).value<QComboBox*>();
				Q_ASSERT(edit != 0);

				columns << field;
				row[field] = edit->itemData( edit->currentIndex() );

				break;
			}
			case AjaxView::BindNumberSet:
			case AjaxView::BindNumberSelector:
			{
				columns << map.value("columns").toList();
				row = row.unite( map.value("data").toMap() );

				break;
			}
			default:
			{
				Q_ASSERT("default in bind switch not allowed!" && false);
				break;
			}
		}
	}

	QVariantMap action;

	if(mID > 0)
		action["action"] = QString("update");
	else
		action["action"] = QString("insert");

	action["table"] = table();
	action["columns"] = columns;

	QVariantList user_data;
	user_data << QString("%1_update").arg( table() );
	user_data << mID;

	action["user_data"] = user_data;

	if(mID > 0)
	{
		QVariantMap where;
		where["type"] = "equals";
		where["column"] = "id";
		where["value"] = mID;

		action["where"] = QVariantList() << where;
	}

	action["rows"] = QVariantList() << row;

	return action;
};

void AjaxView::processBindValues(const QVariantMap& values)
{
	QMapIterator<QString, QVariantMap> i(mBinds);

	while( i.hasNext() )
	{
		i.next();

		QString field = i.key();
		QVariantMap map = i.value();

		switch( map.value("type", AjaxView::BindText).value<AjaxView::BindType>() )
		{
			case AjaxView::BindText:
			{
				//std::cout << "AjaxView::BindText" << std::endl;

				QString text = values.value(field).toString();

				QLabel *view = map.value("view", 0).value<QLabel*>();
				Q_ASSERT(view != 0);

				view->setText(text);

				QLineEdit *edit = map.value("edit", 0).value<QLineEdit*>();
				Q_ASSERT(edit != 0);

				edit->setText(text);

				break;
			}
			case AjaxView::BindTextBox:
			{
				//std::cout << "AjaxView::BindTextBox" << std::endl;

				QString text = values.value(field).toString();

				QLabel *view = map.value("view", 0).value<QLabel*>();
				Q_ASSERT(view != 0);

				view->setText(text);

				QTextEdit *edit = map.value("edit", 0).value<QTextEdit*>();
				Q_ASSERT(edit != 0);

				edit->setPlainText(text);

				break;
			}
			case AjaxView::BindIcon:
			{
				//std::cout << "AjaxView::BindIcon" << std::endl;

				QLabel *view = map.value("view", 0).value<QLabel*>();
				Q_ASSERT(view != 0);

				IconEdit *edit = map.value("edit", 0).value<IconEdit*>();
				Q_ASSERT(edit != 0);

				QString icon_path = map.value("path").toString().arg(
					values.value(field).toString() );

				if( QFileInfo(icon_path).exists() )
				{
					view->setPixmap( QPixmap(icon_path) );

					edit->setValue( values.value(field).toString() );
					edit->setPixmap( QPixmap(icon_path) );
				}
				else
				{
					view->setPixmap( QPixmap(":/blank.png") );

					edit->setValue( QString() );
					edit->setPixmap( QPixmap(":/blank.png") );
				}

				break;
			}
			case AjaxView::BindNumber:
			{
				//std::cout << "AjaxView::BindNumber" << std::endl;

				int number = values.value(field).toInt();

				QLabel *view = map.value("view", 0).value<QLabel*>();
				Q_ASSERT(view != 0);

				if( number < 0 )
				{
					view->setText( tr("N/A") );
				}
				else
				{
					if( map.contains("pattern") )
					{
						view->setText(
							map.value("pattern").toString().arg(number) );
					}
					else
					{
						view->setText( QString::number(number) );
					}
				}

				QSpinBox *edit = map.value("edit", 0).value<QSpinBox*>();
				Q_ASSERT(edit != 0);

				edit->setValue(number);

				break;
			}
			case AjaxView::BindNumberRange:
			{
				//std::cout << "AjaxView::BindNumberRange" << std::endl;

				int min = values.value(
					map.value("min_column").toString() ).toInt();
				int max = values.value(
					map.value("max_column").toString() ).toInt();

				QLabel *view = map.value("view", 0).value<QLabel*>();
				Q_ASSERT(view != 0);

				view->setText( QString("%1 - %2").arg(min).arg(max) );

				QSpinBox *from = map.value("from", 0).value<QSpinBox*>();
				Q_ASSERT(from != 0);

				QSpinBox *to = map.value("to", 0).value<QSpinBox*>();
				Q_ASSERT(to != 0);

				from->setValue(min);
				to->setValue(max);

				break;
			}
			case AjaxView::BindEnum:
			{
				//std::cout << "AjaxView::BindEnum" << std::endl;

				int number = values.value(field).toInt();

				QComboBox *edit = map.value("edit", 0).value<QComboBox*>();
				Q_ASSERT(edit != 0);

				int index = edit->findData(number);
				if(index == -1)
					index = edit->findData( map.value("default") );

				edit->setCurrentIndex(index);

				QLabel *view = map.value("view", 0).value<QLabel*>();
				Q_ASSERT(view != 0);

				view->setText( edit->itemText(index) );

				break;
			}
			case AjaxView::BindBool:
			{
				//std::cout << "AjaxView::BindBool" << std::endl;

				bool cond = values.value(field).toBool();

				QLabel *view = map.value("view", 0).value<QLabel*>();
				Q_ASSERT(view != 0);

				if( map.contains("yes") )
				{
					view->setText( cond ? map.value("yes").toString() :
						map.value("no").toString() );
				}
				else
				{
					view->setText( cond ? tr("x") : tr("-") );
				}

				QCheckBox *edit = map.value("edit", 0).value<QCheckBox*>();
				Q_ASSERT(edit != 0);

				edit->setCheckState( cond ? Qt::Checked : Qt::Unchecked );

				break;
			}
			case AjaxView::BindRelation:
			{
				//std::cout << "AjaxView::BindRelation" << std::endl;

				int id = values.value(field).toInt();

				// Update the id
				map["last_id"] = id;
				mBinds[field] = map;

				if( map.value("filled").toBool() )
				{
					QString text = map.value("cache").toMap().key(id);

					QLabel *view = map.value("view", 0).value<QLabel*>();
					Q_ASSERT(view != 0);

					view->setText(text);

					QComboBox *edit = map.value("edit", 0).value<QComboBox*>();
					Q_ASSERT(edit != 0);

					edit->setCurrentIndex( edit->findData(id) );
				}

				break;
			}
			case AjaxView::BindNumberSet:
			{
				//std::cout << "AjaxView::BindNumberSet" << std::endl;

				QVariantMap data = map.value("data").toMap();
				QStringList columns = map.value("columns").toStringList();
				QStringList patterns = map.value("patterns").toStringList();
				QStringList final;

				int i = 0;
				foreach(QString column, columns)
				{
					int number = values.value(column).toInt();
					QString pattern = patterns.at(i);

					// Save the number
					data[column] = number;

					// If we have a number greater then 0, add it to the view
					if(number > 0)
						final << pattern.arg( QString::number(number) );

					i++;
				}

				// Save the data
				map["data"] = data;
				mBinds[field] = map;

				if( final.isEmpty() )
					final << tr("N/A");

				QLabel *view = map.value("view", 0).value<QLabel*>();
				Q_ASSERT(view != 0);

				view->setText(
					final.join( map.value("separator").toString() ) );

				QSpinBox *edit = map.value("edit", 0).value<QSpinBox*>();
				Q_ASSERT(edit != 0);

				bool block = edit->blockSignals(true);
				edit->setValue( data.value(
					map.value("current").toString() ).toInt() );
				edit->blockSignals(block);

				QComboBox *selector = map.value("selector", 0).value<QComboBox*>();
				Q_ASSERT(view != 0);

				// Select the first item that is > 0 (if there is one)
				i = 0;
				foreach(QString column, columns)
				{
					if(values.value(column).toInt() > 0)
					{
						selector->setCurrentIndex(i);
						break;
					}
					i++;
				}

				break;
			}
			case AjaxView::BindNumberSelector:
			{
				//std::cout << "AjaxView::BindNumberSelector" << std::endl;

				QVariantMap data = map.value("data").toMap();
				QStringList columns = map.value("columns").toStringList();

				foreach(QString column, columns)
					data[column] = values.value(column).toInt();

				// Save the data
				map["data"] = data;
				map["static_data"] = data;
				mBinds[field] = map;

				QLabel *view = map.value("view", 0).value<QLabel*>();
				Q_ASSERT(view != 0);

				QString column = map.value("view_current").toString();

				view->setText( QString::number( data.value(column).toInt() ) );

				QSpinBox *edit = map.value("edit", 0).value<QSpinBox*>();
				Q_ASSERT(edit != 0);

				column = map.value("edit_current").toString();

				bool block = edit->blockSignals(true);
				edit->setValue( data.value(column).toInt() );
				edit->blockSignals(block);

				break;
			}
			default:
			{
				Q_ASSERT("default in bind switch not allowed!" && false);
				break;
			}
		}
	}
};

void AjaxView::ajaxResponse(const QVariant& resp)
{
	QVariantMap result = resp.toMap();

	int updated_id = -1;
	QString user_data = result.value("user_data").toString();
	if( result.value("user_data").canConvert(QVariant::List) )
	{
		// Special case to recover the ID
		// of the item updated after a clear()
		QVariantList user_data_set = result.value("user_data").toList();
		if(user_data_set.count() == 2)
		{
			user_data = user_data_set.at(0).toString();
			updated_id = user_data_set.at(1).toInt();
		}
	}

	QString update_data = QString("%1_update").arg( table() );
	QString view_data = QString("%1_view").arg( table() );

	if(user_data == view_data)
	{
		QVariantList rows = result.value("rows").toList();
		if( rows.count() && rows.first().toMap().value("id") == mID )
			mSleepingResponse = result;
	}
	else if(user_data == update_data)
	{
		view(updated_id);

		emit viewChanged();

		return;
	}
	else if( user_data.right(15) == QString("_relation_cache") )
	{
		// Update a relation cache
		QString field_check = user_data;
		field_check.chop(15);

		QMapIterator<QString, QVariantMap> i(mBinds);

		while( i.hasNext() )
		{
			i.next();

			QString field = i.key();
			QVariantMap bindInfo = i.value();

			if( bindInfo.value("type", AjaxView::BindText).value<
				AjaxView::BindType>() != AjaxView::BindRelation ||
				bindInfo.value("table").toString() != field_check )
					continue;

			QString column_name = bindInfo.value("column").toString();
			if( column_name.isEmpty() )
				column_name = QString("name_%1").arg( settings->lang() );

			QComboBox *edit = bindInfo.value("edit", 0).value<QComboBox*>();
			Q_ASSERT(edit != 0);

			QVariantMap cache;
			QVariantList rows = result.value("rows").toList();

			edit->clear();

			if( bindInfo.contains("none_name") )
				edit->addItem(bindInfo.value("none_name").toString(), 0);

			int index = -1;

			for(int i = 0; i < rows.count(); i++)
			{
				QVariantMap map = rows.at(i).toMap();
				int id = map.value("id").toInt();

				cache[ map.value(column_name).toString() ] = id;
				edit->addItem( map.value(column_name).toString() );
				edit->setItemData(i, id);

				if( id == bindInfo.value("last_id") )
					index = i;
			}

			if(index != -1)
			{
				QLabel *view = bindInfo.value("view", 0).value<QLabel*>();
				Q_ASSERT(view != 0);

				view->setText( edit->itemText(index) );
				edit->setCurrentIndex(index);
			}

			bindInfo["cache"] = cache;
			bindInfo["filled"] = true;

			mBinds[field] = bindInfo;
		}
	}
	else if(user_data == "auth_query_perms")
	{
		if( result.value("perms").toMap().value("modify_db").toBool() )
		{
			Q_ASSERT(ui.editButton != 0);

			ui.editButton->setVisible(true);
		}
	}

	result = mSleepingResponse; // Check if we can now load the view data
	user_data = result.value("user_data").toString();

	if(user_data == view_data)
	{
		// Someone clicked too fast, ignore this request
		if( result.value("rows").toList().first().toMap().value("id") != mID )
		{
			mSleepingResponse = QVariantMap();

			return;
		}

		// Clear out the sleeping response so we don't do this again
		mSleepingResponse = QVariantMap();

		QVariantList rows = result.value("rows").toList();
		QVariantMap map = rows.at(0).toMap();

		processBindValues(map);
		setEnabled(true);
	}
};
