#pragma once

#include "menu.h"

extern Menu DebugMenu;

extern u32 memoryEditorAddress;

void Debug_MemoryEditor(void);
void MemoryEditor_PushHistory(u32 addr);
u32 MemoryEditor_GetSelectedByteAddress(void);

typedef enum {
    FLAGS_SWITCH,
    FLAGS_TEMP_SWITCH,
    FLAGS_UNK1,
    FLAGS_UNK2,
    FLAGS_CHEST,
    FLAGS_CLEAR,
    FLAGS_TEMP_CLEAR,
    FLAGS_COLLECT,
    FLAGS_TEMP_COLLECT,
    FLAGS_GOLD_SKULLTULAS,
    FLAGS_EVENT_CHK_INF,
    FLAGS_ITEM_GET_INF,
    FLAGS_INF_TABLE_1,
    FLAGS_INF_TABLE_2,
    FLAGS_EVENT_INF,
} FlagsGroups;

typedef enum {
    PLAYERSTATES_PART1,
    PLAYERSTATES_PART2,
    PLAYERSTATES_PART3,
    PLAYERSTATES_PART4,
    PLAYERSTATES_PART5,
    PLAYERSTATES_HELD_ITEM,
} PlayerStatesOptions;

typedef enum MemEditorSideInfo {
    SIDEINFO_NOTHING,
    SIDEINFO_CHARACTERS,
    SIDEINFO_TABLE_DATA,
} MemEditorSideInfo;
