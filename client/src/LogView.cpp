/******************************************************************************\
*  client/src/LogView.cpp                                                      *
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

#include "LogView.h"

LogView::LogView(QWidget *parent_widget) : QWidget(parent_widget)
{
	ui.setupUi(this);

	darkenWidget(ui.requestLabel);
	darkenWidget(ui.responseLabel);
	darkenWidget(ui.contentLabel);
}

void LogView::darkenWidget(QWidget *widget)
{
	QPalette dark_palette = palette();
	dark_palette.setBrush(QPalette::Window,
		dark_palette.brush(QPalette::Base));

	qobject_cast<QLabel*>(widget)->setMargin(5);
	widget->setAutoFillBackground(true);
	widget->setPalette(dark_palette);
}

void LogView::loadRequest(const QVariant& request)
{
	ui.request->loadJSON(request);
}

void LogView::loadResponse(const QVariant& response)
{
	ui.response->loadJSON(response);
}

void LogView::loadContent(const QString& content)
{
	if( content.isEmpty() )
		ui.content->setText( tr("(None)") );
	else
		ui.content->setText(content);
}
