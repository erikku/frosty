/******************************************************************************\
*  client/src/Taskbar.h                                                        *
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

#ifndef __Taskbar_h__
#define __Taskbar_h__

#include "ui_Taskbar.h"

class Options;
class Shoutbox;
class UserList;
class LogWidget;
class ItemWindow;
class DevilWindow;
class TraitWindow;
class SkillWindow;
class MashouWindow;
class ImportExport;
class QSystemTrayIcon;
class QAction;

// My Devils
class COMP;
class Storage;

class Clock;

class Taskbar : public QWidget
{
	Q_OBJECT

public:
	static Taskbar* getSingletonPtr();

public slots:
	void quit();
	void showAbout();
	void showShoutbox();
	void showLogWindow();
	void showItemWindow();
	void showDevilWindow();
	void showTraitWindow();
	void showSkillWindow();
	void showUsersWindow();
	void showMashouWindow();
	void showOptionsWindow();
	void showImportExportWindow();

	// My Devils
	void showCOMP();
	void showStorage();

	void showClock();

	void notifyDirty(const QString& user_data);

protected slots:
	void ajaxResponse(const QVariantMap& resp, const QString& user_data);

protected:
	Taskbar(QWidget *parent = 0);

	virtual void closeEvent(QCloseEvent *event);

	Ui::Taskbar ui;

	Options *mOptions;
	Shoutbox *mShoutbox;
	UserList *mUserList;
	LogWidget *mLogWidget;
	ItemWindow *mItemWindow;
	DevilWindow *mDevilWindow;
	TraitWindow *mTraitWindow;
	SkillWindow *mSkillWindow;
	MashouWindow *mMashouWindow;
	ImportExport *mImportExportWindow;

	// My Devils
	COMP *mCOMP;
	Storage *mStorage;

	Clock *mClock;

	QAction *mAdminSep;
	QAction *mUsersAction;
	QAction *mImportExportAction;

	QStringList mDirtyData;

	QSystemTrayIcon *mTray;
};

#endif // __Taskbar_h__
