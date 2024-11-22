#pragma once

#include "menu.h"

extern ToggleMenu CheatsMenu;

void Cheats_ShowCheatsMenu();
void applyCheats();

typedef enum {
    CHEATS_HEALTH = 0,
    CHEATS_MAGIC,
    CHEATS_STICKS,
    CHEATS_NUTS,
    CHEATS_BOMBS,
    CHEATS_ARROWS,
    CHEATS_SEEDS,
    CHEATS_BOMBCHUS,
    CHEATS_BEANS,
    CHEATS_KEYS,
    CHEATS_RUPEES,
    CHEATS_NAYRUS_LOVE,
    CHEATS_FREEZE_TIME,
    CHEATS_NO_MUSIC,
    CHEATS_USABLE_ITEMS_NORMAL,
    CHEATS_USABLE_ITEMS_FORCED,
    CHEATS_ISG,
    CHEATS_QUICK_TEXT,
    CHEATS_SKIP_SONGS_PLAYBACK,
    CHEATS_FIX_BLACK_SCREEN_GLITCH,
    NUMBER_OF_CHEATS,
} CheatNames;

extern u8  cheats[NUMBER_OF_CHEATS];
extern u16 frozenTime;
