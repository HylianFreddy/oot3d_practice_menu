#include "menu.h"
#include "menus/watches.h"
#include "menus/debug.h"
#include "menus/settings.h"
#include "draw.h"
#include "input.h"
#include "z3D/z3D.h"
#include "common.h"

#include <stdio.h>
#include <string.h>

static void Watches_DrawWatch(Watch watch, u32 color);

Watch watches[WATCHES_MAX];

static const char* const WatchTypeNames[] = {
    "S8 ",
    "U8 ",
    "X8 ",
    "S16",
    "U16",
    "X16",
    "S32",
    "U32",
    "X32",
    "F32",
};

static void Watches_EditName(char* nameBuf) {
    KeyboardFill(nameBuf, WATCHES_MAXNAME);
    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();
}

static void Watches_EditPos(s32 watchId) {
    u32 pressed, speed;

    watches[watchId].display = TRUE;

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();

    do {
        Draw_Lock();
        Watches_DrawWatches();
        Watches_DrawWatch(watches[watchId], COLOR_GREEN);
        Draw_DrawString(10, SCREEN_BOT_HEIGHT - 20, COLOR_TITLE, "Hold X: Move fast    Start: Reset position");
        Draw_FlushFramebuffer();
        Draw_Unlock();

        pressed = Input_WaitWithTimeout(1000);

        Draw_Lock();
        Watches_DrawWatch(watches[watchId], COLOR_BLACK);
        Draw_FlushFramebuffer();
        Draw_Unlock();

        if (pressed & (BUTTON_A | BUTTON_B))
            break;
        if (pressed & BUTTON_START) {
            watches[watchId].posX = 70;
            watches[watchId].posY = 40 + watchId * SPACING_Y;
        }

        speed = (pressed & BUTTON_X) ? 10 : 1;
        if (pressed & PAD_RIGHT)
            watches[watchId].posX = (watches[watchId].posX + speed) % SCREEN_BOT_WIDTH;
        if (pressed & PAD_LEFT)
            watches[watchId].posX = (watches[watchId].posX + SCREEN_BOT_WIDTH - speed) % SCREEN_BOT_WIDTH;
        if (pressed & PAD_DOWN)
            watches[watchId].posY = (watches[watchId].posY + speed) % SCREEN_BOT_HEIGHT;
        if (pressed & PAD_UP)
            watches[watchId].posY = (watches[watchId].posY + SCREEN_BOT_HEIGHT - speed) % SCREEN_BOT_HEIGHT;
    } while (onMenuLoop());

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();
}

static void Watches_EditWatch(s32 selected) {
    s32 selectedItem = 0;

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();

    do
    {
        Draw_Lock();
        Draw_DrawString(10, 10, COLOR_TITLE, "Watch Edit");

        Draw_DrawFormattedString(30, 30 + WATCHEDIT_NAME * SPACING_Y, COLOR_WHITE,
                                    "Name:  %s", watches[selected].name);
        Draw_DrawFormattedString(30, 30 + WATCHEDIT_TYPE * SPACING_Y, COLOR_WHITE,
                                    "Type:  < %s >", WatchTypeNames[watches[selected].type]);
        Draw_DrawFormattedString(30, 30 + WATCHEDIT_ADDR * SPACING_Y, COLOR_WHITE,
                                    "Addr:  0x%08X", watches[selected].addr);
        Draw_DrawFormattedString(30, 30 + WATCHEDIT_POS * SPACING_Y, COLOR_WHITE,
                                    "Pos :  x=%03d  y=%03d", watches[selected].posX, watches[selected].posY);
        Draw_DrawFormattedString(30, 30 + WATCHEDIT_DISPLAY * SPACING_Y, COLOR_WHITE,
                                    "Draw:  %s", watches[selected].display ? "ON " : "OFF");

        Draw_DrawString(10, SCREEN_BOT_HEIGHT - 20, COLOR_TITLE, "Y: Select address from Memory Editor");

        // Cursor
        for (u32 i = 0; i < WATCHEDIT_MAX; i++) {
            Draw_DrawCharacter(10, 30 + i * SPACING_Y, COLOR_TITLE, selectedItem == i ? '>' : ' ');
        }

        Draw_FlushFramebuffer();
        Draw_Unlock();

        u32 pressed = Input_WaitWithTimeout(1000);
        void* prevAddr = watches[selected].addr;

        if(pressed & BUTTON_B)
            break;
        else if(pressed & BUTTON_A){
            if (selectedItem == WATCHEDIT_NAME)
                Watches_EditName(watches[selected].name);
            else if(selectedItem == WATCHEDIT_ADDR)
                Menu_EditAmount(66, 30 + selectedItem * SPACING_Y, &watches[selected].addr, VARTYPE_U32, 0, 0, 8, true, NULL, 0);
            else if(selectedItem == WATCHEDIT_POS)
                Watches_EditPos(selected);
            else if(selectedItem == WATCHEDIT_DISPLAY)
                watches[selected].display ^= 1;
        }
        else if(pressed & BUTTON_Y){
            Debug_MemoryEditor();
            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            Draw_Unlock();
            watches[selected].addr = (void*)MemoryEditor_GetSelectedByteAddress();
        }
        else if (pressed & BUTTON_L1)
            selectedItem = 0;
        else if(pressed & PAD_UP)
            selectedItem = (selectedItem + WATCHEDIT_MAX - 1) % WATCHEDIT_MAX;
        else if(pressed & PAD_DOWN)
            selectedItem = (selectedItem + 1) % WATCHEDIT_MAX;
        else if(selectedItem == WATCHEDIT_TYPE && pressed & PAD_LEFT)
            watches[selected].type = (watches[selected].type + WATCHTYPE_MAX - 1) % WATCHTYPE_MAX;
        else if(selectedItem == WATCHEDIT_TYPE && pressed & PAD_RIGHT)
            watches[selected].type = (watches[selected].type + 1) % WATCHTYPE_MAX;

        // Init position and enable this watch the first time the address is set
        if (prevAddr == NULL && watches[selected].addr != NULL && watches[selected].posX == 0 &&
            watches[selected].posY == 0 && watches[selected].display == FALSE
        ) {
            watches[selected].posX = 70;
            watches[selected].posY = 40 + selected * SPACING_Y;
            watches[selected].display = TRUE;
        }
    } while(onMenuLoop());

    if (watches[selected].addr == NULL){
        watches[selected].display = 0;
    }
}

static void Watches_DeleteWatch(u32 selected) {
    watches[selected].type = WATCHTYPE_S8;
    watches[selected].posX = 0;
    watches[selected].posY = 0;
    watches[selected].addr = NULL;
    watches[selected].display = 0;
    memset(watches[selected].name, 0, WATCHES_MAXNAME + 1);
}

void Watches_ShowWatchesMenu(void) {

    static s32 selected = 0;
    s32 toDelete = -1;
    s32 toSwap   = -1;

    if (ToggleSettingsMenu.items[TOGGLESETTINGS_REMEMBER_CURSOR_POSITION].on == 0) {
        selected = 0;
    }

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();

    do
    {
        Draw_Lock();
        Draw_DrawString(10, 10, COLOR_TITLE, "Watches");

        for (u32 i = 0; i < WATCHES_MAX; ++i){
            u32 color = (i == toDelete ? COLOR_RED : (i == toSwap ? COLOR_GREEN : COLOR_WHITE));
            Draw_DrawString(30, 30 + i * SPACING_Y, color, watches[i].addr == NULL ? "." : watches[i].name);
            Draw_DrawCharacter(10, 30 + i * SPACING_Y, COLOR_TITLE, selected == i ? '>' : ' ');
            if (watches[i].addr != NULL) {
                Draw_DrawFormattedString(90, 30 + i * SPACING_Y, color, "%08X", watches[i].addr);
                Draw_DrawFormattedString(150, 30 + i * SPACING_Y, color, watches[i].display ? "ON" : "OFF");
            }
        }

        Draw_DrawString(10, SCREEN_BOT_HEIGHT - 20, COLOR_TITLE, "A: Edit  X: Delete  Y: Go to Memory Editor  R: Swap");
        Draw_FlushFramebuffer();
        Draw_Unlock();

        u32 pressed = Input_WaitWithTimeout(1000);
        if(pressed & BUTTON_B)
            break;
        else if(pressed & BUTTON_A){
            Watches_EditWatch(selected);
            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            Draw_Unlock();
        }
        else if(pressed & BUTTON_X){
            toSwap = -1;
            if (toDelete == selected) {
                Watches_DeleteWatch(selected);
                Draw_Lock();
                Draw_ClearFramebuffer();
                Draw_FlushFramebuffer();
                Draw_Unlock();
                toDelete = -1;
            }
            else {
                toDelete = selected;
            }
        }
        else if(pressed & BUTTON_Y){
            pushHistory(memoryEditorAddress);
            memoryEditorAddress = (int)watches[selected].addr;
            Debug_MemoryEditor();
            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            Draw_Unlock();
        }
        else if(pressed & BUTTON_R1){
            toDelete = -1;
            if (toSwap == selected){
                toSwap = -1;
            }
            else if (toSwap >= 0) {
                // Swap watches in list
                Watch tempWatch = watches[toSwap];
                watches[toSwap] = watches[selected];
                watches[selected] = tempWatch;
                // Swap positions
                u32 tempPos = watches[toSwap].posX;
                watches[toSwap].posX = watches[selected].posX;
                watches[selected].posX = tempPos;
                tempPos = watches[toSwap].posY;
                watches[toSwap].posY = watches[selected].posY;
                watches[selected].posY = tempPos;
                Draw_Lock();
                Draw_ClearFramebuffer();
                Draw_FlushFramebuffer();
                Draw_Unlock();
                toSwap = -1;
            }
            else {
                toSwap = selected;
            }
        }
        else if (pressed & BUTTON_L1) {
            selected = 0;
        }
        else if(pressed & PAD_UP){
            selected--;
        }
        else if(pressed & PAD_DOWN){
            selected++;
        }

        if(selected >= WATCHES_MAX) selected = 0;
        if(selected < 0) selected = WATCHES_MAX - 1;

    } while(onMenuLoop());
}

static void Watches_DrawWatch(Watch watch, u32 color) {
    if (!watch.display) {
        return;
    }

    #define DrawThisWatchWithFormat(format, type) \
        (Draw_DrawFormattedString(watch.posX, watch.posY, color, "%s: "format, watch.name, *(type*)watch.addr))

    // Skip attempting to draw the address if it would otherwise be an invalid read.
    // Attempting to read these locations would crash the game.
    const MemInfo address_info = query_memory_permissions((int)watch.addr);
    if (!is_valid_memory_read(&address_info)) {
        Draw_DrawFormattedString(watch.posX, watch.posY, color, "%s: Invalid address", watch.name);
        return;
    }

    switch(watch.type) {
        case(WATCHTYPE_S8):
            DrawThisWatchWithFormat("%03d", s8);
            break;
        case(WATCHTYPE_U8):
            DrawThisWatchWithFormat("%03u", u8);
            break;
        case(WATCHTYPE_X8):
            DrawThisWatchWithFormat("%02X", u8);
            break;
        case(WATCHTYPE_S16):
            DrawThisWatchWithFormat("%05d", s16);
            break;
        case(WATCHTYPE_U16):
            DrawThisWatchWithFormat("%05u", u16);
            break;
        case(WATCHTYPE_X16):
            DrawThisWatchWithFormat("%04X", u16);
            break;
        case(WATCHTYPE_S32):
            DrawThisWatchWithFormat("%010d", s32);
            break;
        case(WATCHTYPE_U32):
            DrawThisWatchWithFormat("%010u", u32);
            break;
        default:
        case(WATCHTYPE_X32):
            DrawThisWatchWithFormat("%08X", u32);
            break;
        case(WATCHTYPE_F32):
            DrawThisWatchWithFormat("%05.2F", f32);
            break;
    }

    #undef DrawThisWatchWithFormat
}

void Watches_DrawWatches(void) {
    for(u32 i = 0; i < WATCHES_MAX; ++i) {
        Watches_DrawWatch(watches[i], COLOR_WHITE);
    }
    Draw_FlushFramebuffer();
}
