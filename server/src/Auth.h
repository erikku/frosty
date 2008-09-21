/******************************************************************************\
*  server/src/Auth.h                                                           *
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

#ifndef __Auth_h__
#define __Auth_h__

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QVariant>

void auth_start();
bool auth_authenticate(const QString& email, const QString& data,
	const QString& hash);
bool auth_validate_request(const QString& email, const QVariant& action);

QVariant auth_query_perms(const QString& email);
QVariant auth_query_users(const QString& email);
QVariant auth_query_user(const QString& email, const QString& target_email);

QString auth_register(const QString& email, const QString& name,
	const QString& pass);

bool auth_make_inactive(const QString& email, const QString& target_email);
bool auth_make_active(const QString& email, const QString& target_email);

bool auth_modify_user(const QString& email, const QString& target_email,
	const QString& new_email, const QString& name, const QString& pass,
	const QVariantMap& perms);

#endif // __Auth_h__
