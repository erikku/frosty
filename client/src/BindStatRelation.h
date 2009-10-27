/******************************************************************************\
*  client/src/BindStatRelation.h                                               *
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

#ifndef __BindStatRelation_h__
#define __BindStatRelation_h__

#include "AjaxBind.h"

class SearchEdit;
class QPushButton;
class QListWidget;
class QStackedWidget;
class QListWidget;

class BindStatRelation : public AjaxBind
{
	Q_OBJECT

public:
	BindStatRelation(QObject *parent = 0);

	virtual QStringList columns() const;

	virtual QVariantList customViewActions(int id) const;
	virtual QVariantList customUpdateActions(int id) const;
	virtual QVariantList customDeleteActions(int id) const;

	virtual QWidget* viewer() const;
	virtual void setViewer(QWidget *view);

	virtual QWidget* editor() const;
	virtual void setEditor(QWidget *edit);

	virtual void clear();

	virtual void handleViewResponse(const QVariantMap& values);
	virtual void retrieveUpdateData(QVariantMap& row);

	QString textColumn() const;
	void setTextColumn(const QString& col);

	QString relationID() const;
	QString relationForeignTable() const;
	QString relationForeignID() const;

	void setRelation(const QString& id, const QString& foreign_table,
		const QString& foreign_id);

	QString extraColumn() const;
	void setExtraColumn(const QString& column);

	QString extraPrompt() const;
	void setExtraPrompt(const QString& prompt);

	QString table() const;
	void setTable(const QString& table);

	QString otherTable() const;
	void setOtherTable(const QString& table);

	QPushButton* removeButton();
	void setRemoveButton(QPushButton *button);

	QPushButton* editButton();
	void setEditButton(QPushButton *button);

	QPushButton* addButton();
	void setAddButton(QPushButton *button);

	QPushButton* cancelButton();
	void setCancelButton(QPushButton *button);

	QPushButton* newButton();
	void setNewButton(QPushButton *button);

	QStackedWidget* addStack();
	void setAddStack(QStackedWidget *stack);

	QListWidget* addList();
	void setAddList(QListWidget *list);

	SearchEdit* quickSearch();
	void setQuickSearch(SearchEdit *edit);

public slots:
	virtual void addRequested();

protected slots:
	void startAdd();
	void cancelAdd();
	void performAdd();
	void performEdit();
	void queueRemoval();
	void filterAddList();
	void selectionChanged();
	void addSelectionChanged();
	void ajaxResponse(const QVariantMap& resp, const QString& user_data);

protected:
	int promptForExtra(int default_value = -1);
	QVariant refreshItemCache() const;

	void multiRelationColumn(const QString& column);
	bool itemMatches(const QVariantMap& map, const QString& search);

	QListWidget *mViewer;
	QListWidget *mEditor;

	QList<int> mAddedIDs;

	QString mIconPath, mIconColumn, mExtraColumn, mExtraPrompt;
	QString mTable, mOtherTable;

	QVariantMap mRelationColumns;

	QVariantList mRemoveQueue, mUpdateQueue, mInsertQueue;

	QListWidget *mAddList;
	QStackedWidget *mAddStack;
	QPushButton *mRemoveButton, *mEditButton, *mAddButton;
	QPushButton *mCancelButton, *mNewButton;
	SearchEdit *mQuickSearch;

	QString mRelationID, mRelationForeignTable, mRelationForeignID;

	mutable int mCurrentID;
};

#endif // __BindStatRelation_h__
