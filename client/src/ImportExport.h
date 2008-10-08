/******************************************************************************\
*  client/src/ImportExport.h                                                   *
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

#ifndef __ImportExport_h__
#define __ImportExport_h__

#include "ui_ImportExport.h"

class ImportExport : public QWidget
{
	Q_OBJECT

public:
	ImportExport(QWidget *parent = 0);

public slots:
	void pathChanged();
	void toggleView();
	void browse();

	void importDB();
	void exportDB();

	void ajaxResponse(const QVariant& resp);

protected:
	Ui::ImportExport ui;

	QString mImportPath, mExportPath;
};

#endif // __ImportExport_h__
