#include "menu.h"
#include "menus/file.h"
#include "z3D/z3D.h"
#include "draw.h"

static char* Timer1States[] = {
    "Timer 1 State: inactive      ",
    "Timer 1 State: heat starting ",
    "Timer 1 State: heat initial  ",
    "Timer 1 State: heat moving   ",
    "Timer 1 State: heat active   ",
    "Timer 1 State: race starting ",
    "Timer 1 State: race initial  ",
    "Timer 1 State: race moving   ",
    "Timer 1 State: race active   ",
    "Timer 1 State: race stopped  ",
    "Timer 1 State: race ending   ",
    "Timer 1 State: timer starting",
    "Timer 1 State: timer initial ",
    "Timer 1 State: timer moving  ",
    "Timer 1 State: timer active  ",
    "Timer 1 State: timer stopped ",
};

static char* Timer2States[] = {
    "Timer 2 State: inactive      ",
    "Timer 2 State: starting      ",
    "Timer 2 State: initial       ",
    "Timer 2 State: moving        ",
    "Timer 2 State: active        ",
    "Timer 2 State: stopped       ",
    "Timer 2 State: ending        ",
    "Timer 2 State: timer starting",
    "Timer 2 State: timer initial ",
    "Timer 2 State: timer moving  ",
    "Timer 2 State: timer active  ",
    "Timer 2 State: timer stopped ",
};

ToggleMenu FileMenu = {
    "File",
    .nbItems = 9,
    .initialCursorPos = 0,
    {
        {0, "Gold Skulltulas defeated", .method = File_ToggleSkulltulaFlags },
        {0, "Seeds and Magic drops obtained", .method = File_ToggleItemDropsFlags },
        {0, "Call Navi", .method = File_CallNavi },
        {0, "Epona Freed", .method = File_ToggleEponaFreed },
        {0, "Carpenters Freed", .method = File_ToggleCarpentersFreed },
        {0, "Intro Cutscenes", .method = File_ToggleIntroCutscenes },
        {0, "Blue Warps Cleared", .method = File_ToggleBlueWarps },
        {0, "Timers", .method = File_ShowTimersMenu },
        {0, "Master Quest", .method = File_ToggleMasterQuest },
    }
};

AmountMenu TimersMenu = {
    "Timers",
    .nbItems = 4,
    .initialCursorPos = 0,
    {
        {.amount = 0, .isSigned = false, .min = 0, .max = 15, .nDigits = 2, .hex = false,
            .title = "Timer 1 State", .method = File_SetTimerStateAndValue },
        {.amount = 0, .isSigned = true,  .min = 0, .max =  0, .nDigits = 5, .hex = false,
            .title = "Timer 1 Value", .method = File_SetTimerStateAndValue },
        {.amount = 0, .isSigned = false, .min = 0, .max = 11, .nDigits = 2, .hex = false,
            .title = "Timer 2 State", .method = File_SetTimerStateAndValue },
        {.amount = 0, .isSigned = true,  .min = 0, .max =  0, .nDigits = 5, .hex = false,
            .title = "Timer 2 Value", .method = File_SetTimerStateAndValue },
    }
};

void File_FileMenuInit(void) {
    s32 skulltulasDefeated = 0;
    for (u32 i = 0; i < 22; i++) {
        skulltulasDefeated += gSaveContext.gsFlags[i];
    }
    FileMenu.items[FILE_GOLD_SKULLTULAS].on = (skulltulasDefeated == 0x45A);
    FileMenu.items[FILE_ITEM_DROPS].on = (gSaveContext.infTable[0x19] & 0x0100) && (gSaveContext.itemGetInf[1] & 0x0008);
    FileMenu.items[FILE_CALL_NAVI].on = (gSaveContext.naviTimer >= 2000);
    FileMenu.items[FILE_EPONA_FREED].on = (gSaveContext.eventChkInf[0x1] & 0x0100) == 0x0100;
    FileMenu.items[FILE_CARPENTERS_FREED].on = ((gSaveContext.eventChkInf[0x9] & 0x000F) == 0x000F);
    FileMenu.items[FILE_INTRO_CUTSCENES].on = ((gSaveContext.eventChkInf[0xA] & 0x01FB) +
                                              (gSaveContext.eventChkInf[0xB] & 0x07FE) +
                                              (gSaveContext.eventChkInf[0xC] & 0x8081) == 0x01FB + 0x07FE + 0x8081);
    FileMenu.items[FILE_BLUE_WARPS_CLEARED].on = ((gSaveContext.eventChkInf[0x0] & 0x0080) +
                                                 (gSaveContext.eventChkInf[0x2] & 0x0020) +
                                                 (gSaveContext.eventChkInf[0x3] & 0x0080) +
                                                 (gSaveContext.eventChkInf[0x4] & 0x0700) == 0x0080 + 0x0020 + 0x0080 + 0x0700);
    FileMenu.items[FILE_TIMERS].on = (gSaveContext.timer1State != 0) || (gSaveContext.timer2State != 0);
    FileMenu.items[FILE_MASTER_QUEST].on = gSaveContext.masterQuestFlag;
}

void File_ShowFileMenu() {
    File_FileMenuInit();
    ToggleMenuShow(&FileMenu);
}

void File_ToggleSkulltulaFlags(s32 selected) {
    if (FileMenu.items[selected].on) {
        for (u32 i = 0; i < 22; i++) {
            gSaveContext.gsFlags[i] = 0;
        }
        FileMenu.items[selected].on = 0;
    }
    else {
        static u8 areaGsFlags[] = {
            0x0F, 0x1F, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x07, 0x07, 0x03,
            0x0F, 0x07, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0x1F, 0x0F, 0x03, 0x0F,
        };
        for (u32 i = 0; i < 22; i++) {
            gSaveContext.gsFlags[i] = areaGsFlags[i];
        }
        FileMenu.items[selected].on = 1;
    }
}

void File_ToggleItemDropsFlags(s32 selected) {
    if (FileMenu.items[selected].on) {
        gSaveContext.infTable[0x19] &= ~0x0100; //magic
        gSaveContext.itemGetInf[1] &= ~0x0008; //deku seeds
        FileMenu.items[selected].on = 0;
    }
    else {
        gSaveContext.infTable[0x19] |= 0x0100;
        gSaveContext.itemGetInf[0x1] |= 0x0008;
        FileMenu.items[selected].on = 1;
    }
}

void File_CallNavi(s32 selected) {
    if (FileMenu.items[selected].on) {
        gSaveContext.naviTimer = 0;
        FileMenu.items[selected].on = 0;
    }
    else {
        gSaveContext.naviTimer = 2000;
        FileMenu.items[selected].on = 1;
    }
}

void File_ToggleEponaFreed(s32 selected) {
    if (FileMenu.items[selected].on) {
        gSaveContext.eventChkInf[0x1] &= ~0x0100;
        FileMenu.items[selected].on = 0;
    }
    else {
        gSaveContext.eventChkInf[0x1] |= 0x0100;
        FileMenu.items[selected].on = 1;
    }
}

void File_ToggleCarpentersFreed(s32 selected) {
    if (FileMenu.items[selected].on) {
        gSaveContext.eventChkInf[0x9] &= ~0x000F;
        FileMenu.items[selected].on = 0;
    }
    else {
        gSaveContext.eventChkInf[0x9] |= 0x000F;
        FileMenu.items[selected].on = 1;
    }
}

void File_ToggleIntroCutscenes(s32 selected) {
    if (FileMenu.items[selected].on) {
        gSaveContext.eventChkInf[0xA] &= ~0x01FB;
        gSaveContext.eventChkInf[0xB] &= ~0x07FE;
        gSaveContext.eventChkInf[0xC] &= ~0x8081;
        FileMenu.items[selected].on = 0;
    }
    else {
        gSaveContext.eventChkInf[0xA] |= 0x01FB;
        gSaveContext.eventChkInf[0xB] |= 0x07FE;
        gSaveContext.eventChkInf[0xC] |= 0x8081;
        FileMenu.items[selected].on = 1;
    }
}

void File_ToggleBlueWarps(s32 selected) {
    if (FileMenu.items[selected].on) {
        gSaveContext.eventChkInf[0x0] &= ~0x0080;
        gSaveContext.eventChkInf[0x2] &= ~0x0020;
        gSaveContext.eventChkInf[0x3] &= ~0x0080;
        gSaveContext.eventChkInf[0x4] &= ~0x0700;
        FileMenu.items[selected].on = 0;
    }
    else {
        gSaveContext.eventChkInf[0x0] |= 0x0080;
        gSaveContext.eventChkInf[0x2] |= 0x0020;
        gSaveContext.eventChkInf[0x3] |= 0x0080;
        gSaveContext.eventChkInf[0x4] |= 0x0700;
        FileMenu.items[selected].on = 1;
    }
}

void File_ToggleMasterQuest(s32 selected) {
    gSaveContext.masterQuestFlag = FileMenu.items[selected].on = !FileMenu.items[selected].on;
    WriteDungeonSceneTable();
}

void File_TimersMenuInit(void) {
    TimersMenu.items[FILE_TIMER1STATE].amount = gSaveContext.timer1State;
    TimersMenu.items[FILE_TIMER1VALUE].amount = gSaveContext.timer1Value;
    TimersMenu.items[FILE_TIMER2STATE].amount = gSaveContext.timer2State;
    TimersMenu.items[FILE_TIMER2VALUE].amount = gSaveContext.timer2Value;
    TimersMenu.items[FILE_TIMER1STATE].title = Timer1States[gSaveContext.timer1State];
    TimersMenu.items[FILE_TIMER2STATE].title = Timer2States[gSaveContext.timer2State];
}

void File_ShowTimersMenu(s32 selected) {
    File_TimersMenuInit();
    AmountMenuShow(&TimersMenu);
}

void File_SetTimerStateAndValue(s32 selected) {
    //set variable in save context
    s16* varToSet;
    switch (selected) {
        case FILE_TIMER1STATE: varToSet = &gSaveContext.timer1State; break;
        case FILE_TIMER1VALUE: varToSet = &gSaveContext.timer1Value; break;
        case FILE_TIMER2STATE: varToSet = &gSaveContext.timer2State; break;
        case FILE_TIMER2VALUE: varToSet = &gSaveContext.timer2Value; break;
    }
    *(varToSet) = TimersMenu.items[selected].amount;
    //reset toggle in menu
    FileMenu.items[FILE_TIMERS].on = (gSaveContext.timer1State != 0) || (gSaveContext.timer2State != 0);
    //set option titles to display state names
    TimersMenu.items[FILE_TIMER1STATE].title = Timer1States[gSaveContext.timer1State];
    TimersMenu.items[FILE_TIMER2STATE].title = Timer2States[gSaveContext.timer2State];

    if (selected == FILE_TIMER1STATE) {
        Draw_DrawFormattedString(70, 30 + (FILE_TIMER1STATE % AMOUNT_MENU_MAX_SHOW) * SPACING_Y,
                                COLOR_WHITE, Timer1States[gSaveContext.timer1State]);
    } else if (selected == FILE_TIMER2STATE) {
        Draw_DrawFormattedString(70, 30 + (FILE_TIMER2STATE % AMOUNT_MENU_MAX_SHOW) * SPACING_Y,
                                COLOR_WHITE, Timer2States[gSaveContext.timer2State]);
    }
}
