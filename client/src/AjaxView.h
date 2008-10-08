/******************************************************************************\
*  client/src/AjaxView.h                                                       *
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

#ifndef __AjaxView_h__
#define __AjaxView_h__

#include <QtCore/QMap>
#include <QtCore/QVariant>
#include <QtGui/QWidget>

class QLabel;
class QSpinBox;
class QLineEdit;
class QTextEdit;
class QCheckBox;
class QComboBox;
class QPushButton;
class QRadioButton;
class QButtonGroup;
class QStackedWidget;
class IconEdit;

typedef QMap<QRadioButton*, QString> RadioButtonMap;

class AjaxView : public QWidget
{
	Q_OBJECT

public:
	AjaxView(QWidget *parent = 0);

	bool isEditing() const;

	typedef enum _BindType {
		BindText = 0,
		BindTextBox,
		BindIcon,
		BindNumber,
		BindNumberRange,
		BindEnum,
		BindBool,
		BindRelation,
		BindNumberSet,
		BindNumberSelector
	}BindType;

public slots:
	void add();
	void edit();
	void clear();
	void cancel();
	void update();
	void refresh();
	void view(int id, bool switchView = true);

	void showIconSelect();
	void browseRelationList();
	void updateRelationCombo();
	void refreshRelationCombo();
	void updateNumberSetValues();
	void updateNumberSetSelection();
	void updateNumberSelectionValue();
	void updateNumberSelection();

protected slots:
	void ajaxResponse(const QVariant& resp);
	void updateIcon(const QString& path, const QString& value);

signals:
	void viewChanged();

protected:
	int mID;

	virtual bool checkValues();
	virtual QString table() const = 0;

	virtual QString addWarningTitle() const = 0;
	virtual QString addWarningMessage() const = 0;

	void initView(QStackedWidget *stackedWidget, QPushButton *editButton,
		QPushButton *cancelButton, QPushButton *refreshButton,
		QPushButton *updateButton);

	void refreshRelationCache(const QString& field);

	void bindText(const QString& field, QLabel *view, QLineEdit *edit);
	void bindTextBox(const QString& field, QLabel *view, QTextEdit *edit);
	void bindIcon(const QString& field, QLabel *view, IconEdit *edit,
		const QString& path, const QString& searchPath);
	void bindNumber(const QString& field, QLabel *view, QSpinBox *edit,
		int defaultValue = -1);
	void bindNumberRange(const QString& field, QLabel *view, QSpinBox *from,
		QSpinBox *to, const QString& minColumn, const QString& maxColumn,
		int defaultMin = 0, int defaultMax = 100);
	void bindEnum(const QString& field, QLabel *view, QComboBox *edit,
		const QMap<int, QString>& items, int defaultValue = 0);
	void bindBool(const QString& field, QLabel *view, QCheckBox *edit,
		const QString& yes = QString(), const QString& no = QString());
	void bindRelation(const QString& field, QLabel *view, QComboBox *edit,
		const QString& table, const QString& column = QString(),
		QPushButton *browseButton = 0, const QString& listTitle = QString(),
		const QString& noneName = QString(), const QVariantList& filters =
		QVariantList());
	void bindNumberSet(const QString& field, QLabel *view, QSpinBox *edit,
		QComboBox *selector, const QStringList& columns,
		const QStringList& patterns, const QString& separator = QString(),
		int defaultValue = 0);
	void bindNumberSelector(const QString& field, QLabel *view, QSpinBox *edit,
		const RadioButtonMap& selectors_and_columns,
		const RadioButtonMap& selector_edits_and_columns,
		QButtonGroup *group = 0, QButtonGroup *edit_group = 0,
		int defaultValue = 0, QRadioButton *defaultSelector = 0,
		QRadioButton *defaultEditSelector = 0);

	void darkenWidget(QWidget *widget);

	QVariantMap createViewAction() const;
	QVariantMap createUpdateAction() const;
	void processBindValues(const QVariantMap& values);

	QVariantMap mSleepingResponse;
	QMap<QString, QVariantMap> mBinds;

private:
	struct _ui {
		QStackedWidget *stackedWidget;

		QPushButton *editButton;
		QPushButton *cancelButton;
		QPushButton *refreshButton;
		QPushButton *updateButton;
	}ui;
};

#endif // __AjaxView_h__
