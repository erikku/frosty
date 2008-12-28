#  server/server.pro
#  Copyright (C) 2008 John Eric Martin <john.eric.martin@gmail.com>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2 as
#  published by the Free Software Foundation.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the
#  Free Software Foundation, Inc.,
#  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

QT          -= gui
QT          += network sql xml
TEMPLATE     = app
TARGET       = megatendb_server
INCLUDEPATH += src
CONFIG      += debug

OBJECTS_DIR = .objs
MOC_DIR     = .moc
RCC_DIR     = .rcc
UI_DIR      = .ui

LIBS       += -lgd

QMAKE_CXXFLAGS_DEBUG += -W -Wall -ansi -pedantic-errors -Wcast-align \
	-Wcast-qual -Wchar-subscripts -Winline -Wpointer-arith \
	-Wredundant-decls -Wshadow -Wwrite-strings -Werror \
	-Wno-non-virtual-dtor -Wno-long-long -pedantic -Wconversion

QMAKE_CFLAGS_DEBUG += -W -Wall -ansi -pedantic-errors -Wcast-align \
	-Wcast-qual -Wchar-subscripts -Winline -Wpointer-arith \
	-Wredundant-decls -Wshadow -Wwrite-strings \
	-Wno-long-long -pedantic -Wconversion -Wstrict-prototypes \
	-Wbad-function-cast -Wmissing-prototypes -Wnested-externs

win32 {
	RC_FILE = megatendb.rc
	QMAKE_CXXFLAGS_DEBUG += -mnop-fun-dllimport
}

RESOURCES    = server.qrc
TRANSLATIONS =
FORMS       +=
HEADERS     += src/Auth.h \
               src/AuthActions.h \
               src/Backend.h \
               src/BackendActions.h \
               src/Config.h \
               src/DatabaseActions.h \
               src/ShoutboxActions.h \
               src/SimulatorActions.h \
               src/DomUtils.h \
               src/Log.h \
               src/Server.h \
               src/Session.h \
               src/Utils.h \
               src/sha1.h \
               src/json.h
SOURCES     += src/main.cpp \
               src/Auth.cpp \
               src/AuthActions.cpp \
               src/Backend.cpp \
               src/BackendActions.cpp \
               src/Config.cpp \
               src/DatabaseActions.cpp \
               src/ShoutboxActions.cpp \
               src/SimulatorActions.cpp \
               src/DomUtils.cpp \
               src/Log.cpp \
               src/Server.cpp \
               src/ServerActions.cpp \
               src/Session.cpp \
               src/Utils.cpp \
               src/sha1.c \
               src/json.cpp
