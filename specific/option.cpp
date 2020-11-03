/*
 * Copyright (c) 2017-2020 Michael Chaban. All rights reserved.
 * Original game is written by Core Design Ltd. in 1997.
 * Lara Croft and Tomb Raider are trademarks of Square Enix Ltd.
 *
 * This file is part of TR2Main.
 *
 * TR2Main is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TR2Main is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TR2Main.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "global/precompiled.h"
#include "specific/option.h"
#include "game/invtext.h"
#include "game/sound.h"
#include "game/text.h"
#include "specific/game.h"
#include "specific/init_input.h"
#include "specific/output.h"
#include "specific/sndpc.h"
#include "global/vars.h"

#ifdef FEATURE_HUD_IMPROVED
extern DWORD InvTextBoxMode;

bool JoystickHintsEnabled = true;
#endif // FEATURE_HUD_IMPROVED

static DWORD LayoutPage = CTRL_Default;

/*
 * Passport option box parameters
 */
#define PASSPORT_LINE_COUNT	(10)
// Y coordinates relative to the bottom of the screen
#ifdef FEATURE_HUD_IMPROVED
#define PASSPORT_Y_BOX		(-44)
#else // FEATURE_HUD_IMPROVED
#define PASSPORT_Y_BOX		(-32)
#endif
#define PASSPORT_Y_TITLE	(-16)

/*
 * Detail option box parameters
 */
#define DETAIL_WIDTH_L		(160)
#define DETAIL_WIDTH_M		(DETAIL_WIDTH_L - 4)
#define DETAIL_WIDTH_S		(DETAIL_WIDTH_L - 12)

#define DETAIL_LN_HEIGHT	(25)
#define DETAIL_HEIGHT		(DETAIL_LN_HEIGHT * 3 + 32)
// Y coordinates relative to the center of the screen
#define DETAIL_Y_BOX		(-32)
#define DETAIL_Y_TITLE		(DETAIL_Y_BOX + 2)
#define DETAIL_Y_LINE1		(DETAIL_LN_HEIGHT * 0)
#define DETAIL_Y_LINE2		(DETAIL_LN_HEIGHT * 1)
#define DETAIL_Y_LINE3		(DETAIL_LN_HEIGHT * 2)

#define DETAIL_NEARZ		(8)
#define DETAIL_FARZ			(16)

/*
 * Sound option box parameters
 */
#define SOUND_WIDTH_L		(140)
#define SOUND_WIDTH_M		(SOUND_WIDTH_L - 4)
#define SOUND_WIDTH_S		(SOUND_WIDTH_L - 12)

#define SOUND_LN_HEIGHT		(25)
#define SOUND_HEIGHT		(SOUND_LN_HEIGHT * 2 + 32)
// Y coordinates relative to the center of the screen
#define SOUND_Y_BOX			(-32)
#define SOUND_Y_TITLE		(SOUND_Y_BOX + 2)
#define SOUND_Y_LINE1		(SOUND_LN_HEIGHT * 0)
#define SOUND_Y_LINE2		(SOUND_LN_HEIGHT * 1)

#define SOUND_NEARZ			(8)
#define SOUND_FARZ			(48)

/*
 * Control option box parameters
 */
#define CONTROL_LINE_COUNT	(ARRAY_SIZE(Layout->key)/2)
#define CONTROL_LN_HEIGHT	(15)

#define CONTROL_WIDTH_LOW	(300)
#define CONTROL_HEIGHT_LOW	(CONTROL_LN_HEIGHT * CONTROL_LINE_COUNT + 35)

#ifdef FEATURE_HUD_IMPROVED
#define CONTROL_WIDTH_HIGH	(360)
#define CONTROL_HEIGHT_HIGH	CONTROL_HEIGHT_LOW

#define CONTROL_COLUMN_B	(4)
#define CONTROL_COLUMN_A	(60)

// Y coordinates relative to the center of the screen
#define CONTROL_Y_BOX		(-70)
#define CONTROL_Y_TITLE		(CONTROL_Y_BOX + 4)
#define CONTROL_Y_LINE1		(CONTROL_LN_HEIGHT * 0 + CONTROL_Y_BOX + 24)
#define CONTROL_Y_LINE2		(CONTROL_LN_HEIGHT * 1 + CONTROL_Y_BOX + 24)
#define CONTROL_Y_LINE3		(CONTROL_LN_HEIGHT * 2 + CONTROL_Y_BOX + 24)
#define CONTROL_Y_LINE4		(CONTROL_LN_HEIGHT * 3 + CONTROL_Y_BOX + 24)
#define CONTROL_Y_LINE5		(CONTROL_LN_HEIGHT * 4 + CONTROL_Y_BOX + 24)
#define CONTROL_Y_LINE6		(CONTROL_LN_HEIGHT * 5 + CONTROL_Y_BOX + 24)
#define CONTROL_Y_LINE7		(CONTROL_LN_HEIGHT * 6 + CONTROL_Y_BOX + 24)
#else // FEATURE_HUD_IMPROVED
#define CONTROL_WIDTH_HIGH	(420)
#define CONTROL_HEIGHT_HIGH	(CONTROL_LN_HEIGHT * CONTROL_LINE_COUNT + 45)

#define CONTROL_COLUMN_B	(10)
#define CONTROL_COLUMN_A	(80)

// Y coordinates relative to the center of the screen
#define CONTROL_Y_BOX		(-55)
#define CONTROL_Y_TITLE		(CONTROL_Y_BOX + 5)
#define CONTROL_Y_LINE1		(CONTROL_LN_HEIGHT * 0 + CONTROL_Y_BOX + 30)
#define CONTROL_Y_LINE2		(CONTROL_LN_HEIGHT * 1 + CONTROL_Y_BOX + 30)
#define CONTROL_Y_LINE3		(CONTROL_LN_HEIGHT * 2 + CONTROL_Y_BOX + 30)
#define CONTROL_Y_LINE4		(CONTROL_LN_HEIGHT * 3 + CONTROL_Y_BOX + 30)
#define CONTROL_Y_LINE5		(CONTROL_LN_HEIGHT * 4 + CONTROL_Y_BOX + 30)
#define CONTROL_Y_LINE6		(CONTROL_LN_HEIGHT * 5 + CONTROL_Y_BOX + 30)
#define CONTROL_Y_LINE7		(CONTROL_LN_HEIGHT * 6 + CONTROL_Y_BOX + 30)
#endif // FEATURE_HUD_IMPROVED

#define CONTROL_NEARZ		(16)
#define CONTROL_FARZ		(48)

extern GOURAUD_FILL ReqBgndGour1;
extern GOURAUD_OUTLINE ReqBgndGour2;

extern GOURAUD_FILL ReqMainGour1;
extern GOURAUD_OUTLINE ReqMainGour2;

extern GOURAUD_FILL ReqSelGour1;
extern GOURAUD_OUTLINE ReqSelGour2;

/*
 * Control key names
 */
#ifdef FEATURE_HUD_IMPROVED
static LPCSTR ControlKeysText[0x110] = {
	NULL,           K("esc"),       K("1"),         K("2"),         K("3"),         K("4"),         K("5"),         K("6"),
	K("7"),         K("8"),         K("9"),         K("0"),         K("-"),         K("="),         K("backspace"), K("tab"),
	K("q"),         K("w"),         K("e"),         K("r"),         K("t"),         K("y"),         K("u"),         K("i"),
	K("o"),         K("p"),         K("["),         K("]"),         K("return"),    K("ctrl"),      K("a"),         K("s"),
	K("d"),         K("f"),         K("g"),         K("h"),         K("j"),         K("k"),         K("l"),         K(";"),
	K("'"),         K("`"),         K("shift"),     K("\\"),        K("z"),         K("x"),         K("c"),         K("v"),
	K("b"),         K("n"),         K("m"),         K(","),         K("."),         K("/"),         K("shift"),     K("pad*"),
	K("alt"),       K("space"),     K("capslock"),  NULL,           NULL,           NULL,           NULL,           NULL,
	NULL,           NULL,           NULL,           NULL,           NULL,           K("numlock"),   NULL,           K("pad7"),
	K("pad8"),      K("pad9"),      K("pad-"),      K("pad4"),      K("pad5"),      K("pad6"),      K("pad+"),      K("pad1"),
	K("pad2"),      K("pad3"),      K("pad0"),      K("pad."),      NULL,           NULL,           K("<"),         NULL,
	NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,
	NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,
	NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,
	NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,
	NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,
	NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,
	NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,
	NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,
	NULL,           NULL,           NULL,           NULL,           K("enter"),     K("ctrl"),      NULL,           NULL,
	NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,
	NULL,           NULL,           K("shift"),     NULL,           NULL,           NULL,           NULL,           NULL,
	NULL,           NULL,           NULL,           NULL,           NULL,           K("pad/"),      NULL,           NULL,
	K("alt"),       NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,
	NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           K("home"),
	K("up"),        K("pageup"),    NULL,           K("left"),      NULL,           K("right"),     NULL,           K("end"),
	K("down"),      K("pagedown"),  K("insert"),    K("delete"),    NULL,           NULL,           NULL,           NULL,
	NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,
	NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,
	NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,
	NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,
	NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL,
	K("joy1"),      K("joy2"),      K("joy3"),      K("joy4"),      K("joy5"),      K("joy6"),      K("joy7"),      K("joy8"),
	K("joy9"),      K("joy10"),     K("joy11"),     K("joy12"),     K("joy13"),     K("joy14"),     K("joy15"),     K("joy16"),
};

static TEXT_STR_INFO *DeselectHintText, *SelectHintText;

typedef enum {
	HINT_HIDDEN,
	HINT_JOYSTICK,
	HINT_KEYBOARD,
} HINT_MODE;

static void UpdateJoystickHintText(HINT_MODE deselect, HINT_MODE select) {
	const char *keyName = NULL;
	char text[64] = {0};

	if( DeselectHintText != NULL ) {
		if( deselect != HINT_HIDDEN ) {
			if( deselect == HINT_JOYSTICK && Layout[CTRL_Custom].key[KM_WeaponDraw] >= 0x100 ) {
				keyName = ControlKeysText[Layout[CTRL_Custom].key[KM_WeaponDraw]];
			} else {
				keyName = ControlKeysText[DIK_ESCAPE];
			}
			snprintf(text, sizeof(text), "%s Go Back", keyName);
			T_ChangeText(DeselectHintText, text);
		}
		T_HideText(DeselectHintText, deselect == HINT_HIDDEN);
	}

	if( SelectHintText != NULL ) {
		if( select != HINT_HIDDEN ) {
			if( select == HINT_JOYSTICK && Layout[CTRL_Custom].key[KM_Action] >= 0x100 ) {
				keyName = ControlKeysText[Layout[CTRL_Custom].key[KM_Action]];
			} else {
				keyName = ControlKeysText[DIK_RETURN];
			}
			snprintf(text, sizeof(text), "%s Select", keyName);
			T_ChangeText(SelectHintText, text);
		}
		T_HideText(SelectHintText, select == HINT_HIDDEN);
	}
}

void RemoveJoystickHintText(bool deselect, bool select) {
	if( deselect ) {
		T_RemovePrint(DeselectHintText);
		DeselectHintText = NULL;
	}
	if( select ) {
		T_RemovePrint(SelectHintText);
		SelectHintText = NULL;
	}
}

void DisplayJoystickHintText(bool deselect, bool select) {
	if( !JoystickHintsEnabled ) return;
#ifdef FEATURE_INPUT_IMPROVED
	if( GetJoystickType() == JT_NONE ) return;
#else // FEATURE_INPUT_IMPROVED
	if( !SavedAppSettings.JoystickEnabled || !SavedAppSettings.PreferredJoystick ) return;
#endif // FEATURE_INPUT_IMPROVED

	static int renderWidth = 0;
	if( renderWidth != GetRenderWidthDownscaled() ) {
		if( DeselectHintText != NULL ) deselect = true;
		if( SelectHintText != NULL ) select = true;
		RemoveJoystickHintText(true, true);
		renderWidth = GetRenderWidthDownscaled();
	}

	const char *keyName = NULL;
	char text[64] = {0};
	int x = (renderWidth > 325) ? (renderWidth - 320) / 2 : 2;

	if( deselect && DeselectHintText == NULL ) {
		if( Layout[CTRL_Custom].key[KM_WeaponDraw] >= 0x100 ) {
			keyName = ControlKeysText[Layout[CTRL_Custom].key[KM_WeaponDraw]];
		} else {
			keyName = ControlKeysText[DIK_ESCAPE];
		}
		snprintf(text, sizeof(text), "%s Go Back", keyName);
		DeselectHintText = T_Print(-x, -40, 0, text);
		T_BottomAlign(DeselectHintText, 1);
		T_RightAlign(DeselectHintText, 1);
	}

	if( select && SelectHintText == NULL ) {
		if( Layout[CTRL_Custom].key[KM_Action] >= 0x100 ) {
			keyName = ControlKeysText[Layout[CTRL_Custom].key[KM_Action]];
		} else {
			keyName = ControlKeysText[DIK_RETURN];
		}
		snprintf(text, sizeof(text), "%s Select", keyName);
		SelectHintText = T_Print(x, -40, 0, text);
		T_BottomAlign(SelectHintText, 1);
	}
}
#else // FEATURE_HUD_IMPROVED
static LPCSTR ControlKeysText[0x110] = {
	NULL,   "ESC",   "1",     "2",     "3",     "4",     "5",     "6",
	"7",    "8",     "9",     "0",     "-",     "+",     "BKSP",  "TAB",
	"Q",    "W",     "E",     "R",     "T",     "Y",     "U",     "I",
	"O",    "P",     "<",     ">",     "RET",   "CTRL",  "A",     "S",
	"D",    "F",     "G",     "H",     "J",     "K",     "L",     ";",
	"'",    "`",     "SHIFT", "#",     "Z",     "X",     "C",     "V",
	"B",    "N",     "M",     ",",     ".",     "/",     "SHIFT", "PADx",
	"ALT",  "SPACE", "CAPS",  NULL,    NULL,    NULL,    NULL,    NULL,
	NULL,   NULL,    NULL,    NULL,    NULL,    "NMLK",  NULL,    "PAD7",
	"PAD8", "PAD9",  "PAD-",  "PAD4",  "PAD5",  "PAD6",  "PAD+",  "PAD1",
	"PAD2", "PAD3",  "PAD0",  "PAD.",  NULL,    NULL,    "\\",    NULL,
	NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,
	NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,
	NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,
	NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,
	NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,
	NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,
	NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,
	NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,
	NULL,   NULL,    NULL,    NULL,    "ENTER", "CTRL",  NULL,    NULL,
	NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,
	NULL,   NULL,    "SHIFT", NULL,    NULL,    NULL,    NULL,    NULL,
	NULL,   NULL,    NULL,    NULL,    NULL,    "PAD/",  NULL,    NULL,
	"ALT",  NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,
	NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    "HOME",
	"UP",   "PGUP",  NULL,    "LEFT",  NULL,    "RIGHT", NULL,    "END",
	"DOWN", "PGDN",  "INS",   "DEL",   NULL,    NULL,    NULL,    NULL,
	NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,
	NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,
	NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,
	NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,
	NULL,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,
	"JOY1", "JOY2",  "JOY3",  "JOY4",  "JOY5",  "JOY6",  "JOY7",  "JOY8",
	"JOY9", "JOY10", "JOY11", "JOY12", "JOY13", "JOY14", "JOY15", "JOY16",
};
#endif // FEATURE_HUD_IMPROVED

// NOTE: not presented in the original game. SetPCRequesterSize() used directly instead
void SetPassportRequesterSize(REQUEST_INFO *req) {
#ifdef FEATURE_HUD_IMPROVED
	extern DWORD SavegameSlots;
	double scale = (double)GetRenderHeight() / (double)GetRenderScale(480);
	DWORD adjust = ( scale > 1.0 ) ? 5 : 0;
	DWORD lines = (PASSPORT_LINE_COUNT + adjust) * scale - adjust;
	CLAMP(lines, 5, SavegameSlots);
	SetPCRequesterSize(req, lines, PASSPORT_Y_BOX);
#else // !FEATURE_HUD_IMPROVED
	SetPCRequesterSize(req, PASSPORT_LINE_COUNT, PASSPORT_Y_BOX);
#endif // FEATURE_HUD_IMPROVED
}

void __cdecl do_inventory_options(INVENTORY_ITEM *item) {
	switch( item->objectID ) {
	// passport
		case ID_PASSPORT_OPTION:
			do_passport_option(item);
			break;
	// gamma
		case ID_GAMMA_OPTION:
			do_gamma_option(item);
			break;
	// detail
		case ID_DETAIL_OPTION:
			do_detail_option(item);
			break;
	// sound
		case ID_SOUND_OPTION:
			do_sound_option(item);
			break;
	// control
		case ID_CONTROL_OPTION:
			do_control_option(item);
			break;
	// statistics
		case ID_COMPASS_OPTION:
			do_compass_option(item);
			break;
	// weapons
		case ID_PISTOL_OPTION:
		case ID_SHOTGUN_OPTION:
		case ID_MAGNUM_OPTION:
		case ID_UZI_OPTION:
		case ID_HARPOON_OPTION:
		case ID_M16_OPTION:
		case ID_GRENADE_OPTION:
	// medipacks
		case ID_SMALL_MEDIPACK_OPTION:
		case ID_LARGE_MEDIPACK_OPTION:
	// puzzles
		case ID_PUZZLE_OPTION1:
		case ID_PUZZLE_OPTION2:
		case ID_PUZZLE_OPTION3:
		case ID_PUZZLE_OPTION4:
	// keys
		case ID_KEY_OPTION1:
		case ID_KEY_OPTION2:
		case ID_KEY_OPTION3:
		case ID_KEY_OPTION4:
	// pickups
		case ID_PICKUP_OPTION1:
		case ID_PICKUP_OPTION2:
			InputDB |= IN_SELECT;
			break;
	// ammo
		case ID_PISTOL_AMMO_OPTION:
		case ID_SHOTGUN_AMMO_OPTION:
		case ID_MAGNUM_AMMO_OPTION:
		case ID_UZI_AMMO_OPTION:
		case ID_HARPOON_AMMO_OPTION:
		case ID_M16_AMMO_OPTION:
		case ID_GRENADE_AMMO_OPTION:
			break;
	// other
		default:
			if( CHK_ANY(InputDB, IN_SELECT|IN_DESELECT) ) {
				item->goalFrame = 0;
				item->animDirection = -1;
			}
			break;
	}
}

#ifdef FEATURE_HUD_IMPROVED
static void SetPassportTextInfo(GAME_STRING_ID id, bool left, bool right) {
	if( SavedAppSettings.RenderMode == RM_Hardware && InvTextBoxMode ) {
		char text[64] = {0};
		snprintf(text, sizeof(text), "%c    %s   %c ", left?'\x11':' ', GF_GameStringTable[id], right?'\x12':' ');
		PassportTextInfo = T_Print(0, PASSPORT_Y_TITLE, 0, text);
	} else {
		PassportTextInfo = T_Print(0, PASSPORT_Y_TITLE, 0, GF_GameStringTable[id]);
	}
}
#endif // FEATURE_HUD_IMPROVED

void __cdecl do_passport_option(INVENTORY_ITEM *item) {
	static int passportMode = 0;
	int frame, page, select;
	REQUEST_INFO *requester;

	T_RemovePrint(InvItemText[0]);
	InvItemText[0] = NULL;

	frame = item->goalFrame - item->openFrame;
	page = ( (frame % 5) == 0 ) ? (frame / 5) : -1;

#ifdef FEATURE_HUD_IMPROVED
	bool left = page > 0;
	bool right = page < 2;

	if( InventoryMode == INV_DeathMode ) {
		InputDB &= ~IN_DESELECT;
	}
#endif // FEATURE_HUD_IMPROVED

	if( InventoryMode == INV_LoadMode ||
		InventoryMode == INV_SaveMode ||
		CHK_ANY(GF_GameFlow.flags, GFF_LoadSaveDisabled) )
	{
		InputDB &= ~(IN_LEFT|IN_RIGHT);
#ifdef FEATURE_HUD_IMPROVED
		left = false;
		right = false;
#endif // FEATURE_HUD_IMPROVED
	}

	switch( page ) {
		case 0 : // load game
			if( CHK_ANY(GF_GameFlow.flags, GFF_LoadSaveDisabled) ) {
				InputDB = IN_RIGHT;
			}
			else if( passportMode == 1 ) {
				SetPassportRequesterSize(&LoadGameRequester);
				select = Display_Requester(&LoadGameRequester, TRUE, TRUE);
				if( select == 0 ) {
					if( CHK_ANY(InputDB, IN_RIGHT) ) {
						Remove_Requester(&LoadGameRequester);
						passportMode = 0;
					} else {
						InputStatus = 0;
						InputDB = 0;
					}
				} else {
					if( select > 0 ) {
						InventoryExtraData[1] = select - 1;
					}
					passportMode = 0;
				}
			}
			else if( passportMode == 0 ) {
				if( SavedGamesCount == 0 || InventoryMode == INV_SaveMode ) {
					InputDB = IN_RIGHT;
				} else {
					if( PassportTextInfo == NULL ) {
#ifdef FEATURE_HUD_IMPROVED
						SetPassportTextInfo(GSI_Passport_LoadGame, left, right);
#else // FEATURE_HUD_IMPROVED
						PassportTextInfo = T_Print(0, PASSPORT_Y_TITLE, 0, GF_GameStringTable[GSI_Passport_LoadGame]);
#endif // FEATURE_HUD_IMPROVED
						T_BottomAlign(PassportTextInfo, 1);
						T_CentreH(PassportTextInfo, 1);
					}
					T_RemovePrint(InvRingText);
					InvRingText = NULL;
					T_RemovePrint(InvItemText[0]);
					InvItemText[0] = NULL;
					GetSavedGamesList(&LoadGameRequester);
					SetRequesterHeading(&LoadGameRequester, GF_GameStringTable[GSI_Passport_LoadGame], 0, NULL, 0);
					passportMode = 1;
					InputStatus = 0;
					InputDB = 0;
				}
			}
			break;

		case 1 : // new game | save game
			if( CHK_ANY(GF_GameFlow.flags, GFF_LoadSaveDisabled) ) {
				InputDB = IN_RIGHT;
			}
			else if( passportMode == 1 || passportMode == 2 ) {
				requester = ( passportMode == 1 ) ? &LoadGameRequester : &SaveGameRequester;

				SetPassportRequesterSize(requester);
				select = Display_Requester(requester, TRUE, TRUE);
				if( select == 0 ) {
#ifdef FEATURE_HUD_IMPROVED
					if( SavedGamesCount == 0 ) InputDB &= ~IN_LEFT;
#endif // FEATURE_HUD_IMPROVED
					if( CHK_ANY(InputDB, IN_LEFT|IN_RIGHT) ) {
						Remove_Requester(requester);
						passportMode = 0;
					} else {
						InputStatus = 0;
						InputDB = 0;
					}
				} else {
					if( select > 0 ) {
						InventoryExtraData[1] = select - 1;
					}
					passportMode = 0;
				}
			}
			else if( passportMode == 0 ) {
				if( InventoryMode == INV_DeathMode ) {
					InputDB = ( item->animDirection == -1 ) ? IN_LEFT : IN_RIGHT;
				} else {
					if( PassportTextInfo == NULL ) {
						GAME_STRING_ID textID;
						if( InventoryMode != INV_TitleMode && CurrentLevel != 0 )
							textID = GSI_Passport_SaveGame;
						else
							textID = GSI_Passport_NewGame;
#ifdef FEATURE_HUD_IMPROVED
						if( SavedGamesCount == 0 ) left = false;
						SetPassportTextInfo(textID, left, right);
#else // FEATURE_HUD_IMPROVED
						PassportTextInfo = T_Print(0, PASSPORT_Y_TITLE, 0, GF_GameStringTable[textID]);
#endif // FEATURE_HUD_IMPROVED
						T_BottomAlign(PassportTextInfo, 1);
						T_CentreH(PassportTextInfo, 1);
					}

					if( InventoryMode != INV_TitleMode && CurrentLevel != 0 ) {
						T_RemovePrint(InvRingText);
						InvRingText = NULL;
						T_RemovePrint(InvItemText[0]);
						InvItemText[0] = NULL;
						GetSavedGamesList(&LoadGameRequester);
						SetRequesterHeading(&LoadGameRequester, GF_GameStringTable[GSI_Passport_SaveGame], 0, NULL, 0);
						passportMode = 1;
						InputStatus = 0;
						InputDB = 0;
					}
					else if( CHK_ANY(GF_GameFlow.flags, GFF_SelectAnyLevel) ) {
						T_RemovePrint(InvItemText[0]);
						InvItemText[0] = 0;
						Init_Requester(&SaveGameRequester);
						GetValidLevelsList(&SaveGameRequester);
						SetRequesterHeading(&SaveGameRequester, GF_GameStringTable[GSI_Passport_SelectLevel], 0, NULL, 0);
						passportMode = 2;
						InputStatus = 0;
						InputDB = 0;
					}
					else if( CHK_ANY(InputDB, IN_SELECT) ) {
						InventoryExtraData[1] = 1;
					}
				}
			}
			break;

		case 2 : // exit game
			if( PassportTextInfo == NULL ) {
				GAME_STRING_ID textID;
				if( InventoryMode == INV_TitleMode ) {
					textID = GSI_Passport_ExitGame;
				}
				else if( CHK_ANY(GF_GameFlow.flags, GFF_DemoVersion) ) {
					textID = GSI_Passport_ExitDemo;
				}
				else {
					textID = GSI_Passport_ExitToTitle;
				}
#ifdef FEATURE_HUD_IMPROVED
				if( InventoryMode == INV_DeathMode && SavedGamesCount == 0 ) left = false;
				SetPassportTextInfo(textID, left, right);
#else // FEATURE_HUD_IMPROVED
				PassportTextInfo = T_Print(0, PASSPORT_Y_TITLE, 0, GF_GameStringTable[textID]);
#endif // FEATURE_HUD_IMPROVED
				T_BottomAlign(PassportTextInfo, 1);
				T_CentreH(PassportTextInfo, 1);
			}
			break;

		default :
			break;
	}

	if( CHK_ANY(InputDB, IN_LEFT) && (InventoryMode != INV_DeathMode || SavedGamesCount != 0) ) {
		item->animDirection = -1;
		item->goalFrame -= 5;
		if( SavedGamesCount != 0 ) {
			if( item->goalFrame < item->openFrame ) {
				item->goalFrame = item->openFrame;
			} else {
				PlaySoundEffect(115, NULL, SFX_ALWAYS); // page flip SFX
				T_RemovePrint(PassportTextInfo);
				PassportTextInfo = NULL;
			}
		} else {
			if( item->goalFrame < item->openFrame + 5 ) {
				item->goalFrame = item->openFrame + 5;
			} else {
				T_RemovePrint(PassportTextInfo);
				PassportTextInfo = NULL;
			}
		}
		InputStatus = 0;
		InputDB = 0;
	}

	if( CHK_ANY(InputDB, IN_RIGHT) ) {
		item->animDirection = 1;
		item->goalFrame += 5;
		if( item->goalFrame > item->framesTotal - 6 ) {
			item->goalFrame = item->framesTotal - 6;
		} else {
			PlaySoundEffect(115, NULL, SFX_ALWAYS); // page flip SFX
			T_RemovePrint(PassportTextInfo);
			PassportTextInfo = NULL;
		}
		InputStatus = 0;
		InputDB = 0;
	}

	if( CHK_ANY(InputDB, IN_DESELECT) ) {
		if( InventoryMode == INV_DeathMode ) {
			InputStatus = 0;
			InputDB = 0;
		} else {
			if( page == 2 ) {
				item->animDirection = 1;
				item->goalFrame = item->framesTotal - 1;
			} else {
				item->animDirection = -1;
				item->goalFrame = 0;
			}
			T_RemovePrint(PassportTextInfo);
			PassportTextInfo = NULL;
		}
	}

	if( CHK_ANY(InputDB, IN_SELECT) ) {
		InventoryExtraData[0] = page;
		if( page == 2 ) {
			item->animDirection = 1;
			item->goalFrame = item->framesTotal - 1;
		} else {
			item->animDirection = -1;
			item->goalFrame = 0;
		}
		T_RemovePrint(PassportTextInfo);
		PassportTextInfo = NULL;
	}
}

void __cdecl do_gamma_option(INVENTORY_ITEM *item) {
// null function
}

void __cdecl do_detail_option(INVENTORY_ITEM *item) {
	int i;

	if( DetailTextInfo[0] == NULL ) {
		DetailTextInfo[4] = T_Print(0, DETAIL_Y_TITLE, 0, GF_GameStringTable[GSI_Detail_SelectDetail]);
		DetailTextInfo[3] = T_Print(0, DETAIL_Y_BOX, 0, " ");
		DetailTextInfo[2] = T_Print(0, DETAIL_Y_LINE1, 0, GF_GameStringTable[GSI_Detail_High]);
		DetailTextInfo[1] = T_Print(0, DETAIL_Y_LINE2, 0, GF_GameStringTable[GSI_Detail_Medium]);
		DetailTextInfo[0] = T_Print(0, DETAIL_Y_LINE3, 0, GF_GameStringTable[GSI_Detail_Low]);

		T_AddBackground(DetailTextInfo[4], DETAIL_WIDTH_M, 0, 0, 0, DETAIL_NEARZ, ICLR_Black, &ReqMainGour1, 0);
		T_AddOutline(DetailTextInfo[4], TRUE, ICLR_Orange, &ReqMainGour2, 0);

		T_AddBackground(DetailTextInfo[DetailLevel], DETAIL_WIDTH_S, 0, 0, 0, DETAIL_NEARZ, ICLR_Black, &ReqSelGour1, 0);
		T_AddOutline(DetailTextInfo[DetailLevel], TRUE, ICLR_Orange, &ReqSelGour2, 0);

		T_AddBackground(DetailTextInfo[3], DETAIL_WIDTH_L, DETAIL_HEIGHT, 0, 0, DETAIL_FARZ, ICLR_Black, &ReqBgndGour1, 0);
		T_AddOutline(DetailTextInfo[3], TRUE, ICLR_Blue, &ReqBgndGour2, 0);

		for( i=0; i<5 ; ++i ) {
			T_CentreH(DetailTextInfo[i], 1);
			T_CentreV(DetailTextInfo[i], 1);
		}
	}

	if( CHK_ANY(InputDB, IN_BACK) && DetailLevel > 0 ) {
		T_RemoveOutline(DetailTextInfo[DetailLevel]);
		T_RemoveBackground(DetailTextInfo[DetailLevel]);
		--DetailLevel;
		T_AddOutline(DetailTextInfo[DetailLevel], TRUE, ICLR_Orange, &ReqSelGour2, 0);
		T_AddBackground(DetailTextInfo[DetailLevel], DETAIL_WIDTH_S, 0, 0, 0, DETAIL_NEARZ, ICLR_Black, &ReqSelGour1, 0);
	}

	if ( CHK_ANY(InputDB, IN_FORWARD) && DetailLevel < 2 ) {
		T_RemoveOutline(DetailTextInfo[DetailLevel]);
		T_RemoveBackground(DetailTextInfo[DetailLevel]);
		++DetailLevel;
		T_AddOutline(DetailTextInfo[DetailLevel], TRUE, ICLR_Orange, &ReqSelGour2, 0);
		T_AddBackground(DetailTextInfo[DetailLevel], DETAIL_WIDTH_S, 0, 0, 0, DETAIL_NEARZ, ICLR_Black, &ReqSelGour1, 0);
	}

	switch( DetailLevel ) {
		case 2 :
			// NOTE: maybe supposed to be SW_DETAIL_HIGH? Anyway this menu is disabled by Core
			PerspectiveDistance = SW_DETAIL_ULTRA;
			break;
		case 1 :
			PerspectiveDistance = SW_DETAIL_MEDIUM;
			break;
		case 0 :
		default :
			PerspectiveDistance = SW_DETAIL_LOW;
			break;
	}

	if( CHK_ANY(InputDB, IN_SELECT|IN_DESELECT) ) {
		for( i=0; i<5 ; ++i ) {
			T_RemovePrint(DetailTextInfo[i]);
			DetailTextInfo[i] = NULL;
		}
	}
}

void __cdecl do_sound_option(INVENTORY_ITEM *item) {
	int i;
	char volumeString[20];

	if( SoundTextInfo[0] == NULL ) {
		CLAMP(MusicVolume, 0, 10);
		CLAMP(SoundVolume, 0, 10);
		sprintf(volumeString, "| %2d", MusicVolume); // Char '|' is musical note picture
		SoundTextInfo[0] = T_Print(0, 0, 0, volumeString);
		sprintf(volumeString, "} %2d", SoundVolume); // Char '}' is dynamic speaker picture
		SoundTextInfo[1] = T_Print(0, 25, 0, volumeString);

		T_AddBackground(SoundTextInfo[0], SOUND_WIDTH_S, 0, 0, 0, SOUND_NEARZ, ICLR_Black, &ReqSelGour1, 0);
		T_AddOutline(SoundTextInfo[0], TRUE, ICLR_Orange, &ReqSelGour2, 0);

		SoundTextInfo[2] = T_Print(0, SOUND_Y_BOX, 0, " ");
		T_AddBackground(SoundTextInfo[2], SOUND_WIDTH_L, SOUND_HEIGHT, 0, 0, SOUND_FARZ, ICLR_Black, &ReqBgndGour1, 0);
		T_AddOutline(SoundTextInfo[2], TRUE, ICLR_Blue, &ReqBgndGour2, 0);

		SoundTextInfo[3] = T_Print(0, SOUND_Y_TITLE, 0, GF_SpecificStringTable[SSI_SetVolumes]);
		T_AddBackground(SoundTextInfo[3], SOUND_WIDTH_M, 0, 0, 0, SOUND_NEARZ, ICLR_Black, &ReqMainGour1, 0);
		T_AddOutline(SoundTextInfo[3], TRUE, ICLR_Blue, &ReqMainGour2, 0);

		for( i=0; i<4 ; ++i ) {
			T_CentreH(SoundTextInfo[i], 1);
			T_CentreV(SoundTextInfo[i], 1);
		}
	}

	if( CHK_ANY(InputDB, IN_FORWARD) && SoundOptionLine > 0 ) {
		T_RemoveOutline(SoundTextInfo[SoundOptionLine]);
		T_RemoveBackground(SoundTextInfo[SoundOptionLine]);
		--SoundOptionLine;
		T_AddBackground(SoundTextInfo[SoundOptionLine], SOUND_WIDTH_S, 0, 0, 0, SOUND_NEARZ, ICLR_Black, &ReqSelGour1, 0);
		T_AddOutline(SoundTextInfo[SoundOptionLine], TRUE, ICLR_Orange, &ReqSelGour2, 0);
	}

	if( CHK_ANY(InputDB, IN_BACK) && SoundOptionLine < 1 ) {
		T_RemoveOutline(SoundTextInfo[SoundOptionLine]);
		T_RemoveBackground(SoundTextInfo[SoundOptionLine]);
		++SoundOptionLine;
		T_AddBackground(SoundTextInfo[SoundOptionLine], SOUND_WIDTH_S, 0, 0, 0, SOUND_NEARZ, ICLR_Black, &ReqSelGour1, 0);
		T_AddOutline(SoundTextInfo[SoundOptionLine], TRUE, ICLR_Orange, &ReqSelGour2, 0);
	}

	switch( SoundOptionLine ) {
		case 0 :
			if( CHK_ANY(InputStatus, IN_LEFT) && MusicVolume > 0 )
				--MusicVolume;
			else if( CHK_ANY(InputStatus, IN_RIGHT) && MusicVolume < 10 )
				++MusicVolume;
			else
				break;

			IsInvOptionsDelay = 1;
			InvOptionsDelayCounter = 10;
			sprintf(volumeString, "| %2d", MusicVolume); // Char '|' is musical note picture
			T_ChangeText(SoundTextInfo[0], volumeString);
			S_CDVolume(( MusicVolume == 0 ) ? 0 : (25 * MusicVolume + 5));
			PlaySoundEffect(115, NULL, SFX_ALWAYS); // page flip SFX
			break;

		case 1 :
			if( CHK_ANY(InputStatus, IN_LEFT) && SoundVolume > 0 )
				--SoundVolume;
			else if( CHK_ANY(InputStatus, IN_RIGHT) && SoundVolume < 10 )
				++SoundVolume;
			else
				break;

			IsInvOptionsDelay = 1;
			InvOptionsDelayCounter = 10;
			sprintf(volumeString, "} %2d", SoundVolume); // Char '}' is dynamic speaker picture
			T_ChangeText(SoundTextInfo[1], volumeString);
			S_SoundSetMasterVolume(( SoundVolume == 0 ) ? 0 : (6 * SoundVolume + 4));
			PlaySoundEffect(115, NULL, SFX_ALWAYS); // page flip SFX
			break;

		default :
			break;
	}

	InvSpriteSoundVolume[6].param1 = SoundVolume;
	InvSpriteSoundVolumeLow[6].param1 = SoundVolume;
	InvSpriteMusicVolume[6].param1 = MusicVolume;
	InvSpriteMusicVolumeLow[6].param1 = MusicVolume;

	if( CHK_ANY(InputDB, IN_SELECT|IN_DESELECT) ) {
		for( i=0; i<4 ; ++i ) {
			T_RemovePrint(SoundTextInfo[i]);
			SoundTextInfo[i] = NULL;
		}
	}
}

void __cdecl do_compass_option(INVENTORY_ITEM *item) {
	if( CurrentLevel == 0 ) {
		// level is Assault (Lara's Home)
		ShowGymStatsText();
	} else {
		// normal level
		int hours, minutes, seconds;
		char timeString[32] = {0};

		seconds = SaveGame.statistics.timer / 30 % 60;
		minutes = SaveGame.statistics.timer / 30 / 60 % 60;
		hours   = SaveGame.statistics.timer / 30 / 60 / 60;
		sprintf(timeString, "%02d:%02d:%02d", hours, minutes, seconds);
		ShowStatsText(timeString, TRUE);
	}

	if( CHK_ANY(InputDB, IN_SELECT|IN_DESELECT) ) {
		item->animDirection = 1;
		item->goalFrame = item->framesTotal - 1;
	}

	PlaySoundEffect(113, NULL, SFX_ALWAYS); // ticking clock sound
}

void __cdecl FlashConflicts() {
	UINT16 key;

	for( DWORD i=0; i<CONTROL_LINE_COUNT*2; ++i ) {
		key = Layout[LayoutPage].key[i];
		T_FlashText(CtrlTextB[i], 0, 0);

		for( DWORD j=0; j<CONTROL_LINE_COUNT*2; ++j ) {
			if( (i != j) && (key == Layout[LayoutPage].key[j]) ) {
				T_FlashText(CtrlTextB[i], 1, 20);
				break;
			}
		}
	}
}

void __cdecl DefaultConflict() {
	for( DWORD i=0; i<CONTROL_LINE_COUNT*2; ++i ) {
		ConflictLayout[i] = false;

		for( DWORD j=0; j<CONTROL_LINE_COUNT*2; ++j ) {
			if( Layout[CTRL_Custom].key[j] == Layout[CTRL_Default].key[i] ) {
				ConflictLayout[i] = true;
				break;
			}
		}
	}
}

#ifdef FEATURE_HUD_IMPROVED
static const char *GetControlsHeaderString() {
	const char *header = GF_SpecificStringTable[(LayoutPage == CTRL_Default) ? SSI_DefaultKeys : SSI_UserKeys];
	if( SavedAppSettings.RenderMode == RM_Hardware && InvTextBoxMode && KeyCursor < 0 ) {
		static char text[64] = {0};
		snprintf(text, sizeof(text), " \x11    %s   \x12  ", header);
		return text;
	}
	return header;
}
#endif // FEATURE_HUD_IMPROVED

void __cdecl do_control_option(INVENTORY_ITEM *item) {
	static int KeySelector = 0;
	int i;

#ifdef FEATURE_HUD_IMPROVED
	static int renderWidth = 0;
	if( renderWidth != GetRenderWidthDownscaled() ) {
		T_RemovePrint(ControlTextInfo[0]);
		ControlTextInfo[0] = NULL;
		T_RemovePrint(ControlTextInfo[1]);
		ControlTextInfo[1] = NULL;
		S_RemoveCtrlText();
		renderWidth = GetRenderWidthDownscaled();
	}
#endif // !FEATURE_HUD_IMPROVED

	if( ControlTextInfo[0] == NULL ) {
#ifdef FEATURE_HUD_IMPROVED
		KeyCursor = -1;
		ControlTextInfo[0] = T_Print(0, CONTROL_Y_TITLE, 0, GetControlsHeaderString());
#else // FEATURE_HUD_IMPROVED
		ControlTextInfo[0] = T_Print(0, CONTROL_Y_TITLE, 0, GF_SpecificStringTable[(LayoutPage == CTRL_Default) ? SSI_DefaultKeys : SSI_UserKeys]);
#endif // FEATURE_HUD_IMPROVED
		T_CentreH(ControlTextInfo[0], 1);
		T_CentreV(ControlTextInfo[0], 1);

		S_ShowControls();
#ifndef FEATURE_HUD_IMPROVED
		KeyCursor = -1;
#endif // FEATURE_HUD_IMPROVED
		T_AddBackground(ControlTextInfo[0], 0, 0, 0, 0, CONTROL_FARZ, ICLR_Black, &ReqSelGour1, 0);
		T_AddOutline(ControlTextInfo[0], TRUE, ICLR_Blue, &ReqSelGour2, 0);
		FlashConflicts(); // NOTE: this line is absent in the original game
	}

	switch( KeySelector ) {
		case 0 :
			if( CHK_ANY(InputDB, IN_LEFT|IN_RIGHT) ) {
				if( KeyCursor == -1 ) {
					LayoutPage = !LayoutPage;
					S_ChangeCtrlText();
					FlashConflicts();
				} else {
					CtrlTextA[KeyCursor]->zPos = CONTROL_NEARZ;
					T_RemoveBackground(CtrlTextA[KeyCursor]);
					T_RemoveOutline(CtrlTextA[KeyCursor]);

					if( KeyCursor < (int)CONTROL_LINE_COUNT ) {
						KeyCursor += CONTROL_LINE_COUNT;
					} else if ( KeyCursor < (int)CONTROL_LINE_COUNT*2 ) {
						KeyCursor -= CONTROL_LINE_COUNT;
					} else {
						KeyCursor = CONTROL_LINE_COUNT;
					}

					CtrlTextA[KeyCursor]->zPos = 0;
					T_AddBackground(CtrlTextA[KeyCursor], 0, 0, 0, 0, 0, ICLR_Black, &ReqSelGour1, 0);
					T_AddOutline(CtrlTextA[KeyCursor], TRUE, ICLR_Blue, &ReqSelGour2, 0);
				}
			}
			else if( (CHK_ANY(InputDB, IN_DESELECT) && !CHK_ANY(InputDB, IN_SELECT)) || (CHK_ANY(InputDB, IN_SELECT) && KeyCursor == -1) ) {
				T_RemovePrint(ControlTextInfo[0]);
				ControlTextInfo[0] = NULL;
				T_RemovePrint(ControlTextInfo[1]);
				ControlTextInfo[1] = NULL;
				S_RemoveCtrlText();
				DefaultConflict();
				return;
			}

			if( LayoutPage == CTRL_Default )
				break;

			if( CHK_ANY(InputDB, IN_SELECT) ) {
#ifdef FEATURE_HUD_IMPROVED
				UpdateJoystickHintText(HINT_KEYBOARD, HINT_HIDDEN);
#endif // FEATURE_HUD_IMPROVED
				KeySelector = 1;

				CtrlTextA[KeyCursor]->zPos = CONTROL_NEARZ;
				T_RemoveBackground(CtrlTextA[KeyCursor]);
				T_RemoveOutline(CtrlTextA[KeyCursor]);

				CtrlTextB[KeyCursor]->zPos = 0;
				T_AddBackground(CtrlTextB[KeyCursor], 0, 0, 0, 0, 0, ICLR_Black, &ReqSelGour1, 0);
				T_AddOutline(CtrlTextB[KeyCursor], TRUE, ICLR_Blue, &ReqSelGour2, 0);
			}
			else if( CHK_ANY(InputDB, IN_FORWARD) ) {
				if( KeyCursor == -1 ) {
					T_RemoveBackground(ControlTextInfo[0]);
					T_RemoveOutline(ControlTextInfo[0]);
				} else {
					CtrlTextA[KeyCursor]->zPos = CONTROL_NEARZ;
					T_RemoveBackground(CtrlTextA[KeyCursor]);
					T_RemoveOutline(CtrlTextA[KeyCursor]);
				}

				if( --KeyCursor < -1 ) {
					KeyCursor = CONTROL_LINE_COUNT*2 - 1;
				}

				if( KeyCursor == -1 ) {
					T_AddBackground(ControlTextInfo[0], 0, 0, 0, 0, 0, ICLR_Black, &ReqSelGour1, 0);
					T_AddOutline(ControlTextInfo[0], TRUE, ICLR_Blue, &ReqSelGour2, 0);
				} else {
					CtrlTextA[KeyCursor]->zPos = 0;
					T_AddBackground(CtrlTextA[KeyCursor], 0, 0, 0, 0, 0, ICLR_Black, &ReqSelGour1, 0);
					T_AddOutline(CtrlTextA[KeyCursor], TRUE, ICLR_Blue, &ReqSelGour2, 0);
				}
			}
			else if( CHK_ANY(InputDB, IN_BACK) ) {
				if( KeyCursor == -1 ) {
					T_RemoveBackground(ControlTextInfo[0]);
					T_RemoveOutline(ControlTextInfo[0]);
				} else {
					CtrlTextA[KeyCursor]->zPos = CONTROL_NEARZ;
					T_RemoveBackground(CtrlTextA[KeyCursor]);
					T_RemoveOutline(CtrlTextA[KeyCursor]);
				}

				if( ++KeyCursor >= (int)CONTROL_LINE_COUNT*2 ) {
					KeyCursor = -1;
				}

				if( KeyCursor == -1 ) {
					T_AddBackground(ControlTextInfo[0], 0, 0, 0, 0, 0, ICLR_Black, &ReqSelGour1, 0);
					T_AddOutline(ControlTextInfo[0], TRUE, ICLR_Blue, &ReqSelGour2, 0);
				} else {
					CtrlTextA[KeyCursor]->zPos = 0;
					T_AddBackground(CtrlTextA[KeyCursor], 0, 0, 0, 0, 0, ICLR_Black, &ReqSelGour1, 0);
					T_AddOutline(CtrlTextA[KeyCursor], TRUE, ICLR_Blue, &ReqSelGour2, 0);
				}
			}
			break;

		case 1:
			// NOTE: InputDB replaced by InputStatus here. This fixes the original game bug
			if( !CHK_ANY(InputStatus, IN_SELECT) && !CHK_ANY(DIKeys[DIK_RETURN], 0x80) ) {
				KeySelector = 2;
			}
			break;

		case 2:
			if( JoyKeys != 0 ) {
				for( i = 0; i < 0x20; ++i ) {
					if( CHK_ANY((1 << i), JoyKeys) )
						break;
				}
				if( i == 0x20 ) {
					break;
				}
				i += 0x100;
			} else {
				for( i = 0; i < 0x100; ++i ) {
					if( CHK_ANY(DIKeys[i], 0x80) )
						break;
				}
				if( i == 0x100 ) {
					break;
				}
			}

			if( i != 0 && ControlKeysText[i] != NULL && i != DIK_RETURN &&
				i != DIK_LEFT && i != DIK_RIGHT && i != DIK_UP && i != DIK_DOWN )
			{
				if( i != DIK_ESCAPE ) {
					Layout[LayoutPage].key[KeyCursor] = i;
					T_ChangeText(CtrlTextB[KeyCursor], ControlKeysText[i]);
				}

				CtrlTextB[KeyCursor]->zPos = CONTROL_NEARZ;
				T_RemoveBackground(CtrlTextB[KeyCursor]);
				T_RemoveOutline(CtrlTextB[KeyCursor]);

				CtrlTextA[KeyCursor]->zPos = 0;
				T_AddBackground(CtrlTextA[KeyCursor], 0, 0, 0, 0, 0, ICLR_Black, &ReqSelGour1, 0);
				T_AddOutline(CtrlTextA[KeyCursor], TRUE, ICLR_Blue, &ReqSelGour2, 0);

				KeySelector = 3;
				FlashConflicts();
			}
			break;

		case 3:
			if( CHK_ANY(Layout[LayoutPage].key[KeyCursor], 0x100) ) {
				if( !CHK_ANY((1 << Layout[LayoutPage].key[KeyCursor]), JoyKeys) ) {
					KeySelector = 0;
				}
			}
			else if( !CHK_ANY(DIKeys[Layout[LayoutPage].key[KeyCursor]], 0x80) ) {
				KeySelector = 0;
				if( Layout[LayoutPage].key[KeyCursor] == DIK_LCONTROL )
					Layout[LayoutPage].key[KeyCursor] = DIK_RCONTROL;
				if( Layout[LayoutPage].key[KeyCursor] == DIK_LSHIFT )
					Layout[LayoutPage].key[KeyCursor] = DIK_RSHIFT;
				if( Layout[LayoutPage].key[KeyCursor] == DIK_LMENU )
					Layout[LayoutPage].key[KeyCursor] = DIK_RMENU;
				FlashConflicts();
			}
#ifdef FEATURE_HUD_IMPROVED
			if( KeySelector == 0 ) {
				UpdateJoystickHintText(HINT_JOYSTICK, HINT_JOYSTICK);
			}
#endif // FEATURE_HUD_IMPROVED
			break;
	}

	InputStatus = 0;
	InputDB = 0;
}

void __cdecl S_ShowControls() {
	DWORD i;
	int x0, x1, xCenter;
	bool isCompact;

#ifdef FEATURE_HUD_IMPROVED
	xCenter = GetRenderWidthDownscaled() / 2;
	isCompact = (xCenter < (CONTROL_WIDTH_HIGH + 20) / 2);
#else // !FEATURE_HUD_IMPROVED
	xCenter = GetRenderWidth() / 2;
	CLAMPG(xCenter, 320);
	isCompact = (xCenter < 320);
#endif // FEATURE_HUD_IMPROVED

	if( CtrlTextB[0] == NULL ) {
		x0 = xCenter - (isCompact ? CONTROL_WIDTH_LOW : CONTROL_WIDTH_HIGH) / 2 + CONTROL_COLUMN_B;
		x1 = xCenter + CONTROL_COLUMN_B;

		CtrlTextB[0]  = T_Print(x0, CONTROL_Y_LINE1, CONTROL_NEARZ, ControlKeysText[Layout[LayoutPage].key[0]]);
		CtrlTextB[1]  = T_Print(x0, CONTROL_Y_LINE2, CONTROL_NEARZ, ControlKeysText[Layout[LayoutPage].key[1]]);
		CtrlTextB[2]  = T_Print(x0, CONTROL_Y_LINE3, CONTROL_NEARZ, ControlKeysText[Layout[LayoutPage].key[2]]);
		CtrlTextB[3]  = T_Print(x0, CONTROL_Y_LINE4, CONTROL_NEARZ, ControlKeysText[Layout[LayoutPage].key[3]]);
		CtrlTextB[4]  = T_Print(x0, CONTROL_Y_LINE5, CONTROL_NEARZ, ControlKeysText[Layout[LayoutPage].key[4]]);
		CtrlTextB[5]  = T_Print(x0, CONTROL_Y_LINE6, CONTROL_NEARZ, ControlKeysText[Layout[LayoutPage].key[5]]);
		CtrlTextB[6]  = T_Print(x0, CONTROL_Y_LINE7, CONTROL_NEARZ, ControlKeysText[Layout[LayoutPage].key[6]]);

		CtrlTextB[7]  = T_Print(x1, CONTROL_Y_LINE1, CONTROL_NEARZ, ControlKeysText[Layout[LayoutPage].key[7]]);
		CtrlTextB[8]  = T_Print(x1, CONTROL_Y_LINE2, CONTROL_NEARZ, ControlKeysText[Layout[LayoutPage].key[8]]);
		CtrlTextB[9]  = T_Print(x1, CONTROL_Y_LINE3, CONTROL_NEARZ, ControlKeysText[Layout[LayoutPage].key[9]]);
		CtrlTextB[10] = T_Print(x1, CONTROL_Y_LINE4, CONTROL_NEARZ, ControlKeysText[Layout[LayoutPage].key[10]]);
		CtrlTextB[11] = T_Print(x1, CONTROL_Y_LINE5, CONTROL_NEARZ, ControlKeysText[Layout[LayoutPage].key[11]]);
		CtrlTextB[12] = T_Print(x1, CONTROL_Y_LINE6, CONTROL_NEARZ, ControlKeysText[Layout[LayoutPage].key[12]]);
		CtrlTextB[13] = T_Print(x1, CONTROL_Y_LINE7, CONTROL_NEARZ, ControlKeysText[Layout[LayoutPage].key[13]]);

		for( i=0; i<CONTROL_LINE_COUNT*2; ++i ) {
			T_CentreV(CtrlTextB[i], 1);
		}

#ifndef FEATURE_HUD_IMPROVED
		KeyCursor = 0;
#endif // FEATURE_HUD_IMPROVED
	}

	if( CtrlTextA[0] == NULL ) {
		x0 = xCenter - (isCompact ? CONTROL_WIDTH_LOW : CONTROL_WIDTH_HIGH) / 2 + CONTROL_COLUMN_A;
#ifdef FEATURE_HUD_IMPROVED
		x1 = xCenter + CONTROL_COLUMN_A;
#else // !FEATURE_HUD_IMPROVED
		x1 = xCenter + CONTROL_COLUMN_A + 10;
#endif // FEATURE_HUD_IMPROVED

		CtrlTextA[0]  = T_Print(x0, CONTROL_Y_LINE1, CONTROL_NEARZ, GF_GameStringTable[GSI_Keymap_Run]);
		CtrlTextA[1]  = T_Print(x0, CONTROL_Y_LINE2, CONTROL_NEARZ, GF_GameStringTable[GSI_Keymap_Back]);
		CtrlTextA[2]  = T_Print(x0, CONTROL_Y_LINE3, CONTROL_NEARZ, GF_GameStringTable[GSI_Keymap_Left]);
		CtrlTextA[3]  = T_Print(x0, CONTROL_Y_LINE4, CONTROL_NEARZ, GF_GameStringTable[GSI_Keymap_Right]);
		CtrlTextA[4]  = T_Print(x0, CONTROL_Y_LINE5, CONTROL_NEARZ, GF_GameStringTable[GSI_Keymap_StepLeft]);
		CtrlTextA[5]  = T_Print(x0, CONTROL_Y_LINE6, CONTROL_NEARZ, GF_GameStringTable[GSI_Keymap_StepRight]);
		CtrlTextA[6]  = T_Print(x0, CONTROL_Y_LINE7, CONTROL_NEARZ, GF_GameStringTable[GSI_Keymap_Walk]);

		CtrlTextA[7]  = T_Print(x1, CONTROL_Y_LINE1, CONTROL_NEARZ, GF_GameStringTable[GSI_Keymap_Jump]);
		CtrlTextA[8]  = T_Print(x1, CONTROL_Y_LINE2, CONTROL_NEARZ, GF_GameStringTable[GSI_Keymap_Action]);
		CtrlTextA[9]  = T_Print(x1, CONTROL_Y_LINE3, CONTROL_NEARZ, GF_GameStringTable[GSI_Keymap_DrawWeapon]);
		CtrlTextA[10] = T_Print(x1, CONTROL_Y_LINE4, CONTROL_NEARZ, GF_GameStringTable[GSI_Keymap_Flare]);
		CtrlTextA[11] = T_Print(x1, CONTROL_Y_LINE5, CONTROL_NEARZ, GF_GameStringTable[GSI_Keymap_Look]);
		CtrlTextA[12] = T_Print(x1, CONTROL_Y_LINE6, CONTROL_NEARZ, GF_GameStringTable[GSI_Keymap_Roll]);
		CtrlTextA[13] = T_Print(x1, CONTROL_Y_LINE7, CONTROL_NEARZ, GF_GameStringTable[GSI_Keymap_Inventory]);

		for( i=0; i<CONTROL_LINE_COUNT*2; ++i ) {
			T_CentreV(CtrlTextA[i], 1);
		}
	}

	ControlTextInfo[1] = T_Print(0, CONTROL_Y_BOX, 0, " ");
	T_CentreV(ControlTextInfo[1], 1);
	T_CentreH(ControlTextInfo[1], 1);
	T_AddOutline(ControlTextInfo[1], TRUE, ICLR_Blue, &ReqBgndGour2, 0);

	if( isCompact ) {
		for( i=0; i<CONTROL_LINE_COUNT*2; ++i ) {
#ifdef FEATURE_HUD_IMPROVED
			T_SetScale(CtrlTextA[i], PHD_ONE*3/4, PHD_ONE);
#else // !FEATURE_HUD_IMPROVED
			T_SetScale(CtrlTextB[i], PHD_ONE/2, PHD_ONE);
			T_SetScale(CtrlTextA[i], PHD_ONE/2, PHD_ONE);
#endif // FEATURE_HUD_IMPROVED
		}
		T_AddBackground(ControlTextInfo[1], CONTROL_WIDTH_LOW, CONTROL_HEIGHT_LOW, 0, 0, CONTROL_FARZ, ICLR_Black, &ReqBgndGour1, 0);
	} else {
		T_AddBackground(ControlTextInfo[1], CONTROL_WIDTH_HIGH, CONTROL_HEIGHT_HIGH, 0, 0, CONTROL_FARZ, ICLR_Black, &ReqBgndGour1, 0);
	}
}

void __cdecl S_ChangeCtrlText() {
	UINT16 key;
#ifdef FEATURE_HUD_IMPROVED
	T_ChangeText(ControlTextInfo[0], GetControlsHeaderString());
#else // FEATURE_HUD_IMPROVED
	char headerStr[40];

	if( LayoutPage == CTRL_Default ) {
		T_ChangeText(ControlTextInfo[0], GF_SpecificStringTable[SSI_DefaultKeys]);
	} else {
		// NOTE: it seems Core wanted to use several custom control layout pages
		sprintf(headerStr, GF_SpecificStringTable[SSI_UserKeys], LayoutPage);
		T_ChangeText(ControlTextInfo[0], headerStr);
	}
#endif // FEATURE_HUD_IMPROVED

	for( DWORD i=0; i<CONTROL_LINE_COUNT*2; ++i ) {
		key = Layout[LayoutPage].key[i];
		// NOTE: there was no key range check in the original code
		if( key < 0x110 && ControlKeysText[key] != NULL )
			T_ChangeText(CtrlTextB[i], ControlKeysText[key]);
		else
#ifdef FEATURE_HUD_IMPROVED
			T_ChangeText(CtrlTextB[i], "?");
#else // !FEATURE_HUD_IMPROVED
			T_ChangeText(CtrlTextB[i], "BAD");
#endif // FEATURE_HUD_IMPROVED
	}
}

void __cdecl S_RemoveCtrlText() {
	for( DWORD i=0; i<CONTROL_LINE_COUNT*2; ++i ) {
		T_RemovePrint(CtrlTextA[i]);
		T_RemovePrint(CtrlTextB[i]);
		CtrlTextA[i] = NULL;
		CtrlTextB[i] = NULL;
	}
}

/*
 * Inject function
 */
void Inject_Option() {
	INJECT(0x0044EE80, do_inventory_options);
	INJECT(0x0044EF90, do_passport_option);
//	INJECT(----------, do_gamma_option); // NOTE: this is null in the original code
	INJECT(0x0044F5E0, do_detail_option);
	INJECT(0x0044F8C0, do_sound_option);
	INJECT(0x0044FD60, do_compass_option);
	INJECT(0x0044FE20, FlashConflicts);
	INJECT(0x0044FEA0, DefaultConflict);
	INJECT(0x0044FEE0, do_control_option);
	INJECT(0x004505F0, S_ShowControls);
	INJECT(0x00450AC0, S_ChangeCtrlText);
	INJECT(0x00450B60, S_RemoveCtrlText);
}
