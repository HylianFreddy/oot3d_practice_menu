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

#include "3ds/srv.h"
#include "menu.h"
#include "draw.h"
#include "menus.h"
#include "z3D/z3D.h"
#include "utils.h"
#include "input.h"
#include "menus/commands.h"
#include "menus/settings.h"
#include "common.h"
#include <stdio.h>
#include <string.h>

static void menuDraw(Menu *menu, u32 selected)
{
    Draw_DrawString(10, 10, COLOR_TITLE, menu->title);

    for(u32 i = 0; i < 15; i++)
    {
        if(i >= menu->nbItems)
            break;
        Draw_DrawString(30, 30 + i * SPACING_Y, COLOR_WHITE, menu->items[i].title);
        Draw_DrawCharacter(10, 30 + i * SPACING_Y, COLOR_TITLE, i == selected ? '>' : ' ');
    }

    Draw_DrawFormattedString(SCREEN_BOT_WIDTH - 10 - 4 * SPACING_X, SCREEN_BOT_HEIGHT - 20, COLOR_WHITE, "    ");
    Draw_FlushFramebuffer();
}

void menuShow(Menu* rootMenu)
{
    Menu *currentMenu = rootMenu;
    u32 selectedItem = currentMenu->initialCursorPos;
    u32 nbPreviousMenus = 0;
    Menu *previousMenus[0x80];
    u32 previousSelectedItems[0x80];

    if (ToggleSettingsMenu.items[TOGGLESETTINGS_REMEMBER_CURSOR_POSITION].on == 0) {
        selectedItem = 0;
    }

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    menuDraw(currentMenu, selectedItem);
    Draw_Unlock();

    do
    {
        u32 pressed = Input_WaitWithTimeout(1000);

        if(pressed & BUTTON_A)
        {
            switch(currentMenu->items[selectedItem].action_type)
            {
                case METHOD:
                    if(currentMenu->items[selectedItem].method != NULL)
                        currentMenu->items[selectedItem].method();
                    break;
                case MENU:
                    previousSelectedItems[nbPreviousMenus] = selectedItem;
                    previousMenus[nbPreviousMenus++] = currentMenu;
                    currentMenu = currentMenu->items[selectedItem].menu;
                    selectedItem = currentMenu->initialCursorPos;
                    if (ToggleSettingsMenu.items[TOGGLESETTINGS_REMEMBER_CURSOR_POSITION].on == 0) {
                        selectedItem = 0;
                    }
                    break;
            }

            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            Draw_Unlock();
        }
        else if(pressed & BUTTON_B)
        {
            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            Draw_Unlock();

            if(nbPreviousMenus > 0)
            {
                currentMenu = previousMenus[--nbPreviousMenus];
                selectedItem = previousSelectedItems[nbPreviousMenus];
            }
            else
            {
                break;
            }
        }
        else if (pressed & BUTTON_L1) {
            selectedItem = 0;
        }
        else if(pressed & PAD_DOWN)
        {
            if(++selectedItem >= currentMenu->nbItems)
                selectedItem = 0;
        }
        else if(pressed & PAD_UP)
        {
            if(selectedItem-- <= 0)
                selectedItem = currentMenu->nbItems - 1;
        }

        currentMenu->initialCursorPos = selectedItem;

        Draw_Lock();
        menuDraw(currentMenu, selectedItem);
        Draw_Unlock();
    }
    while(onMenuLoop());

    // Wait 300 milliseconds for users to buffer inputs and set menu as closed, but only if:
    //  - the menu is closing normally (as opposed to something else setting menuOpen to false);
    //  - this function was not called from some other menu.
    if (menuOpen && (rootMenu == &gz3DMenu)) {
        svcSleepThread(1000 * 1000 * 300LL);
        menuOpen = false;
    }

    if (ToggleSettingsMenu.items[TOGGLESETTINGS_MAIN_HOOK].on == 0) {
        commandList[COMMAND_OPEN_MENU].waiting = 0;
    }
}

void ToggleMenuShow(ToggleMenu *menu) //displays a toggle menu, analogous to rosalina cheats page
{
    s32 selected = menu->initialCursorPos, page = selected / TOGGLE_MENU_MAX_SHOW, pagePrev = page;

    if (ToggleSettingsMenu.items[TOGGLESETTINGS_REMEMBER_CURSOR_POSITION].on == 0) {
        selected = page = pagePrev = 0;
    }

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();

    do
    {
        Draw_Lock();
        if (page != pagePrev)
        {
            Draw_ClearFramebuffer();
        }
        Draw_DrawFormattedString(10, 10, COLOR_TITLE, menu->title);

        for (s32 i = 0; i < TOGGLE_MENU_MAX_SHOW && page * TOGGLE_MENU_MAX_SHOW + i < menu->nbItems; ++i)
        {
            s32 j = page * TOGGLE_MENU_MAX_SHOW + i;
            const char* checkbox = (menu->items[j].on ? "(x) " : "( ) ");
            Draw_DrawFormattedString(30, 30 + i * SPACING_Y, COLOR_WHITE, "%s%s", checkbox, menu->items[j].title);
            Draw_DrawCharacter(10, 30 + i * SPACING_Y, COLOR_TITLE, j == selected ? '>' : ' ');
        }

        Draw_FlushFramebuffer();
        Draw_Unlock();

        u32 pressed = Input_WaitWithTimeout(1000);
        if(pressed & BUTTON_B)
            break;
        if(pressed & BUTTON_A)
        {
            if(menu->items[selected].method != NULL) {
                menu->items[selected].method(selected); //the method will handle swapping on/off
            }

            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            Draw_Unlock();
        }
        else if (pressed & BUTTON_L1) {
            selected = 0;
        }
        else if(pressed & PAD_DOWN)
        {
            selected++;
        }
        else if(pressed & PAD_UP)
        {
            selected--;
        }
        else if(pressed & PAD_LEFT){
            selected -= TOGGLE_MENU_MAX_SHOW;
        }
        else if(pressed & PAD_RIGHT){
            if(selected + TOGGLE_MENU_MAX_SHOW < menu->nbItems)
                selected += TOGGLE_MENU_MAX_SHOW;
            else if((menu->nbItems - 1) / TOGGLE_MENU_MAX_SHOW == page)
                selected %= TOGGLE_MENU_MAX_SHOW;
            else selected = menu->nbItems - 1;
        }

        if(selected < 0)
            selected = menu->nbItems - 1;
        else if(selected >= menu->nbItems) selected = 0;

        menu->initialCursorPos = selected;

        pagePrev = page;
        page = selected / TOGGLE_MENU_MAX_SHOW;
    } while(onMenuLoop());
}

void AmountMenuShow(AmountMenu* menu){ //displays an amount menu
    s32 selected = menu->initialCursorPos, page = selected / AMOUNT_MENU_MAX_SHOW, pagePrev = page;

    if (ToggleSettingsMenu.items[TOGGLESETTINGS_REMEMBER_CURSOR_POSITION].on == 0) {
        selected = page = pagePrev = 0;
    }

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();

    do
    {
        Draw_Lock();
        if (page != pagePrev)
        {
            Draw_ClearFramebuffer();
        }
        Draw_DrawFormattedString(10, 10, COLOR_TITLE, menu->title);

        for (s32 i = 0; i < AMOUNT_MENU_MAX_SHOW && page * AMOUNT_MENU_MAX_SHOW + i < menu->nbItems; ++i)
        {
            s32 j = page * AMOUNT_MENU_MAX_SHOW + i;
            Draw_DrawString(70, 30 + i * SPACING_Y, COLOR_WHITE, menu->items[j].title);
            s16 signedAmount = (s16)(menu->items[j].amount);
            u8 digitsOffDecimalNegative = (menu->items[j].isSigned && signedAmount < 0) ? 1 : 0;
            Draw_DrawFormattedString(10 + ((menu->items[j].hex ? 4 : 5) - menu->items[j].nDigits - digitsOffDecimalNegative) * SPACING_X,
                                     30 + i * SPACING_Y,
                                     j == selected ? COLOR_GREEN : COLOR_TITLE,
                                     menu->items[j].hex ? " 0x%0*X" : "  %0*d",
                                     menu->items[j].nDigits + digitsOffDecimalNegative,
                                     menu->items[j].isSigned ? signedAmount : menu->items[j].amount);
        }

        Draw_FlushFramebuffer();
        Draw_Unlock();

        u32 pressed = Input_WaitWithTimeout(1000);
        if(pressed & BUTTON_B)
            break;
        else if(pressed & BUTTON_A)
        {
            bool isHex = menu->items[selected].hex;
            u32 posX = 10 + ((isHex ? 4 : 6) - menu->items[selected].nDigits) * SPACING_X;
            u32 posY = 30 + (selected % AMOUNT_MENU_MAX_SHOW) * SPACING_Y;
            Menu_EditAmount(posX, posY, &menu->items[selected].amount,
                            menu->items[selected].isSigned ? VARTYPE_S16 : VARTYPE_U16,
                            menu->items[selected].min,
                            menu->items[selected].max,
                            menu->items[selected].nDigits,
                            isHex,
                            menu->items[selected].method, selected);
        }
        else if (pressed & BUTTON_L1) {
            selected = 0;
        }
        else if(pressed & PAD_DOWN)
        {
            selected++;
        }
        else if(pressed & PAD_UP)
        {
            selected--;
        }
        else if(pressed & PAD_LEFT)
        {
            selected -= AMOUNT_MENU_MAX_SHOW;
        }
        else if(pressed & PAD_RIGHT)
        {
            if(selected + AMOUNT_MENU_MAX_SHOW < menu->nbItems)
                selected += AMOUNT_MENU_MAX_SHOW;
            else if((menu->nbItems - 1) / AMOUNT_MENU_MAX_SHOW == page)
                selected %= AMOUNT_MENU_MAX_SHOW;
            else selected = menu->nbItems - 1;
        }

        if(selected < 0)
            selected = menu->nbItems - 1;
        else if(selected >= menu->nbItems) selected = 0;

        menu->initialCursorPos = selected;

        pagePrev = page;
        page = selected / AMOUNT_MENU_MAX_SHOW;
    } while(onMenuLoop());
}

u32 KeyboardFill(char * buf, u32 len){
    const char* Upper = "1234567890QWERTYUIOPASDFGHJKL'ZXCVBNM,.+";
    const char* Lower = "1234567890qwertyuiopasdfghjkl'zxcvbnm,.+";

    const char* keys = Lower;
    s32 selected = 0;
    u32 idx = strlen(buf);

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();

    do
    {
        Draw_Lock();
        Draw_DrawString(10, 10, COLOR_TITLE, "Edit Watch Name");


        for(u32 i = 0; i < 10; ++i){
            Draw_DrawFormattedString(30 + (i * 2 * SPACING_X), 30, selected == i ? COLOR_GREEN : COLOR_WHITE, "%c ", keys[i]);
        }
        for(u32 i = 0; i < 10; ++i){
            u32 j = 10 + i;
            Draw_DrawFormattedString(30 + (i * 2 * SPACING_X), 30 + 2 * SPACING_Y, selected == j ? COLOR_GREEN : COLOR_WHITE, "%c ", keys[j]);
        }
        for(u32 i = 0; i < 10; ++i){
            u32 j = 20 + i;
            Draw_DrawFormattedString(30 + (i * 2 * SPACING_X), 30 + 4 * SPACING_Y, selected == j ? COLOR_GREEN : COLOR_WHITE, "%c ", keys[j]);
        }
        for(u32 i = 0; i < 10; ++i){
            u32 j = 30 + i;
            Draw_DrawFormattedString(30 + (i * 2 * SPACING_X), 30 + 6 * SPACING_Y, selected == j ? COLOR_GREEN : COLOR_WHITE, "%c ", keys[j]);
        }
        Draw_DrawString(30, 30 + 7 * SPACING_Y, COLOR_RED, buf);

        Draw_DrawString(10, SCREEN_BOT_HEIGHT - 20, COLOR_TITLE, "L/R to switch case. Start to save and return.");

        Draw_FlushFramebuffer();
        Draw_Unlock();

        u32 pressed = Input_WaitWithTimeout(1000);
        if(pressed & BUTTON_B){
            idx--;
            if(idx < 0) idx = 0;
            buf[idx] = '\0';
            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            Draw_Unlock();
        }
        else if(pressed & BUTTON_A){
            if(idx >= len) idx = len - 1;
            buf[idx] = keys[selected];
            idx++;
            buf[idx] = '\0';
        }
        else if(pressed & (BUTTON_R1 | BUTTON_L1)){
            keys = (keys == Lower) ? Upper : Lower;
        }
        else if(pressed & PAD_DOWN){
            selected += 10;
        }
        else if(pressed & PAD_UP){
            selected -= 10;
        }
        else if(pressed & PAD_RIGHT){
            selected++;
        }
        else if(pressed & PAD_LEFT){
            selected--;
        }
        else if(pressed & BUTTON_START){
            break;
        }

        if (selected >= 40) selected = 0;
        if (selected < 0) selected = 39;
    } while(onMenuLoop());

    return idx;
}

/**
 * @brief Allow the user to edit a numeric value displayed at an arbitrary position on the screen
 */
void Menu_EditAmount(u32 posX, u32 posY, void* valueAddress, VarType varType, s32 customMin, s32 customMax,
                     s32 digitCount, bool isHex, void (*method)(s32), s32 amountMenuIndex) {

    static void* lastEditedValue = 0;
    static s32 digitIndex = 0;
    static char* formatString = "%s%0*_";
    static char* formatCursor = "%_";

    if (valueAddress != lastEditedValue) {
        lastEditedValue = valueAddress;
        digitIndex = 0;
    }

    s64 longValue;
    switch (varType) {
        case VARTYPE_S8:  longValue =  *(s8*)valueAddress; break;
        case VARTYPE_U8:  longValue =  *(u8*)valueAddress; break;
        case VARTYPE_S16: longValue = *(s16*)valueAddress; break;
        case VARTYPE_U16: longValue = *(u16*)valueAddress; break;
        case VARTYPE_S32: longValue = *(s32*)valueAddress; break;
        default:          longValue = *(u32*)valueAddress; break;
    }

    s64 min = (customMin != 0) ? customMin : varTypeLimits[varType].min;
    s64 max = (customMax != 0) ? customMax : varTypeLimits[varType].max;

    // Update format strings (hex/signed/unsigned)
    if (isHex) {
        formatString[5] = formatCursor[1] = 'X';
    } else {
        formatString[5] = formatCursor[1] = (varType == VARTYPE_U32 ? 'u' : 'd');
    }

    do
    {
        Draw_Lock();

        // Draw value
        const char* prefix = isHex
            ? longValue < 0
                ? "-0x"
                : " 0x"
            : longValue < 0
                ? ""
                : " "
        ;
        Draw_DrawFormattedString(posX, posY, COLOR_GREEN, formatString,
            prefix,
            digitCount + ((isHex || longValue >= 0) ? 0 : 1),
            (isHex && longValue < 0) ? -longValue : longValue
        );
        if (method) {
            method(amountMenuIndex);
        }

        // Calculate the positional value of the selected digit
        s32 digitValue = 1;
        for (s32 i = 0; i < digitIndex; i++) {
            digitValue = digitValue * (isHex ? 16 : 10);
        }

        // Draw cursor
        Draw_DrawFormattedString(posX + (digitCount - digitIndex + (isHex ? 2 : 0)) * SPACING_X, posY, COLOR_RED, formatCursor,
            ((longValue < 0 ? -longValue : longValue) / digitValue) % (isHex ? 16 : 10));

        Draw_Unlock();

        // Handle input
        u32 pressed = Input_WaitWithTimeout(1000);

        if (pressed & (BUTTON_B | BUTTON_A)){
            break;
        }
        else if ((pressed & BUTTON_R1) && (pressed & BUTTON_L1)) {
            longValue = 0;
        }
        else if (pressed & PAD_UP) {
            longValue += digitValue;
        }
        else if (pressed & PAD_DOWN) {
            longValue -= digitValue;
        }
        else if (pressed & PAD_RIGHT){
            digitIndex--;
        }
        else if (pressed & PAD_LEFT){
            digitIndex++;
        }

        // Limit cursor position
        if(digitIndex >= digitCount)
            digitIndex = 0;
        else if(digitIndex < 0)
            digitIndex = digitCount - 1;

        // Limit value
        while ((longValue > max) || (longValue < min)) {
            s64 offset = 1 + max - min;
            if (longValue > max) {
                longValue -= offset;
            } else {
                longValue += offset;
            }
        }

        // Write new value to address
        if (varType <= VARTYPE_U8) {
            *(u8*)valueAddress = (u8)longValue;
        } else if (varType <= VARTYPE_U16) {
            *(u16*)valueAddress = (u16)longValue;
        } else {
            *(u32*)valueAddress = longValue;
        }

    } while(menuOpen);
}
