/*
*   This file is a modified part of Luma3DS
*   Copyright (C) 2016-2019 Aurora Wright, TuxSH
*   Modified 2020 Gamestabled
*   Modified 2021 HylianFreddy
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

#pragma once

#include <stdbool.h>
#include "utils.h"

#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)

#define CORE_APPLICATION  0
#define CORE_SYSTEM       1
#define CORE_DEFAULT     -2

typedef enum {
    VARTYPE_S8,
    VARTYPE_U8,
    VARTYPE_S16,
    VARTYPE_U16,
    VARTYPE_S32,
    VARTYPE_U32,
    VARTYPE_MAX,
} VarType;

typedef enum MenuItemAction {
    METHOD,
    MENU
} MenuItemAction;

typedef struct MenuItem {
    char *title;

    MenuItemAction action_type;
    union {
        struct Menu *menu;
        void (*method)(void);
    };
} MenuItem;

typedef struct Menu {
    const char *title;

    u32 nbItems;
    u32 initialCursorPos;
    MenuItem items[0x40];
} Menu;

typedef struct ToggleMenuItem {
    u8 on;
    char *title;
    void (*method)(s32);
} ToggleMenuItem;

typedef struct ToggleMenu {
    const char *title;

    u32 nbItems;
    s32 initialCursorPos;
    ToggleMenuItem items[0x40];
} ToggleMenu;

typedef struct AmountMenuItem {
    u16 amount; //current amount
    VarType varType;
    s32 min;    //min amount, 0 = no limit
    s32 max;    //max amount, 0 = no limit
    s32 nDigits;//number of digits to display
    u8 hex;     //display in hex or decimal
    char *title;
    void (*method)(s32);
} AmountMenuItem;

typedef struct AmountMenu {
    const char *title;

    u32 nbItems;
    s32 initialCursorPos;
    AmountMenuItem items[0x40];
} AmountMenu;

static const struct {s64 min; s64 max;} varTypeLimits[VARTYPE_MAX] = {
    { // S8
        .min = -128,
        .max = 127,
    },
    { // U8
        .min = 0,
        .max = 255,
    },
    { // S16
        .min = -32768,
        .max = 32767,
    },
    { // U16
        .min = 0,
        .max = 65535,
    },
    { // S32
        .min = -2147483648,
        .max = 2147483647,
    },
    { // U32
        .min = 0,
        .max = 4294967295,
    },
};

#define TOGGLE_MENU_MAX_SHOW 18
#define AMOUNT_MENU_MAX_SHOW 18

void menuShow();
void ToggleMenuShow(ToggleMenu *menu);
void AmountMenuShow(AmountMenu *menu);
void Menu_EditAmount(u32 posX, u32 posY, void* value, VarType varType, s32 min, s32 max, s32 digitCount, u8 isHex);

u32 KeyboardFill(char* buf, u32 len);

extern bool onMenuLoop();
extern bool menuOpen;
