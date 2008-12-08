/******************************************************************************\
*  client/src/IconImport.h                                                     *
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

#ifndef __IconImport_h__
#define __IconImport_h__

#include "ui_IconImport.h"

#include <QtCore/QStringList>
#include <QtCore/QProcess>

class IconImport : public QWidget
{
	Q_OBJECT

public:
	IconImport(QWidget *parent = 0);

public slots:
	void performCheck();

protected slots:
	void iconError();
	void scanIcons();
	void handleNext();
	void iconFinished(int exitCode, QProcess::ExitStatus exitStatus);

protected:

	QProcess *mProc;

	QString mSrcPath;
	QString mAppPath, mLastSrc, mLastDest;
	QStringList mDevils, mSkills, mMashou;

	Ui::IconImport ui;
};

#endif // __IconImport_h__
