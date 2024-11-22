#include "z3D/z3D.h"
#include "actors/player.h"
#include "actors/songs_visual_effects.h"
#include "menus/scene.h"
#include <string.h>

void Actor_Init() {
    memcpy(&vanillaActorInit_Player, gActorOverlayTable[0x0].initInfo, sizeof(ActorInit));

    memcpy(&vanillaActorInit_OceffSpot,  gActorOverlayTable[0x17E].initInfo, sizeof(ActorInit));
    memcpy(&vanillaActorInit_OceffWipe,  gActorOverlayTable[0x18A].initInfo, sizeof(ActorInit));
    memcpy(&vanillaActorInit_OceffStorm, gActorOverlayTable[0x18B].initInfo, sizeof(ActorInit));
    memcpy(&vanillaActorInit_OceffWipe2, gActorOverlayTable[0x198].initInfo, sizeof(ActorInit));
    memcpy(&vanillaActorInit_OceffWipe3, gActorOverlayTable[0x199].initInfo, sizeof(ActorInit));
    memcpy(&vanillaActorInit_OceffWipe4, gActorOverlayTable[0x1CB].initInfo, sizeof(ActorInit));

    gActorOverlayTable[0x0].initInfo->init    = PlayerActor_rInit;
    gActorOverlayTable[0x0].initInfo->update  = PlayerActor_rUpdate;
    gActorOverlayTable[0x0].initInfo->destroy = PlayerActor_rDestroy;
    gActorOverlayTable[0x0].initInfo->draw    = PlayerActor_rDraw;

    gActorOverlayTable[0x17E].initInfo->update = OceffSpot_rUpdate;
    gActorOverlayTable[0x18A].initInfo->update = OceffWipe_rUpdate;
    gActorOverlayTable[0x18B].initInfo->update = OceffStorm_rUpdate;
    gActorOverlayTable[0x198].initInfo->update = OceffWipe2_rUpdate;
    gActorOverlayTable[0x199].initInfo->update = OceffWipe3_rUpdate;
    gActorOverlayTable[0x1CB].initInfo->update = OceffWipe4_rUpdate;
}

void Actor_rDrawContext(GlobalContext *globalCtx,ActorContext *actorCtx) {
    // Collision display is normally handled in Actor_DrawContext, but in order to draw its models even
    // when "Hide Actors" is enabled, we disable it and then call CollisionCheck_DrawCollision manually
    u8 shouldHideActors = HideEntitiesMenu.items[HIDEENTITIES_ACTORS].on;
    u8 exceptLink       = HideEntitiesMenu.items[HIDEENTITIES_EXCEPT_LINK].on;
    u8 delayCollDisplay = shouldHideActors && gStaticContext.showColliders;
    if (delayCollDisplay) {
        gStaticContext.showColliders = 0;
    }

    s32 saModels3DCount_before_actors = gMainClass->sub180.saModels3DCount;
    s32 saModels2DCount_before_actors = gMainClass->sub180.saModels2DCount;

    Actor_DrawContext(globalCtx, actorCtx);

    if (shouldHideActors) {
        // To hide actors, reset the model counts to exclude all the ones added by the various draw functions.
        gMainClass->sub180.saModels3DCount = saModels3DCount_before_actors;
        gMainClass->sub180.saModels2DCount = saModels2DCount_before_actors;

        if (exceptLink && Player_SamPlusUnk != NULL) {
            // Keep the player model by moving it at the beginning of the list (after room models but before BgActor models)
            // and increasing the count by 1.
            gMainClass->sub180.saModels3DList[gMainClass->sub180.saModels3DCount].saModel = Player_SamPlusUnk->saModel;
            gMainClass->sub180.saModels3DList[gMainClass->sub180.saModels3DCount].unk     = Player_SamPlusUnk->unk;
            gMainClass->sub180.saModels3DCount++;
        }
    }
    Player_SamPlusUnk = NULL;

    if (delayCollDisplay) {
        gStaticContext.showColliders = 1;
        CollisionCheck_DrawCollision(globalCtx,&globalCtx->colChkCtx);
    }

    if (haltActors) {
        // Zero out some collision display values so the colliders will
        // keep drawing correctly while actors are halted
        gMainClass->sub32A0.coll3DModelsCount = 0;
        gMainClass->sub32A0.coll2DModelsCount = 0;
    }
}
