/******************************************************************************\
*  Utopia Player - A cross-platform, multilingual, tagging media manager       *
*  Copyright (C) 2006-2007 John Eric Martin <john.eric.martin@gmail.com>       *
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

#ifndef __PaletteEditor_h__
#define __PaletteEditor_h__

#include "ui_PaletteEditor.h"

#include <QtGui/QPalette>

class PaletteData
{
public:
	PaletteData();

	QMap<QString, QGradient::Type> gradType;
	QMap<QString, Qt::BrushStyle> brushStyles;
	QMap<QString, QGradient::Spread> gradSpread;
	QMap<QString, QPalette::ColorRole> colorRoles;
	QMap<QString, QPalette::ColorGroup> colorGroups;
	QMap<QPalette::ColorRole, QString> descriptions;
};

class PaletteEditor : public QWidget
{
	Q_OBJECT

public:
	PaletteEditor(const QPalette& pal = QPalette(), QWidget *parent = 0);

	QPalette currentPalette() const;

	static QPalette importPalette(const QString& xml);
	static QString exportPalette(const QPalette& pal);

public slots:
	void setCurrentPalette(const QPalette& pal);

protected slots:
	void promptColor();
	void updateColor();
	void updatePalette();

protected:
	PaletteData data;
	QPalette mPalette;
	Ui::PaletteEditor ui;
};

#endif // __PaletteEditor_h__
