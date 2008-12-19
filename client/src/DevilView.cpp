/******************************************************************************\
*  client/src/DevilView.cpp                                                    *
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

#include "DevilView.h"
#include "InfoListWidgetItem.h"

#include <QtGui/QMessageBox>
#include <QtGui/QButtonGroup>

bool numberSortFunction2(const InfoListWidgetItem *item1,
	const InfoListWidgetItem *item2)
{
	return item1->text2().toInt() < item2->text2().toInt();
}

DevilView::DevilView(QWidget *parent_widget) : AjaxView(parent_widget)
{
	ui.setupUi(this);

	QList<QWidget*> widgets;

	// View Labels
	widgets << ui.nameLabel << ui.englishNameLabel << ui.japaneseNameLabel
		<< ui.levelLabel << ui.lncLabel << ui.genusLabel << ui.growthLabel
		<< ui.statsLabel << ui.statPowerLabel << ui.statMagicPowerLabel
		<< ui.statVitalityLabel << ui.statIntelligenceLabel << ui.statSpeedLabel
		<< ui.statLuckLabel << ui.statAttackLabel << ui.statGunLabel
		<< ui.statMagicLabel << ui.statSupportLabel << ui.statDefenseLabel
		<< ui.statMagicDefenseLabel << ui.hpLabel << ui.mpLabel << ui.magLabel
		<< ui.seireiLabel << ui.parentDevilLabel << ui.mashouLabel
		<< ui.fusionLabel << ui.fusionRangeLabel << ui.chanceLabel
		<< ui.fusion2Label << ui.fusion3Label << ui.convoLabel << ui.greetLabel
		<< ui.provokeLabel << ui.tauntLabel << ui.seireiModLabel
		<< ui.earthiesLabel << ui.airiesLabel << ui.aquansLabel
		<< ui.flamiesLabel << ui.inheritanceLabel << ui.inheritBreathLabel
		<< ui.inheritWingLabel << ui.inheritPierceLabel << ui.inheritFangLabel
		<< ui.inheritClawLabel << ui.inheritNeedleLabel << ui.inheritSwordLabel
		<< ui.inheritStrangeLabel << ui.inheritSpecialLabel << ui.skillsLabel
		<< ui.traitsLabel << ui.locationsLabel << ui.notesLabel
		<< ui.weakpointLabel << ui.weakpointLabel1 << ui.protectionLabel1
		<< ui.nullLabel1 << ui.reflectLabel1 << ui.absorbLabel1
		<< ui.weakpointLabel2 << ui.protectionLabel2 << ui.nullLabel2
		<< ui.reflectLabel2 << ui.absorbLabel2 << ui.wpAllPhysicalLabel
		<< ui.wpAllMagicLabel << ui.wpSlashLabel << ui.wpAssaultLabel
		<< ui.wpBlowLabel << ui.wpGunLabel << ui.wpShotLabel << ui.wpPierceLabel
		<< ui.wpBlazeLabel << ui.wpFreezeLabel << ui.wpShockLabel
		<< ui.wpElectricShockLabel << ui.wpHamaLabel << ui.wpMudoLabel
		<< ui.wpMagicForceLabel << ui.wpNerveLabel << ui.wpMindLabel
		<< ui.wpSuicideLabel << ui.wpAlmightyLabel << ui.wpSpecialLabel
		<< ui.separatorWidget << ui.devilBookLabel;

	// Edit labels
	widgets << ui.nameEditLabel << ui.englishNameEditLabel
		<< ui.japaneseNameEditLabel << ui.levelEditLabel << ui.lncEditLabel
		<< ui.genusEditLabel << ui.growthEditLabel << ui.statsEditLabel
		<< ui.statPowerEditLabel << ui.statMagicPowerEditLabel
		<< ui.statVitalityEditLabel << ui.statIntelligenceEditLabel
		<< ui.statSpeedEditLabel << ui.statLuckEditLabel
		<< ui.statAttackEditLabel << ui.statGunEditLabel
		<< ui.statMagicEditLabel << ui.statSupportEditLabel
		<< ui.statDefenseEditLabel << ui.statMagicDefenseEditLabel
		<< ui.hpEditLabel << ui.mpEditLabel << ui.magEditLabel
		<< ui.seireiEditLabel << ui.parentDevilEditLabel << ui.mashouEditLabel
		<< ui.fusionEditLabel << ui.fusionRangeEditLabel << ui.chanceEditLabel
		<< ui.convoEditLabel << ui.seireiModEditLabel << ui.earthiesEditLabel
		<< ui.airiesEditLabel << ui.aquansEditLabel << ui.flamiesEditLabel
		<< ui.inheritanceEditLabel << ui.skillsEditLabel << ui.traitsEditLabel
		<< ui.locationsEditLabel << ui.notesEditLabel << ui.weakpointEditLabel
		<< ui.weakpointEditLabel1 << ui.protectionEditLabel1
		<< ui.nullEditLabel1 << ui.reflectEditLabel1 << ui.absorbEditLabel1
		<< ui.weakpointEditLabel2 << ui.protectionEditLabel2
		<< ui.nullEditLabel2 << ui.reflectEditLabel2 << ui.absorbEditLabel2
		<< ui.wpAllPhysicalEditLabel << ui.wpAllMagicEditLabel
		<< ui.wpSlashEditLabel << ui.wpAssaultEditLabel << ui.wpBlowEditLabel
		<< ui.wpGunEditLabel << ui.wpShotEditLabel << ui.wpPierceEditLabel
		<< ui.wpBlazeEditLabel << ui.wpFreezeEditLabel << ui.wpShockEditLabel
		<< ui.wpElectricShockEditLabel << ui.wpHamaEditLabel
		<< ui.wpMudoEditLabel << ui.wpMagicForceEditLabel << ui.wpNerveEditLabel
		<< ui.wpMindEditLabel << ui.wpSuicideEditLabel << ui.wpAlmightyEditLabel
		<< ui.wpSpecialEditLabel << ui.devilBookEditLabel
		<< ui.skillsEditAddLabel << ui.traitsEditAddLabel
		<< ui.locationsEditAddLabel;

	foreach(QWidget *widget, widgets)
		darkenWidget(widget);

	QList<QRadioButton*> selectors;
	selectors << ui.loyaltyRank1 << ui.loyaltyRank2 << ui.loyaltyRank3
		<< ui.loyaltyRank4 << ui.loyaltyRank5 << ui.loyaltyRank6
		<< ui.loyaltyRank7 << ui.loyaltyRank8;

	QButtonGroup *loyaltyGroup = new QButtonGroup;
	foreach(QRadioButton *button, selectors)
		loyaltyGroup->addButton(button);

	QList<QRadioButton*> edit_selectors;
	edit_selectors << ui.loyaltyRank1Edit << ui.loyaltyRank2Edit
		<< ui.loyaltyRank3Edit << ui.loyaltyRank4Edit << ui.loyaltyRank5Edit
		<< ui.loyaltyRank6Edit << ui.loyaltyRank7Edit << ui.loyaltyRank8Edit;

	QButtonGroup *loyaltyEditGroup = new QButtonGroup;
	foreach(QRadioButton *button, edit_selectors)
		loyaltyEditGroup->addButton(button);

	QMap<int, QString> lncEnum;
	lncEnum[0] = tr("Law");
	lncEnum[1] = tr("Neutral");
	lncEnum[2] = tr("Chaos");

	QMap<int, QString> seireiEnum;
	seireiEnum[0] = tr("N/A");
	seireiEnum[1] = tr("Earthies");
	seireiEnum[2] = tr("Airies");
	seireiEnum[3] = tr("Aquans");
	seireiEnum[4] = tr("Flamies");

	QMap<int, QString> modifierEnum;
	modifierEnum[0] = tr("UP");
	modifierEnum[1] = tr("N/A");
	modifierEnum[2] = tr("DOWN");

	QVariantMap relation1_columns;
	relation1_columns["name_ja"] = "expert_ja";
	relation1_columns["name_en"] = "expert_en";

	QVariantMap relation1;
	relation1["id"] = "db_skills.expert";
	relation1["table"] = "db_expert";
	relation1["foreign_id"] = "id";
	relation1["columns"] = relation1_columns;

	QVariantMap relation2_columns;
	relation2_columns["name_ja"] = "category_ja";
	relation2_columns["name_en"] = "category_en";

	QVariantMap relation2;
	relation2["id"] = "db_skills.category";
	relation2["table"] = "db_category";
	relation2["foreign_id"] = "id";
	relation2["columns"] = relation2_columns;

	// Bind the data
	bindText("name_en", ui.englishName, ui.englishNameEdit);
	bindText("name_ja", ui.japaneseName, ui.japaneseNameEdit);
	bindIcon("icon", ui.icon, ui.iconEdit, "icons/devils/icon_%1.png",
		"icons/devils");
	bindNumber("lvl", ui.level, ui.levelEdit);
	bindEnum("lnc", ui.lnc, ui.lncEdit, lncEnum, 1);
	bindRelation("genus", ui.genus, ui.genusEdit, "db_genus", QString(),
		ui.genusEditButton, tr("Genus"));
	bindRelation("growth", ui.growth, ui.growthEdit, "db_growth_type");
	bindNumber("hp", ui.hp, ui.hpEdit);
	bindNumber("mp", ui.mp, ui.mpEdit);
	bindNumber("mag", ui.mag, ui.magEdit);
	bindBool("devil_book", ui.devilBook, ui.devilBookEdit);
	bindEnum("seirei", ui.seirei, ui.seireiEdit, seireiEnum);
	bindRelation("parent", ui.parentDevil, ui.parentDevilEdit, "db_devils",
		QString(), 0, QString(), tr("N/A"));
	bindRelation("mashou", ui.mashou, ui.mashouEdit, "db_mashou",
		QString(), 0, QString(), tr("N/A"));
	bindBool("fusion2", ui.fusion2, ui.fusion2Edit);
	bindBool("fusion3", ui.fusion3, ui.fusion3Edit);
	bindNumberRange("fusion_range", ui.fusionRange, ui.fusionRangeFromEdit,
		ui.fusionRangeToEdit, "fuse_min_lvl", "fuse_max_lvl", -1, -1, true);
	bindNumber("fuse_chance", ui.chance, ui.chanceEdit, 100, "%1%");
	bindBool("greet", ui.greet, ui.greetEdit);
	bindBool("provoke", ui.provoke, ui.provokeEdit);
	bindBool("taunt", ui.taunt, ui.tauntEdit);
	bindEnum("earthies", ui.earthies, ui.earthiesEdit, modifierEnum, 1);
	bindEnum("airies", ui.airies, ui.airiesEdit, modifierEnum, 1);
	bindEnum("aquans", ui.aquans, ui.aquansEdit, modifierEnum, 1);
	bindEnum("flamies", ui.flamies, ui.flamiesEdit, modifierEnum, 1);
	bindBool("breath", ui.inheritBreath, ui.inheritBreathEdit);
	bindBool("wing", ui.inheritWing, ui.inheritWingEdit);
	bindBool("pierce", ui.inheritPierce, ui.inheritPierceEdit);
	bindBool("fang", ui.inheritFang, ui.inheritFangEdit);
	bindBool("claw", ui.inheritClaw, ui.inheritClawEdit);
	bindBool("needle", ui.inheritNeedle, ui.inheritNeedleEdit);
	bindBool("sword", ui.inheritSword, ui.inheritSwordEdit);
	bindBool("strange", ui.inheritStrange, ui.inheritStrangeEdit);
	bindBool("eye", ui.inheritEye, ui.inheritEyeEdit);
	bindTextBox("notes", ui.notes, ui.notesEdit);
	bindNumberSelector("loyalty_power", ui.statPower, ui.statPowerEdit,
		radioMap("power", selectors), radioMap("power", edit_selectors),
		loyaltyGroup, loyaltyEditGroup, 0, ui.loyaltyRank5,
		ui.loyaltyRank5Edit);
	bindNumberSelector("loyalty_magic_power", ui.statMagicPower,
		ui.statMagicPowerEdit, radioMap("magic_power", selectors),
		radioMap("magic_power", edit_selectors), loyaltyGroup, loyaltyEditGroup,
		0, ui.loyaltyRank5, ui.loyaltyRank5Edit);
	bindNumberSelector("vitality", ui.statVitality, ui.statVitalityEdit,
		radioMap("vitality", selectors), radioMap("vitality", edit_selectors),
		loyaltyGroup, loyaltyEditGroup, 0, ui.loyaltyRank5,
		ui.loyaltyRank5Edit);
	bindNumberSelector("loyalty_intelligence", ui.statIntelligence,
		ui.statIntelligenceEdit, radioMap("intelligence", selectors),
		radioMap("intelligence", edit_selectors), loyaltyGroup,
		loyaltyEditGroup, 0, ui.loyaltyRank5, ui.loyaltyRank5Edit);
	bindNumberSelector("loyalty_speed", ui.statSpeed, ui.statSpeedEdit,
		radioMap("speed", selectors), radioMap("speed", edit_selectors),
		loyaltyGroup, loyaltyEditGroup, 0, ui.loyaltyRank5,
		ui.loyaltyRank5Edit);
	bindNumberSelector("loyalty_luck", ui.statLuck, ui.statLuckEdit,
		radioMap("luck", selectors), radioMap("luck", edit_selectors),
		loyaltyGroup, loyaltyEditGroup, 0, ui.loyaltyRank5,
		ui.loyaltyRank5Edit);
	bindNumberSelector("loyalty_attack", ui.statAttack, ui.statAttackEdit,
		radioMap("attack", selectors), radioMap("attack", edit_selectors),
		loyaltyGroup, loyaltyEditGroup, 0, ui.loyaltyRank5,
		ui.loyaltyRank5Edit);
	bindNumberSelector("loyalty_gun", ui.statGun, ui.statGunEdit,
		radioMap("gun", selectors), radioMap("gun", edit_selectors),
		loyaltyGroup, loyaltyEditGroup, 0, ui.loyaltyRank5,
		ui.loyaltyRank5Edit);
	bindNumberSelector("loyalty_magic", ui.statMagic, ui.statMagicEdit,
		radioMap("magic", selectors), radioMap("magic", edit_selectors),
		loyaltyGroup, loyaltyEditGroup, 0, ui.loyaltyRank5,
		ui.loyaltyRank5Edit);
	bindNumberSelector("loyalty_support", ui.statSupport, ui.statSupportEdit,
		radioMap("support", selectors), radioMap("support", edit_selectors),
		loyaltyGroup, loyaltyEditGroup, 0, ui.loyaltyRank5,
		ui.loyaltyRank5Edit);
	bindNumberSelector("loyalty_defense", ui.statDefense, ui.statDefenseEdit,
		radioMap("defense", selectors), radioMap("defense", edit_selectors),
		loyaltyGroup, loyaltyEditGroup, 0, ui.loyaltyRank5,
		ui.loyaltyRank5Edit);
	bindNumberSelector("loyalty_magic_defense", ui.statMagicDefense,
		ui.statMagicDefenseEdit, radioMap("magic_defense", selectors),
		radioMap("magic_defense", edit_selectors), loyaltyGroup,
		loyaltyEditGroup, 0, ui.loyaltyRank5, ui.loyaltyRank5Edit);

	bindBool("weak_all_physical", ui.weakpointAllPhysical,
		ui.weakpointAllPhysicalEdit);
	bindBool("weak_all_magic", ui.weakpointAllMagic, ui.weakpointAllMagicEdit);
	bindBool("weak_slash", ui.weakpointSlash, ui.weakpointSlashEdit);
	bindBool("weak_assault", ui.weakpointAssault, ui.weakpointAssaultEdit);
	bindBool("weak_blow", ui.weakpointBlow, ui.weakpointBlowEdit);
	bindBool("weak_gun", ui.weakpointGun, ui.weakpointGunEdit);
	bindBool("weak_shot", ui.weakpointShot, ui.weakpointShotEdit);
	bindBool("weak_pierce", ui.weakpointPierce, ui.weakpointPierceEdit);
	bindBool("weak_blaze", ui.weakpointBlaze, ui.weakpointBlazeEdit);
	bindBool("weak_freeze", ui.weakpointFreeze, ui.weakpointFreezeEdit);
	bindBool("weak_shock", ui.weakpointShock, ui.weakpointShockEdit);
	bindBool("weak_electric_shock", ui.weakpointElectricShock,
		ui.weakpointElectricShockEdit);
	bindBool("weak_hama", ui.weakpointHama, ui.weakpointHamaEdit);
	bindBool("weak_mudo", ui.weakpointMudo, ui.weakpointMudoEdit);
	bindBool("weak_magic_force", ui.weakpointMagicForce,
		ui.weakpointMagicForceEdit);
	bindBool("weak_nerve", ui.weakpointNerve, ui.weakpointNerveEdit);
	bindBool("weak_mind", ui.weakpointMind, ui.weakpointMindEdit);
	bindBool("weak_suicide", ui.weakpointSuicide, ui.weakpointSuicideEdit);
	bindBool("weak_almighty", ui.weakpointAlmighty, ui.weakpointAlmightyEdit);
	bindBool("weak_special", ui.weakpointSpecial, ui.weakpointSpecialEdit);
	bindBool("protect_all_physical", ui.protectionAllPhysical,
		ui.protectionAllPhysicalEdit);
	bindBool("protect_all_magic", ui.protectionAllMagic,
		ui.protectionAllMagicEdit);
	bindBool("protect_slash", ui.protectionSlash, ui.protectionSlashEdit);
	bindBool("protect_assault", ui.protectionAssault, ui.protectionAssaultEdit);
	bindBool("protect_blow", ui.protectionBlow, ui.protectionBlowEdit);
	bindBool("protect_gun", ui.protectionGun, ui.protectionGunEdit);
	bindBool("protect_shot", ui.protectionShot, ui.protectionShotEdit);
	bindBool("protect_pierce", ui.protectionPierce, ui.protectionPierceEdit);
	bindBool("protect_blaze", ui.protectionBlaze, ui.protectionBlazeEdit);
	bindBool("protect_freeze", ui.protectionFreeze, ui.protectionFreezeEdit);
	bindBool("protect_shock", ui.protectionShock, ui.protectionShockEdit);
	bindBool("protect_electric_shock", ui.protectionElectricShock,
		ui.protectionElectricShockEdit);
	bindBool("protect_hama", ui.protectionHama, ui.protectionHamaEdit);
	bindBool("protect_mudo", ui.protectionMudo, ui.protectionMudoEdit);
	bindBool("protect_magic_force", ui.protectionMagicForce,
		ui.protectionMagicForceEdit);
	bindBool("protect_nerve", ui.protectionNerve, ui.protectionNerveEdit);
	bindBool("protect_mind", ui.protectionMind, ui.protectionMindEdit);
	bindBool("protect_suicide", ui.protectionSuicide, ui.protectionSuicideEdit);
	bindBool("protect_almighty", ui.protectionAlmighty,
		ui.protectionAlmightyEdit);
	bindBool("protect_special", ui.protectionSpecial, ui.protectionSpecialEdit);
	bindBool("null_all_physical", ui.nullAllPhysical, ui.nullAllPhysicalEdit);
	bindBool("null_all_magic", ui.nullAllMagic, ui.nullAllMagicEdit);
	bindBool("null_slash", ui.nullSlash, ui.nullSlashEdit);
	bindBool("null_assault", ui.nullAssault, ui.nullAssaultEdit);
	bindBool("null_blow", ui.nullBlow, ui.nullBlowEdit);
	bindBool("null_gun", ui.nullGun, ui.nullGunEdit);
	bindBool("null_shot", ui.nullShot, ui.nullShotEdit);
	bindBool("null_pierce", ui.nullPierce, ui.nullPierceEdit);
	bindBool("null_blaze", ui.nullBlaze, ui.nullBlazeEdit);
	bindBool("null_freeze", ui.nullFreeze, ui.nullFreezeEdit);
	bindBool("null_shock", ui.nullShock, ui.nullShockEdit);
	bindBool("null_electric_shock", ui.nullElectricShock,
		ui.nullElectricShockEdit);
	bindBool("null_hama", ui.nullHama, ui.nullHamaEdit);
	bindBool("null_mudo", ui.nullMudo, ui.nullMudoEdit);
	bindBool("null_magic_force", ui.nullMagicForce, ui.nullMagicForceEdit);
	bindBool("null_nerve", ui.nullNerve, ui.nullNerveEdit);
	bindBool("null_mind", ui.nullMind, ui.nullMindEdit);
	bindBool("null_suicide", ui.nullSuicide, ui.nullSuicideEdit);
	bindBool("null_almighty", ui.nullAlmighty, ui.nullAlmightyEdit);
	bindBool("null_special", ui.nullSpecial, ui.nullSpecialEdit);
	bindBool("reflect_all_physical", ui.reflectAllPhysical,
		ui.reflectAllPhysicalEdit);
	bindBool("reflect_all_magic", ui.reflectAllMagic, ui.reflectAllMagicEdit);
	bindBool("reflect_slash", ui.reflectSlash, ui.reflectSlashEdit);
	bindBool("reflect_assault", ui.reflectAssault, ui.reflectAssaultEdit);
	bindBool("reflect_blow", ui.reflectBlow, ui.reflectBlowEdit);
	bindBool("reflect_gun", ui.reflectGun, ui.reflectGunEdit);
	bindBool("reflect_shot", ui.reflectShot, ui.reflectShotEdit);
	bindBool("reflect_pierce", ui.reflectPierce, ui.reflectPierceEdit);
	bindBool("reflect_blaze", ui.reflectBlaze, ui.reflectBlazeEdit);
	bindBool("reflect_freeze", ui.reflectFreeze, ui.reflectFreezeEdit);
	bindBool("reflect_shock", ui.reflectShock, ui.reflectShockEdit);
	bindBool("reflect_electric_shock", ui.reflectElectricShock,
		ui.reflectElectricShockEdit);
	bindBool("reflect_hama", ui.reflectHama, ui.reflectHamaEdit);
	bindBool("reflect_mudo", ui.reflectMudo, ui.reflectMudoEdit);
	bindBool("reflect_magic_force", ui.reflectMagicForce,
		ui.reflectMagicForceEdit);
	bindBool("reflect_nerve", ui.reflectNerve, ui.reflectNerveEdit);
	bindBool("reflect_mind", ui.reflectMind, ui.reflectMindEdit);
	bindBool("reflect_suicide", ui.reflectSuicide, ui.reflectSuicideEdit);
	bindBool("reflect_almighty", ui.reflectAlmighty, ui.reflectAlmightyEdit);
	bindBool("reflect_special", ui.reflectSpecial, ui.reflectSpecialEdit);
	bindBool("absorb_all_physical", ui.absorbAllPhysical,
		ui.absorbAllPhysicalEdit);
	bindBool("absorb_all_magic", ui.absorbAllMagic, ui.absorbAllMagicEdit);
	bindBool("absorb_slash", ui.absorbSlash, ui.absorbSlashEdit);
	bindBool("absorb_assault", ui.absorbAssault, ui.absorbAssaultEdit);
	bindBool("absorb_blow", ui.absorbBlow, ui.absorbBlowEdit);
	bindBool("absorb_gun", ui.absorbGun, ui.absorbGunEdit);
	bindBool("absorb_shot", ui.absorbShot, ui.absorbShotEdit);
	bindBool("absorb_pierce", ui.absorbPierce, ui.absorbPierceEdit);
	bindBool("absorb_blaze", ui.absorbBlaze, ui.absorbBlazeEdit);
	bindBool("absorb_freeze", ui.absorbFreeze, ui.absorbFreezeEdit);
	bindBool("absorb_shock", ui.absorbShock, ui.absorbShockEdit);
	bindBool("absorb_electric_shock", ui.absorbElectricShock,
		ui.absorbElectricShockEdit);
	bindBool("absorb_hama", ui.absorbHama, ui.absorbHamaEdit);
	bindBool("absorb_mudo", ui.absorbMudo, ui.absorbMudoEdit);
	bindBool("absorb_magic_force", ui.absorbMagicForce,
		ui.absorbMagicForceEdit);
	bindBool("absorb_nerve", ui.absorbNerve, ui.absorbNerveEdit);
	bindBool("absorb_mind", ui.absorbMind, ui.absorbMindEdit);
	bindBool("absorb_suicide", ui.absorbSuicide, ui.absorbSuicideEdit);
	bindBool("absorb_almighty", ui.absorbAlmighty, ui.absorbAlmightyEdit);
	bindBool("absorb_special", ui.absorbSpecial, ui.absorbSpecialEdit);
	bindDetailedMultiRelation("db_devil_skills", ui.skills, ui.skillsEditList,
		"application/x-megatendb-skills", "devil", "skill", "db_skills", "icon",
		"icons/skills/icon_%1.png", QStringList() << "name_{$lang}"
		<< "__extra__" << "expert_{$lang}" << "category_{$lang}",
		ui.skillsEditAdd, ui.skillsEditEdit, ui.skillsEditRemove,
		ui.skillsEditSearchNew,
		ui.skillsEditSearchCancel, ui.skillsEditAddSearch, ui.skillsEditAddList,
		QVariant(), QVariant(), true, QVariantList() << relation1 << relation2,
		"lvl", tr("At what level does the devil learn this skill? "
		"(-1 for starting level)"), ui.skillsEditStack);
	bindMultiRelation("db_devil_traits", ui.traits, ui.traitsEditList,
		"devil", "trait", "db_traits", "name_{$lang}", "desc_{$lang}",
		ui.traitsEditAdd, ui.traitsEditRemove, ui.traitsEditStack,
		ui.traitsEditAddSearch, ui.traitsEditAddList, ui.traitsEditSearchAdd,
		ui.traitsEditSearchCancel);
	bindMultiRelation("db_devil_locations", ui.locations, ui.locationsEditList,
		"devil", "location", "db_locations", "name_{$lang}", QString(),
		ui.locationsEditAdd, ui.locationsEditRemove, ui.locationsEditStack,
		ui.locationsEditAddSearch, ui.locationsEditAddList,
		ui.locationsEditSearchAdd, ui.locationsEditSearchCancel);

	// Make sure the level sorts right
	ui.skills->setSortFunction2(numberSortFunction2);
	ui.skills->toggleSort2();
	ui.skills->toggleSort2();
	ui.skillsEditList->setSortFunction2(numberSortFunction2);
	ui.skillsEditList->toggleSort2();
	ui.skillsEditList->toggleSort2();

	// Give the relation buttons an icon
	QIcon edit_icon(":/edit.png");
	ui.genusEditButton->setIcon(edit_icon);

	// Init the view
	initView(ui.stackedWidget, ui.editButton, ui.cancelButton,
		ui.refreshButton, ui.updateButton);
}

bool DevilView::checkValues()
{
	// Check name_ja and name_en
	if( ui.japaneseNameEdit->text().isEmpty() || ui.englishNameEdit->text().isEmpty() )
	{
		QMessageBox::warning(this, tr("Update Error"), tr("You must both a valid "
			"Japanese and English devil name."));

		return false;
	}

	// Check icon
	if( ui.iconEdit->value().isEmpty() )
	{
		QMessageBox::warning(this, tr("Update Error"), tr("You must set a valid "
			"icon."));

		return false;
	}

	return true;
}

QString DevilView::table() const
{
	return QString("db_devils");
}

QString DevilView::addWarningTitle() const
{
	return tr("Add Devil");
}

QString DevilView::addWarningMessage() const
{
	return tr("You are attemping to add another devil while editing a devil. "
			"If you continue you will lose any changes you made to the devil. "
			"Are you sure you want to continue?");
}

RadioButtonMap DevilView::radioMap(const QString& field,
	const QList<QRadioButton*>& selectors) const
{
	RadioButtonMap map;

	int i = 0;
	foreach(QRadioButton *selector, selectors)
		map[selector] = QString("loyalty%1_%2").arg(++i).arg(field);

	return map;
}
