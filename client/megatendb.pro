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
               ui/AjaxList.ui \
               ui/DevilView.ui \
               ui/MashouView.ui \
               ui/SkillView.ui \
               ui/IconSelect.ui \
               ui/InfoListWidget.ui \
               ui/InfoListWidgetItem.ui \
               ui/ImportExport.ui \
               ui/RelationEdit.ui \
               ui/RelationList.ui \
               ui/Options.ui \
               ui/Register.ui \
               ui/Taskbar.ui \
               ui/Login.ui \
               ui/UserEdit.ui \
               ui/UserList.ui \
               ui/LogView.ui
HEADERS     += src/ajax.h \
               src/ajaxTransfer.h \
               src/PaletteEditor.h \
               src/ClickableLabel.h \
               src/AjaxList.h \
               src/DevilList.h \
               src/MashouList.h \
               src/SkillList.h \
               src/AjaxView.h \
               src/DevilView.h \
               src/MashouView.h \
               src/SkillView.h \
               src/DevilWindow.h \
               src/MashouWindow.h \
               src/SkillWindow.h \
               src/SearchEdit.h \
               src/HttpTransfer.h \
               src/IconEdit.h \
               src/IconSelect.h \
               src/ImportExport.h \
               src/InfoListWidget.h \
               src/InfoListWidgetItem.h \
               src/RelationEdit.h \
               src/BasicRelationEdit.h \
               src/RelationList.h \
               src/BasicRelationList.h \
               src/Settings.h \
               src/Options.h \
               src/registration.h \
               src/Register.h \
               src/Taskbar.h \
               src/Login.h \
               src/UserEdit.h \
               src/UserList.h \
               src/LogTree.h \
               src/LogView.h \
               src/LogWidget.h \
               src/VersionCheck.h
SOURCES     += src/main.cpp \
               src/ajax.cpp \
               src/ajaxTransfer.cpp \
               src/json.cpp \
               src/PaletteEditor.cpp \
               src/ClickableLabel.cpp \
               src/AjaxList.cpp \
               src/DevilList.cpp \
               src/MashouList.cpp \
               src/SkillList.cpp \
               src/AjaxView.cpp \
               src/DevilView.cpp \
               src/MashouView.cpp \
               src/SkillView.cpp \
               src/DevilWindow.cpp\
               src/MashouWindow.cpp \
               src/SkillWindow.cpp \
               src/HttpTransfer.cpp \
               src/IconEdit.cpp \
               src/IconSelect.cpp \
               src/ImportExport.cpp \
               src/InfoListWidget.cpp \
               src/InfoListWidgetItem.cpp \
               src/SearchEdit.cpp \
               src/RelationEdit.cpp \
               src/BasicRelationEdit.cpp \
               src/RelationList.cpp \
               src/BasicRelationList.cpp \
               src/Settings.cpp \
               src/Options.cpp \
               src/registration.cpp \
               src/Register.cpp \
               src/Taskbar.cpp \
               src/sha1.c \
               src/Login.cpp \
               src/UserEdit.cpp \
               src/UserList.cpp \
               src/LogTree.cpp \
               src/LogView.cpp \
               src/LogWidget.cpp \
               src/VersionCheck.cpp
