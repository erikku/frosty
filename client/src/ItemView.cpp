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
		<< ui.desc_enLabel << ui.statModifiersLabel << ui.modifierPowerLabel
		<< ui.modifierAttackLabel << ui.modifierMagicPowerLabel
		<< ui.modifierGunLabel << ui.modifierVitalityLabel
		<< ui.modifierMagicLabel << ui.locationsDivider
		<< ui.modifierIntelligenceLabel << ui.modifierSupportLabel
		<< ui.modifierSpeedLabel << ui.modifierDefenseLabel
		<< ui.modifierLuckLabel << ui.modifierMagicDefenseLabel
		<< ui.elementBronzeLabel << ui.elementGoldLabel << ui.elementIronLabel
		<< ui.elementMeteoricIronLabel << ui.elementAluminumLabel
		<< ui.elementDemonicIronLabel << ui.elementSteelLabel
		<< ui.elementHeavenlyIronLabel << ui.elementSilverLabel
		<< ui.elementHihiIrokaneLabel << ui.elementCottonLabel
		<< ui.elementLeatherLabel << ui.elementHempLabel
		<< ui.elementGatorskinLabel << ui.elementSilkLabel
		<< ui.elementSnakeskinLabel << ui.elementWoolLabel
		<< ui.elementTigerskinLabel << ui.elementDownLabel
		<< ui.elementCedarLabel << ui.elementOakLabel << ui.elementOilLabel
		<< ui.elementCypressLabel << ui.elementGunpowderLabel
		<< ui.elementHolyWoodLabel << ui.elementPlasticLabel
		<< ui.elementDemonWoodLabel << ui.elementTreeSapLabel
		<< ui.elementWhetstoneLabel << ui.elementCircuitryLabel
		<< ui.elementLegOrbLabel << ui.elementSpiritOrbLabel
		<< ui.elementWaxLabel << ui.elementGlassLabel << ui.elementRubberLabel
		<< ui.locations_jaLabel << ui.locations_enLabel << ui.elementsLabel1
		<< ui.elementsLabel2 << ui.elementsLabel3 << ui.elementsLabel4
		<< ui.affinitySlashLabel << ui.affinityAssaultLabel
		<< ui.affinityBlowLabel << ui.affinityGunLabel << ui.affinityShotLabel
		<< ui.affinityPierceLabel << ui.affinityBlazeLabel
		<< ui.affinityFreezeLabel << ui.affinityShockLabel
		<< ui.affinityElectricShockLabel << ui.affinityHamaLabel
		<< ui.affinityMudoLabel << ui.affinityMagicForceLabel
		<< ui.affinityNerveLabel << ui.affinityMindLabel
		<< ui.affinitySuicideLabel << ui.affinityAlmightyLabel
		<< ui.affinitySpecialLabel << ui.affinitiesLabel
		<< ui.modLabel << ui.modCompatLabel << ui.stackSizeLabel
		<< ui.separatorWidget << ui.descSeparator << ui.modifierCriticalLabel;

	// Edit labels
	widgets << ui.elementsLabel2 << ui.nameLabel2 << ui.japaneseNameLabel2
		<< ui.englishNameLabel2 << ui.durabilityLabel2 << ui.buyPriceLabel2
		<< ui.tradableLabel2 << ui.sellPriceLabel2 << ui.genderLabel2
		<< ui.typeLabel2 << ui.equipTypeLabel2 << ui.affinityLabel2
		<< ui.lncReqLabel2 << ui.fusion2Label2 << ui.slotCountLabel2
		<< ui.cashShopLabel2 << ui.levelReqLabel2 << ui.fusion1Label2
		<< ui.desc_jaLabel2 << ui.desc_enLabel2 << ui.statModifiersLabel2
		<< ui.modifierPowerLabel2 << ui.modifierAttackLabel2
		<< ui.modifierMagicPowerLabel2 << ui.modifierGunLabel2
		<< ui.modifierVitalityLabel2 << ui.modifierMagicLabel2
		<< ui.modifierIntelligenceLabel2 << ui.modifierSupportLabel2
		<< ui.modifierSpeedLabel2 << ui.modifierDefenseLabel2
		<< ui.modifierLuckLabel2 << ui.modifierMagicDefenseLabel2
		<< ui.elementBronzeLabel2 << ui.elementGoldLabel2
		<< ui.elementIronLabel2 << ui.elementMeteoricIronLabel2
		<< ui.elementAluminumLabel2 << ui.elementDemonicIronLabel2
		<< ui.elementSteelLabel2 << ui.elementHeavenlyIronLabel2
		<< ui.elementSilverLabel2 << ui.elementHihiIrokaneLabel2
		<< ui.elementCottonLabel2 << ui.elementLeatherLabel2
		<< ui.elementHempLabel2 << ui.elementGatorskinLabel2
		<< ui.elementSilkLabel2 << ui.elementSnakeskinLabel2
		<< ui.elementWoolLabel2 << ui.elementTigerskinLabel2
		<< ui.elementDownLabel2 << ui.elementCedarLabel2
		<< ui.elementOakLabel2 << ui.elementOilLabel2
		<< ui.elementCypressLabel2 << ui.elementGunpowderLabel2
		<< ui.elementHolyWoodLabel2 << ui.elementPlasticLabel2
		<< ui.elementDemonWoodLabel2 << ui.elementTreeSapLabel2
		<< ui.elementWhetstoneLabel2 << ui.elementCircuitryLabel2
		<< ui.elementLegOrbLabel2 << ui.elementSpiritOrbLabel2
		<< ui.elementWaxLabel2 << ui.elementGlassLabel2
		<< ui.elementRubberLabel2 << ui.locations_jaLabel2
		<< ui.locations_enLabel2 << ui.elementsLabel5
		<< ui.elementsLabel6 << ui.elementsLabel7 << ui.elementsLabel8
		<< ui.affinitySlashLabel2 << ui.separatorWidget2
		<< ui.affinityAssaultLabel2 << ui.affinityBlowLabel2
		<< ui.affinityGunLabel2 << ui.affinityShotLabel2
		<< ui.affinityPierceLabel2 << ui.affinityBlazeLabel2
		<< ui.affinityFreezeLabel2 << ui.affinityShockLabel2
		<< ui.affinityElectricShockLabel2 << ui.affinityHamaLabel2
		<< ui.affinityMudoLabel2 << ui.affinityMagicForceLabel2
		<< ui.affinityNerveLabel2 << ui.affinityMindLabel2
		<< ui.affinitySuicideLabel2 << ui.affinityAlmightyLabel2
		<< ui.affinitySpecialLabel2 << ui.locationsEditDivider
		<< ui.affinitiesLabel2 << ui.stackSizeLabel2
		<< ui.modLabel2 << ui.modCompatLabel2 << ui.stackSizeLabel2
		<< ui.descSeparator2 << ui.modifierCriticalLabel2;

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
	bindRelation("equip_type", ui.equipType, ui.equipTypeEdit, "db_equip_type",
		QString(), ui.equipTypeEditButton, tr("Equip Type"));
	bindRelation("affinity", ui.affinity, ui.affinityEdit, "db_affinity",
		QString(), ui.affinityEditButton, tr("Affinity"));
	bindEnum("lnc_req", ui.lncReq, ui.lncReqEdit, lncEnum, 0);
	bindBool("fusion1", ui.fusion1, ui.fusion1Edit,
		QString(), QString(), true);
	bindBool("fusion2", ui.fusion2, ui.fusion2Edit,
		QString(), QString(), true);
	bindNumber("slot_count", ui.slotCount, ui.slotCountEdit, 0);
	bindNumber("level_req", ui.levelReq, ui.levelReqEdit);
	bindBool("cash_shop", ui.cashShop, ui.cashShopEdit);
	bindTextBox("desc_en", ui.desc_en, ui.desc_enEdit);
	bindTextBox("desc_ja", ui.desc_ja, ui.desc_jaEdit);
	bindTextBox("locations_en", ui.locations_en, ui.locations_enEdit);
	bindTextBox("locations_ja", ui.locations_ja, ui.locations_jaEdit);
	bindNumber("modifier_power", ui.modifierPower, ui.modifierPowerEdit, 0);
	bindNumber("modifier_magic_power", ui.modifierMagicPower,
		ui.modifierMagicPowerEdit, 0);
	bindNumber("modifier_vitality", ui.modifierVitality,
		ui.modifierVitalityEdit, 0);
	bindNumber("modifier_intelligence", ui.modifierIntelligence,
		ui.modifierIntelligenceEdit, 0);
	bindNumber("modifier_speed", ui.modifierSpeed, ui.modifierSpeedEdit, 0);
	bindNumber("modifier_luck", ui.modifierLuck, ui.modifierLuckEdit, 0);
	bindNumber("modifier_critical", ui.modifierCritical, ui.modifierCriticalEdit, 0);
	bindNumber("modifier_attack", ui.modifierAttack, ui.modifierAttackEdit, 0);
	bindNumber("modifier_gun", ui.modifierGun, ui.modifierGunEdit, 0);
	bindNumber("modifier_magic", ui.modifierMagic, ui.modifierMagicEdit, 0);
	bindNumber("modifier_support", ui.modifierSupport,
		ui.modifierSupportEdit, 0);
	bindNumber("modifier_defense", ui.modifierDefense,
		ui.modifierDefenseEdit, 0);
	bindNumber("modifier_magic_defense", ui.modifierMagicDefense,
		ui.modifierMagicDefenseEdit, 0);
	bindNumber("modifier_slash", ui.affinitySlash, ui.affinitySlashEdit, 0);
	bindNumber("modifier_assault", ui.affinityAssault,
		ui.affinityAssaultEdit, 0);
	bindNumber("modifier_blow", ui.affinityBlow, ui.affinityBlowEdit, 0);
	bindNumber("modifier_gun_dmg", ui.affinityGun, ui.affinityGunEdit, 0);
	bindNumber("modifier_shot", ui.affinityShot, ui.affinityShotEdit, 0);
	bindNumber("modifier_pierce", ui.affinityPierce, ui.affinityPierceEdit, 0);
	bindNumber("modifier_blaze", ui.affinityBlaze, ui.affinityBlazeEdit, 0);
	bindNumber("modifier_freeze", ui.affinityFreeze, ui.affinityFreezeEdit, 0);
	bindNumber("modifier_shock", ui.affinityShock, ui.affinityShockEdit, 0);
	bindNumber("modifier_electric_shock", ui.affinityElectricShock,
		ui.affinityElectricShockEdit, 0);
	bindNumber("modifier_hama", ui.affinityHama, ui.affinityHamaEdit, 0);
	bindNumber("modifier_mudo", ui.affinityMudo, ui.affinityMudoEdit, 0);
	bindNumber("modifier_magic_force", ui.affinityMagicForce,
		ui.affinityMagicForceEdit, 0);
	bindNumber("modifier_nerve", ui.affinityNerve, ui.affinityNerveEdit, 0);
	bindNumber("modifier_mind", ui.affinityMind, ui.affinityMindEdit, 0);
	bindNumber("modifier_suicide", ui.affinitySuicide,
		ui.affinitySuicideEdit, 0);
	bindNumber("modifier_almighty", ui.affinityAlmighty,
		ui.affinityAlmightyEdit, 0);
	bindNumber("modifier_special", ui.affinitySpecial,
		ui.affinitySpecialEdit, 0);
	bindNumber("element_bronze", ui.elementBronze, ui.elementBronzeEdit, 0);
	bindNumber("element_iron", ui.elementIron, ui.elementIronEdit, 0);
	bindNumber("element_aluminum", ui.elementAluminum,
		ui.elementAluminumEdit, 0);
	bindNumber("element_steel", ui.elementSteel, ui.elementSteelEdit, 0);
	bindNumber("element_silver", ui.elementSilver, ui.elementSilverEdit, 0);
	bindNumber("element_gold", ui.elementGold, ui.elementGoldEdit, 0);
	bindNumber("element_meteoric_iron", ui.elementMeteoricIron,
		ui.elementMeteoricIronEdit, 0);
	bindNumber("element_demonic_iron", ui.elementDemonicIron,
		ui.elementDemonicIronEdit, 0);
	bindNumber("element_heavenly_iron", ui.elementHeavenlyIron,
		ui.elementHeavenlyIronEdit, 0);
	bindNumber("element_hihi_irokane", ui.elementHihiIrokane,
		ui.elementHihiIrokaneEdit, 0);
	bindNumber("element_cotton", ui.elementCotton, ui.elementCottonEdit, 0);
	bindNumber("element_hemp", ui.elementHemp, ui.elementHempEdit, 0);
	bindNumber("element_silk", ui.elementSilk, ui.elementSilkEdit, 0);
	bindNumber("element_wool", ui.elementWool, ui.elementWoolEdit, 0);
	bindNumber("element_down", ui.elementDown, ui.elementDownEdit, 0);
	bindNumber("element_leather", ui.elementLeather,
		ui.elementLeatherEdit, 0);
	bindNumber("element_gatorskin", ui.elementGatorskin,
		ui.elementGatorskinEdit, 0);
	bindNumber("element_snakeskin", ui.elementSnakeskin,
		ui.elementSnakeskinEdit, 0);
	bindNumber("element_tigerskin", ui.elementTigerskin,
		ui.elementTigerskinEdit, 0);
	bindNumber("element_cedar", ui.elementCedar, ui.elementCedarEdit, 0);
	bindNumber("element_oak", ui.elementOak, ui.elementOakEdit, 0);
	bindNumber("element_cypress", ui.elementCypress,
		ui.elementCypressEdit, 0);
	bindNumber("element_holy_wood", ui.elementHolyWood,
		ui.elementHolyWoodEdit, 0);
	bindNumber("element_demon_wood", ui.elementDemonWood,
		ui.elementDemonWoodEdit, 0);
	bindNumber("element_whetstone", ui.elementWhetstone,
		ui.elementWhetstoneEdit, 0);
	bindNumber("element_oil", ui.elementOil, ui.elementOilEdit, 0);
	bindNumber("element_gunpowder", ui.elementGunpowder,
		ui.elementGunpowderEdit, 0);
	bindNumber("element_plastic", ui.elementPlastic,
		ui.elementPlasticEdit, 0);
	bindNumber("element_tree_sap", ui.elementTreeSap,
		ui.elementTreeSapEdit, 0);
	bindNumber("element_circuitry", ui.elementCircuitry,
		ui.elementCircuitryEdit, 0);
	bindNumber("element_leg_orb", ui.elementLegOrb,
		ui.elementLegOrbEdit, 0);
	bindNumber("element_spirit_orb", ui.elementSpiritOrb,
		ui.elementSpiritOrbEdit, 0);
	bindNumber("element_wax", ui.elementWax, ui.elementWaxEdit, 0);
	bindNumber("element_glass", ui.elementGlass, ui.elementGlassEdit, 0);
	bindNumber("element_rubber", ui.elementRubber, ui.elementRubberEdit, 0);
	bindNumber("element_bronze_chance", ui.elementBronzeChance,
		ui.elementBronzeChanceEdit, 0, tr("%1%"));
	bindNumber("element_iron_chance", ui.elementIronChance,
		ui.elementIronChanceEdit, 0, tr("%1%"));
	bindNumber("element_aluminum_chance", ui.elementAluminumChance,
		ui.elementAluminumChanceEdit, 0, tr("%1%"));
	bindNumber("element_steel_chance", ui.elementSteelChance,
		ui.elementSteelChanceEdit, 0, tr("%1%"));
	bindNumber("element_silver_chance", ui.elementSilverChance,
		ui.elementSilverChanceEdit, 0, tr("%1%"));
	bindNumber("element_gold_chance", ui.elementGoldChance,
		ui.elementGoldChanceEdit, 0, tr("%1%"));
	bindNumber("element_meteoric_iron_chance", ui.elementMeteoricIronChance,
		ui.elementMeteoricIronChanceEdit, 0, tr("%1%"));
	bindNumber("element_demonic_iron_chance", ui.elementDemonicIronChance,
		ui.elementDemonicIronChanceEdit, 0, tr("%1%"));
	bindNumber("element_heavenly_iron_chance", ui.elementHeavenlyIronChance,
		ui.elementHeavenlyIronChanceEdit, 0, tr("%1%"));
	bindNumber("element_hihi_irokane_chance", ui.elementHihiIrokaneChance,
		ui.elementHihiIrokaneChanceEdit, 0, tr("%1%"));
	bindNumber("element_cotton_chance", ui.elementCottonChance,
		ui.elementCottonChanceEdit, 0, tr("%1%"));
	bindNumber("element_hemp_chance", ui.elementHempChance,
		ui.elementHempChanceEdit, 0, tr("%1%"));
	bindNumber("element_silk_chance", ui.elementSilkChance,
		ui.elementSilkChanceEdit, 0, tr("%1%"));
	bindNumber("element_wool_chance", ui.elementWoolChance,
		ui.elementWoolChanceEdit, 0, tr("%1%"));
	bindNumber("element_down_chance", ui.elementDownChance,
		ui.elementDownChanceEdit, 0, tr("%1%"));
	bindNumber("element_leather_chance", ui.elementLeatherChance,
		ui.elementLeatherChanceEdit, 0, tr("%1%"));
	bindNumber("element_gatorskin_chance", ui.elementGatorskinChance,
		ui.elementGatorskinChanceEdit, 0, tr("%1%"));
	bindNumber("element_snakeskin_chance", ui.elementSnakeskinChance,
		ui.elementSnakeskinChanceEdit, 0, tr("%1%"));
	bindNumber("element_tigerskin_chance", ui.elementTigerskinChance,
		ui.elementTigerskinChanceEdit, 0, tr("%1%"));
	bindNumber("element_cedar_chance", ui.elementCedarChance,
		ui.elementCedarChanceEdit, 0, tr("%1%"));
	bindNumber("element_oak_chance", ui.elementOakChance,
		ui.elementOakChanceEdit, 0, tr("%1%"));
	bindNumber("element_cypress_chance", ui.elementCypressChance,
		ui.elementCypressChanceEdit, 0, tr("%1%"));
	bindNumber("element_holy_wood_chance", ui.elementHolyWoodChance,
		ui.elementHolyWoodChanceEdit, 0, tr("%1%"));
	bindNumber("element_demon_wood_chance", ui.elementDemonWoodChance,
		ui.elementDemonWoodChanceEdit, 0, tr("%1%"));
	bindNumber("element_whetstone_chance", ui.elementWhetstoneChance,
		ui.elementWhetstoneChanceEdit, 0, tr("%1%"));
	bindNumber("element_oil_chance", ui.elementOilChance,
		ui.elementOilChanceEdit, 0, tr("%1%"));
	bindNumber("element_gunpowder_chance", ui.elementGunpowderChance,
		ui.elementGunpowderChanceEdit, 0, tr("%1%"));
	bindNumber("element_plastic_chance", ui.elementPlasticChance,
		ui.elementPlasticChanceEdit, 0, tr("%1%"));
	bindNumber("element_tree_sap_chance", ui.elementTreeSapChance,
		ui.elementTreeSapChanceEdit, 0, tr("%1%"));
	bindNumber("element_circuitry_chance", ui.elementCircuitryChance,
		ui.elementCircuitryChanceEdit, 0, tr("%1%"));
	bindNumber("element_leg_orb_chance", ui.elementLegOrbChance,
		ui.elementLegOrbChanceEdit, 0, tr("%1%"));
	bindNumber("element_spirit_orb_chance", ui.elementSpiritOrbChance,
		ui.elementSpiritOrbChanceEdit, 0, tr("%1%"));
	bindNumber("element_wax_chance", ui.elementWaxChance,
		ui.elementWaxChanceEdit, 0, tr("%1%"));
	bindNumber("element_glass_chance", ui.elementGlassChance,
		ui.elementGlassChanceEdit, 0, tr("%1%"));
	bindNumber("element_rubber_chance", ui.elementRubberChance,
		ui.elementRubberChanceEdit, 0, tr("%1%"));
	bindRelation("mod_type", ui.modType, ui.modTypeEdit,
		"db_mod_type", QString(), ui.modTypeEditButton,
		tr("Modification Type"), tr("N/A"));
	bindRelation("mod_compat", ui.modCompat, ui.modCompatEdit,
		"db_mod_compat", QString(), ui.modCompatEditButton,
		tr("Modification Compatability"), tr("N/A"));
	bindNumber("mod_level", ui.modLevel, ui.modLevelEdit, -1);
	bindNumber("mod_slot", ui.modSlot, ui.modSlotEdit, -1);
	bindText("mod_desc", ui.modDesc, ui.modDescEdit);

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
