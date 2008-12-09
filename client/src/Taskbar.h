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
class UserList;
class LogWidget;
class ItemWindow;
class DevilWindow;
class SkillWindow;
class MashouWindow;
class ImportExport;
class QAction;

class Taskbar : public QWidget
{
	Q_OBJECT

public:
	Taskbar(QWidget *parent = 0);

public slots:
	void showAbout();
	void showLogWindow();
	void showItemWindow();
	void showDevilWindow();
	void showSkillWindow();
	void showUsersWindow();
	void showMashouWindow();
	void showOptionsWindow();
	void showImportExportWindow();

protected slots:
	void ajaxResponse(const QVariant& resp);

protected:
	Ui::Taskbar ui;

	Options *mOptions;
	UserList *mUserList;
	LogWidget *mLogWidget;
	ItemWindow *mItemWindow;
	DevilWindow *mDevilWindow;
	SkillWindow *mSkillWindow;
	MashouWindow *mMashouWindow;
	ImportExport *mImportExportWindow;

	QAction *mAdminSep;
	QAction *mUsersAction;
	QAction *mImportExportAction;
};

#endif // __Taskbar_h__
