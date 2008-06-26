/******************************************************************************\
*  Utopia Player - A cross-platform, multilingual, tagging media manager       *
*  Copyright (C) 2006-2007 John Eric Martin <john.eric.martin@gmail.com>       *
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

#include "PaletteEditor.h"

#include <QtCore/QtDebug>
#include <QtCore/QSettings>
#include <QtGui/QColorDialog>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

PaletteData::PaletteData()
{
	colorGroups["Disabled"] = QPalette::Disabled;
	colorGroups["Active"] = QPalette::Active;
	colorGroups["Inactive"] = QPalette::Inactive;
	colorGroups["Normal"] = QPalette::Normal;

	colorRoles["Window"] = QPalette::Window;
	colorRoles["Window Text"] = QPalette::WindowText;
	colorRoles["Base"] = QPalette::Base;
	colorRoles["Alternate Base"] = QPalette::AlternateBase;
	colorRoles["Text"] = QPalette::Text;
	colorRoles["Button"] = QPalette::Button;
	colorRoles["Button Text"] = QPalette::ButtonText;
	colorRoles["Bright Text"] = QPalette::BrightText;
	colorRoles["Light"] = QPalette::Light;
	colorRoles["Medium Light"] = QPalette::Midlight;
	colorRoles["Dark"] = QPalette::Dark;
	colorRoles["Medium"] = QPalette::Mid;
	colorRoles["Shadow"] = QPalette::Shadow;
	colorRoles["Highlight"] = QPalette::Highlight;
	colorRoles["Highlighted Text"] = QPalette::HighlightedText;
	colorRoles["Link"] = QPalette::Link;
	colorRoles["Link Visited"] = QPalette::LinkVisited;

	brushStyles["None"] = Qt::NoBrush;
	brushStyles["Solid"] = Qt::SolidPattern;
	brushStyles["Dense 1"] = Qt::Dense1Pattern;
	brushStyles["Dense 2"] = Qt::Dense2Pattern;
	brushStyles["Dense 3"] = Qt::Dense3Pattern;
	brushStyles["Dense 4"] = Qt::Dense4Pattern;
	brushStyles["Dense 5"] = Qt::Dense5Pattern;
	brushStyles["Dense 6"] = Qt::Dense6Pattern;
	brushStyles["Dense 7"] = Qt::Dense7Pattern;
	brushStyles["Horizontal"] = Qt::HorPattern;
	brushStyles["Vertical"] = Qt::VerPattern;
	brushStyles["Cross"] = Qt::CrossPattern;
	brushStyles["Backward Diagonal"] = Qt::BDiagPattern;
	brushStyles["Forward Diagonal"] = Qt::FDiagPattern;
	brushStyles["Diagonal Cross"] = Qt::DiagCrossPattern;
	brushStyles["Linear Gradient"] = Qt::LinearGradientPattern;
	brushStyles["Conical Gradient"] = Qt::ConicalGradientPattern;
	brushStyles["Radial Gradient"] = Qt::RadialGradientPattern;
	brushStyles["Texture Pattern"] = Qt::TexturePattern;

	gradType["Linear"] = QGradient::LinearGradient;
	gradType["Radial"] = QGradient::RadialGradient;
	gradType["Conical"] = QGradient::ConicalGradient;
	gradType["None"] = QGradient::NoGradient;
	gradSpread["Pad"] = QGradient::PadSpread;
	gradSpread["Repeat"] = QGradient::RepeatSpread;
	gradSpread["Reflect"] = QGradient::ReflectSpread;

	descriptions[QPalette::Window] = "A general background color.";
	descriptions[QPalette::WindowText] = "A general foreground color.";
	descriptions[QPalette::Base] = "Used as the background color for text entry widgets; usually white or another light color.";
	descriptions[QPalette::AlternateBase] = "Used as the alternate background color in views with alternating row colors (see QAbstractItemView::setAlternatingRowColors()).";
	descriptions[QPalette::Text] = "The foreground color used with Base. This is usually the same as the WindowText, in which case it must provide good contrast with Window and Base.";
	descriptions[QPalette::Button] = "The general button background color. This background can be different from Window as some styles require a different background color for buttons.";
	descriptions[QPalette::ButtonText] = "A foreground color used with the Button color.";
	descriptions[QPalette::BrightText] = "A text color that is very different from WindowText, and contrasts well with e.g. Dark. Typically used for text that needs to be drawn where Text or WindowText would give poor contrast, such as on pressed push buttons. Note that text colors can be used for things other than just words; text colors are usually used for text, but it's quite common to use the text color roles for lines, icons, etc.";
	descriptions[QPalette::Light] = "Lighter than Button color.";
	descriptions[QPalette::Midlight] = "Between Button and Light.";
	descriptions[QPalette::Dark] = "Darker than Button.";
	descriptions[QPalette::Mid] = "Between Button and Dark.";
	descriptions[QPalette::Shadow] = "A very dark color. By default, the shadow color is Qt::black.";
	descriptions[QPalette::Highlight] = "A color to indicate a selected item or the current item. By default, the highlight color is Qt::darkBlue.";
	descriptions[QPalette::HighlightedText] = "A text color that contrasts with Highlight. By default, the highlighted text color is Qt::white.";
	descriptions[QPalette::Link] = "A text color used for unvisited hyperlinks. By default, the link color is Qt::blue.";
	descriptions[QPalette::LinkVisited] = "A text color used for already visited hyperlinks. By default, the linkvisited color is Qt::magenta.";
};

PaletteEditor::PaletteEditor(const QPalette& pal, QWidget *parent) : QWidget(parent), mPalette(pal)
{
	ui.setupUi(this);

	ui.RoleCombo->insertItems( 0, data.colorRoles.keys() );
	ui.GroupCombo->insertItems( 0, data.colorGroups.keys() );

	updateColor();
	updatePalette();

	connect(ui.RoleCombo, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(updateColor()));
	connect(ui.GroupCombo, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(updateColor()));

	connect(ui.RBox, SIGNAL(valueChanged(int)), this, SLOT(updatePalette()));
	connect(ui.GBox, SIGNAL(valueChanged(int)), this, SLOT(updatePalette()));
	connect(ui.BBox, SIGNAL(valueChanged(int)), this, SLOT(updatePalette()));
	connect(ui.ColorButton, SIGNAL(clicked()), this, SLOT(promptColor()));
};

QPalette PaletteEditor::currentPalette() const
{
	return mPalette;
};

void PaletteEditor::setCurrentPalette(const QPalette& pal)
{
	mPalette = pal;
};

void PaletteEditor::promptColor()
{
	QColor color = QColorDialog::getColor( QColor(ui.RBox->value(), ui.GBox->value(), ui.BBox->value()), this );
	if( !color.isValid() )
		return;

	ui.RBox->setValue( color.red()   );
	ui.GBox->setValue( color.green() );
	ui.BBox->setValue( color.blue()  );
};

QPalette PaletteEditor::importPalette(const QString& xml)
{
	PaletteData data;
	QPalette pal;

	QDomDocument doc("palette");
	doc.setContent(xml);

	QDomNodeList roles = doc.elementsByTagName("role");
	for(int i = 0; i < roles.length(); i++)
	{
		QDomElement roleElement = roles.at(i).toElement();
		QPalette::ColorRole colorRole = data.colorRoles.value( roleElement.attribute("name") );

		QDomNodeList brushes = roleElement.elementsByTagName("brush");
		for(int j = 0; j < brushes.length(); j++)
		{
			QDomElement brushElement = brushes.at(j).toElement();
			QPalette::ColorGroup colorGroup = data.colorGroups.value( brushElement.attribute("group") );
			Qt::BrushStyle brushStyle = data.brushStyles.value( brushElement.attribute("style") );

			QDomElement colorElement = brushElement.elementsByTagName("color").at(0).toElement();
			QColor brushColor( colorElement.text() );

			QBrush brush(brushColor, brushStyle);
			pal.setBrush(colorGroup, colorRole, brush);
		}
	}

	return pal;
};

QString PaletteEditor::exportPalette(const QPalette& pal)
{
	PaletteData data;

	QString xml;
	QBrush brush;
	const QGradient *grad;
	QGradientStops stops;

	xml += "<palette>\n";
	foreach(QString role, data.colorRoles.keys())
	{
		xml += "  <role name=\"" + role + "\">\n";
		foreach(QString group, data.colorGroups.keys())
		{
			brush = pal.brush(data.colorGroups.value(group), data.colorRoles.value(role));
			if(brush.style() == Qt::TexturePattern)
			{
				qWarning() << QString("Ignoring brush for ") + group + " - " + role + " because it is a texture!";
				xml += "    <brush group=\"" + group + "\"/>\n";
				continue;
			}
			if(brush.style() == Qt::NoBrush)
			{
				xml += "    <brush group=\"" + group + "\"/>\n";
				continue;
			}

			xml += "    <brush group=\"" + group + "\" style=\"" + data.brushStyles.key(brush.style()) + "\">\n";
			if(brush.style() == Qt::LinearGradientPattern || brush.style() == Qt::ConicalGradientPattern ||
				brush.style() == Qt::RadialGradientPattern)
			{
				grad = brush.gradient();
				if(grad->type() == QGradient::NoGradient)
				{
					xml += "      <gradient/>\n";
				}
				else
				{
					xml += "      <gradient type=\"" + data.gradType.key(grad->type()) + "\" spread=\"" + data.gradSpread.key(grad->spread()) + "\"/>\n";
					stops = grad->stops();
					for(int i = 0; i < stops.size(); ++i)
					{
						xml += "      <stop pos=\"" + QString::number(stops.at(i).first) + "\" color=\"" + stops.at(i).second.name() + "\"/>\n";
					}
					xml += "      </gradient>\n";
				}
			}
			else
			{
				xml += "      <color>" + brush.color().name() + "</color>\n";
			}
			xml += "    </brush>\n";
		}
		xml += "  </role>\n";
	}
	xml += "</palette>\n";

	return xml;
};

void PaletteEditor::updateColor()
{
	QPalette::ColorRole cr = data.colorRoles.value( ui.RoleCombo->currentText() );
	QPalette::ColorGroup gr = data.colorGroups.value( ui.GroupCombo->currentText() );

	QBrush brush = mPalette.brush(gr, cr);
	QColor color = brush.color();

	ui.RBox->setValue(color.red());
	ui.GBox->setValue(color.green());
	ui.BBox->setValue(color.blue());

	QPixmap pix(24, 24);
	pix.fill(color);
	ui.ColorButton->setIcon( QIcon(pix) );

	ui.Description->setText( data.descriptions.value(cr) );
};

void PaletteEditor::updatePalette()
{
	QPalette::ColorRole cr = data.colorRoles.value( ui.RoleCombo->currentText() );
	QPalette::ColorGroup gr = data.colorGroups.value( ui.GroupCombo->currentText() );

	QBrush brush = mPalette.brush(gr, cr);
	QColor color = QColor( ui.RBox->value(), ui.GBox->value(), ui.BBox->value() );

	brush.setColor(color);
	mPalette.setBrush(gr, cr, brush);

	QPixmap pix(24, 24);
	pix.fill(color);
	ui.ColorButton->setIcon( QIcon(pix) );

	ui.PreviewBox->setPalette(mPalette);
	ui.PreviewBox->repaint();
};
