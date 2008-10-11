/******************************************************************************\
*  client/src/Register.h                                                       *
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

#ifndef __Register_h__
#define __Register_h__

#include "ui_Register.h"

#include <QtGui/QPixmap>

class registration;

class Register : public QWidget
{
	Q_OBJECT

public:
	Register(QWidget *parent = 0);
	~Register();

public slots:
	void showLogin();
	void guestLogin();
	void showOptions();
	void sendRequest();
	void registrationComplete();
	void validationImage(const QPixmap& image, const QString& error);

protected:
	void darkenWidget(QWidget *widget);

	Ui::Register ui;

	bool mFirst;
	registration *mRegistration;
};

#endif // __Register_h__
