/******************************************************************************\
*  libkawaii - A Japanese language support library for Qt4                     *
*  Copyright (C) 2007 John Eric Martin <john.eric.martin@gmail.com>            *
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

#ifndef __LineEdit_h__
#define __LineEdit_h__

#include "SearchEdit.h"

class KawaiiLineEdit : public SearchEdit
{
	Q_OBJECT
	Q_PROPERTY(int romajiMode READ romajiMode WRITE setRomajiMode)

public:
	KawaiiLineEdit(QWidget *parent = 0);
	KawaiiLineEdit(const QString& contents, QWidget *parent = 0);

	bool romajiMode();

public slots:
	void setRomajiMode(bool enabled);
	void updateText();

protected:
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void paintEvent(QPaintEvent *event);

	enum
	{
		Hiragana,
		Katakana
	}mDisplayMode;

	bool mRomajiMode;
	int mInsertPosition;
	QString mRealText, mActiveText, mDisplayText;
};

#endif // __LineEdit_h__
