#  client/megatendb.pro
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

QT          += network xml
TEMPLATE     = app
TARGET       = megatendb
INCLUDEPATH += src

OBJECTS_DIR = .objs
MOC_DIR     = .moc
RCC_DIR     = .rcc
UI_DIR      = .ui

RESOURCES    = \#megatendb.rc \
               megatendb.qrc
TRANSLATIONS = trans/megatendb_en.ts \
               trans/megatendb_ja.ts
FORMS       += ui/PaletteEditor.ui \
               ui/SkillList.ui \
               ui/SkillView.ui \
               ui/IconSelect.ui \
               ui/RelationEdit.ui \
               ui/RelationList.ui \
               ui/Options.ui \
               ui/Register.ui \
               ui/Login.ui \
               ui/UserEdit.ui \
               ui/UserList.ui \
               ui/LogView.ui
HEADERS     += src/ajax.h \
               src/ajaxTransfer.h \
               src/PaletteEditor.h \
               src/SkillList.h \
               src/SkillView.h \
               src/SkillWindow.h \
               src/SearchEdit.h \
               src/IconEdit.h \
               src/IconSelect.h \
               src/RelationEdit.h \
               src/RelationList.h \
               src/Settings.h \
               src/Options.h \
               src/registration.h \
               src/Register.h \
               src/Login.h \
               src/UserEdit.h \
               src/UserList.h \
               src/LogTree.h \
               src/LogView.h \
               src/LogWidget.h
SOURCES     += src/main.cpp \
               src/ajax.cpp \
               src/ajaxTransfer.cpp \
               src/json.cpp \
               src/PaletteEditor.cpp \
               src/SkillList.cpp \
               src/SkillView.cpp \
               src/IconEdit.cpp \
               src/IconSelect.cpp \
               src/SkillWindow.cpp \
               src/SearchEdit.cpp \
               src/RelationEdit.cpp \
               src/RelationList.cpp \
               src/Settings.cpp \
               src/Options.cpp \
               src/registration.cpp \
               src/Register.cpp \
               src/sha1.c \
               src/Login.cpp \
               src/UserEdit.cpp \
               src/UserList.cpp \
               src/LogTree.cpp \
               src/LogView.cpp \
               src/LogWidget.cpp
