#include "menu.h"
#include "menus/debug.h"
#include "menus/commands.h"
#include "menus/watches.h"
#include "draw.h"
#include "input.h"
#include "common.h"
#include "loader.h"
#include "z3D/z3D.h"
#include <stdio.h>
#include <string.h>


#define ACTOR_LIST_MAX_SHOW 15

//new actor values
static s16 newId = 0x0010;
static s16 newParams = 0x0000;
static s16 storedPosRotIndex = -1;

//Memory Editor values
u32 memoryEditorAddress = (int)&gSaveContext;
static s32 selectedRow = 0;
static s32 selectedColumn = 0;
static u8  isValidMemory = 0;
static u32 storedTableStart = 0;
static u16 tableElementSize = 0;
static u8  tableIndexType = TABLEINDEX_U8;
static s32 tableIndex = 0;
static char tableIndexSign;
static u16 tableIndexAbs;

typedef struct {
    Actor* instance;
    s16    id;
    s16    params;
} ShowActor_Info;

static const char* const ActorTypeNames[] = {
    "SWITCH", //0x0
    "BG",
    "PLAYER",
    "EXPLOSIVES",
    "NPC",
    "ENEMY",
    "PROP",
    "ITEM/ACTION",
    "MISC",
    "BOSS",
    "DOOR",
    "CHEST",
    "ALL", //0xC
};

static const char* const FlagGroupNames[] = {
    "switch", //0x0
    "temp_switch",
    "unknown_1",
    "unknown_2",
    "chest",
    "clear",
    "temp_clear",
    "collect",
    "temp_collect",
    "gold_skulltulas",
    "event_chk_inf",
    "item_get_inf",
    "inf_table (part 1/2)",
    "inf_table (part 2/2)",
    "event_inf", // 0xE
};

Menu DebugMenu = {
    "Debug",
    .nbItems = 5,
    .initialCursorPos = 0,
    {
        {"Objects", METHOD, .method = Debug_ShowObjects},
        {"Actors", METHOD, .method = DebugActors_ShowActors},
        {"Flags", METHOD, .method = Debug_FlagsEditor},
        {"Player States", METHOD, .method = Debug_PlayerStatesMenuShow},
        {"Memory Editor", METHOD, .method = Debug_MemoryEditor},
    }
};

/* give type 0xC for "all" */
static s32 PopulateActorList(ShowActor_Info* list, ActorType type) {
    s32 i = 0;
    ActorHeapNode* cur = (ActorHeapNode*)((u32)PLAYER - sizeof(ActorHeapNode));
    while (cur){
        if (!cur->free){
            Actor* actor = (Actor*)((u32)cur + sizeof(ActorHeapNode));
            if (type == 0xC || actor->type == type){
                list[i].instance = actor;
                list[i].id = actor->id;
                list[i].params = actor->params;
                ++i;
            }
        }
        cur = cur->next;
    }
    return i;
}

static void DebugActors_ShowMoreInfo(Actor* actor) {
    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();

    do
    {
        Draw_Lock();
        Draw_DrawString(10, 10, COLOR_TITLE, "Actor Details");
        Draw_DrawFormattedString(30, 30, COLOR_WHITE, "ID:              %04X", actor->id);
        Draw_DrawFormattedString(30, 30 + SPACING_Y, COLOR_WHITE, "Type:            %s", ActorTypeNames[actor->type]);
        Draw_DrawFormattedString(30, 30 + 2 * SPACING_Y, COLOR_WHITE, "Params:          %04X", actor->params & 0xFFFF);
        Draw_DrawFormattedString(30, 30 + 3 * SPACING_Y, COLOR_WHITE, "Pos:             x:%05.2f  y:%05.2f  z:%05.2f", actor->world.pos.x, actor->world.pos.y, actor->world.pos.z);
        Draw_DrawFormattedString(30, 30 + 4 * SPACING_Y, COLOR_WHITE, "Rot:             x:%04X  y:%04X  z:%04X", actor->world.rot.x & 0xFFFF, actor->world.rot.y & 0xFFFF, actor->world.rot.z & 0xFFFF);
        Draw_DrawFormattedString(30, 30 + 5 * SPACING_Y, COLOR_WHITE, "Vel:             x:%05.2f  y:%05.2f  z:%05.2f", actor->velocity.x, actor->velocity.y, actor->velocity.z);
        Draw_DrawFormattedString(30, 30 + 6 * SPACING_Y, COLOR_WHITE, "Floor:           %08X", actor->floorPoly);
        Draw_DrawFormattedString(30, 30 + 7 * SPACING_Y, COLOR_WHITE, "Dist. from Link: xz:%05.2f  y:%05.2f", actor->xyzDistToPlayerSq, actor->xzDistToPlayer);
        Draw_DrawFormattedString(30, 30 + 8 * SPACING_Y, COLOR_WHITE, "Text ID:         %04X", actor->textId & 0xFFFF);
        Draw_DrawFormattedString(30, 30 + 9 * SPACING_Y, COLOR_WHITE, "Parent:          %08X", actor->parent);
        Draw_DrawFormattedString(30, 30 + 10 * SPACING_Y, COLOR_WHITE, "Child:           %08X", actor->child);

        Draw_DrawString(10, SCREEN_BOT_HEIGHT - 40, COLOR_TITLE, "Press Y to open Memory Editor");
        Draw_DrawString(10, SCREEN_BOT_HEIGHT - 30, COLOR_TITLE, "Press START to bring this actor to Link");
        Draw_DrawString(10, SCREEN_BOT_HEIGHT - 20, COLOR_TITLE, "Press SELECT to bring Link to this actor");

        Draw_FlushFramebuffer();
        Draw_Unlock();

        u32 pressed = Input_WaitWithTimeout(1000);
        if(pressed & BUTTON_B)
            break;
        else if(pressed & BUTTON_START)
        {
            actor->world.pos = PLAYER->actor.world.pos;
            actor->world.rot = PLAYER->actor.world.rot;
        }
        else if(pressed & BUTTON_SELECT)
        {
            PLAYER->actor.world.pos = actor->world.pos;
            PLAYER->actor.world.rot = actor->world.rot;
            PLAYER->actor.home.pos = actor->world.pos;
            PLAYER->actor.home.rot = actor->world.rot;
        }
        else if(pressed & BUTTON_Y){
            pushHistory(memoryEditorAddress);
            memoryEditorAddress = (int)actor;
            Debug_MemoryEditor();
            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            Draw_Unlock();
        }

    } while(onMenuLoop());
}

static bool DebugActors_SpawnActor(void) {
    PosRot selectedPosRot = storedPosRotIndex < 0 ? PLAYER->actor.world : storedPosRot[storedPosRotIndex];
    s32 selected = 0;
    u32 xCoords[] = {30 + SPACING_X * 3, 100 + SPACING_X * 7, 200 + SPACING_X * 16};
    s16* values[] = {&newId, &newParams, &storedPosRotIndex};
    s32 digitCounts[] = {4, 4, 1};

    do
    {
        Draw_Lock();
        Draw_DrawString(10, 10, COLOR_TITLE, "Spawn new Actor");
        Draw_DrawFormattedString(30, 70, selected == 0 ? COLOR_GREEN : COLOR_WHITE, "ID: 0x%04X", (u16)newId);
        Draw_DrawFormattedString(100, 70, selected == 1 ? COLOR_GREEN : COLOR_WHITE, "Params: 0x%04X", (u16)newParams);
        Draw_DrawFormattedString(200, 70, selected == 2 ? COLOR_GREEN : COLOR_WHITE,
                                 storedPosRotIndex < 0 ? "Position: Link    " : "Position: Stored %01d", storedPosRotIndex);

        Draw_DrawFormattedString(30, 100, COLOR_WHITE, "POS   X: %08.2f", selectedPosRot.pos.x);
        Draw_DrawFormattedString(30 + SPACING_X * 19, 100, COLOR_WHITE, "Y: %08.2f", selectedPosRot.pos.y);
        Draw_DrawFormattedString(30 + SPACING_X * 32, 100, COLOR_WHITE, "Z: %08.2f", selectedPosRot.pos.z);
        Draw_DrawFormattedString(30, 120, COLOR_WHITE, "ROT   X: %04X", (u16)selectedPosRot.rot.x);
        Draw_DrawFormattedString(30 + SPACING_X * 19, 120, COLOR_WHITE, "Y: %04X", (u16)selectedPosRot.rot.y);
        Draw_DrawFormattedString(30 + SPACING_X * 32, 120, COLOR_WHITE, "Z: %04X", (u16)selectedPosRot.rot.z);

        Draw_DrawString(30, 160, COLOR_TITLE, "A: Edit      X: Fetch Position from Link\n"
                                              "B: Cancel    Y: Confirm and Spawn");

        Draw_FlushFramebuffer();
        Draw_Unlock();

        u32 pressed = Input_WaitWithTimeout(1000);

        if (pressed & BUTTON_B) {
            break;
        }
        if (pressed & BUTTON_A) {
            if (selected == 2 && storedPosRotIndex < 0) {
                storedPosRotIndex = 0;
                Draw_DrawString(200, 70, COLOR_GREEN, "Position: Stored  ");
            }
            Menu_EditAmount(xCoords[selected], 70, values[selected], VARTYPE_U16, 0,
                            selected == 2 ? 8 : 0,
                            digitCounts[selected],
                            selected != 2,
                            NULL, 0);
            if (selected == 2) {
                selectedPosRot = storedPosRot[storedPosRotIndex];
            }
        }
        else if ((pressed & BUTTON_X)) {
            storedPosRotIndex = -1;
            selectedPosRot = PLAYER->actor.world;
        }
        else if (pressed & BUTTON_Y) {
            PosRot p = selectedPosRot;
            Actor_Spawn(&(gGlobalContext->actorCtx), gGlobalContext, newId, p.pos.x, p.pos.y, p.pos.z, p.rot.x, p.rot.y, p.rot.z, newParams);
            return true;
        }
        else if (pressed & BUTTON_RIGHT) {
            selected++;
        }
        else if (pressed & BUTTON_LEFT) {
            selected--;
        }

        if(selected > 2)
            selected = 0;
        else if(selected < 0)
            selected = 2;

    } while(onMenuLoop());

    return false;
}

void DebugActors_ShowActors(void) {
    if(!isInGame()) {
        return;
    }
    static ShowActor_Info actorList[200];
    static s32 type = 0xC;
    s32 selected = 0, page = 0, pagePrev = 0;
    s32 listSize = 0;

    listSize = PopulateActorList(actorList, type);

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
        Draw_DrawFormattedString(10, 10, COLOR_TITLE, "Currently Loaded Actors");
        Draw_DrawFormattedString(30, 30, COLOR_RED, "Filter by type (R/L to move): %s", ActorTypeNames[type]);

        for (s32 i = 0; i < ACTOR_LIST_MAX_SHOW && page * ACTOR_LIST_MAX_SHOW + i < listSize; ++i)
        {
            s32 j = page * ACTOR_LIST_MAX_SHOW + i;
            Actor* act = actorList[j].instance;
            u32 curColor = ((act->flags & 0x1) || act->draw != NULL || act->update != NULL) ? COLOR_WHITE : COLOR_GRAY;
            Draw_DrawFormattedString(30, 30 + (1 + i) * SPACING_Y, curColor, "Addr: %08X  ID: %04X", actorList[j].instance, actorList[j].id & 0xFFFF);
            Draw_DrawCharacter(10, 30 + (1 + i) * SPACING_Y, COLOR_TITLE, j == selected ? '>' : ' ');
        }

        Draw_DrawString(10, SCREEN_BOT_HEIGHT - 20, COLOR_TITLE, "A: Details    X: Delete    Start: Spawn new Actor");

        Draw_FlushFramebuffer();
        Draw_Unlock();

        u32 pressed = Input_WaitWithTimeout(1000);
        if(pressed & BUTTON_B)
            break;
        if(pressed & BUTTON_A)
        {
            DebugActors_ShowMoreInfo(actorList[selected].instance);
            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            Draw_Unlock();
        }
        else if(pressed & BUTTON_START)
        {
            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            Draw_Unlock();

            if (DebugActors_SpawnActor()) // true if a new actor has spawned
                listSize = PopulateActorList(actorList, type);

            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            Draw_Unlock();
        }
        else if(pressed & BUTTON_X)
        {
            // prevent accidentally deleting the player actor
            if(actorList[selected].instance->id != 0 || ADDITIONAL_FLAG_BUTTON) {
                Actor_Kill(actorList[selected].instance);
            }
        }
        else if(pressed & BUTTON_Y){
            pushHistory(memoryEditorAddress);
            memoryEditorAddress = (int)actorList[selected].instance;
            Debug_MemoryEditor();
            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            Draw_Unlock();
        }
        else if(pressed & BUTTON_DOWN)
        {
            selected++;
        }
        else if(pressed & BUTTON_UP)
        {
            selected--;
        }
        else if(pressed & BUTTON_LEFT)
        {
            selected -= ACTOR_LIST_MAX_SHOW;
        }
        else if(pressed & BUTTON_RIGHT)
        {
            selected += ACTOR_LIST_MAX_SHOW;
        }
        else if(pressed & BUTTON_R1)
        {
            type++;
            if (type > 0xC) type = 0;
            selected = 0;
            page = 0;
            pagePrev = -1;
            listSize = PopulateActorList(actorList, type);
            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            Draw_Unlock();
            continue;
        }
        else if(pressed & BUTTON_L1)
        {
            type--;
            if (type < 0) type = 0xC;
            selected = 0;
            page = 0;
            pagePrev = -1;
            listSize = PopulateActorList(actorList, type);
            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            Draw_Unlock();
            continue;
        }

        if(selected < 0)
        {
            selected = listSize - 1;
        }
        else if(selected >= listSize)
        {
            selected = 0;
        }

        pagePrev = page;
        page = selected / ACTOR_LIST_MAX_SHOW;

    } while(onMenuLoop());
}

void Debug_ShowObjects(void) {
    static u16 objectId = 0;
    static s8  digitIdx = 0;

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();

    do
    {
        Draw_Lock();
        Draw_DrawFormattedString(10, 10, COLOR_TITLE, "Currently Loaded Objects: %02d/%02d", gGlobalContext->objectCtx.num, OBJECT_EXCHANGE_BANK_MAX);
        Draw_DrawFormattedString(30, 50, COLOR_TITLE, "Object ID: %04X      (Y) Push    (X) Pop", objectId);
        Draw_DrawFormattedString(30 + (14 - digitIdx) * SPACING_X, 50, COLOR_GREEN, "%01X", (objectId >> (digitIdx*4)) & 0xF);

        for (int i = 0; i < gGlobalContext->objectCtx.num; i++) {
            Draw_DrawFormattedString((i % 2 ? 171 : 51), 70 + (i / 2) * SPACING_Y, COLOR_WHITE, "%08X %04X",
                                        &gGlobalContext->objectCtx.status[i], gGlobalContext->objectCtx.status[i].id);
        }
        Draw_FlushFramebuffer();
        Draw_Unlock();

        u32 pressed = Input_WaitWithTimeout(1000);
        if(pressed & BUTTON_B)
            break;
        else if((pressed & BUTTON_Y) && objectId != 0 && gGlobalContext->objectCtx.num < OBJECT_EXCHANGE_BANK_MAX) {
            Object_Spawn(&(gGlobalContext->objectCtx), (s16)objectId);
        }
        else if((pressed & BUTTON_X) && gGlobalContext->objectCtx.num > 0) {
            gGlobalContext->objectCtx.status[--gGlobalContext->objectCtx.num].id = 0;
            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            Draw_Unlock();
        }
        else if(pressed & BUTTON_UP) {
            objectId += (1 << digitIdx*4);
        }
        else if(pressed & BUTTON_DOWN) {
            objectId -= (1 << digitIdx*4);
        }
        else if(pressed & BUTTON_RIGHT) {
            digitIdx--;
        }
        else if(pressed & BUTTON_LEFT) {
            digitIdx++;
        }

        if(digitIdx > 3)
            digitIdx = 0;
        else if(digitIdx < 0)
            digitIdx = 3;

    } while(onMenuLoop());
}

void Debug_FlagsEditor(void) {
    static s32 row = 0;
    static s32 column = 0;
    static s32 group = 10;
    static s32 groupToSelect = 10;
    static u16* flags = (u16*)&gSaveContext.eventChkInf;

    static const u8 RowAmounts[] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 14, 4, 16, 14, 4};

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();

    #define WHITE_OR_BLUE_AT(X,Y) ((row == X && column == Y) ? COLOR_TITLE : COLOR_WHITE)
    #define FLAG_STATUS(X,Y)  (*(flags + X) >> Y & 1)
    #define CURSOR_CHAR 176

    do
    {
        Draw_Lock();
        // Title
        Draw_DrawString(10, 10, COLOR_TITLE, "Flags");
        // Arrows to change group
        Draw_DrawCharacter(70, 30, WHITE_OR_BLUE_AT(0,0), 17);
        Draw_DrawCharacter(90, 30, WHITE_OR_BLUE_AT(0,1), 16);
        // Group name
        Draw_DrawString(120, 30, COLOR_WHITE, FlagGroupNames[group]);
        // Flags column indices
        for (s32 j = 0; j < 16; j++) {
            Draw_DrawFormattedString(70 + j * 2 * SPACING_X, 50, (row > 0 && column == j) ? COLOR_TITLE : COLOR_GRAY, "%X", j);
        }
        // Flags row indices
        for (s32 i = 0; i < RowAmounts[group]; i++) {
            Draw_DrawFormattedString(30, 50 + (i + 1) * SPACING_Y, (row == i + 1) ? COLOR_TITLE : COLOR_GRAY, "%04X", i * 16 + (group == FLAGS_INF_TABLE_2 ? 0x100 : 0));
        }
        // Flags
        for (s32 i = 0; i < RowAmounts[group]; i++) {
            for (s32 j = 0; j < 16; j++) {
                Draw_DrawCharacter(70 + j * 2 * SPACING_X, 50 + (i + 1) * SPACING_Y, FLAG_STATUS(i, j) ? COLOR_GREEN : COLOR_RED, FLAG_STATUS(i, j) ? '1' : '0');
            }
        }
        // Cursor
        if (row > 0) {
            Draw_DrawOverlaidCharacter(70 + column * 2 * SPACING_X, 50 + (row) * SPACING_Y, COLOR_TITLE, CURSOR_CHAR);
        }

        Draw_FlushFramebuffer();
        Draw_Unlock();

        u32 pressed = Input_WaitWithTimeout(1000);

        if (pressed & BUTTON_B){
            break;
        }
        else if (pressed & BUTTON_A){
            if(row != 0) {
                *(flags + row - 1) ^= 1 << column;
            }
            else {
                groupToSelect = group + (column == 0 ? -1 : 1);
            }
        }
        else if (pressed & BUTTON_R1) {
            groupToSelect = group + 1;
        }
        else if (pressed & BUTTON_L1) {
            groupToSelect = group - 1;
        }
        else{
            if (pressed & BUTTON_UP){
                row--;
                if (row == 0 && column > 1) column = 1;
            }
            if (pressed & BUTTON_DOWN){
                row++;
                if (row > RowAmounts[group] && column > 1) column = 1;
            }
            if (pressed & BUTTON_RIGHT){
                column++;
            }
            if (pressed & BUTTON_LEFT){
                column--;
            }
        }

        if (groupToSelect != group) {
            group = groupToSelect;
            if (group > 14) group = 0;
            else if (group < 0) group = 14;
            else if (group == FLAGS_UNK1) group = FLAGS_CHEST;
            else if (group == FLAGS_UNK2) group = FLAGS_TEMP_SWITCH;

            groupToSelect = group;

            switch (group) {
                case FLAGS_GOLD_SKULLTULAS  : flags = (u16*)&gSaveContext.gsFlags; break;
                case FLAGS_EVENT_CHK_INF    : flags = (u16*)&gSaveContext.eventChkInf; break;
                case FLAGS_ITEM_GET_INF     : flags = (u16*)&gSaveContext.itemGetInf; break;
                case FLAGS_INF_TABLE_1      : flags = (u16*)&gSaveContext.infTable; break;
                case FLAGS_INF_TABLE_2      : flags = ((u16*)&gSaveContext.infTable) + 16; break;
                case FLAGS_EVENT_INF        : flags = (u16*)&gSaveContext.eventInf; break;
                default : flags = ((u16*)&(gGlobalContext->actorCtx.flags.swch)) + group * 2; break;
            }

            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_Unlock();
        }

        if(row > RowAmounts[group])
            row = 0;
        else if(row < 0)
            row = RowAmounts[group];

        if(column > 15 || (row == 0 && column > 1))
            column = 0;
        else if(column < 0) {
            column = (row == 0 ? 1 : 15);
        }

    } while(onMenuLoop());

    #undef WHITE_OR_BLUE_AT
}

AmountMenu PlayerStatesMenu = {
    "Player States",
    .nbItems = 6,
    .initialCursorPos = 0,
    {
        {.amount = 0, .isSigned = false, .min = 0, .max =   0, .nDigits = 4, .hex = true,
            .title = "Lock=2000, DownA=0020, ReturnA=0010,...", .method = NULL},
        {.amount = 0, .isSigned = false, .min = 0, .max =   0, .nDigits = 4, .hex = true,
            .title = "LedgeCancel=4000, GID=0400, GJ=0800,...", .method = NULL},
        {.amount = 0, .isSigned = false, .min = 0, .max =   0, .nDigits = 4, .hex = true,
            .title = "Invisible=2000, BlankA=0004,...", .method = NULL},
        {.amount = 0, .isSigned = false, .min = 0, .max =   0, .nDigits = 4, .hex = true,
            .title = "Underwater=0400,...", .method = NULL},
        {.amount = 0, .isSigned = false, .min = 0, .max = 255, .nDigits = 2, .hex = true,
            .title = "stateFlags3", .method = NULL},
        {.amount = 0, .isSigned = false, .min = 0, .max = 255, .nDigits = 3, .hex = false,
            .title = "Held Item ID (simulate QuickDraw)", .method = NULL},
    }
};

void PlayerStatesMenuInit(void) {
    PlayerStatesMenu.items[PLAYERSTATES_PART1].amount = (PLAYER->stateFlags1 >> 0x10) & 0xFFFF;
    PlayerStatesMenu.items[PLAYERSTATES_PART2].amount = PLAYER->stateFlags1 & 0xFFFF;
    PlayerStatesMenu.items[PLAYERSTATES_PART3].amount = (PLAYER->stateFlags2 >> 0x10) & 0xFFFF;
    PlayerStatesMenu.items[PLAYERSTATES_PART4].amount = PLAYER->stateFlags2 & 0xFFFF;
    PlayerStatesMenu.items[PLAYERSTATES_PART5].amount = PLAYER->stateFlags3;
    PlayerStatesMenu.items[PLAYERSTATES_HELD_ITEM].amount = PLAYER->heldItemId;
}

void Debug_PlayerStatesMenuShow(void) {
    if (isInGame()) {
        PlayerStatesMenuInit();
        AmountMenuShow(&PlayerStatesMenu);
        PLAYER->stateFlags1 = (PlayerStatesMenu.items[PLAYERSTATES_PART1].amount << 0x10) | PlayerStatesMenu.items[PLAYERSTATES_PART2].amount;
        PLAYER->stateFlags2 = (PlayerStatesMenu.items[PLAYERSTATES_PART3].amount << 0x10) | PlayerStatesMenu.items[PLAYERSTATES_PART4].amount;
        PLAYER->stateFlags3 = PlayerStatesMenu.items[PLAYERSTATES_PART5].amount;
        PLAYER->heldItemId = PlayerStatesMenu.items[PLAYERSTATES_HELD_ITEM].amount;
        Draw_Lock();
        Draw_ClearFramebuffer();
        Draw_FlushFramebuffer();
        Draw_Unlock();
    }
}

static void checkValidMemory(void) {
    MemInfo address_start_info = query_memory_permissions((int)memoryEditorAddress);
    MemInfo address_end_info = query_memory_permissions((int)memoryEditorAddress + 127);
    isValidMemory = is_valid_memory_read(&address_start_info) && is_valid_memory_read(&address_end_info);
}

static u32 addrHistory[10] = {0};
static s8 addrHistoryTop = -1;

void pushHistory(u32 addr) {
    if (addrHistoryTop >= 9) {
        for (s32 i = 0; i < 10; i++) {
            addrHistory[i] = addrHistory[i+1];
        }
        addrHistory[addrHistoryTop] = addr;
    }
    else
        addrHistory[++addrHistoryTop] = addr;
}
u32 popHistory(void) {
    if (addrHistoryTop < 0)
        return memoryEditorAddress;

    return addrHistory[addrHistoryTop--];
}

void Debug_MemoryEditor(void) {

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();

    checkValidMemory();
    if (!isValidMemory) selectedColumn = selectedRow = 0;

    #define WHITE_OR_GREEN_AT(X,Y) ((selectedRow == X && selectedColumn == Y) ? COLOR_GREEN : COLOR_WHITE)
    #define WHITE_OR_BLUE_AT(X,Y)  ((selectedRow == X && selectedColumn == Y) ? COLOR_TITLE : COLOR_WHITE)

    do
    {
        Draw_Lock();
        // Title
        Draw_DrawString(10, 10, COLOR_TITLE, "Memory Editor");
        // Address selection
        Draw_DrawFormattedString(30, 30, (selectedRow == 0 && selectedColumn == 0) ? COLOR_GREEN : COLOR_WHITE, "%08X", memoryEditorAddress);
        // Scroll buttons
        Draw_DrawCharacter(40, 30 + SPACING_Y, WHITE_OR_BLUE_AT(1,0), 31);
        Draw_DrawCharacter(60, 30 + SPACING_Y, WHITE_OR_BLUE_AT(1,1), 30);
        // Go To Preset button
        Draw_DrawString(90, 30, WHITE_OR_BLUE_AT(0,1), "Go To Preset");
        // Info
        Draw_DrawString(180, 15, COLOR_GRAY, "Start : Info & Settings");
        if (tableElementSize != 0) {
            tableIndexSign = tableIndex < 0 ? '-' : ' ';
            tableIndexAbs = tableIndex < 0 ? -tableIndex : tableIndex;
            Draw_DrawFormattedString(240, 30 + SPACING_Y * 2, COLOR_GRAY, "Table Start\n  %08X\nElement Size\n  %04X\nIndex Type\n  %s\nIndex\n %c%04X",
                                        storedTableStart, tableElementSize, TableIndexTypeNames[tableIndexType], tableIndexSign, tableIndexAbs);
        }
        // Byte index markers
        for (s32 j = 0; j < 8; j++) {
            s32 digit = (j + memoryEditorAddress + ((selectedRow > 1 && selectedRow % 2 == 0) ? 0 : 8)) % 16;
            Draw_DrawFormattedString(90 + j * SPACING_X * 3, 30 + SPACING_Y, (selectedRow > 1 && selectedColumn == j) ? COLOR_TITLE : COLOR_GRAY, "%X", digit);
        }
        // Memory addresses and values
        for (s32 i = 0; i < 16; i++) {
            u32 yPos = 30 + (i + 2) * SPACING_Y;
            Draw_DrawFormattedString(30, yPos, selectedRow == (i+2) ? COLOR_TITLE : COLOR_GRAY, "%08X", memoryEditorAddress + i * 8);
            if (isValidMemory) {
                for (s32 j = 0; j < 8; j++) {
                    u8 dst;
                    memcpy(&dst, (void*)(memoryEditorAddress + i * 8 + j), sizeof(dst));
                    Draw_DrawFormattedString(90 + j * SPACING_X * 3, yPos, WHITE_OR_GREEN_AT(i+2,j), "%02X", dst);
                }
            }
            else {
                Draw_DrawString(120, 30 + 10 * SPACING_Y, COLOR_RED, "Invalid Memory");
            }
        }

        Draw_FlushFramebuffer();
        Draw_Unlock();

        u32 pressed = Input_WaitWithTimeout(1000);

        if (pressed & BUTTON_B){
            if (ADDITIONAL_FLAG_BUTTON) {
                memoryEditorAddress = popHistory();
                checkValidMemory();
                Draw_Lock();
                Draw_ClearFramebuffer();
                Draw_FlushFramebuffer();
                Draw_Unlock();
            }
            else
                break;
        }
        else if (pressed & BUTTON_A){
            if (selectedRow == 0 && selectedColumn == 0) {
                MemoryEditor_EditAddress();
            }
            else if (selectedRow == 0 && selectedColumn == 1) {
                MemoryEditor_GoToPreset();
            }
            else if (selectedRow == 1) {
                u8 amount = ADDITIONAL_FLAG_BUTTON ? 0x80 : 8;
                memoryEditorAddress += (selectedColumn == 0 ? amount : -amount);
                checkValidMemory();
            }
            else if (ADDITIONAL_FLAG_BUTTON) {
                MemoryEditor_FollowPointer();
            }
            else {
                MemoryEditor_EditValue();
            }
        }
        else if (pressed & BUTTON_Y && ADDITIONAL_FLAG_BUTTON) {
            if (selectedRow == 0 && selectedColumn == 0) {
                storedTableStart = memoryEditorAddress;
            }
            else if (selectedRow > 1 && tableElementSize != 0) {
                MemoryEditor_JumpToTableElement();
            }
        }
        else if (pressed & BUTTON_START) {
            MemoryEditor_TableSettings();
        }
        else {
            if (pressed & BUTTON_UP){
                selectedRow--;
                if (selectedRow == 0) selectedColumn = 0;
                if (selectedRow == 1) selectedColumn = 1;
            }
            if (pressed & BUTTON_DOWN){
                selectedRow++;
                if (selectedRow == 2) selectedColumn = 0;
            }
            if (pressed & BUTTON_RIGHT){
                selectedColumn++;
            }
            if (pressed & BUTTON_LEFT){
                selectedColumn--;
            }
            if (pressed & BUTTON_L1) {
                selectedRow = selectedColumn = 0;
            }
        }

        if(selectedRow > 17 || (selectedRow > 1 && !isValidMemory))
            selectedRow = selectedColumn = 0;
        else if(selectedRow < 0) {
            selectedRow = isValidMemory ? 17 : 1;
            selectedColumn = 0;
        }

        if(selectedColumn > 7 || (selectedRow <= 1 && selectedColumn > 1))
            selectedColumn = 0;
        else if(selectedColumn < 0) {
            switch (selectedRow) {
                case 0:
                case 1:
                    selectedColumn = 1; break;
                default:
                    selectedColumn = 7; break;
            }
        }

    } while(onMenuLoop());
}

void MemoryEditor_EditAddress(void) {
    u32 oldAddress = memoryEditorAddress;

    Menu_EditAmount(30 - 3 * SPACING_X, 30, &memoryEditorAddress, VARTYPE_U32, 0, 0, 8, TRUE, NULL, 0);

    if (memoryEditorAddress != oldAddress)
        pushHistory(oldAddress);

    checkValidMemory();

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();
}

void MemoryEditor_EditValue(void) {
    u32 posX = 90 + selectedColumn * SPACING_X * 3;
    u32 posY = 30 + selectedRow * SPACING_Y;
    void* address = (void*)(memoryEditorAddress + (selectedRow - 2) * 8 + selectedColumn);

    u8 value;
    memcpy(&value, address, sizeof(value));

    do
    {
        Draw_Lock();
        Draw_DrawFormattedString(posX, posY, COLOR_RED, "%02X", value);
        Draw_Unlock();

        u32 pressed = Input_WaitWithTimeout(1000);

        if (pressed & (BUTTON_B | BUTTON_A)){
            break;
        }
        else if (pressed & BUTTON_UP){
            value++;
        }
        else if (pressed & BUTTON_DOWN){
            value--;
        }
        else if (pressed & BUTTON_RIGHT){
            value+=0x10;
        }
        else if (pressed & BUTTON_LEFT){
            value-=0x10;
        }

    } while(onMenuLoop());

    MemInfo address_info = query_memory_permissions((int)address);
    if (is_valid_memory_write(&address_info)) {
        memcpy(address, &value, sizeof(value));
    }
    else if (MemoryEditor_ConfirmPermissionOverride()) {
        // hack to allow writing to read-only memory, if the user chooses to do so after being asked
        int truncated_address = (((int)address) & 0xFFFFF000);
        svcControlProcessMemory(getCurrentProcessHandle(), truncated_address, truncated_address, 0x1000, MEMOP_PROT,
                                    MEMPERM_READ | MEMPERM_WRITE | MEMPERM_EXECUTE);

        memcpy(address, &value, sizeof(value));
    }
}

bool MemoryEditor_ConfirmPermissionOverride(void) {
    bool ret = false;

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_DrawString(52, 70, COLOR_RED, "NO WRITE PERMISSION ON THIS ADDRESS!");
    Draw_DrawString(40, 90, COLOR_WHITE, "Do you want to overwrite the permission?");
    Draw_DrawString(76, 120, COLOR_WHITE, "(X) Cancel       (Y) Confirm");
    Draw_DrawString(40, SCREEN_BOT_HEIGHT - 20, COLOR_TITLE, "This will give RWX perms to 0x1000 bytes");
    Draw_Unlock();

    do
    {
        u32 pressed = Input_WaitWithTimeout(1000);
        if (pressed & BUTTON_X)
            break;
        else if (pressed & BUTTON_Y) {
            ret = true;
            break;
        }
    } while(onMenuLoop());

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();
    return ret;
}

void MemoryEditor_GoToPreset(void) {

    static s32 selected = 0;
    static const char* const names[] = {
        "Save Context",
        "Static Context / GameInfo",
        "Global Context / PlayState",
        "Current Scene Segment",
        "Gear Usability Table",
        "Item Usability Table",
        "Actor Overlay Table",
        "Entrance Table",
        "Scene Table",
    };
    const void* const addresses[] = {
        &gSaveContext,
        &gStaticContext,
        gGlobalContext,
        gGlobalContext->sceneSegment,
        gGearUsabilityTable,
        gItemUsabilityTable,
        gActorOverlayTable,
        gEntranceTable,
        gSceneTable,
    };
    const s32 addressesCount = sizeof(addresses)/sizeof(addresses[0]);

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();

    do
    {
        Draw_Lock();
        // Title
        Draw_DrawString(10, 10, COLOR_TITLE, "Preset Memory Addresses");
        // Address presets
        for (s32 j = 0; j < addressesCount; j++) {
            Draw_DrawFormattedString(30, 30 + j * SPACING_Y, (selected == j) ? COLOR_TITLE : COLOR_WHITE, "%08X : %s", addresses[j], names[j]);
            Draw_DrawCharacter(10, 30 + j * SPACING_Y, COLOR_TITLE, (selected == j) ? '>' : ' ');
        }

        Draw_FlushFramebuffer();
        Draw_Unlock();

        u32 pressed = Input_WaitWithTimeout(1000);

        if (pressed & BUTTON_B){
            break;
        }
        else if (pressed & BUTTON_A){
            pushHistory(memoryEditorAddress);
            memoryEditorAddress = (u32)(addresses[selected]);
            break;
        }
        else {
            if (pressed & BUTTON_UP){
                selected--;
            }
            if (pressed & BUTTON_DOWN){
                selected++;
            }
        }

        if (selected > addressesCount - 1)
            selected = 0;

        if (selected < 0)
            selected = addressesCount - 1;

    } while(onMenuLoop());

    checkValidMemory();

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();
}

void MemoryEditor_FollowPointer(void) {
    pushHistory(memoryEditorAddress);
    u32 byteAddress = (memoryEditorAddress + (selectedRow - 2) * 8 + selectedColumn);
    u32 pointerAddress = byteAddress - byteAddress % 4;
    if (pointerAddress >= (u32)gGlobalContext->sceneSegment && pointerAddress < (u32)gGlobalContext->sceneSegment + 0x1000) // Manage segment addresses for the scene file headers
        memoryEditorAddress = (u32)gGlobalContext->sceneSegment + *(u32*)(pointerAddress);
    else
        memoryEditorAddress = *(u32*)pointerAddress;

    checkValidMemory();

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();
}

void MemoryEditor_TableSettings(void) {
    static s32 selected = 0;
    u8 chosen = 0;
    u32 curColor = COLOR_GREEN;

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();

    do
    {
        curColor = chosen ? COLOR_RED : COLOR_GREEN;
        tableIndexSign = tableIndex < 0 ? '-' : ' ';
        tableIndexAbs = tableIndex < 0 ? -tableIndex : tableIndex;

        Draw_Lock();
        // Title
        Draw_DrawString(10, 10, COLOR_TITLE, "Memory Editor Info & Settings");
        // Info
        Draw_DrawString(30, 30, COLOR_WHITE, "R+A : Follow Pointer\n"
                                             "R+B : Go Back\n"
                                             "R+Y on editor address: Store as Table Start\n"
                                             "R+Y on memory value: Jump to Table Element\n"
                                             "R+Y from this menu: Jump to chosen Index");
        // Table Settings
        Draw_DrawFormattedString(30, 120, COLOR_GRAY, "Stored Table Start : %08X", storedTableStart);
        Draw_DrawFormattedString(30, 120 + SPACING_Y, selected == 0 ? curColor : COLOR_WHITE, "Table Element Size : %04X", tableElementSize);
        Draw_DrawFormattedString(30, 120 + SPACING_Y * 2, selected == 1 ? curColor : COLOR_WHITE, "Table Index Type : %s", TableIndexTypeNames[tableIndexType]);
        Draw_DrawFormattedString(30, 120 + SPACING_Y * 3, selected == 2 ? curColor : COLOR_WHITE, "Table Index : %c%04X", tableIndexSign, tableIndexAbs);

        Draw_FlushFramebuffer();
        Draw_Unlock();

        u32 pressed = Input_WaitWithTimeout(1000);

        if (pressed & BUTTON_B) {
            if (chosen)
                chosen = 0;
            else
                break;
        }
        else if (pressed & BUTTON_A) {
            if (selected == 1) {
                tableIndexType++;
            }
            else
                chosen = 1 - chosen;

            if (selected == 2)
                tableIndexType = TABLEINDEX_U16;
        }
        else if ((pressed & BUTTON_L1) && chosen) {
            if (selected == 0)
                tableElementSize = 0;
            else if (selected == 2)
                tableIndex = 0;
        }
        else if (pressed & BUTTON_Y && ADDITIONAL_FLAG_BUTTON) {
            MemoryEditor_JumpToTableElementFromIndex();
            break;
        }
        else if (chosen && selected == 0) {
            if (pressed & BUTTON_UP){
                tableElementSize += pressed & BUTTON_X ? 0x100 : 0x1;
            }
            if (pressed & BUTTON_DOWN){
                tableElementSize -= pressed & BUTTON_X ? 0x100 : 0x1;
            }
            if (pressed & BUTTON_RIGHT){
                tableElementSize += pressed & BUTTON_X ? 0x1000 : 0x10;
            }
            if (pressed & BUTTON_LEFT){
                tableElementSize -= pressed & BUTTON_X ? 0x1000 : 0x10;
            }
        }
        else if (chosen && selected == 2) {
            if (pressed & BUTTON_UP){
                tableIndex += pressed & BUTTON_X ? 0x100 : 0x1;
            }
            if (pressed & BUTTON_DOWN){
                tableIndex -= pressed & BUTTON_X ? 0x100 : 0x1;
            }
            if (pressed & BUTTON_RIGHT){
                tableIndex += pressed & BUTTON_X ? 0x1000 : 0x10;
            }
            if (pressed & BUTTON_LEFT){
                tableIndex -= pressed & BUTTON_X ? 0x1000 : 0x10;
            }
        }
        else {
            if (pressed & BUTTON_DOWN){
                selected++;
                if (selected > 2)
                    selected = 0;
            }
            if (pressed & BUTTON_UP){
                selected--;
                if (selected < 0)
                    selected = 2;
            }
        }

        if (tableIndexType > 3)
            tableIndexType = 0;

        MemoryEditor_BoundTableIndexValue();

    } while(onMenuLoop());

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();
}

void MemoryEditor_JumpToTableElementFromIndex(void) {
    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();

    pushHistory(memoryEditorAddress);
    memoryEditorAddress = storedTableStart + tableIndex * tableElementSize;
    checkValidMemory();
}

void MemoryEditor_JumpToTableElement(void) {
    void* byteAddress = (void*)(memoryEditorAddress + (selectedRow - 2) * 8 + selectedColumn);
    switch (tableIndexType) {
        case TABLEINDEX_U8:
            tableIndex = (*(u8*)byteAddress);
            break;
        case TABLEINDEX_S8:
            tableIndex = (*(s8*)byteAddress);
            break;
        case TABLEINDEX_U16:
            tableIndex = (*(u16*)(byteAddress - (u32)byteAddress % 2));
            break;
        case TABLEINDEX_S16:
            tableIndex = (*(s16*)(byteAddress - (u32)byteAddress % 2));
            break;
    }

    MemoryEditor_JumpToTableElementFromIndex();
}

void MemoryEditor_BoundTableIndexValue(void) {

    if (tableIndex > 0xFFFF)
        tableIndex -= 0x10000;
    else if (tableIndex < 0)
        tableIndex += 0x10000;

    switch (tableIndexType) {
        case TABLEINDEX_U8:
            tableIndex &= 0xFF;
            break;
        case TABLEINDEX_S8:
            tableIndex &= 0xFF;
            if (tableIndex >= 0x80)
                tableIndex -= 0x100;
            break;
        case TABLEINDEX_U16:
            tableIndex &= 0xFFFF;
            break;
        case TABLEINDEX_S16:
            if (tableIndex >= 0x8000)
                tableIndex -= 0x10000;
            break;
    }
}

u32 MemoryEditor_GetSelectedByteAddress(void) {
    u32 offset = selectedRow > 1
        ? ((selectedRow - 2) * 8) + selectedColumn
        : 0;

    return memoryEditorAddress + offset;
}
