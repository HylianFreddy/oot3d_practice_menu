OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(loader_patch)
SECTIONS
{
	_LD_OFF = 0;

	. = 0x100000 + _LD_OFF;
	.patch_loader 0x100000 + _LD_OFF : { *(.patch_loader) }
	nninitRegion = 0x100028 + _LD_OFF;
	OceffSpot_End = 0x10FBD0 + _LD_OFF;
	EffectBlure_Update = 0x226D04 + _LD_OFF;
	BgCheck_GetStaticLookupIndicesFromPos = 0x2BD988 + _LD_OFF;
	Collider_DrawPolyImpl = 0x2C3B48 + _LD_OFF;
	Math_SinS = 0x2CE62C + _LD_OFF;
	EffectSs_Delete = 0x2D5004 + _LD_OFF;
	.patch_CameraUpdate 0x2DB6C0 + _LD_OFF : { *(.patch_CameraUpdate) }
	.patch_ParseTextCharacter_Wide 0x2DDC48 + _LD_OFF : { *(.patch_ParseTextCharacter_Wide) }
	.patch_ParseTextControlCode_Wide 0x2DDD38 + _LD_OFF : { *(.patch_ParseTextControlCode_Wide) }
	.patch_ParseTextCharacter 0x2DE7E0 + _LD_OFF : { *(.patch_ParseTextCharacter) }
	.patch_ParseTextControlCode 0x2DE8D0 + _LD_OFF : { *(.patch_ParseTextControlCode) }
	Play_Draw = 0x2E0970 + _LD_OFF;
	.patch_ActorDrawContext 0x2E105C + _LD_OFF : { *(.patch_ActorDrawContext) }
	.patch_BlackScreenFix 0x2E135C + _LD_OFF : { *(.patch_BlackScreenFix) }
	.patch_HaltActors 0x2E2480 + _LD_OFF : { *(.patch_HaltActors) }
	PlayUpdate_PostActorUpdate = 0x2E2558 + _LD_OFF;
	WriteDungeonSceneTable = 0x2E8FF8 + _LD_OFF;
	.patch_Gfx_Update 0x30CB74 + _LD_OFF : { *(.patch_Gfx_Update) }
	Object_Spawn = 0x32A92C + _LD_OFF;
	.patch_SetBGMEntrance 0x32D75C + _LD_OFF : { *(.patch_SetBGMEntrance) }
	Math_CosS = 0x335670 + _LD_OFF;
	Room_StartTransition = 0x337DA4 + _LD_OFF;
	.patch_TurboTextSignalNPC 0x343100 + _LD_OFF : { *(.patch_TurboTextSignalNPC) }
	Player_SetEquipmentData = 0x345080 + _LD_OFF;
	Flags_SetEventChkInf = 0x348B3C + _LD_OFF;
	Effect_Delete = 0x34B038 + _LD_OFF;
	SystemArena_Malloc = 0x34C050 + _LD_OFF;
	Flags_GetEventChkInf = 0x34CC38 + _LD_OFF;
	Audio_PlayFanfare = 0x35844C + _LD_OFF;
	.patch_Audio_PlayFanfare 0x358454 + _LD_OFF : { *(.patch_Audio_PlayFanfare) }
	Player_LinkDamage = 0x359228 + _LD_OFF;
	Player_FireDamage = 0x3597FC + _LD_OFF;
	CutsceneFlags_Set = 0x362628 + _LD_OFF;
	Message_StartTextbox = 0x363BA0 + _LD_OFF;
	Room_ClearPrevRoom = 0x368444 + _LD_OFF;
	.patch_SetBGMEvent 0x36AB68 + _LD_OFF : { *(.patch_SetBGMEvent) }
	Actor_HasParent = 0x36DD64 + _LD_OFF;
	Actor_OfferGetItem = 0x36E400 + _LD_OFF;
	Message_CloseTextbox = 0x36E504 + _LD_OFF;
	Actor_Spawn = 0x36F7F4 + _LD_OFF;
	Rupees_ChangeBy = 0x372950 + _LD_OFF;
	Item_Give = 0x372968 + _LD_OFF;
	Inventory_HasEmptyBottle = 0x3738F4 + _LD_OFF;
	.patch_SleepQueryCallback 0x3FAB58 + _LD_OFF : { *(.patch_SleepQueryCallback) }
	Message_SetInstantText = 0x40869C + _LD_OFF;
	.patch_before_GameState_Loop 0x4133DC + _LD_OFF : { *(.patch_before_GameState_Loop) }
	.patch_WrapGameStateDraw 0x413438 + _LD_OFF : { *(.patch_WrapGameStateDraw) }
	GameState_Draw = 0x415C54 + _LD_OFF;

	/* 0x416EE4 - 0x4171E3 : Language related function, don't place anything in this range */
	/* 0x4171E4 */ _LD_OFF = _LD_DEMO_EUR ? 0x50 : _LD_OFF;

	.patch_GameButtonInputs 0x417A94 + _LD_OFF : { *(.patch_GameButtonInputs) }
	.patch_ItemUsability_AnyAction 0x42BE8C + _LD_OFF : { *(.patch_ItemUsability_AnyAction) }
	Play_Init = 0x432B20 + _LD_OFF;
	Load_Savefiles_Buffer = 0x44326C + _LD_OFF;
	FileSelect_LoadGame = 0x443478 + _LD_OFF;
	.patch_LoadGame 0x44349C + _LD_OFF : { *(.patch_LoadGame) }
	.patch_OverrideSceneLayer 0x4448D4 + _LD_OFF : { *(.patch_OverrideSceneLayer) }
	Play_Main = 0x44E114 + _LD_OFF;
	.patch_before_Play_Update 0x44E118 + _LD_OFF : { *(.patch_before_Play_Update) }
	.patch_after_Play_Update 0x44E1BC + _LD_OFF : { *(.patch_after_Play_Update) }
	Actor_DrawContext = 0x45D9BC + _LD_OFF;
	.patch_InitSubMainClass32A0 0x4605D4 + _LD_OFF : { *(.patch_InitSubMainClass32A0) }
	.patch_ItemUsability_AnyArea 0x46FD90 + _LD_OFF : { *(.patch_ItemUsability_AnyArea) }
	CollisionCheck_DrawCollision = 0x47607C + _LD_OFF;
	.patch_TurboTextAdvance 0x47B004 + _LD_OFF : { *(.patch_TurboTextAdvance) }
	.patch_SetBGMDayNight 0x481100 + _LD_OFF : { *(.patch_SetBGMDayNight) }
	.patch_TurboTextClose 0x4846D4 + _LD_OFF : { *(.patch_TurboTextClose) }

	/* 0x4A5B00 */ _LD_OFF = _LD_DEMO_EUR ? 0x0 : _LD_OFF;

	. = 0x4C6CCC + _LD_OFF;
	. = ALIGN(4);
	.loader : {
		*(.loader*)
	}

	gGearUsabilityTable = 0x4D1858 + _LD_OFF;
	gDrawItemTable = 0x4D61A4 + _LD_OFF;
	gDungeonSceneTable = 0x4DA244 + _LD_OFF;
	gMQDungeonSceneTable = 0x4DA9EC + _LD_OFF;
	gItemUsabilityTable = 0x504C58 + _LD_OFF;
	gRandInt = 0x50A118 + _LD_OFF;
	gRandFloat = 0x50A11C + _LD_OFF;
	gActorOverlayTable = 0x50ADD8 + _LD_OFF;
	gRestrictionFlags = 0x537E48 + _LD_OFF;
	gObjectTable = 0x53AD78 + _LD_OFF;
	gEntranceTable = 0x541C3C + _LD_OFF;
	gSceneTable = 0x543508 + _LD_OFF;
	ControlStick_X = 0x5637A8 + _LD_OFF;
	ControlStick_Y = 0x5637AC + _LD_OFF;
	gSaveContext = 0x585B48 + _LD_OFF;
	gEffectContext = 0x5894D8 + _LD_OFF;
	gEffectSsInfo = 0x596728 + _LD_OFF;
	gActorHeapAddress = 0x5A1034 + _LD_OFF;
	gMainClass = 0x5BC7B8 + _LD_OFF;

	. = 0x005C5000 + _LD_OFF;
	_LD_TEXT_START = . ;
	.text : {
		__text_start = . ;
		*(.text)
		*(.text.*)
		*(.asm_hooks.*)
		__text_end = . ;
		*(.rodata)
		*(.rodata.*)
		*(.data)
		*(.bss)
		*(COMMON)
	}
	_LD_TEXT_SIZE = __text_end - __text_start;

	gStaticContext = 0x8080010 + _LD_OFF;
	real_hid = 0x10002000 + _LD_OFF;
	Z3D_TOP_SCREEN_LEFT_1 = 0x14313890 + _LD_OFF;
	Z3D_TOP_SCREEN_LEFT_2 = 0x14359DA0 + _LD_OFF;
	Z3D_BOTTOM_SCREEN_1 = 0x143A02B0 + _LD_OFF;
	Z3D_BOTTOM_SCREEN_2 = 0x143D86C0 + _LD_OFF;
	Z3D_TOP_SCREEN_RIGHT_1 = 0x14410AD0 + _LD_OFF;
	Z3D_TOP_SCREEN_RIGHT_2 = 0x14456FE0 + _LD_OFF;

	/* 0xFFFFFFFF */
}
