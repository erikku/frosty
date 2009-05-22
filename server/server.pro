#  server/server.pro
#  Copyright (C) 2008-2009 John Eric Martin <john.eric.martin@gmail.com>
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

QT          += network sql xml

offline {
	TEMPLATE = lib
	VERSION  = 1.0.0
	CONFIG  += staticlib
} else {
	TEMPLATE = app
}

TARGET       = frosty_server
INCLUDEPATH += src

OBJECTS_DIR = .objs
MOC_DIR     = .moc
RCC_DIR     = .rcc
UI_DIR      = .ui

gui {
	message(Building with GUI support.)
} else {
	message(Building without GUI support.)
	QT -= gui
}

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
	RC_FILE = server.rc
	QMAKE_CXXFLAGS_DEBUG += -mnop-fun-dllimport
}

gui {
	RESOURCES    = server_gui.qrc
	TRANSLATIONS =
	FORMS       += ui/PaletteEditor.ui \
	               ui/Settings.ui \
	               ui/About.ui
	HEADERS     += src/PaletteEditor.h \
	               src/TrayIcon.h \
	               src/Settings.h
	SOURCES     += src/PaletteEditor.cpp \
	               src/TrayIcon.cpp \
	               src/Settings.cpp
} else {
	RESOURCES = server.qrc
}

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
               src/SslServer.h \
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
               src/SslServer.cpp \
               src/Utils.cpp \
               src/sha1.c \
               src/json.cpp
