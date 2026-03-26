OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(loader_patch)
SECTIONS
{
	_LD_OFF = 0;

	. = 0x100000 + _LD_OFF;
	.patch_loader 0x100000 + _LD_OFF : { *(.patch_loader) }
	nninitRegion = 0x100024 + _LD_OFF;
	.patch_before_GameState_Loop 0x100444 + _LD_OFF : { *(.patch_before_GameState_Loop) }
	.patch_WrapGameStateDraw 0x1004A0 + _LD_OFF : { *(.patch_WrapGameStateDraw) }
	GameState_Draw = 0x102880 + _LD_OFF;

	/* 0x103E78 - 0x104287 : Language related function, don't place anything in this range */
	/* 0x104288 */ _LD_OFF = _LD_TWN ? 0xD8 : _LD_OFF;

	.patch_GameButtonInputs 0x104D0C + _LD_OFF : { *(.patch_GameButtonInputs) }
	.patch_ItemUsability_AnyAction 0x118220 + _LD_OFF : { *(.patch_ItemUsability_AnyAction) }
	Play_Init = 0x11EE6C + _LD_OFF;
	Load_Savefiles_Buffer = 0x12E3C0 + _LD_OFF;
	FileSelect_LoadGame = 0x12E5BC + _LD_OFF;
	.patch_LoadGame 0x12E5E0 + _LD_OFF : { *(.patch_LoadGame) }
	.patch_OverrideSceneLayer 0x12FB34 + _LD_OFF : { *(.patch_OverrideSceneLayer) }
	Play_Main = 0x13A1E8 + _LD_OFF;
	.patch_before_Play_Update 0x13A1F0 + _LD_OFF : { *(.patch_before_Play_Update) }
	.patch_after_Play_Update 0x13A248 + _LD_OFF : { *(.patch_after_Play_Update) }
	Actor_DrawContext = 0x1491C4 + _LD_OFF;
	.patch_InitSubMainClass32A0 0x14BB44 + _LD_OFF : { *(.patch_InitSubMainClass32A0) }
	CollisionCheck_DrawCollision = 0x15E6AC + _LD_OFF;
	.patch_TurboTextAdvance 0x162B7C + _LD_OFF : { *(.patch_TurboTextAdvance) }
	.patch_SetBGMDayNight 0x169154 + _LD_OFF : { *(.patch_SetBGMDayNight) }
	.patch_TurboTextClose 0x16D4F0 + _LD_OFF : { *(.patch_TurboTextClose) }

	/* 0x188000 - 0x19B1BF : Unknown code with regional differences, don't place anything in this range */
	/* 0x19B1C0 */ _LD_OFF = _LD_TWN ? 0x100 : _LD_OFF;

	OceffSpot_End = 0x1C0844 + _LD_OFF;
	Room_StartTransition = 0x2842FC + _LD_OFF;
	BgCheck_GetStaticLookupIndicesFromPos = 0x2C17C4 + _LD_OFF;
	Room_ClearPrevRoom = 0x2C304C + _LD_OFF;
	CutsceneFlags_Set = 0x2CED18 + _LD_OFF;
	Collider_DrawPolyImpl = 0x2D15D4 + _LD_OFF;
	.patch_TurboTextSignalNPC 0x2DFDFC + _LD_OFF : { *(.patch_TurboTextSignalNPC) }
	Audio_PlayFanfare = 0x2E0EFC + _LD_OFF;
	.patch_Audio_PlayFanfare 0x2E0F04 + _LD_OFF : { *(.patch_Audio_PlayFanfare) }
	.patch_CameraUpdate 0x2ED1C8 + _LD_OFF : { *(.patch_CameraUpdate) }
	Message_CloseTextbox = 0x2EEFAC + _LD_OFF;
	Object_Spawn = 0x2F5DCC + _LD_OFF;
	.patch_SetBGMEntrance 0x2F69B4 + _LD_OFF : { *(.patch_SetBGMEntrance) }
	.patch_SetBGMEvent 0x2F6BB0 + _LD_OFF : { *(.patch_SetBGMEvent) }
	Actor_Spawn = 0x2F7CAC + _LD_OFF;
	.patch_ParseTextCharacter_Wide 0x2F926C + _LD_OFF : { *(.patch_ParseTextCharacter_Wide) }
	.patch_ParseTextControlCode_Wide 0x2F9348 + _LD_OFF : { *(.patch_ParseTextControlCode_Wide) }
	.patch_ParseTextCharacter 0x2F9E60 + _LD_OFF : { *(.patch_ParseTextCharacter) }
	.patch_ParseTextControlCode 0x2F9F3C + _LD_OFF : { *(.patch_ParseTextControlCode) }
	Play_Draw = 0x2FC0A0 + _LD_OFF;
	.patch_ActorDrawContext 0x2FC794 + _LD_OFF: { *(.patch_ActorDrawContext) }
	.patch_BlackScreenFix 0x2FC9F0 + _LD_OFF : { *(.patch_BlackScreenFix) }
	.patch_HaltActors 0x2FDC24 + _LD_OFF : { *(.patch_HaltActors) }
	PlayUpdate_PostActorUpdate = 0x2FDD08 + _LD_OFF;
	WriteDungeonSceneTable = 0x305710 + _LD_OFF;
	.patch_ItemUsability_AnyArea 0x305B78 + _LD_OFF : { *(.patch_ItemUsability_AnyArea) }
	Player_SetEquipmentData = 0x306E34 + _LD_OFF;
	.patch_Gfx_Update 0x311BA8 + _LD_OFF : { *(.patch_Gfx_Update) }
	SystemArena_Malloc = 0x3232C0 + _LD_OFF;
	EffectBlure_Update = 0x397EC0 + _LD_OFF;
	.patch_SleepQueryCallback 0x4ACC30 + _LD_OFF : { *(.patch_SleepQueryCallback) }
	Message_SetInstantText = 0x4BC5BC + _LD_OFF;

	. = 0x4CF2C4 + _LD_OFF;
	. = ALIGN(4);
	.loader : {
		*(.loader*)
	}

	/* 0x4D0000 */ _LD_OFF = _LD_TWN ? 0 : _LD_OFF;

	gGearUsabilityTable = 0x4DC7C0 + _LD_OFF;
	gDrawItemTable = 0x4E0AB0 + _LD_OFF;
	gItemUsabilityTable = 0x513110 + _LD_OFF;
	gActorOverlayTable = 0x5184AC + _LD_OFF;
	gEntranceTable = 0x5518F8 + _LD_OFF;
	gSceneTable = 0x5531D8 + _LD_OFF;
	ControlStick_X = 0x573C38 + _LD_OFF;
	ControlStick_Y = 0x573C3C + _LD_OFF;
	gSaveContext = 0x595FD0 + _LD_OFF;
	gActorHeapAddress = 0x5B14B4 + _LD_OFF;
	gMainClass = 0x5C5AA4 + _LD_OFF;
	nnMainLoopFlag = 0x5CDB1D + _LD_OFF;

	. = 0x005CE000 + _LD_OFF;
	_LD_TEXT_START = . ;
	.text : {
		__text_start = . ;
		*(.text)
		*(.text.*)
		*(.asm_hooks.all_versions)
		__text_end = . ;
		*(.rodata)
		*(.rodata.*)
		*(.data)
		*(.bss)
		*(COMMON)
	}
	_LD_TEXT_SIZE = __text_end - __text_start;

	/DISCARD/ : {
		/* Discard patches not implemented for KOR/TWN */
		*(.patch_*)
	}

	gStaticContext = 0x8080010 + _LD_OFF;
	real_hid = 0x10002000 + _LD_OFF;
	Z3D_TOP_SCREEN_LEFT_1 = 0x1430A900 + _LD_OFF;
	Z3D_TOP_SCREEN_LEFT_2 = 0x14350E10 + _LD_OFF;
	Z3D_BOTTOM_SCREEN_1 = 0x14397380 + _LD_OFF;
	Z3D_BOTTOM_SCREEN_2 = 0x143CF790 + _LD_OFF;
	Z3D_TOP_SCREEN_RIGHT_1 = 0x14407DD0 + _LD_OFF;
	Z3D_TOP_SCREEN_RIGHT_2 = 0x1444E2E0 + _LD_OFF;

	/* 0xFFFFFFFF */
}
