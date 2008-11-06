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

#ifndef __KanaRomajiConverter_h__
#define __KanaRomajiConverter_h__

#include <QtCore/QString>

typedef enum RubyFormat
{
	Normal = 0,
	Flat,
	Bottom,
	Top,
	ShortHand,
	Wiki,
	NoParentheses
};

bool containsRuby(const QString& text);
QString reduceRuby(const QString& bottom, const QString& top);
QString parseRuby(const QString& string, RubyFormat format = Flat);
QString romajiToKana(const QString& string, bool special = true);
QString kanaToRomaji(const QString& string, bool special = true);
QString katakanaToHiragana(const QString& string);
QString hiraganaToKatakana(const QString& string);

#endif // __KanaRomajiConverter_h__
