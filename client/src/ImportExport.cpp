/******************************************************************************\
*  client/src/ImportExport.cpp                                                 *
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

#include "ImportExport.h"
#include "Settings.h"
#include "ajax.h"
#include "json.h"

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

ImportExport::ImportExport(QWidget *parent_widget) : QWidget(parent_widget)
{
	ui.setupUi(this);

	ui.exportButton->setVisible(false);

	QIcon browse_icon(":/fileopen.png");
	ui.pathBrowse->setIcon(browse_icon);

	connect(ui.pathEdit, SIGNAL(textChanged(const QString&)),
		this, SLOT(pathChanged()));

	connect(ui.importRadio, SIGNAL(toggled(bool)), this, SLOT(toggleView()));
	connect(ui.pathBrowse, SIGNAL(clicked(bool)), this, SLOT(browse()));
	connect(ui.importButton, SIGNAL(clicked(bool)), this, SLOT(importDB()));
	connect(ui.exportButton, SIGNAL(clicked(bool)), this, SLOT(exportDB()));

	ajax::getSingletonPtr()->subscribe(this);

	{
		QVariantMap action;
		action["action"] = "db_tables";
		action["user_data"] = "db_tables";

		ajax::getSingletonPtr()->request(settings->url(), action);
	}

	toggleView();
	setEnabled(false);
}

void ImportExport::pathChanged()
{
	QString path = ui.pathEdit->text();

	QFileInfo info(path);

	ui.importButton->setEnabled(true);
	ui.exportButton->setEnabled(true);

	if( path.isEmpty() )
	{
		ui.pathEdit->setStyleSheet( QString() );
		ui.importButton->setEnabled(false);
		ui.exportButton->setEnabled(false);
	}
	else if( ui.importRadio->isChecked() )
	{
		if( info.exists() && info.isReadable() && info.isFile() )
		{
			ui.pathEdit->setStyleSheet( QString() );
		}
		else
		{
			ui.pathEdit->setStyleSheet("color: red");
			ui.importButton->setEnabled(false);
			ui.exportButton->setEnabled(false);
		}
	}
	else // export
	{
		if( info.exists() && info.isWritable() && info.isFile() )
		{
			ui.pathEdit->setStyleSheet("color: yellow");
		}
		else if( QFileInfo( info.dir().path() ).isWritable() && !info.isDir() )
		{
			ui.pathEdit->setStyleSheet( QString() );
		}
		else
		{
			ui.pathEdit->setStyleSheet("color: red");
			ui.importButton->setEnabled(false);
			ui.exportButton->setEnabled(false);
		}
	}

	if( ui.importRadio->isChecked() )
		mImportPath = path;
	else
		mExportPath = path;
}

void ImportExport::toggleView()
{
	if( ui.importRadio->isChecked() )
	{
		ui.importButton->setVisible(true);
		ui.exportButton->setVisible(false);
		ui.pathEdit->setText(mImportPath);
		ui.tableListWidget->hide();
		setFixedSize(400, 120);
	}
	else // export
	{
		ui.importButton->setVisible(false);
		ui.exportButton->setVisible(true);
		ui.pathEdit->setText(mExportPath);
		ui.tableListWidget->show();
		setFixedSize(400, 300);
	}
}

void ImportExport::browse()
{
	QString path;
	if( ui.importRadio->isChecked() )
	{
		path = QFileDialog::getOpenFileName(this, tr("Select an import file"),
			QString(), tr("Database Export Files (*.dbx)"));
	}
	else // export
	{
		path = QFileDialog::getSaveFileName(this, tr("Select an import file"),
			QString(), tr("Database Export Files (*.dbx)"));

		if( QFileInfo(path).suffix().isEmpty() )
			path = path + ".dbx";
	}

	if( !path.isEmpty() )
		ui.pathEdit->setText(path);
}

void ImportExport::importDB()
{
	QFile file(mImportPath);
	if( !file.open(QIODevice::ReadOnly) )
	{
		QMessageBox::critical(this, tr("Import Failed"), tr("Failed to "
			"open the import file for reading."));

		return;
	}

	QString json = QString::fromUtf8( file.readAll() );
	file.close();

	QVariantMap action;
	action["action"] = "db_import";
	action["user_data"] = "db_import";
	action["export"] = json::parse(json);

	ajax::getSingletonPtr()->request(settings->url(), action);

	setEnabled(false);
}

void ImportExport::exportDB()
{
	QString path = ui.pathEdit->text();
	QFileInfo info(path);

	if( info.exists() && info.isWritable() )
	{
		QMessageBox::StandardButton button = QMessageBox::question(this,
			tr("File Overwrite"), tr("The file already exists. Overwrite?"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		if(button == QMessageBox::No)
			return;
	}

	QVariantList tables;
	for(int i = 0; i < ui.tableListWidget->count(); i++)
	{
		if(ui.tableListWidget->item(i)->checkState() == Qt::Checked)
			tables << ui.tableListWidget->item(i)->text();
	}

	QVariantMap action;
	action["action"] = "db_export";
	action["user_data"] = "db_export";
	action["tables"] = tables;

	ajax::getSingletonPtr()->request(settings->url(), action);

	setEnabled(false);
}

void ImportExport::ajaxResponse(const QVariantMap& resp, const QString& user_data)
{
	if(user_data == "db_export")
	{
		QString export_data = json::toJSON( resp.value("export") );

		QFile file(mExportPath);
		if( !file.open(QIODevice::WriteOnly) )
		{
			QMessageBox::critical(this, tr("Export Failed"), tr("Failed to "
				"open the export file for writing."));

			return;
		}
		else
		{
			file.write( export_data.toUtf8() );
			file.close();
		}

		QMessageBox::information(this, tr("Export Successful"), tr("The export "
			"has completed successfully."));

		setEnabled(true);
	}
	else if(user_data == "db_import")
	{
		QMessageBox::information(this, tr("Import Successful"), tr("The import "
			"has completed successfully."));

		setEnabled(true);
	}
	else if(user_data == "db_tables")
	{
		QVariantList tables = resp.value("tables").toList();
		foreach(QVariant table, tables)
		{
			QListWidgetItem *item = new QListWidgetItem(table.toString());
			item->setCheckState(Qt::Checked);

			ui.tableListWidget->addItem(item);
		}

		setEnabled(true);
	}
}
