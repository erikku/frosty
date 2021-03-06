#  client/client.pro
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
TEMPLATE     = app
TARGET       = frosty
INCLUDEPATH += src ../server/src
LIBS        += -L../server -L../server/release -lfrosty_server -lbz2

OBJECTS_DIR = .objs
MOC_DIR     = .moc
RCC_DIR     = .rcc
UI_DIR      = .ui

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
	RC_FILE = client.rc
	QMAKE_CXXFLAGS_DEBUG += -mnop-fun-dllimport
}

RESOURCES   += client.qrc
TRANSLATIONS = trans/frosty_en.ts \
               trans/frosty_ja.ts
FORMS       += ui/PaletteEditor.ui \
               ui/AjaxList.ui \
               ui/DevilView.ui \
               ui/ItemView.ui \
               ui/MashouView.ui \
               ui/SkillView.ui \
               ui/TraitView.ui \
               ui/IconImport.ui \
               ui/IconSelect.ui \
               ui/IconListWidgetItem.ui \
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
               ui/LogView.ui \
               ui/About.ui \
               ui/Shoutbox.ui \
               ui/COMP.ui \
               ui/Storage.ui \
               ui/AddDevil.ui \
               ui/Clock.ui \
               ui/DevilProperties.ui \
               ui/FusionChart.ui \
               ui/MoonPhases.ui \
               ui/AddParents.ui \
               ui/p3p/Home.ui \
               ui/p3p/PersonaView.ui
HEADERS     += src/ajax.h \
               src/ajaxTransfer.h \
               src/baseTransfer.h \
               src/localTransfer.h \
               src/offlineTransfer.h \
               src/PaletteEditor.h \
               src/ClickableLabel.h \
               src/AjaxList.h \
               src/DevilList.h \
               src/ItemList.h \
               src/MashouList.h \
               src/SkillList.h \
               src/TraitList.h \
               src/AjaxView.h \
               src/DevilView.h \
               src/ItemView.h \
               src/MashouView.h \
               src/SkillView.h \
               src/TraitView.h \
               src/DevilWindow.h \
               src/ItemWindow.h \
               src/MashouWindow.h \
               src/SkillWindow.h \
               src/TraitWindow.h \
               src/SearchEdit.h \
               src/HttpTransfer.h \
               src/IconEdit.h \
               src/IconImport.h \
               src/IconSelect.h \
               src/ImportExport.h \
               src/IconListWidget.h \
               src/IconListWidgetItem.h \
               src/InfoListWidget.h \
               src/InfoListWidgetItem.h \
               src/Instance.h \
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
               src/VersionCheck.h \
               src/LineEdit.h \
               src/AjaxBind.h \
               src/BindText.h \
               src/BindTextBox.h \
               src/BindIcon.h \
               src/BindNumber.h \
               src/BindNumberRange.h \
               src/BindEnum.h \
               src/BindBool.h \
               src/BindRelation.h \
               src/BindNumberSet.h \
               src/BindNumberSelector.h \
               src/BindDetailedMultiRelation.h \
               src/BindMultiRelation.h \
               src/BindStatRelation.h \
               src/Shoutbox.h \
               src/COMP.h \
               src/Storage.h \
               src/StorageBase.h \
               src/DevilCache.h \
               src/AddDevil.h \
               src/Clock.h \
               src/DevilProperties.h \
               src/FusionChart.h \
               src/MoonPhases.h \
               src/AddParents.h \
               src/BackdropWidget.h \
               src/p3p/Home.h \
               src/p3p/PersonaList.h \
               src/p3p/PersonaView.h \
               src/p3p/PersonaWindow.h
SOURCES     += src/main.cpp \
               src/ajax.cpp \
               src/ajaxTransfer.cpp \
               src/localTransfer.cpp \
               src/offlineTransfer.cpp \
               src/json.cpp \
               src/PaletteEditor.cpp \
               src/ClickableLabel.cpp \
               src/AjaxList.cpp \
               src/DevilList.cpp \
               src/ItemList.cpp \
               src/MashouList.cpp \
               src/SkillList.cpp \
               src/TraitList.cpp \
               src/AjaxView.cpp \
               src/DevilView.cpp \
               src/ItemView.cpp \
               src/MashouView.cpp \
               src/SkillView.cpp \
               src/TraitView.cpp \
               src/DevilWindow.cpp\
               src/ItemWindow.cpp \
               src/MashouWindow.cpp \
               src/SkillWindow.cpp \
               src/TraitWindow.cpp \
               src/HttpTransfer.cpp \
               src/IconEdit.cpp \
               src/IconImport.cpp \
               src/IconSelect.cpp \
               src/ImportExport.cpp \
               src/IconListWidget.cpp \
               src/IconListWidgetItem.cpp \
               src/InfoListWidget.cpp \
               src/InfoListWidgetItem.cpp \
               src/Instance.cpp \
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
               src/VersionCheck.cpp \
               src/KanaCOnversion.cpp \
               src/LineEdit.cpp \
               src/AjaxBind.cpp \
               src/BindText.cpp \
               src/BindTextBox.cpp \
               src/BindIcon.cpp \
               src/BindNumber.cpp \
               src/BindNumberRange.cpp \
               src/BindEnum.cpp \
               src/BindBool.cpp \
               src/BindRelation.cpp \
               src/BindNumberSet.cpp \
               src/BindNumberSelector.cpp \
               src/BindDetailedMultiRelation.cpp \
               src/BindMultiRelation.cpp \
               src/BindStatRelation.cpp \
               src/Shoutbox.cpp \
               src/COMP.cpp \
               src/Storage.cpp \
               src/StorageBase.cpp \
               src/DevilCache.cpp \
               src/AddDevil.cpp \
               src/Clock.cpp \
               src/DevilProperties.cpp \
               src/FusionChart.cpp \
               src/MoonPhases.cpp \
               src/AddParents.cpp \
               src/BackdropWidget.cpp \
               src/p3p/Home.cpp \
               src/p3p/PersonaList.cpp \
               src/p3p/PersonaView.cpp \
               src/p3p/PersonaWindow.cpp
