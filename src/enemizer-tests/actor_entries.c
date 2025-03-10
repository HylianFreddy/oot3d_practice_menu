#include "enemizer.h"

s32 rSceneLayer;

static const char* actorEnumNames[] = {
    [ACTOR_STALFOS] = "ACTOR_STALFOS",
    [ACTOR_POE] = "ACTOR_POE",
    [ACTOR_OCTOROK] = "ACTOR_OCTOROK",
    [ACTOR_WALLMASTER] = "ACTOR_WALLMASTER",
    [ACTOR_DODONGO] = "ACTOR_DODONGO",
    [ACTOR_KEESE] = "ACTOR_KEESE",
    [ACTOR_TEKTITE] = "ACTOR_TEKTITE",
    [ACTOR_LEEVER] = "ACTOR_LEEVER",
    [ACTOR_PEAHAT] = "ACTOR_PEAHAT",
    [ACTOR_LIZALFOS] = "ACTOR_LIZALFOS",
    [ACTOR_GOHMA_LARVA] = "ACTOR_GOHMA_LARVA",
    [ACTOR_SHABOM] = "ACTOR_SHABOM",
    [ACTOR_BABY_DODONGO] = "ACTOR_BABY_DODONGO",
    [ACTOR_DARK_LINK] = "ACTOR_DARK_LINK",
    [ACTOR_BIRI] = "ACTOR_BIRI",
    [ACTOR_TAILPASARAN] = "ACTOR_TAILPASARAN",
    [ACTOR_SKULLTULA] = "ACTOR_SKULLTULA",
    [ACTOR_TORCH_SLUG] = "ACTOR_TORCH_SLUG",
    [ACTOR_STINGER_FLOOR] = "ACTOR_STINGER_FLOOR",
    [ACTOR_MOBLIN] = "ACTOR_MOBLIN",
    [ACTOR_ARMOS] = "ACTOR_ARMOS",
    [ACTOR_DEKU_BABA] = "ACTOR_DEKU_BABA",
    [ACTOR_MAD_SCRUB] = "ACTOR_MAD_SCRUB",
    [ACTOR_BARI] = "ACTOR_BARI",
    [ACTOR_BUBBLE] = "ACTOR_BUBBLE",
    [ACTOR_FLYING_FLOOR_TILE] = "ACTOR_FLYING_FLOOR_TILE",
    [ACTOR_BEAMOS] = "ACTOR_BEAMOS",
    [ACTOR_FLOORMASTER] = "ACTOR_FLOORMASTER",
    [ACTOR_REDEAD] = "ACTOR_REDEAD",
    [ACTOR_POE_SISTER] = "ACTOR_POE_SISTER",
    [ACTOR_SKULLWALLTULA] = "ACTOR_SKULLWALLTULA",
    [ACTOR_FLARE_DANCER] = "ACTOR_FLARE_DANCER",
    [ACTOR_DEAD_HAND] = "ACTOR_DEAD_HAND",
    [ACTOR_DEAD_HAND_HAND] = "ACTOR_DEAD_HAND_HAND",
    [ACTOR_ENEMY_SPAWNER] = "ACTOR_ENEMY_SPAWNER",
    [ACTOR_SHELL_BLADE] = "ACTOR_SHELL_BLADE",
    [ACTOR_BIG_OCTO] = "ACTOR_BIG_OCTO",
    [ACTOR_WITHERED_DEKU_BABA] = "ACTOR_WITHERED_DEKU_BABA",
    [ACTOR_LIKE_LIKE] = "ACTOR_LIKE_LIKE",
    [ACTOR_PARASITIC_TENTACLE] = "ACTOR_PARASITIC_TENTACLE",
    [ACTOR_ANUBIS] = "ACTOR_ANUBIS",
    [ACTOR_SPIKE] = "ACTOR_SPIKE",
    [ACTOR_ANUBIS_SPAWNER] = "ACTOR_ANUBIS_SPAWNER",
    [ACTOR_IRON_KNUCKLE] = "ACTOR_IRON_KNUCKLE",
    [ACTOR_SKULL_KID] = "ACTOR_SKULL_KID",
    [ACTOR_FLYING_POT] = "ACTOR_FLYING_POT",
    [ACTOR_FREEZARD] = "ACTOR_FREEZARD",
    [ACTOR_STINGER_WATER] = "ACTOR_STINGER_WATER",
    [ACTOR_GERUDO_FIGHTER] = "ACTOR_GERUDO_FIGHTER",
    [ACTOR_WOLFOS] = "ACTOR_WOLFOS",
    [ACTOR_STALCHILD] = "ACTOR_STALCHILD",
    [ACTOR_GUAY] = "ACTOR_GUAY",
};

void Scene_GetLoadedLayer(void** altHeaders) {
    if (altHeaders[gSaveContext.sceneSetupIndex - 1] != NULL) {
        rSceneLayer = gSaveContext.sceneSetupIndex;
    } else if (gSaveContext.sceneSetupIndex == 3 && altHeaders[1] != NULL) {
        rSceneLayer = 2;
    } else {
        rSceneLayer = 0;
    }
    // CitraPrint("rSceneLayer: %X", rSceneLayer);
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
            (actorEntry->id != ACTOR_ARMOS || actorEntry->params == 0xFFFF) &&
            (actorEntry->id != ACTOR_SKULLWALLTULA || (actorEntry->params & 0xE000) == 0)) {
            foundEnemy = enemyTypes[i];
            break;
        }
    }

    if (foundEnemy.actorId != 0 && locationActor == 0) {
        CitraPrint("Missing location: %d %d %d %d, should be 0x%X (%s) params=0x%X", gGlobalContext->sceneNum, rSceneLayer,
                   (u8)gGlobalContext->roomCtx.curRoom.num, (u8)actorEntryIndex, foundEnemy.actorId, foundEnemy.name, actorEntry->params);
    } else if (foundEnemy.actorId != 0 && locationActor != actorEntry->id) {
        CitraPrint("Wrong actor at location: %d %d %d %d, real actor is 0x%X (%s), location is 0x%X", gGlobalContext->sceneNum,
                   rSceneLayer, (u8)gGlobalContext->roomCtx.curRoom.num, (u8)actorEntryIndex, foundEnemy.actorId,
                   foundEnemy.name, locationActor);
    } else if (foundEnemy.actorId == 0 && locationActor != 0) {
        CitraPrint("Bad Location (not an enemy) at: %d %d %d %d, actorId=0x%X params=0x%X", gGlobalContext->sceneNum,
                   rSceneLayer, (u8)gGlobalContext->roomCtx.curRoom.num, (u8)actorEntryIndex, actorEntry->id,
                   actorEntry->params);
    } else if (foundEnemy.actorId != 0) {
        CitraPrint("Location: [%d][%d][%d][%d] %s 0x%X", gGlobalContext->sceneNum, rSceneLayer, (u8)gGlobalContext->roomCtx.curRoom.num,
                    (u8)actorEntryIndex, actorEnumNames[actorEntry->id], actorEntry->params);
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
