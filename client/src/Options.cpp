/******************************************************************************\
*  client/src/Options.cpp                                                      *
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

#include "Options.h"
#include "Taskbar.h"
#include "Settings.h"

#include <QtCore/QCoreApplication>
#include <QtGui/QMessageBox>

static Options* g_options_inst = 0;

Options::Options(QWidget *parent_widget) : QWidget(parent_widget)
{
	ui.setupUi(this);

	setWindowTitle( tr("%1 - Options").arg(
		tr("Absolutely Frosty") ) );

	setWindowModality(Qt::ApplicationModal);

	darkenWidget(ui.interfaceLanguageLabel);
	darkenWidget(ui.backendUrlLabel);
	darkenWidget(ui.updaterUrlLabel);
	darkenWidget(ui.trayNoticeLabel);
	darkenWidget(ui.closePolicyLabel);
	darkenWidget(ui.offlineModeLabel);

	connect(ui.clearLoginInfoButton, SIGNAL(clicked(bool)),
		this, SLOT(clearLoginInfo()));

	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(saveSettings()));
	connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

Options* Options::getSingletonPtr()
{
	if(!g_options_inst)
		g_options_inst = new Options;

	Q_ASSERT(g_options_inst != 0);

	return g_options_inst;
}

void Options::darkenWidget(QWidget *widget)
{
	QPalette dark_palette = palette();
	dark_palette.setBrush(QPalette::Window,
		dark_palette.brush(QPalette::Base));

	qobject_cast<QLabel*>(widget)->setMargin(5);
	widget->setAutoFillBackground(true);
	widget->setPalette(dark_palette);
}

void Options::loadSettings()
{
	ui.backendUrl->clear();
	foreach(QUrl url, settings->urls())
		ui.backendUrl->addItem(url.toString());

	ui.updaterUrl->clear();
	foreach(QUrl url, settings->updateUrls())
		ui.updaterUrl->addItem(url.toString());

	ui.offlineMode->setCheckState(settings->offline() ?
		Qt::Checked : Qt::Unchecked);
	ui.backendUrl->setCurrentIndex(0);
	ui.updaterUrl->setCurrentIndex(0);

	ui.interfaceLanguage->setCurrentIndex(settings->lang() == "ja" ? 0 : 1);
	ui.trayNotice->setCheckState(settings->remindTrayIcon() ?
		Qt::Checked : Qt::Unchecked);
	ui.closePolicy->setCurrentIndex(settings->taskbarClosePolicy());

	show();
}

void Options::saveSettings()
{
	emit optionsChanged();

	settings->setUrl( QUrl(ui.backendUrl->currentText()) );
	settings->setUpdateUrl( QUrl(ui.updaterUrl->currentText()) );
	settings->setOffline(ui.offlineMode->checkState() == Qt::Checked);

	if(ui.interfaceLanguage->currentIndex() == 1)
		settings->setLang("en");
	else
		settings->setLang("ja");

	settings->setRemindTrayIcon(ui.trayNotice->checkState() == Qt::Checked);
	settings->setTaskbarClosePolicy(ui.closePolicy->currentIndex());

	hide();

	QMessageBox::StandardButton button = QMessageBox::warning(this,
		tr("Options Change"),
		tr("Some of these options may not take effect until the application "
		"is restarted. Do you wish to close the application now?"),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

	if(button == QMessageBox::No)
		return;

	Taskbar::getSingletonPtr()->quit();
}

void Options::clearLoginInfo()
{
	QMessageBox::StandardButton button = QMessageBox::warning(this,
		tr("Clear Login Info"),
		tr("This will clear your login information and quit the application. "
		"Are you sure you want to continue?"),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

	if(button == QMessageBox::No)
		return;

	settings->setEmail( QString() );
	settings->setPass( QString() );

	Taskbar::getSingletonPtr()->quit();
}
