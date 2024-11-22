#include "z3D/z3D.h"
#include "menus/cheats.h"
#include "actors/songs_visual_effects.h"

#if Version_USA || Version_EUR || Version_JPN
    #define OceffSpot_End_addr 0x10FBD0
#elif Version_KOR
    #define OceffSpot_End_addr 0x1C0844
#elif Version_TWN
    #define OceffSpot_End_addr 0x1C0944
#endif
#define OceffSpot_End ((ActorFunc)OceffSpot_End_addr)

ActorInit vanillaActorInit_OceffSpot  = {0};
ActorInit vanillaActorInit_OceffWipe  = {0};
ActorInit vanillaActorInit_OceffStorm = {0};
ActorInit vanillaActorInit_OceffWipe2 = {0};
ActorInit vanillaActorInit_OceffWipe3 = {0};
ActorInit vanillaActorInit_OceffWipe4 = {0};

// Zelda's Lullaby, Song of Time
void OceffWipe_rUpdate(Actor* thisx, GlobalContext* globalCtx) {
    if (!cheats[CHEATS_SKIP_SONGS_PLAYBACK]) {
        vanillaActorInit_OceffWipe.update(thisx, globalCtx);
    } else {
        Actor_Kill(thisx);
    }
}

// Epona's Song
void OceffWipe2_rUpdate(Actor* thisx, GlobalContext* globalCtx) {
    if (!cheats[CHEATS_SKIP_SONGS_PLAYBACK]) {
        vanillaActorInit_OceffWipe2.update(thisx, globalCtx);
    } else {
        Actor_Kill(thisx);
    }
}

// Saria's Song
void OceffWipe3_rUpdate(Actor* thisx, GlobalContext* globalCtx) {
    if (!cheats[CHEATS_SKIP_SONGS_PLAYBACK]) {
        vanillaActorInit_OceffWipe3.update(thisx, globalCtx);
    } else {
        Actor_Kill(thisx);
    }
}

// Scarecrow's Song
void OceffWipe4_rUpdate(Actor* thisx, GlobalContext* globalCtx) {
    if (!cheats[CHEATS_SKIP_SONGS_PLAYBACK]) {
        vanillaActorInit_OceffWipe4.update(thisx, globalCtx);
    } else {
        Actor_Kill(thisx);
    }
}

// Sun's Song
void OceffSpot_rUpdate(Actor* thisx, GlobalContext* globalCtx) {
    if (!cheats[CHEATS_SKIP_SONGS_PLAYBACK]) {
        vanillaActorInit_OceffSpot.update(thisx, globalCtx);
    }
    // Wait for a bit before calling OceffSpot_End, otherwise Link and the music staff spots could start waiting
    // for the Sun's Song light after it's been killed already, causing softlocks and non-functional song switches in MQ
    // Spirit
    else if (cheats[CHEATS_SKIP_SONGS_PLAYBACK] && ((PLAYER->stateFlags1 >> 24) & 0x30) != 0x30) {
        OceffSpot_End(thisx, globalCtx);
    }
}

// Song of Storms
void OceffStorm_rUpdate(Actor* thisx, GlobalContext* globalCtx) {
    if (!cheats[CHEATS_SKIP_SONGS_PLAYBACK]) {
        vanillaActorInit_OceffStorm.update(thisx, globalCtx);
    } else {
        CutsceneFlags_Set(globalCtx, 5);
        Actor_Kill(thisx);
    }
}
