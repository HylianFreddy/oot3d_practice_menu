#pragma once

#include "menu.h"

#define WATCHES_MAX 15
#define WATCHES_MAXNAME 6

typedef enum WatchType {
    WATCHTYPE_S8,
    WATCHTYPE_U8,
    WATCHTYPE_X8,
    WATCHTYPE_S16,
    WATCHTYPE_U16,
    WATCHTYPE_X16,
    WATCHTYPE_S32,
    WATCHTYPE_U32,
    WATCHTYPE_X32,
    WATCHTYPE_F32,
    WATCHTYPE_MAX,
} WatchType;

enum WatchEditOption {
    WATCHEDIT_NAME,
    WATCHEDIT_TYPE,
    WATCHEDIT_ADDR,
    WATCHEDIT_POS,
    WATCHEDIT_DISPLAY,
    WATCHEDIT_MAX,
};

typedef struct Watch {
    WatchType type;
    u32 posX;
    u32 posY;
    void* addr;
    u8 display; //whether to display on top screen
    char name[WATCHES_MAXNAME + 1];
} Watch;

extern Watch watches[WATCHES_MAX];
extern bool shouldDrawWatches;

void Watches_ShowWatchesMenu(void);
void Watches_DrawWatches(u32 color);
