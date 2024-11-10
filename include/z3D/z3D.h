#ifndef _Z3D_H_
#define _Z3D_H_

#include "z3Dactor.h"
#include "z3Dvec.h"
// #include "z3Dequipment.h"
#include "z3Dcutscene.h"
#include "z3Ditem.h"
#include "z3Dmath.h"
#include "z3Dbgcheck.h"
#include "color.h"

// #include "hid.h"

#define TRUE 1
#define FALSE 0

typedef struct {
    /* 0x00 */ u8  buttonItems[5]; //B,Y,X,I,II
    /* 0x05 */ u8  buttonSlots[4]; //Y,X,I,II
    /* 0x0A */ u16 equipment;
} ItemEquips; // size = 0x0C

typedef struct {
    /* 0x00 */ u32   chest;
    /* 0x04 */ u32   swch;
    /* 0x08 */ u32   clear;
    /* 0x0C */ u32   collect;
    /* 0x10 */ u32   unk;
    /* 0x14 */ u32   rooms1;
    /* 0x18 */ u32   rooms2;
} SaveSceneFlags; // size = 0x1C

typedef struct {
    /* 0x00 */ s16   scene;
    /* 0x02 */ Vec3s pos;
    /* 0x08 */ s16   angle;
} HorseData; // size = 0x0A

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ s16   yaw;
    /* 0x0E */ s16   playerParams;
    /* 0x10 */ s16   entranceIndex;
    /* 0x12 */ u8    roomIndex;
    /* 0x13 */ u8    data;
    /* 0x14 */ u32   tempSwchFlags;
    /* 0x18 */ u32   tempCollectFlags;
} RespawnData; // size = 0x1C

typedef enum {
    /* 0x00 */ RESPAWN_MODE_DOWN,   /* Normal Void Outs */
    /* 0x01 */ RESPAWN_MODE_RETURN, /* Grotto Returnpoints */
    /* 0x02 */ RESPAWN_MODE_TOP     /* Farore's Wind */
} RespawnMode;

typedef enum {
    /* 0x00 */ BTN_ENABLED,
    /* 0xFF */ BTN_DISABLED = 0xFF
} ButtonStatus;

// Save Context (ram start: 0x00587958)
typedef struct SaveContext {
    /* 0x0000 */ s32          entranceIndex;
    /* 0x0004 */ s32          linkAge; // 0: Adult; 1: Child
    /* 0x0008 */ s32          cutsceneIndex;
    /* 0x000C */ u16          dayTime; // "zelda_time"
    /* 0x000E */ u8           masterQuestFlag;
    /* 0x000F */ u8           motionControlSetting;
    /* 0x0010 */ s32          nightFlag;
    /* 0x0014 */ s32          unk_14;
    /* 0x0018 */ s32          unk_18;
    /* 0x001C */ s16          playerName[0x8];
    /* 0x002C */ u8           playerNameLength;
    /* 0x002D */ u8           zTargetingSetting;
    /* 0x002E */ s16          unk_2E;
    /* 0x0030 */ char         newf[6]; // string "ZELDAZ"
    /* 0x0036 */ u16          saveCount;
    /* 0x0038 */ char         unk_38[0x000A];
    /* 0x0042 */ u16          healthCapacity; // "max_life"
    /* 0x0044 */ s16          health; // "now_life"
    /* 0x0046 */ s8           magicLevel;
    /* 0x0047 */ s8           magic;
    /* 0x0048 */ s16          rupees;
    /* 0x004A */ u16          bgsHitsLeft;
    /* 0x004C */ u16          naviTimer;
    /* 0x004E */ u8           magicAcquired;
    /* 0x004F */ char         unk_4F[0x0001];
    /* 0x0050 */ u8           doubleMagic;
    /* 0x0051 */ u8           doubleDefense;
    /* 0x0052 */ s8           bgsFlag;
    /* 0x0054 */ ItemEquips   childEquips;
    /* 0x0060 */ ItemEquips   adultEquips;
    /* 0x006C */ char         unk_6C[0x0012];
    /* 0x007E */ u16          sceneIndex;
    /* 0x0080 */ ItemEquips   equips;
    /* 0x008C */ u8           items[26];
    /* 0x00A6 */ s8           ammo[15];
    /* 0x00B5 */ u8           magic_beans_bought;
    /* 0x00B6 */ u16          equipment; //bits: swords 0-3, shields 4-6, tunics 8-10, boots 12-14
    /* 0x00B8 */ u32          upgrades; //bits: quiver 0-2, bombs 3-5, strength 6-8, dive 9-11, wallet 12-13, seeds 14-16, sticks 17-19, nuts 20-22
    /* 0x00BC */ u32          questItems; //bits: medallions 0-5, warp songs 6-11, songs 12-17, stones 18-20, shard 21, token 22, skull 23, heart pieces 24-31
    /* 0x00C0 */ u8           dungeonItems[20]; //bits: boss key 0, compass 1, map 2
    /* 0x00D4 */ s8           dungeonKeys[19];
    /* 0x00E7 */ char         unk_E7[0x0001]; //in oot: defenseHearts. seems not here.
    /* 0x00E8 */ s16          gsTokens;
    /* 0x00EC */ SaveSceneFlags sceneFlags[124];
    struct {
        /* 0x0E7C */ Vec3i pos;
        /* 0x0E88 */ s32  yaw;
        /* 0x0E8C */ s32  playerParams;
        /* 0x0E90 */ s32  entranceIndex;
        /* 0x0E94 */ s32  roomIndex;
        /* 0x0E98 */ s32  set;
        /* 0x0E9C */ s32  tempSwchFlags;
        /* 0x0EA0 */ s32  tempCollectFlags;
    }                         fw;
    /* 0x0EA4 */ char         unk_EA4[0x0010];
    /* 0x0EB4 */ u8           gsFlags[22]; //due to reordering, array is smaller
    /* 0x0ECA */ char         unk_ECA[0x0006]; //the extra two bytes move here
    /* 0x0ED0 */ u32          unk_ED0; //horseback archery highscore?
    /* 0x0ED4 */ u32          bigPoePoints; //number of big poes sold * 100
    struct {
        /* 0x0ED4 */ u8 recordFishChild; //seems to be unique ID of fish, this is copied into adult value if player has not yet fished as adult
        /* 0x0ED5 */ u8 flags; //bits: 0 - ever fished as child, 1 - ever fished as adult, 2 - caught record as child, 3 - caught record as adult
        /* 0x0ED6 */ u8 timesPaidToFish;
        /* 0x0ED7 */ u8 recordFishAdult; //seems to be unique ID of fish
    }                         fishingStats;
    /* 0x0EDC */ u32          unk_EDC; //horse race record time?
    /* 0x0EE0 */ u32          unk_EE0; //marathon race record time?
    /* 0x0EE4 */ char         unk_EE4[0x0008];
    /* 0x0EEC */ u16          eventChkInf[14]; // "event_chk_inf"
    /* 0x0F08 */ u16          itemGetInf[4]; // "item_get_inf"
    /* 0x0F10 */ u16          infTable[30]; // "inf_table"
    /* 0x0F4C */ char         unk_F34[0x0004];
    /* 0x0F50 */ u32          worldMapAreaData; // "area_arrival"
    /* 0x0F54 */ char         unk_F54[0x0410]; // TODO: scarecrow's song
    /* 0x1364 */ HorseData    horseData;
    /* 0x136E */ char         unk_136E[0x0002];
    /* 0x1370 */ u8           itemSlotDataRecords[26];
    /* 0x138A */ u8           itemMenuChild[24];
    /* 0x13A2 */ u8           itemMenuAdult[24];
    /* 0x13BA */ char         unk_13BA[0x0002];
    struct {
        /* 0x13BC */ u32 year;
        /* 0x13C0 */ u32 month;
        /* 0x13C4 */ u32 day;
        /* 0x13C8 */ u32 hour;
        /* 0x13CC */ u32 minute;
    }                         saveTime;
    /* 0x13D0 */ char         unk_13D0[0x0080];
    /* 0x1450 */ u32          bossBattleVictories[9];
    /* 0x1474 */ u32          bossBattleScores[9];
    /* 0x1498 */ char         unk_1498[0x0040]; //sheikah stone flags?
    /* 0x14D8 */ u16          checksum; // "check_sum"
    /* 0x14DC */ s32          fileNum; // "file_no"
    /* 0x14E0 */ char         unk_14E0[0x0004];
    /* 0x14E4 */ s32          gameMode;
    /* 0x14E8 */ s32          sceneSetupIndex;
    /* 0x14EC */ s32          respawnFlag; // "restart_flag"
    /* 0x14F0 */ RespawnData  respawn[3]; // "restart_data"
    /* 0x1544 */ char         unk_1544[0x000E];
    /* 0x1552 */ s16          nayrusLoveTimer;
    /* 0x1554 */ char         unk_1554[0x0004];
    /* 0x1558 */ u32          seqIndex;
    /* 0x155C */ s16          rupeeAccumulator;
    /* 0x155E */ s16          timer1State;
    /* 0x1560 */ s16          timer1Value;
    /* 0x1562 */ s16          timer2State;
    /* 0x1564 */ s16          timer2Value;
    /* 0x1566 */ s16          timerX[2]; //changing these doesn't seem to actually move the timer?
    /* 0x156A */ s16          timerY[2]; //changing these doesn't seem to actually move the timer?
    /* 0x156E */ u8           nightSeqIndex;
    /* 0x156F */ u8           buttonStatus[5];
    /* 0x1574 */ char         unk_1574[1];
    /* 0x1575 */ u8           ocarinaButtonStatus;
    /* 0x1576 */ char         unk_1576[0x0009];
    /* 0x1580 */ s16          magicState;
    /* 0x1582 */ s16          prevMagicState;
    /* 0x1584 */ u16          magicCapacity; // magicMeterSize
    /* 0x1586 */ s16          magicFillTarget;
    /* 0x1588 */ s16          magicTarget;
    /* 0x158A */ u16          eventInf[4];
    /* 0x1592 */ u16          dungeonIndex;
    /* 0x1594 */ char         unk_1594[0x000C];
    /* 0x15A0 */ u16          nextCutsceneIndex;
    /* 0x15A2 */ u8           cutsceneTrigger;
    /* 0x15A3 */ char         unk_15A3[0x008];
    /* 0x15AB */ u8           nextTransition;
    /* 0x15AC */ char         unk_15AC[0x006];
    /* 0x15B2 */ s16          healthAccumulator;
    /* 0x15B4 */ char         unk_15B4[0x10];
} SaveContext; // size = 0x15C4
_Static_assert(sizeof(SaveContext) == 0x15C4, "Save Context size");

typedef struct GraphicsContext GraphicsContext; //TODO
typedef struct GlobalContext GlobalContext;
typedef struct Camera {
    /* 0x000 */ char unk_000[0x080];
    /* 0x080 */ Vec3f at;
    /* 0x08C */ Vec3f eye;
    /* 0x098 */ Vec3f up;
    /* 0x0A4 */ Vec3f eyeNext;
    /* 0x0B0 */ Vec3f skyboxOffset;
    /* 0x0BC */ char unk_0BC[0x018];
    /* 0x0D4 */ GlobalContext* globalCtx;
    /* 0x0D8 */ Player* player;
    /* 0x0DC */ PosRot playerPosRot;
    /* 0x0F0 */ Actor* target;
    /* 0x0F4 */ PosRot targetPosRot;
    /* 0x108 */ f32 rUpdateRateInv;
    /* 0x10C */ f32 pitchUpdateRateInv;
    /* 0x110 */ f32 yawUpdateRateInv;
    /* 0x114 */ f32 xzOffsetUpdateRate;
    /* 0x118 */ f32 yOffsetUpdateRate;
    /* 0x11C */ f32 fovUpdateRate;
    /* 0x120 */ f32 xzSpeed;
    /* 0x124 */ f32 dist;
    /* 0x128 */ f32 speedRatio;
    /* 0x12C */ Vec3f playerToAtOffset;
    /* 0x138 */ Vec3f playerPosDelta;
    /* 0x144 */ f32 fov;
    /* 0x148 */ f32 atLERPStepScale;
    /* 0x14C */ f32 playerGroundY;
    /* 0x150 */ Vec3f floorNorm;
    /* 0x15C */ f32 waterYPos;
    /* 0x160 */ s32 waterPrevCamIdx;
    /* 0x164 */ s32 waterPrevCamSetting;
    /* 0x168 */ s32 waterQuakeIdx;
    /* 0x16C */ char unk_16C[0x00C];
    /* 0x178 */ s16 uid;
    /* 0x17A */ char unk_17A[0x002];
    /* 0x17C */ Vec3s inputDir;
    /* 0x182 */ Vec3s camDir;
    /* 0x188 */ s16 status;
    /* 0x18A */ s16 setting;
    /* 0x18C */ s16 mode;
    /* 0x18E */ s16 bgCheckId;
    /* 0x190 */ s16 camDataIdx;
    /* 0x192 */ s16 behaviorFlags;
    /* 0x194 */ s16 stateFlags;
    /* 0x196 */ s16 childCamIdx;
    /* 0x198 */ s16 waterDistortionTimer;
    /* 0x19A */ s16 distortionFlags;
    /* 0x19C */ s16 prevSetting;
    /* 0x19E */ s16 nextCamDataIdx;
    /* 0x1A0 */ s16 nextBgCheckId;
    /* 0x1A2 */ s16 roll;
    /* 0x1A4 */ s16 paramFlags;
    /* 0x1A6 */ s16 animState;
    /* 0x1A8 */ s16 timer;
    /* 0x1AA */ s16 parentCamIdx;
    /* 0x1AC */ s16 thisIdx;
    /* 0x1AE */ s16 prevCamDataIdx;
    /* 0x1B0 */ s16 csId;
    /* 0x1B2 */ char unk_1B2[0x00A];
} Camera; // size = 0x1BC
_Static_assert(sizeof(Camera) == 0x1BC, "Camera size");

typedef struct {
    /* 0x00 */ u8*  texture;
    /* 0x04 */ s16  x;
    /* 0x06 */ s16  y;
    /* 0x08 */ s16  width;
    /* 0x0A */ s16  height;
    /* 0x0C */ s32  unk_0C;
    /* 0x10 */ u8   durationTimer;
    /* 0x11 */ u8   delayTimer;
    /* 0x12 */ s16  alpha;
    /* 0x14 */ s16  intensity;
    /* 0x16 */ s16  unk_16;
} TitleCardContext; // size = 0x18

typedef struct {
    /* 0x00 */ u32    length; // number of actors loaded of this type
    /* 0x04 */ Actor* first;  // pointer to first actor of this type
} ActorListEntry; // size = 0x08

typedef struct {
    /* 0x0000 */ u8     unk_00;
    /* 0x0001 */ char   unk_01[0x01];
    /* 0x0002 */ u8     unk_02;
    /* 0x0003 */ u8     unk_03;
    /* 0x0004 */ char   unk_04[0x04];
    /* 0x0008 */ u8     total; // total number of actors loaded
    /* 0x0009 */ char   unk_09[0x03];
    /* 0x000C */ ActorListEntry actorList[12];
    // /* 0x006C */ TargetContext targetCtx;
    /* 0x006C */ char   unk_6C[0x130];
    struct {
        /* 0x019C */ u32    swch;
        /* 0x01A0 */ u32    tempSwch;
        /* 0x01A4 */ u32    unk0;
        /* 0x01A8 */ u32    unk1;
        /* 0x01AC */ u32    chest;
        /* 0x01B0 */ u32    clear;
        /* 0x01B4 */ u32    tempClear;
        /* 0x01B8 */ u32    collect;
        /* 0x01BC */ u32    tempCollect;
    }                   flags;
    /* 0x01C0 */ TitleCardContext titleCtx;
} ActorContext; // TODO: size = 0x1D8

typedef enum {
    /* 0 */ CS_STATE_IDLE,
    /* 1 */ CS_STATE_START,
    /* 2 */ CS_STATE_RUN,
    /* 3 */ CS_STATE_STOP,
    /* 4 */ CS_STATE_RUN_UNSTOPPABLE
} CutsceneState;

typedef struct CutsceneContext {
    /* 0x00 */ char  unk_00[0x4];
    /* 0x04 */ void* segment;
    /* 0x08 */ u8    state;
    /* 0x09 */ char  unk_09[0x13];
    /* 0x1C */ f32   unk_1C;
    /* 0x20 */ u16   frames;
    /* 0x22 */ u16   unk_22;
    /* 0x24 */ s32   unk_24;
    /* 0x28 */ char  unk_28[0x18];
    /* 0x40 */ CsCmdActorAction* linkAction;
    /* 0x44 */ CsCmdActorAction* actorActions[10]; // "npcdemopnt"
} CutsceneContext; // size = 0x6C

typedef struct Collider Collider; //TODO
typedef struct OcLine OcLine; //TODO
#define COLLISION_CHECK_AT_MAX 50
#define COLLISION_CHECK_AC_MAX 60
#define COLLISION_CHECK_OC_MAX 50
#define COLLISION_CHECK_OC_LINE_MAX 3

typedef struct {
    /* 0x000 */ s16 colAtCount;
    /* 0x002 */ u16 sacFlags;
    /* 0x004 */ Collider* colAt[COLLISION_CHECK_AT_MAX];
    /* 0x0CC */ s32 colAcCount;
    /* 0x0D0 */ Collider* colAc[COLLISION_CHECK_AC_MAX];
    /* 0x1C0 */ s32 colOcCount;
    /* 0x1C4 */ Collider* colOc[COLLISION_CHECK_OC_MAX];
    /* 0x28C */ s32 colOcLineCount;
    /* 0x290 */ OcLine* colOcLine[COLLISION_CHECK_OC_LINE_MAX];

} CollisionCheckContext; // size = 0x29C
// _Static_assert(sizeof(CollisionCheckContext) == 0x29C, "CollisionCheckContext size");

#define OBJECT_EXCHANGE_BANK_MAX 19
#define OBJECT_ID_MAX 417

typedef struct ZAR {
    /* 0x00 */ char	magic[4]; //"ZAR\1"
    /* 0x04 */ u32  size;
    /* 0x08 */ u16  numTypes;
    /* 0x0A */ u16  numFiles;
    /* 0x0C */ u32  fileTypesOffset;
    /* 0x10 */ u32  fileMetadataOffset;
    /* 0x14 */ u32  dataOffset;
    /* 0x18 */ char magic2[8]; // "queen"
    /* 0x20 */ char data[1];
} ZAR;

typedef struct ZARFileTypeEntry {
    /* 0x00 */ u32	numFiles;
    /* 0x04 */ u32  filesListOffset;
    /* 0x08 */ u16  typeNameOffset;
    /* 0x0C */ u16  unk_0C; // always -1?
} ZARFileTypeEntry;

typedef struct ZARInfo {
    /* 0x00 */ void* buf;
    /* 0x04 */ s32 size;
    /* 0x08 */ ZAR* buf2;
    /* 0x0C */ ZARFileTypeEntry* fileTypes;
    /* 0x10 */ void* fileMetadata;
    /* 0x14 */ void* data;
    /* 0x18 */ ZAR* buf3;
    /* 0x1C */ s32 fileTypeMap[11];
    /* 0x48 */ char unk_48[0x4];
    /* 0x4C */ void*** cmbPtrs;  /* Really, this is a pointer to an array of pointers to CMB managers,
                                    the first member of which is a pointer to the CMB data */
    /* 0x50 */ void*** csabPtrs; /* Same as above but for CSAB */
    /* 0x54 */ void* ctxb;
    /* 0x58 */ void*** cmabPtrs; /* Same as above but for CMAB */
    /* 0x5C */ char unk_5C[0x14];
} ZARInfo; // size = 0x70
_Static_assert(sizeof(ZARInfo) == 0x70, "ZARInfo size");

typedef struct {
    /* 0x00 */ s16 id;
    /* 0x02 */ char unk_02[0x0E];
    /* 0x10 */ ZARInfo zarInfo;
} ObjectStatus; // size = 0x80

typedef struct {
    /* 0x000 */ u8 num;
    /* 0x001 */ char unk_01[0x3];
    /* 0x004 */ ObjectStatus status[OBJECT_EXCHANGE_BANK_MAX];
} ObjectContext; // size = 0x984

typedef struct {
    /* 0x00 */ char filename[0x40];
    /* 0x40 */ u32 size;
} ObjectFile;

typedef struct GameState {
    /* 0x00 */ GraphicsContext* gfxCtx;
    /* 0x04 */ void (*main)(struct GameState*);
    /* 0x08 */ void (*destroy)(struct GameState*); // "cleanup"
    /* 0x0C */ void (*init)(struct GameState*);
    /* 0x10 */ u32 size;
    /* 0x14 */ char unk_14[0xED];
    /* 0x101*/ u8 running;
} GameState;
_Static_assert(sizeof(GameState) == 0x104, "GameState size");

typedef struct {
    /* 0x00 */ s8    num;
    /* 0x01 */ u8    unk_01;
    /* 0x02 */ u8    behaviorType2;
    /* 0x03 */ u8    behaviorType1;
    /* 0x04 */ s8    echo;
    /* 0x05 */ u8    lensMode;
    /* 0x08 */ void* roomShape; // maybe?
    /* 0x0C */ void* segment;   // maybe?
    /* 0x10 */ char  unk_10[0x3CC];
} Room;
_Static_assert(sizeof(Room) == 0x3DC, "Room size");

typedef struct {
    /* 0x000 */ Room curRoom;
    /* 0x3DC */ Room prevRoom;
    /* 0x7B8 */ char unk_7B8[0x19];
    /* 0x7D1 */ s8   status;
    /* 0x7D2 */ char unk_7D2[0x786];
} RoomContext;
_Static_assert(sizeof(RoomContext) == 0xF58, "RoomContext size");

typedef struct {
    /* 0x000 */ s32 topY;
    /* 0x004 */ s32 bottomY;
    /* 0x008 */ s32 leftX;
    /* 0x00C */ s32 rightX;
    /* 0x010 */ f32 fovY;
    /* 0x014 */ f32 zNear;
    /* 0x018 */ f32 zFar;
    /* 0x01C */ f32 scale;
    /* 0x020 */ char unk_01A8[0x0010];
    /* 0x030 */ Vec3f eye;
    /* 0x03C */ Vec3f at;
    /* 0x048 */ Vec3f up;
    /* 0x054 */ char unk_01DC[0x0150];
    /* 0x1A4 */ Vec3f distortionOrientation;
    /* 0x1B0 */ Vec3f distortionScale;
    /* 0x1BC */ char unk_0344[0x0018];
    /* 0x1D4 */ f32 distortionSpeed;
    /* 0x1D8 */ char unk_0360[0x0004];
} View; // size 0x1DC

typedef struct MessageContext {
    /* 0x000 */ char                  unk_000[0x1F0];
    /* 0x1F0 */ u8                    msgMode;
    /* 0x1F1 */ char                  unk_1F1[0xEB];
    /* 0x2DC */ u16                   lastPlayedSong;
    /* 0x2DE */ u16                   ocarinaMode;
    /* 0x2E0 */ char                  unk_2DF[0x20];
    /* 0x300 */ u8                    lastOcarinaButtonIndex;
} MessageContext;
_Static_assert(sizeof(MessageContext) == 0x302, "MessageContext size");

typedef struct {
    /* 0x000 */ u8 type;
    /* 0x001 */ u8 isDone;
    /* 0x002 */ u8 direction;
    /* 0x004 */ Color_RGBA8_u32 color;
    /* 0x008 */ u16 timer;
} TransitionFade;
_Static_assert(sizeof(TransitionFade) == 0xC, "TransitionFade size");

typedef struct {
    /* 0x00 */ char fileName[0x40];
    /* 0x40 */ u32  unk_40; // size?
} RomFile;
_Static_assert(sizeof(RomFile) == 0x44, "RomFile size");

// Global Context (ram start: 0871E840)
typedef struct GlobalContext {
    /* 0x0000 */ GameState             state;
    /* 0x0104 */ s16                   sceneNum;
    /* 0x0106 */ char                  unk_106[0x000A];
    /* 0x0110 */ void*                 sceneSegment;
    /* 0x0114 */ ZAR*                  sceneZAR;
    /* 0x0118 */ ZARInfo               sceneZARInfo;
    /* 0x0188 */ View                  view;
    /* 0x0364 */ Camera                mainCamera;
    /* 0x0520 */ Camera                subCameras[3];
    /* 0x0A54 */ Camera*               cameraPtrs[4];
    /* 0x0A64 */ s16                   activeCamera;
    /* 0x0A66 */ char                  unk_A66[0x0032];
    /* 0x0A98 */ CollisionContext      colCtx;
    /* 0x208C */ ActorContext          actorCtx;
    /* 0x2264 */ char                  unk_2264[0x0034];
    /* 0x2298 */ CutsceneContext       csCtx; // "demo_play"
    /* 0x2304 */ char                  unk_2304[0x059C];
    /* 0x28A0 */ MessageContext        msgCtx;
    /* 0x2BA2 */ char                  unk_2BA2[0x068E];
    /* 0x3230 */ u32                   lightSettingsList_addr;
    /* 0x3234 */ char                  unk_3234[0x0824];
    /* 0x3A58 */ ObjectContext         objectCtx;
    /* 0x43DC */ char                  unk_43DC[0x0854];
    /* 0x4C30 */ RoomContext           roomCtx;
    /* 0x5B88 */ char                  unk_5B88[0x0074];
    /* 0x5BFC */ u32                   gameplayFrames;
    /* 0x5C00 */ u8                    linkAgeOnLoad;
    /* 0x5C01 */ u8                    haltAllActors;
    /* 0x5C02 */ u8                    spawn;
    /* 0x5C03 */ u8                    numActorEntries;
    /* 0x5C04 */ u8                    numRooms;
    /* 0x5C08 */ RomFile*              roomList;
    /* 0x5C0C */ char                  unk_5C0C[0x0010];
    /* 0x5C1C */ s16*                  setupExitList;
    /* 0x5C20 */ char                  unk_5C20[0x000D];
    /* 0x5C2D */ s8                    transitionTrigger; // "fade_direction"
    /* 0x5C2E */ char                  unk_5C2E[0x0004];
    /* 0x5C32 */ s16                   nextEntranceIndex;
    /* 0x5C34 */ char                  unk_5C34[0x0042];
    /* 0x5C76 */ u8                    transitionType; // fadeOutTransition
    /* 0x5C78 */ CollisionCheckContext colChkCtx;
    /* 0x5F14 */ char                  unk_5F14[0x1FFE];
    /* 0x7F12 */ u8                    transitionMode;
    /* 0x7F14 */ TransitionFade        transitionFadeFlash;
    /* 0x7F20 */ char                  unk_7F20[0x118];
} GlobalContext;
_Static_assert(sizeof(GlobalContext) == 0x8038, "Global Context size");

typedef struct StaticContext {
    /* 0x0000 */ char unk_000[0x0E60];
    /* 0x0E60 */ u16  spawnOnEpona;
    /* 0x0E62 */ char unk_E62[0x0010];
    /* 0x0E72 */ u16  showColliders;
    /* 0x0E74 */ char unk_E74[0x000A];
    /* 0x0E7E */ u16  showAT;
    /* 0x0E80 */ u16  showAC;
    /* 0x0E82 */ u16  showOC;
    /* 0x0E84 */ char unk_E84[0x014C];
    /* 0x0FD0 */ u16  disableRoomDraw;
    /* 0x0FD2 */ char unk_FD2[0x0602];
} StaticContext; //size 0x15D4
_Static_assert(sizeof(StaticContext) == 0x15D4, "Static Context size");

typedef struct {
    /* 0x00 */ s8  scene;
    /* 0x01 */ s8  spawn;
    /* 0x02 */ u16 field;
} EntranceInfo; // size = 0x4

typedef struct {
    /* 0x00 */ char infoFilename[0x44];
    /* 0x44 */ char filename[0x44];
    /* 0x88 */ char unk_88[0x01];
    /* 0x89 */ u8   config;
    /* 0x8A */ char unk_8A[0x02];
} Scene; // size = 0x8C

typedef struct {
    /* 0x00 */ s16 objectId;
    /* 0x02 */ u8 objectModelIdx;
    /* 0x03 */ char unk_03[0x3];
} DrawItemTableEntry;

typedef struct {
    /* 0x00 */ u8 scene;
    /* 0x01 */ u8 flags1;
    /* 0x02 */ u8 flags2;
    /* 0x03 */ u8 flags3;
} RestrictionFlags;

typedef struct SubMainClass_180 {
    /* 0x000 */ char unk_00[0x8];
    /* 0x008 */ s32 saModelsCount1;
    /* 0x00C */ s32 saModelsCount2;
    /* 0x010 */ char unk_10[0x10];
    /* 0x020 */ struct {SkeletonAnimationModel* saModel; u32 unk;}* saModelsList1;
    /* 0x024 */ struct {SkeletonAnimationModel* saModel; u32 unk;}* saModelsList2;
    /* ... size unknown*/
} SubMainClass_180;

// This struct contains data related to the built-in Collision Display
typedef struct SubMainClass_32A0 {
    /* 0x00 */ void* bufferPointer_00; // Start of 0xC40 buffer
    /* 0x04 */ s16 saModelCount; // 3D sphere and cylinder models
    /* 0x06 */ s16 saModelMax;
    /* 0x08 */ void* cmbMan;
    /* 0x0C */ s16 polyCounter; // 2D quad models
    /* 0x0E */ s16 polyMax;
    /* 0x10 */ void*(*array_10)[]; // pointer to array of pointers, offset 0xA40 in 0xC40 buffer (size 0x100?)
    /* 0x14 */ void*(*array_14)[]; // pointer to array of pointers, offset 0xB40 in 0xC40 buffer (size 0x100?)
    /* 0x18 */ void* bufferPointer_18; // Offset 0x140 in 0xC40 buffer
    /* 0x1C */ void* bufferPointer_1C; // Start of 0xC40 buffer
} SubMainClass_32A0;
_Static_assert(sizeof(SubMainClass_32A0) == 0x20, "SubMainClass_32A0 size");

typedef struct MainClass {
    /* 0x0000 */ char unk_00[0x180];
    /* 0x0180 */ SubMainClass_180 sub180;
    /* 0x01A8 */ char unk_1A8[0x30F8];
#if Version_KOR || Version_TWN
    /* 0x???? */ char unk_kor_twn[0x4]; // the stuff below is 4 bytes ahead on KOR/TWN
#endif
    /* 0x32A0 */ SubMainClass_32A0 sub32A0;
    /* ... size unknown*/
} MainClass;
#define MAIN_CLASS_TEMP_SIZE (0x32C0 + (Version_KOR || Version_TWN) * 4)
_Static_assert(sizeof(MainClass) == MAIN_CLASS_TEMP_SIZE, "MainClass size");

extern GlobalContext* gGlobalContext;
extern void* gStoredActorHeapAddress;
extern const u32 ItemSlots[];
extern const char DungeonNames[][25];

#define gStaticContext (*(StaticContext*)0x08080010)
#define gObjectTable ((ObjectFile*)0x53CCF4)
#define gEntranceTable ((EntranceInfo*)0x543BB8)
#define gItemUsabilityTable ((u8*)0x506C58)
#define gGearUsabilityTable ((u32*)0x4D47C8)
#define gDungeonSceneTable ((Scene*)0x4DC400)
#define gMQDungeonSceneTable ((Scene*)0x4DCBA8)
#define gSceneTable ((Scene*)0x545484)
#define gRandInt (*(u32*)0x50C0C4)
#define gRandFloat (*(f32*)0x50C0C8)
#define gDrawItemTable ((DrawItemTableEntry*)0x4D88C8)
#define gRestrictionFlags ((RestrictionFlags*)0x539DC4)
#define PLAYER ((Player*)gGlobalContext->actorCtx.actorList[ACTORTYPE_PLAYER].first)

#if Version_KOR || Version_TWN
    #define gSaveContext (*(SaveContext*)0x595FD0)
    #define ControlStick_X (*(f32*)0x573C38)
    #define ControlStick_Y (*(f32*)0x573C3C)
    #define gActorHeapAddress (*(void**)0x5B14B4)
    #define gMainClass ((MainClass*)0x5C5AA4)
#else
    #define gSaveContext (*(SaveContext*)0x587958)
    #define ControlStick_X (*(f32*)0x5655C0)
    #define ControlStick_Y (*(f32*)0x5655C4)
    #define gActorHeapAddress (*(void**)0x5A2E3C)
    #define gMainClass ((MainClass*)0x5BE5B8)
#endif

typedef enum {
    DUNGEON_DEKU_TREE = 0,
    DUNGEON_DODONGOS_CAVERN,
    DUNGEON_JABUJABUS_BELLY,
    DUNGEON_FOREST_TEMPLE,
    DUNGEON_FIRE_TEMPLE,
    DUNGEON_WATER_TEMPLE,
    DUNGEON_SPIRIT_TEMPLE,
    DUNGEON_SHADOW_TEMPLE,
    DUNGEON_BOTTOM_OF_THE_WELL,
    DUNGEON_ICE_CAVERN,
    DUNGEON_GANONS_CASTLE_SECOND_PART,
    DUNGEON_GERUDO_TRAINING_GROUNDS,
    DUNGEON_GERUDO_FORTRESS,
    DUNGEON_GANONS_CASTLE_FIRST_PART,
    DUNGEON_GANONS_CASTLE_FLOOR_BENEATH_BOSS_CHAMBER,
    DUNGEON_GANONS_CASTLE_CRUMBLING,
    DUNGEON_TREASURE_CHEST_SHOP,
    DUNGEON_DEKU_TREE_BOSS_ROOM,
    DUNGEON_DODONGOS_CAVERN_BOSS_ROOM,
    DUNGEON_JABUJABUS_BELLY_BOSS_ROOM,
} DungeonId;

/* TODO: figure out what to do with this stuff */
#define real_hid_addr   0x10002000
#define real_hid        (*(hid_mem_t *) real_hid_addr)

#if Version_KOR || Version_TWN
    #define Z3D_TOP_SCREEN_LEFT_1 0x1430A900
    #define Z3D_TOP_SCREEN_LEFT_2 0x14350E10
    #define Z3D_TOP_SCREEN_RIGHT_1 0x14407DD0
    #define Z3D_TOP_SCREEN_RIGHT_2 0x1444E2E0
    #define Z3D_BOTTOM_SCREEN_1 0x14397380
    #define Z3D_BOTTOM_SCREEN_2 0x143CF790
#else
    #define Z3D_TOP_SCREEN_LEFT_1 0x14313890
    #define Z3D_TOP_SCREEN_LEFT_2 0x14359DA0
    #define Z3D_TOP_SCREEN_RIGHT_1 0x14410AD0
    #define Z3D_TOP_SCREEN_RIGHT_2 0x14456FE0
    #define Z3D_BOTTOM_SCREEN_1 0x143A02B0
    #define Z3D_BOTTOM_SCREEN_2 0x143D86C0
#endif

typedef Actor* (*Actor_Spawn_proc)(ActorContext *actorCtx,GlobalContext *globalCtx,s16 actorId,float posX,float posY,float posZ,s16 rotX,s16 rotY,s16 rotZ,s16 params)
    __attribute__((pcs("aapcs-vfp")));
#if Version_USA || Version_EUR
    #define Actor_Spawn_addr 0x3738D0
#elif Version_JPN
    #define Actor_Spawn_addr 0x3733E8
#elif Version_KOR
    #define Actor_Spawn_addr 0x2F7CAC
#elif Version_TWN
    #define Actor_Spawn_addr 0x2F7DAC
#endif
#define Actor_Spawn ((Actor_Spawn_proc)Actor_Spawn_addr)

typedef s32 (*Object_proc)(ObjectContext* objectCtx, s16 objectId);
#if Version_JPN
    #define Object_Spawn_addr 0x32DD34
#else //USA & EUR
    #define Object_Spawn_addr 0x32E21C
#endif
#define Object_Spawn ((Object_proc)Object_Spawn_addr)

typedef void (*Player_SetEquipmentData_proc)(GlobalContext* globalCtx, Player* player);
#if Version_JPN
    #define Player_SetEquipmentData_addr 0x348C54
#else //USA & EUR
    #define Player_SetEquipmentData_addr 0x34913C
#endif
#define Player_SetEquipmentData ((Player_SetEquipmentData_proc)Player_SetEquipmentData_addr)

typedef void (*Flags_SetEnv_proc)(GlobalContext* globalCtx, s16 flag);
#if Version_JPN
    #define Flags_SetEnv_addr 0x36621C
#else //USA & EUR
    #define Flags_SetEnv_addr 0x366704
#endif
#define Flags_SetEnv ((Flags_SetEnv_proc)Flags_SetEnv_addr)

typedef void (*DisplayTextbox_proc)(GlobalContext* globalCtx, u16 textId, Actor* actor);
#define DisplayTextbox_addr 0x367C7C
#define DisplayTextbox ((DisplayTextbox_proc)DisplayTextbox_addr)

typedef void (*CloseTextbox_proc)(GlobalContext* globalCtx);
#if Version_JPN
    #define CloseTextbox_addr 0x3720F8
#else //USA & EUR
    #define CloseTextbox_addr 0x3725E0
#endif
#define Message_CloseTextbox ((CloseTextbox_proc)CloseTextbox_addr)

typedef void (*PlaySound_proc)(u32);
#define PlaySound_addr 0x35C528
#define PlaySound ((PlaySound_proc)PlaySound_addr) //this function plays sound effects and music tracks, overlaid on top of the current BGM

typedef void (*Play_Init_proc)(GameState*);
#if Version_EUR
    #define Play_Init_addr 0x435314
#elif Version_JPN
    #define Play_Init_addr 0x4352C8
#elif Version_KOR
    #define Play_Init_addr 0x11EE6C
#elif Version_TWN
    #define Play_Init_addr 0x11EF44
#else // Version_USA
    #define Play_Init_addr 0x4352F0
#endif
#define Play_Init ((Play_Init_proc)Play_Init_addr)

typedef void (*Play_Main_proc)(GameState*);
#if Version_EUR
    #define Play_Main_addr 0x4523AC
#elif Version_JPN
    #define Play_Main_addr 0x452364
#elif Version_KOR
    #define Play_Main_addr 0x13A1E8
#elif Version_TWN
    #define Play_Main_addr 0x13A2C0
#else // Version_USA
    #define Play_Main_addr 0x45238C
#endif
#define Play_Main ((Play_Main_proc)Play_Main_addr)

typedef void (*FileSelect_LoadGame_proc)(GameState* gameState, s32 fileNum);
#if Version_EUR
    #define FileSelect_LoadGame_addr 0x44737C
#elif Version_JPN
    #define FileSelect_LoadGame_addr 0x447334
#elif Version_KOR
    #define FileSelect_LoadGame_addr 0x12E5BC
#elif Version_TWN
    #define FileSelect_LoadGame_addr 0x12E694
#else // Version_USA
    #define FileSelect_LoadGame_addr 0x44735C
#endif
#define FileSelect_LoadGame ((FileSelect_LoadGame_proc)FileSelect_LoadGame_addr)

typedef void (*Load_Savefiles_Buffer_proc)();
#if Version_EUR
    #define Load_Savefiles_Buffer_addr 0x447170
#elif Version_JPN
    #define Load_Savefiles_Buffer_addr 0x447128
#elif Version_KOR
    #define Load_Savefiles_Buffer_addr 0x12E3C0
#elif Version_TWN
    #define Load_Savefiles_Buffer_addr 0x12E498
#else // Version_USA
    #define Load_Savefiles_Buffer_addr 0x447150
#endif
#define Load_Savefiles_Buffer ((Load_Savefiles_Buffer_proc)Load_Savefiles_Buffer_addr)

typedef void (*Actor_DrawContext_proc)(GlobalContext*, ActorContext*);
#if Version_EUR
    #define Actor_DrawContext_addr 0x461904
#elif Version_JPN
    #define Actor_DrawContext_addr 0x4618BC
#elif Version_KOR
    #define Actor_DrawContext_addr 0x1491C4
#elif Version_TWN
    #define Actor_DrawContext_addr 0x14929C
#else // Version_USA
    #define Actor_DrawContext_addr 0x4618E4
#endif
#define Actor_DrawContext ((Actor_DrawContext_proc)Actor_DrawContext_addr)

typedef void (*CollisionCheck_DrawCollision_proc)(GlobalContext*, CollisionCheckContext*);
#if Version_EUR
    #define CollisionCheck_DrawCollision_addr 0x47CAEC
#elif Version_JPN
    #define CollisionCheck_DrawCollision_addr 0x47CAA4
#elif Version_KOR
    #define CollisionCheck_DrawCollision_addr 0x15E6AC
#elif Version_TWN
    #define CollisionCheck_DrawCollision_addr 0x15E784
#else // Version_USA
    #define CollisionCheck_DrawCollision_addr 0x47CACC
#endif
#define CollisionCheck_DrawCollision ((CollisionCheck_DrawCollision_proc)CollisionCheck_DrawCollision_addr)

typedef s32 (*Room_StartTransition_proc)(GlobalContext*, RoomContext*, s32);
#if Version_JPN
    #define Room_StartTransition_addr 0x33B1D4
#else //USA & EUR
    #define Room_StartTransition_addr 0x33B6BC
#endif
#define Room_StartTransition ((Room_StartTransition_proc)Room_StartTransition_addr)

typedef s32 (*Room_ClearPrevRoom_proc)(GlobalContext*, RoomContext*);
#if Version_JPN
    #define Room_ClearPrevRoom_addr 0x36C038
#else //USA & EUR
    #define Room_ClearPrevRoom_addr 0x36C520
#endif
#define Room_ClearPrevRoom ((Room_ClearPrevRoom_proc)Room_ClearPrevRoom_addr)

typedef void (*WriteDungeonSceneTable_proc)(void);
#if Version_JPN
    #define WriteDungeonSceneTable_addr 0x2EAACC
#else //USA & EUR
    #define WriteDungeonSceneTable_addr 0x2EAFB4
#endif
#define WriteDungeonSceneTable ((WriteDungeonSceneTable_proc)WriteDungeonSceneTable_addr)

typedef void (*Collider_DrawPolyImpl_proc)(SubMainClass_32A0* sub32A0, Vec3f* vA, Vec3f* vB, Vec3f* vC,
                                           Color_RGBAf* rgba);
#if Version_USA || Version_EUR
    #define Collider_DrawPolyImpl_addr 0x2C56C4
#elif Version_JPN
    #define Collider_DrawPolyImpl_addr 0x2C51DC
#else
    #define Collider_DrawPolyImpl_addr 0
#endif
#define Collider_DrawPolyImpl ((Collider_DrawPolyImpl_proc)Collider_DrawPolyImpl_addr)

typedef void (*BgCheck_GetStaticLookupIndicesFromPos_Proc)(CollisionContext *col_ctx,Vec3f *pos,Vec3i *sector);
#if Version_USA || Version_EUR
    #define BgCheck_GetStaticLookupIndicesFromPos_addr 0x2BF5B0
#elif Version_JPN
    #define BgCheck_GetStaticLookupIndicesFromPos_addr 0x2BF0C8
#else
    #define BgCheck_GetStaticLookupIndicesFromPos_addr 0
#endif
#define BgCheck_GetStaticLookupIndicesFromPos ((BgCheck_GetStaticLookupIndicesFromPos_Proc)BgCheck_GetStaticLookupIndicesFromPos_addr)

/*
typedef void (*Item_Give_proc)(GlobalContext* globalCtx, u8 item);
#define Item_Give_addr 0x376A78
#define Item_Give ((Item_Give_proc)Item_Give_addr)

typedef u32 (*EventCheck_proc)(u32 param_1);
#define EventCheck_addr 0x350CF4
#define EventCheck ((EventCheck_proc)EventCheck_addr)

typedef void (*EventSet_proc)(u32 param_1);
#define EventSet_addr 0x34CBF8
#define EventSet ((EventSet_proc)EventSet_addr)

typedef void (*Rupees_ChangeBy_proc)(s16 rupeeChange);
#define Rupees_ChangeBy_addr 0x376A60
#define Rupees_ChangeBy ((Rupees_ChangeBy_proc)Rupees_ChangeBy_addr)

typedef void (*LinkDamage_proc)(GlobalContext* globalCtx, Player* player, s32 arg2, f32 arg3, f32 arg4, s16 arg5, s32 arg6);
#define LinkDamage_addr 0x35D304
#define LinkDamage ((LinkDamage_proc)LinkDamage_addr)

typedef u32 (*Inventory_HasEmptyBottle_proc)(void);
#define Inventory_HasEmptyBottle_addr 0x377A04
#define Inventory_HasEmptyBottle ((Inventory_HasEmptyBottle_proc)Inventory_HasEmptyBottle_addr)

typedef void (*FireDamage_proc)(Actor* player, GlobalContext* globalCtx, int flamesColor);
#define FireDamage_addr 0x35D8D8
#define FireDamage ((FireDamage_proc)FireDamage_addr)

typedef void (*Flags_SetEnv_proc)(GlobalContext* globalCtx, s16 flag);
#define Flags_SetEnv_addr 0x366704
#define Flags_SetEnv ((Flags_SetEnv_proc)Flags_SetEnv_addr)

typedef void (*GiveItem_proc)(Actor* actor, GlobalContext* globalCtx, s32 getItemId, f32 xzRange, f32 yRange)
    __attribute__((pcs("aapcs-vfp")));
#define GiveItem_addr 0x3724DC
#define GiveItem ((GiveItem_proc)0x3724DC)
*/

#endif //_Z3D_H_
