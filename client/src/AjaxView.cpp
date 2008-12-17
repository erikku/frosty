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

#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QLineEdit>
#include <QtGui/QTextEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QMessageBox>

#ifndef QT_NO_DEBUG
#include <iostream>
#endif // QT_NO_DEBUG

#include "Settings.h"
#include "ajax.h"

#include "IconEdit.h"
#include "InfoListWidget.h"

#include "AjaxBind.h"
#include "BindText.h"
#include "BindTextBox.h"
#include "BindIcon.h"
#include "BindNumber.h"
#include "BindNumberRange.h"
#include "BindEnum.h"
#include "BindBool.h"
#include "BindRelation.h"
#include "BindNumberSet.h"
#include "BindNumberSelector.h"
#include "BindDetailedMultiRelation.h"

AjaxView::AjaxView(QWidget *parent_widget) : QWidget(parent_widget),
	mID(-1), mCanEdit(false)
{
	ui.stackedWidget = 0;

	ui.editButton = 0;
	ui.cancelButton = 0;
	ui.refreshButton = 0;
	ui.updateButton = 0;
}

void AjaxView::initView(QStackedWidget *stackedWidget, QPushButton *editButton,
	QPushButton *cancelButton, QPushButton *refreshButton,
	QPushButton *updateButton)
{
	ui.stackedWidget = stackedWidget;

	ui.editButton = editButton;
	ui.cancelButton = cancelButton;
	ui.refreshButton = refreshButton;
	ui.updateButton = updateButton;

	Q_ASSERT(stackedWidget && editButton && cancelButton &&
		refreshButton && updateButton);

	connect(editButton,    SIGNAL(clicked(bool)), this, SLOT(edit()));
	connect(cancelButton,  SIGNAL(clicked(bool)), this, SLOT(cancel()));
	connect(refreshButton, SIGNAL(clicked(bool)), this, SLOT(refresh()));
	connect(updateButton,  SIGNAL(clicked(bool)), this, SLOT(update()));

	updateButton->setVisible(false);
	cancelButton->setVisible(false);
	editButton->setVisible(false);

	ajax::getSingletonPtr()->subscribe(this);

	refresh();
}

void AjaxView::darkenWidget(QWidget *widget)
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

bool AjaxView::checkValues()
{
	return true;
}

void AjaxView::update()
{
	if( !checkValues() )
		return;

	setEnabled(false);

	QUrl url = settings->url();
	QVariantList update_actions = createUpdateActions();

	foreach(QVariant action, update_actions)
		ajax::getSingletonPtr()->request(url, action);

	cancel();
	clear();
}

void AjaxView::clear()
{
	foreach(AjaxBind *bind, mBinds)
		bind->clear();

	ui.updateButton->setText( QObject::tr("&Update") );

	mID = -1;
}

void AjaxView::refresh()
{
	setEnabled(false);

	// Calling clear will reset mID to -1
	// so we need to save the id first
	int id = mID;

	clear();
	if(id > 0)
		view(id, false);
}

void AjaxView::view(int id, bool switchView)
{
	if(switchView)
		cancel();

	clear();
	setEnabled(true);

	if(id <= 0)
		return;

	mID = id;

	QUrl url = settings->url();
	QVariantList view_actions = createViewActions();

	foreach(QVariant action, view_actions)
		ajax::getSingletonPtr()->request(url, action);

	setEnabled(false);
}

void AjaxView::edit()
{
	ui.editButton->setVisible(false);
	ui.refreshButton->setVisible(false);
	ui.updateButton->setVisible(true);
	ui.cancelButton->setVisible(true);
	ui.stackedWidget->setCurrentIndex(1);

	refresh();
}

void AjaxView::cancel()
{
	if(mCanEdit)
		ui.editButton->setVisible(true);

	ui.refreshButton->setVisible(true);
	ui.updateButton->setVisible(false);
	ui.cancelButton->setVisible(false);
	ui.stackedWidget->setCurrentIndex(0);

	if(mID < 0)
		setEnabled(false);
}

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

	ui.updateButton->setText( QObject::tr("&Add") );

	QListIterator<AjaxBind*> it(mBinds);
	while( it.hasNext() )
	{
		AjaxBind *bind = it.next();

		bind->addRequested();
	}

	ui.editButton->setVisible(false);
	ui.refreshButton->setVisible(false);
	ui.updateButton->setVisible(true);
	ui.cancelButton->setVisible(true);
	ui.stackedWidget->setCurrentIndex(1);
	setEnabled(true);
}

bool AjaxView::isEditing() const
{
	return (ui.stackedWidget->currentIndex() == 1);
}

void AjaxView::bindText(const QString& field, QLabel *view_widget,
	QLineEdit *edit_widget)
{
	BindText *bind = new BindText;
	bind->setViewer(view_widget);
	bind->setEditor(edit_widget);
	bind->setColumn(field);

	mBinds << bind;
}

void AjaxView::bindTextBox(const QString& field, QLabel *view_widget,
	QTextEdit *edit_widget)
{
	BindTextBox *bind = new BindTextBox;
	bind->setViewer(view_widget);
	bind->setEditor(edit_widget);
	bind->setColumn(field);

	mBinds << bind;
}

void AjaxView::bindIcon(const QString& field, QLabel *view_widget,
	IconEdit *edit_widget, const QString& path, const QString& searchPath)
{
	BindIcon *bind = new BindIcon(this);
	bind->setViewer(view_widget);
	bind->setEditor(edit_widget);
	bind->setColumn(field);

	bind->setPath(path);
	bind->setSearchPath(searchPath);

	mBinds << bind;
}

void AjaxView::bindNumber(const QString& field, QLabel *view_widget,
	QSpinBox *edit_widget, int defaultValue, const QString& pattern)
{
	BindNumber *bind = new BindNumber;
	bind->setViewer(view_widget);
	bind->setEditor(edit_widget);
	bind->setColumn(field);

	bind->setDefaultValue(defaultValue);
	bind->setPattern(pattern);

	mBinds << bind;
}

void AjaxView::bindNumberRange(const QString& field, QLabel *view_widget,
	QSpinBox *from, QSpinBox *to, const QString& minColumn,
	const QString& maxColumn, int defaultMin, int defaultMax, bool hideNegative)
{
	Q_UNUSED(field);

	BindNumberRange *bind = new BindNumberRange;
	bind->setViewer(view_widget);
	bind->setFromEditor(from);
	bind->setToEditor(to);

	bind->setFromColumn(minColumn);
	bind->setToColumn(maxColumn);

	bind->setDefaultMin(defaultMin);
	bind->setDefaultMax(defaultMax);
	bind->setHideNegative(hideNegative);

	mBinds << bind;
}

void AjaxView::bindEnum(const QString& field, QLabel *view_widget,
	QComboBox *edit_widget, const QMap<int, QString>& items, int defaultValue)
{
	BindEnum *bind = new BindEnum;
	bind->setViewer(view_widget);
	bind->setEditor(edit_widget);
	bind->setColumn(field);

	bind->setItems(items);
	bind->setDefaultValue(defaultValue);

	mBinds << bind;
}

void AjaxView::bindBool(const QString& field, QLabel *view_widget,
	QCheckBox *edit_widget, const QString& yes, const QString& no,
	bool defaultValue)
{
	BindBool *bind = new BindBool;
	bind->setViewer(view_widget);
	bind->setEditor(edit_widget);
	bind->setColumn(field);

	bind->setDefaultValue(defaultValue);
	bind->setYesText(yes);
	bind->setNoText(no);

	mBinds << bind;
}

void AjaxView::bindRelation(const QString& field, QLabel *view_widget,
	QComboBox *edit_widget, const QString& rel_table, const QString& column,
	QPushButton *browseButton, const QString& listTitle,
	const QString& noneName, const QVariantList& filters)
{
	BindRelation *bind = new BindRelation(this);
	bind->setViewer(view_widget);
	bind->setEditor(edit_widget);
	bind->setColumn(field);

	bind->setBrowseButton(browseButton);
	bind->setForeignTable(rel_table);
	bind->setForeignColumn(column);
	bind->setListTitle(listTitle);
	bind->setNoneName(noneName);
	bind->setFilters(filters);

	bind->refreshRelationCache();

	mBinds << bind;
}

// For this one the feild value is just used for keeping track of the bind
void AjaxView::bindNumberSet(const QString& field, QLabel *view_widget,
	QSpinBox *edit_widget, QComboBox *selector, const QStringList& columns,
	const QStringList& patterns, const QString& separator, int defaultValue)
{
	Q_UNUSED(field);

	Q_ASSERT(selector->count() && !columns.isEmpty());
	Q_ASSERT(selector->count() == columns.count());
	Q_ASSERT(selector->count() == patterns.count());

	BindNumberSet *bind = new BindNumberSet;
	bind->setViewer(view_widget);
	bind->setEditor(edit_widget);
	bind->setColumns(columns);

	bind->setDefaultValue(defaultValue);
	bind->setSeparator(separator);
	bind->setPatterns(patterns);
	bind->setSelector(selector);

	mBinds << bind;
}

// For this one the feild value is just used for keeping track of the bind
void AjaxView::bindNumberSelector(const QString& field, QLabel *view_widget,
	QSpinBox *edit_widget, const RadioButtonMap& selectors_and_columns,
	const RadioButtonMap& selector_edits_and_columns,
	QButtonGroup *group, QButtonGroup *edit_group, int defaultValue,
	QRadioButton *defaultSelector, QRadioButton *defaultEditSelector)
{
	Q_UNUSED(field);

	QStringList columns = selectors_and_columns.values();
	QStringList columns_2 = selector_edits_and_columns.values();
	qSort(columns); qSort(columns_2);

	Q_ASSERT(columns == columns_2);
	Q_ASSERT( !columns.isEmpty() );

	// TODO: Make sure none of the radio buttons in the two sets are the same
	// or they will be removed from one group and added to the other, causing
	// chaos and bugs and destruction of the world as we know it!

	BindNumberSelector *bind = new BindNumberSelector;
	bind->setViewer(view_widget);
	bind->setEditor(edit_widget);
	bind->setColumns(columns);

	bind->setViewerGroup(group);
	bind->setEditorGroup(edit_group);
	bind->setDefaultValue(defaultValue);
	bind->setViewerSelectors(selectors_and_columns);
	bind->setEditorSelectors(selector_edits_and_columns);
	bind->setDefaultViewerSelector(defaultSelector);
	bind->setDefaultEditorSelector(defaultEditSelector);

	mBinds << bind;
}

// The field argument is the table used for the relation
void AjaxView::bindDetailedMultiRelation(const QString& field,
	InfoListWidget *view_widget, InfoListWidget *edit_widget,
	const QString& drop_mime_type, const QString& id,
	const QString& foreign_id, const QString& foreign_table,
	const QString& icon_column, const QString& icon_path,
	const QStringList& cols,
	QPushButton *add_button, QPushButton *edit_button,
	QPushButton *remove_button, QPushButton *new_button,
	QPushButton *cancel_button,
	SearchEdit *quick_filter, QListWidget *add_list,
	const QVariant& view_action, const QVariant& search_action,
	bool allow_drops, const QVariantList& additional_relations,
	const QString& extra_column, const QString& extra_prompt,
	QStackedWidget *add_stack)
{
	// TODO: Implement these
	Q_UNUSED(drop_mime_type); Q_UNUSED(allow_drops);
	Q_UNUSED(view_action); Q_UNUSED(search_action);

	BindDetailedMultiRelation *bind = new BindDetailedMultiRelation;
	bind->setAdditionalRelations(additional_relations);
	bind->setViewer(view_widget);
	bind->setEditor(edit_widget);
	bind->setOtherTable(field);
	bind->setTable( table() );

	bind->setIconPath(icon_path);
	bind->setIconColumn(icon_column);
	bind->setRelation(id, foreign_table, foreign_id);
	bind->setExtraColumn(extra_column);
	bind->setExtraPrompt(extra_prompt);
	bind->setColumns(cols);

	bind->setRemoveButton(remove_button);
	bind->setEditButton(edit_button);
	bind->setAddButton(add_button);

	bind->setCancelButton(cancel_button);
	bind->setNewButton(new_button);

	bind->setAddStack(add_stack);
	bind->setAddList(add_list);

	bind->setQuickSearch(quick_filter);

	mBinds << bind;
}

QVariantList AjaxView::createViewActions() const
{
	// TODO: Make sure we don't have duplicates

	QVariantList columns;
	columns << QString("id");

	QVariantList custom_actions;

	QListIterator<AjaxBind*> it(mBinds);
	while( it.hasNext() )
	{
		AjaxBind *bind = it.next();

		QStringList bind_cols = bind->columns();
		foreach(QString column, bind_cols)
			columns << column;

		custom_actions << bind->customViewActions(mID);
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

	custom_actions.prepend(action);

	return custom_actions;
}

QVariantList AjaxView::createUpdateActions() const
{
	// TODO: Make sure we don't have duplicates

	QVariantMap row;
	QVariantList columns;

	QVariantList custom_actions;

	QListIterator<AjaxBind*> it(mBinds);
	while( it.hasNext() )
	{
		AjaxBind *bind = it.next();

		QStringList bind_cols = bind->columns();
		foreach(QString column, bind_cols)
			columns << column;

		bind->retrieveUpdateData(row);

		custom_actions << bind->customUpdateActions(mID);
	}

	QVariantMap action;

	if(mID > 0)
		action["action"] = QString("update");
	else
		action["action"] = QString("insert");

	action["table"] = table();
	action["columns"] = columns;

	QVariantList user_data;

	if(mID > 0)
		user_data << QString("%1_update").arg( table() );
	else
		user_data << QString("%1_insert").arg( table() );

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

	custom_actions.prepend(action);

	return custom_actions;
}

QVariantList AjaxView::createDeleteActions(int id) const
{
	QVariantList custom_actions;

	QListIterator<AjaxBind*> it(mBinds);
	while( it.hasNext() )
	{
		AjaxBind *bind = it.next();

		custom_actions << bind->customDeleteActions(id);
	}

	return custom_actions;
}

void AjaxView::processBindValues(const QVariantMap& values)
{
	foreach(AjaxBind *bind, mBinds)
		bind->handleViewResponse(values);
}

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
	QString insert_data = QString("%1_insert").arg( table() );
	QString view_data = QString("%1_view").arg( table() );

	if(user_data == view_data)
	{
		QVariantList rows = result.value("rows").toList();
		if( rows.count() && rows.first().toMap().value("id") == mID )
			mSleepingResponse = result;
	}
	else if(user_data == update_data )
	{
		view(updated_id);

		emit viewChanged();

		return;
	}
	else if(user_data == insert_data)
	{
		int id = result.value("ids").toList().first().toInt();

		view(id);

		emit viewChanged();
		emit itemInserted(id);

		return;
	}
	else if(user_data == "auth_query_perms")
	{
		if( result.value("perms").toMap().value("modify_db").toBool() )
		{
			Q_ASSERT(ui.editButton != 0);

			ui.editButton->setVisible(true);
			mCanEdit = true;
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
}
