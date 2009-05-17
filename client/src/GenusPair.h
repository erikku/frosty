/******************************************************************************\
*  client/src/GenusPair.h                                                      *
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

#ifndef __GenusPair_h__
#define __GenusPair_h__

#include <QtCore/QHash>

class GenusPair
{
public:
	GenusPair() : a(0), b(0) { }
	GenusPair(int id1, int id2) : a(id1), b(id2) { }

	int a, b;
};

inline bool operator==(const GenusPair& a, const GenusPair& b)
{
	return (a.a == b.a && a.b == b.b) || (a.b == b.a && a.a == b.b);
}


inline uint qHash(const GenusPair& pair)
{
	if(pair.a > pair.b)
		return qHash((pair.a << 0x0F) | pair.b);

	return qHash((pair.b << 0x0F) | pair.a);
}

#endif // __GenusPair_h__
