/*
*   This file is a modified part of Luma3DS
*   Copyright (C) 2016-2019 Aurora Wright, TuxSH
*   Modified 2020 Gamestabled
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include "3ds/os.h"
#include "menus.h"
#include "menu.h"
#include "draw.h"
#include "z3D/z3D.h"
#include "input.h"
#include "common.h"
#include "commit_string.h"

#include "menus/warps.h"
#include "menus/scene.h"
#include "menus/cheats.h"
#include "menus/inventory.h"
#include "menus/equips.h"
#include "menus/file.h"
#include "menus/watches.h"
#include "menus/debug.h"
#include "menus/commands.h"
#include "menus/settings.h"

#if GZ3D_EXTRAS
static u32 sfxId = 0;
AmountMenu PlaySFXMenu;

void PlaySFX(s32 selected) {
#if Version_KOR || Version_TWN
    setAlert(UNSUPPORTED_WARNING, 90);
    return;
#endif
    sfxId = PlaySFXMenu.items[selected].amount;
    PlaySound(0x1000000 + sfxId);
}

AmountMenu PlaySFXMenu = {
    "Play SFX",
    .nbItems = 1,
    .initialCursorPos = 0,
    {
        {.amount = 0, .isSigned = false, .min = 0, .max = 0, .nDigits = 4, .hex = true,
            .title = "SFX ID", .method = PlaySFX},
    }
};

void showSFXMenu(void) {
    AmountMenuShow(&PlaySFXMenu);
}

void quitGame(void) {
    if (!gGlobalContext) {
        return;
    }
    if (!ADDITIONAL_FLAG_BUTTON) {
        setAlert("Hold R", 90);
        return;
    }

#if Version_KOR || Version_TWN
    setAlert(UNSUPPORTED_WARNING, 90);
    return;
#endif
    gGlobalContext->state.running = 0;
    gGlobalContext->state.init = 0;
    *((u8*)0x5C6605) = 1; // break loop calling Graph_ThreadEntry
    menuOpen = false;
}
#endif // GZ3D_EXTRAS

Menu gz3DMenu = {
    "Practice Menu (" COMMIT_STRING ")",
    .nbItems = 11 + (!!GZ3D_EXTRAS * 2),
    .initialCursorPos = 0,
    {
        { "Warps", MENU, .menu = &WarpsMenu },
        { "Scene", MENU, .menu = &SceneMenu },
        { "Cheats", METHOD, .method = Cheats_ShowCheatsMenu },
        { "Inventory", MENU, .menu = &InventoryMenu },
        { "Equips", METHOD, .method = Equips_ShowEquipsMenu },
        { "File", METHOD, .method = File_ShowFileMenu },
        { "Watches", METHOD, .method = Watches_ShowWatchesMenu },
        { "Debug", MENU, .menu = &DebugMenu },
        { "Commands", METHOD, .method = Commands_ShowCommandsMenu },
        { "Settings", METHOD, .method = Settings_ShowSettingsMenu },
        { "Profiles", MENU, .menu = &ProfilesMenu },
#if GZ3D_EXTRAS
        { "Play SFX", METHOD, .method = showSFXMenu },
        { "Quit Game", METHOD, .method = quitGame },
#endif // GZ3D_EXTRAS
    }
};
