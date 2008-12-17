/******************************************************************************\
*  client/src/BindRelation.h                                                   *
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

#ifndef __BindRelation_h__
#define __BindRelation_h__

#include "AjaxBind.h"

class QLabel;
class QComboBox;
class QPushButton;
class RelationList;
class AjaxView;

class BindRelation : public AjaxBind
{
	Q_OBJECT

public:
	BindRelation(AjaxView *ajax_view, QObject *parent = 0);

	virtual QWidget* viewer() const;
	virtual void setViewer(QWidget *view);

	virtual QWidget* editor() const;
	virtual void setEditor(QWidget *edit);

	virtual void clear();

	virtual void handleViewResponse(const QVariantMap& values);
	virtual void retrieveUpdateData(QVariantMap& row);

	QString noneName() const;
	void setNoneName(const QString& name);

	QString listTitle() const;
	void setListTitle(const QString& title);

	QVariantList filters() const;
	void setFilters(const QVariantList& filters);

	QString foreignTable() const;
	void setForeignTable(const QString& table);

	QString foreignColumn() const;
	void setForeignColumn(const QString& column);

	QPushButton* browseButton() const;
	void setBrowseButton(QPushButton *button);

public slots:
	void browseRelationList();
	void updateRelationCombo();
	void refreshRelationCache();

protected slots:
	void ajaxResponse(const QVariant& resp);

protected:
	QLabel *mViewer;
	QComboBox *mEditor;
	RelationList *mList;
	QPushButton *mBrowseButton;

	int mLastID;
	bool mFilled;
	QString mNoneName;
	QString mListTitle;
	QVariantMap mCache;
	QVariantList mFilters;
	QString mForeignTable;
	QString mForeignColumn;
};

#endif // __BindRelation_h__
