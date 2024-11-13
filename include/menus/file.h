#pragma once

#include "menu.h"

void File_ShowFileMenu(void);

typedef enum {
    FILE_GOLD_SKULLTULAS,
    FILE_ITEM_DROPS,
    FILE_CALL_NAVI,
    FILE_EPONA_FREED,
    FILE_CARPENTERS_FREED,
    FILE_INTRO_CUTSCENES,
    FILE_BLUE_WARPS_CLEARED,
    FILE_TIMERS,
    FILE_MASTER_QUEST,
} FileOptions;

typedef enum {
    FILE_TIMER1STATE,
    FILE_TIMER1VALUE,
    FILE_TIMER2STATE,
    FILE_TIMER2VALUE,
} TimersOptions;
