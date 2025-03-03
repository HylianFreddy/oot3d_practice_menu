#include "enemizer.h"

s32 rSceneLayer;

void Scene_GetLoadedLayer(void** altHeaders) {
    if (altHeaders[gSaveContext.sceneSetupIndex - 1] != NULL) {
        rSceneLayer = gSaveContext.sceneSetupIndex;
    } else if (gSaveContext.sceneSetupIndex == 3 && altHeaders[1] != NULL) {
        rSceneLayer = 2;
    } else {
        rSceneLayer = 0;
    }
}

// Return true to skip spawning this actor entry
u8 ActorSetup_OverrideEntry(ActorEntry* actorEntry, s32 actorEntryIndex) {
    if (!gInit) {
        return FALSE;
    }
    u16 locationActor = enemyLocations[gGlobalContext->sceneNum][rSceneLayer][(u8)gGlobalContext->roomCtx.curRoom.num]
                                      [(u8)actorEntryIndex];
    EnemyType foundEnemy = { "", 0 };

    for (u32 i = 0; i < ARRAY_SIZE(enemyTypes); i++) {
        if (actorEntry->id == enemyTypes[i].actorId &&
            (actorEntry->id != ACTOR_ARMOS || actorEntry->params == 0xFFFF)) {
            foundEnemy = enemyTypes[i];
        }
    }

    if (foundEnemy.actorId != 0 && locationActor == 0) {
        CitraPrint("Missing location: %d %d %d %d, should be 0x%X (%s)", gGlobalContext->sceneNum, rSceneLayer,
                   (u8)gGlobalContext->roomCtx.curRoom.num, (u8)actorEntryIndex, foundEnemy.actorId, foundEnemy.name);
    } else if (foundEnemy.actorId != 0 && locationActor != actorEntry->id) {
        CitraPrint("Wrong actor at location: %d %d %d %d, is 0x%X (%s), should be 0x%X (%s)", gGlobalContext->sceneNum,
                   rSceneLayer, (u8)gGlobalContext->roomCtx.curRoom.num, (u8)actorEntryIndex, foundEnemy.actorId,
                   foundEnemy.name, locationActor);
    } else if (foundEnemy.actorId == 0 && locationActor != 0) {
        CitraPrint("Bad Location (not an enemy) at: %d %d %d %d, actorId=0x%X params=0x%X", gGlobalContext->sceneNum,
                   rSceneLayer, (u8)gGlobalContext->roomCtx.curRoom.num, (u8)actorEntryIndex, actorEntry->id,
                   actorEntry->params);
    }

    return FALSE;
}

static s32 sSceneRestart = 0;
void Enemizer_ChangeRoom(void) {
    u8 roomToLoad;
    if (gGlobalContext->roomCtx.prevRoom.num != -1) {
        setAlert("roomCtx busy!", 90);
        return;
    }

    if (sSceneRestart && gGlobalContext->roomCtx.curRoom.num != 0) {
        roomToLoad    = 0;
    } else {
        roomToLoad = gGlobalContext->roomCtx.curRoom.num + 1;
    }
    sSceneRestart = 0;

    if (roomToLoad == gGlobalContext->numRooms) {
        setAlert("All rooms checked!", 90);
        return;
    }

    Room_StartTransition(gGlobalContext, &gGlobalContext->roomCtx, roomToLoad);
    Room_ClearPrevRoom(gGlobalContext, &gGlobalContext->roomCtx);

    static char message[128];
    snprintf(message, 128, "Loaded Room %d", roomToLoad);
    setAlert(message, 90);
}

void Enemizer_OnPlayDestroy(void) {
    CitraPrint("________________");
    rSceneLayer   = 0;
    sSceneRestart = 1;
}
