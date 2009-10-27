/******************************************************************************\
*  client/src/ItemView.cpp                                                     *
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

#include "ItemView.h"

#include <QtGui/QMessageBox>

ItemView::ItemView(QWidget *parent_widget) : AjaxView(parent_widget)
{
	ui.setupUi(this);

	QList<QWidget*> widgets;

	// View Labels
	widgets << ui.nameLabel << ui.japaneseNameLabel << ui.englishNameLabel
		<< ui.durabilityLabel << ui.buyPriceLabel << ui.tradableLabel
		<< ui.sellPriceLabel << ui.genderLabel << ui.typeLabel
		<< ui.equipTypeLabel << ui.affinityLabel << ui.lncReqLabel
		<< ui.fusion2Label << ui.slotCountLabel << ui.cashShopLabel
		<< ui.levelReqLabel << ui.fusion1Label << ui.desc_jaLabel
		<< ui.desc_enLabel << ui.locations_jaLabel << ui.locations_enLabel
		<< ui.modLabel << ui.modCompatLabel << ui.stackSizeLabel
		<< ui.descSeparator << ui.locationsDivider << ui.stockLabel
		<< ui.levelCapLabel << ui.notesLabel;

	// Edit labels
	widgets << ui.nameLabel2 << ui.japaneseNameLabel2
		<< ui.englishNameLabel2 << ui.durabilityLabel2 << ui.buyPriceLabel2
		<< ui.tradableLabel2 << ui.sellPriceLabel2 << ui.genderLabel2
		<< ui.typeLabel2 << ui.equipTypeLabel2 << ui.affinityLabel2
		<< ui.lncReqLabel2 << ui.fusion2Label2 << ui.slotCountLabel2
		<< ui.cashShopLabel2 << ui.levelReqLabel2 << ui.fusion1Label2
		<< ui.desc_jaLabel2 << ui.desc_enLabel2 << ui.locations_jaLabel2
		<< ui.locations_enLabel2 << ui.locationsEditDivider
		<< ui.stackSizeLabel2 << ui.modLabel2 << ui.modCompatLabel2
		<< ui.stackSizeLabel2 << ui.descSeparator2 << ui.stockLabel2
		<< ui.levelCapLabel2 << ui.notesLabel2;

	foreach(QWidget *widget, widgets)
		darkenWidget(widget);

	QMap<int, QString> genderEnum;
	genderEnum[0] = tr("N/A");
	genderEnum[1] = tr("Male");
	genderEnum[2] = tr("Female");

	QMap<int, QString> lncEnum;
	lncEnum[0] = tr("N/A");
	lncEnum[1] = tr("Law");
	lncEnum[2] = tr("Neutral");
	lncEnum[3] = tr("Chaos");

	// Bind the data
	bindText("name_en", ui.englishName, ui.englishNameEdit);
	bindText("name_ja", ui.japaneseName, ui.japaneseNameEdit);
	bindIcon("icon", ui.icon, ui.iconEdit, "icons/items/icon_%1.png",
		"icons/items");
	bindEnum("gender", ui.gender, ui.genderEdit, genderEnum, 0);
	bindNumber("durability", ui.durability, ui.durabilityEdit, 0);
	bindNumber("buy_price", ui.buyPrice, ui.buyPriceEdit, 0);
	bindNumber("sell_price", ui.sellPrice, ui.sellPriceEdit, 0);
	bindBool("tradable", ui.tradable, ui.tradeableEdit,
		QString(), QString(), true);
	bindNumber("stack_size", ui.stackSize, ui.stackSizeEdit, 1);
	bindRelation("type", ui.type, ui.typeEdit, "db_item_type",
		QString(), ui.typeEditButton, tr("Item Type"));
	bindRelation("equip_type", ui.equipType, ui.equipTypeEdit, "db_item_equip_type",
		QString(), ui.equipTypeEditButton, tr("Equip Type"));
	bindRelation("affinity", ui.affinity, ui.affinityEdit, "db_item_affinity",
		QString(), ui.affinityEditButton, tr("Affinity"));
	bindNumber("stock", ui.stock, ui.stockEdit, 0);
	bindEnum("lnc_req", ui.lncReq, ui.lncReqEdit, lncEnum, 0);
	bindBool("fusion1", ui.fusion1, ui.fusion1Edit,
		QString(), QString(), true);
	bindBool("fusion2", ui.fusion2, ui.fusion2Edit,
		QString(), QString(), true);
	bindNumber("slot_count", ui.slotCount, ui.slotCountEdit, 0);
	bindNumber("level_req", ui.levelReq, ui.levelReqEdit);
	bindNumber("level_cap", ui.levelCap, ui.levelCapEdit);
	bindBool("cash_shop", ui.cashShop, ui.cashShopEdit);
	bindTextBox("desc_en", ui.desc_en, ui.desc_enEdit);
	bindTextBox("desc_ja", ui.desc_ja, ui.desc_jaEdit);
	bindTextBox("locations_en", ui.locations_en, ui.locations_enEdit);
	bindTextBox("locations_ja", ui.locations_ja, ui.locations_jaEdit);
	bindTextBox("notes", ui.notes, ui.notesEdit);
	bindRelation("mod_type", ui.modType, ui.modTypeEdit,
		"db_mod_type", QString(), ui.modTypeEditButton,
		tr("Modification Type"), tr("N/A"));
	bindRelation("mod_compat", ui.modCompat, ui.modCompatEdit,
		"db_mod_compat", QString(), ui.modCompatEditButton,
		tr("Modification Compatability"), tr("N/A"));
	bindNumber("mod_level", ui.modLevel, ui.modLevelEdit, -1);
	bindNumber("mod_slot", ui.modSlot, ui.modSlotEdit, -1);
	bindText("mod_desc", ui.modDesc, ui.modDescEdit);
	bindStatRelation("db_item_stat_pair", ui.stats, ui.statsEditList,
		"item", "stat", "db_item_stats", "name_{$lang}", "val",
		ui.statsEditAdd, ui.statsEditEdit, ui.statsEditRemove,
		ui.statsEditSearchAdd, ui.statsEditSearchCancel, ui.statsEditAddSearch,
		ui.statsEditAddList, tr("What is the value for this stat?"),
		ui.statsEditStack);

	// Give the relation buttons an icon
	QIcon edit_icon(":/edit.png");
	ui.typeEditButton->setIcon(edit_icon);
	ui.equipTypeEditButton->setIcon(edit_icon);
	ui.affinityEditButton->setIcon(edit_icon);
	ui.modTypeEditButton->setIcon(edit_icon);
	ui.modCompatEditButton->setIcon(edit_icon);

	// Init the view
	initView(ui.stackedWidget, ui.editButton, ui.cancelButton,
		ui.refreshButton, ui.updateButton);

	ui.selectionWidget->widget(2)->hide();
	ui.selectionWidget->removeTab(2);

	ui.fusion1->hide();
	ui.fusion2->hide();
	ui.cashShop->hide();
	ui.fusion1Label->hide();
	ui.fusion2Label->hide();
	ui.cashShopLabel->hide();
}

bool ItemView::checkValues()
{
	// Check name_ja and name_en
	if( ui.japaneseNameEdit->text().isEmpty() ||
		ui.englishNameEdit->text().isEmpty() )
	{
		QMessageBox::warning(this, tr("Update Error"), tr("You must both a "
			"valid Japanese and English item name."));

		return false;
	}

	// Check icon
	if( ui.iconEdit->value().isEmpty() )
	{
		QMessageBox::warning(this, tr("Update Error"), tr("You must set a "
			"valid icon."));

		return false;
	}

	return true;
}

QString ItemView::table() const
{
	return QString("db_items");
}

QString ItemView::addWarningTitle() const
{
	return tr("Add Item");
}

QString ItemView::addWarningMessage() const
{
	return tr("You are attemping to add another item while editing a item. "
			"If you continue you will lose any changes you made to the item. "
			"Are you sure you want to continue?");
}
