/******************************************************************************\
*  client/src/Clock.cpp                                                        *
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

#include "Clock.h"

#include <QtCore/QVariant>
#include <QtCore/QTimer>
#include <cmath>

#ifndef QT_NO_DEBUG
#include <iostream>
#endif // QT_NO_DEBUG

Clock::Clock(QWidget *parent_widget, Qt::WindowFlags f) :
	QWidget(parent_widget, f)
{
	ui.setupUi(this);

	setWindowTitle( tr("%1 - Clock").arg(
		tr("Absolutely Frosty") ) );

	days << trUtf8("月曜日") << trUtf8("火曜日") << trUtf8("水曜日")
		<< trUtf8("木曜日") << trUtf8("金曜日") << trUtf8("土曜日")
		<< trUtf8("日曜日");

	phases << trUtf8("NEW MOON (新月)") << trUtf8("1/8 MOON (上)")
		<< trUtf8("2/8 MOON (上)") << trUtf8("3/8 MOON (上)")
		<< trUtf8("HALF MOON (上弦)") << trUtf8("5/8 MOON (上)")
		<< trUtf8("6/8 MOON (上)") << trUtf8("7/8 MOON (上)")
		<< trUtf8("FULL MOON (満月)") << trUtf8("7/8 MOON (下)")
		<< trUtf8("6/8 MOON (下)") << trUtf8("5/8 MOON (下)")
		<< trUtf8("HALF MOON (下弦)") << trUtf8("3/8 MOON (下)")
		<< trUtf8("2/8 MOON (下)") << trUtf8("1/8 MOON (下)");

	darkenWidget(ui.localTimeLabel);
	darkenWidget(ui.japanTimeLabel);
	darkenWidget(ui.megatenTimeLabel);

	updateClocks();
}

void Clock::megatenTime(const QDateTime& stamp, uint now)
{
	int current_day = stamp.date().dayOfWeek() - 1;
	int current_hour = stamp.time().hour();
	int offset = 0;

	current_day = (current_day + 3) % 7;
	offset = (current_day * 2) + (current_hour < 12 ? -1 : 0);
	offset = (offset + 14) % 14;

	now += offset * 2;

	uint hr = (uint)floor(now / 120) % 24;
	uint min = (uint)floor(now / 2) % 60;

	QString phase = phases.at((uint)floor(now / 1440) % 16);

	ui.megatenTime->setText( QString(tr("%1 %2:%3")).arg(phase).arg(
		hr, 2, 10, QLatin1Char('0')).arg(min, 2, 10, QLatin1Char('0')) );
}

void Clock::updateClocks()
{
	QDateTime local = QDateTime::currentDateTime();
	QDateTime japan = local.toUTC().addSecs(32400);
	QString japanTime = japan.toString( trUtf8("hh時mm分ss秒 yyyy年MM月dd日") );
	japanTime = tr("%1 (%2)").arg(japanTime).arg(
		days.at(japan.date().dayOfWeek() - 1) );

	ui.localTime->setText(local.toString(tr("hh:mm:ssap yyyy-MM-dd (dddd)")));
	ui.japanTime->setText(japanTime);
	megatenTime( local.toUTC(), local.toUTC().toTime_t() );

	QTimer::singleShot(1000, this, SLOT(updateClocks()));
}

void Clock::darkenWidget(QWidget *widget)
{
	QPalette dark_palette = palette();
	dark_palette.setBrush(QPalette::Window,
		dark_palette.brush(QPalette::Base));

	QLabel *label = qobject_cast<QLabel*>(widget);
	if(label)
		label->setMargin(5);
#ifndef QT_NO_DEBUG
	else
		std::cout << tr("darkenWidget(%1) is not a QLabel").arg(
			widget->objectName()).toLocal8Bit().data() << std::endl;
#endif // QT_NO_DEBUG

	widget->setAutoFillBackground(true);
	widget->setPalette(dark_palette);
}
