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
	EffectBlure_Update = 0x227000 + _LD_OFF;

	/* 0x2B6D4C */ _LD_OFF = _LD_JPN ? 0xC0B5C : _LD_OFF;

	/* 0x2B7234 */ _LD_OFF = _LD_JPN ? -0x4E8 : _LD_OFF;

	BgCheck_GetStaticLookupIndicesFromPos = 0x2BF5B0 + _LD_OFF;
	Collider_DrawPolyImpl = 0x2C56C4 + _LD_OFF;
	Math_SinS = 0x2CFCA0 + _LD_OFF;
	EffectSs_Delete = 0x2D6A50 + _LD_OFF;
	.patch_CameraUpdate 0x2D84C8 + _LD_OFF : { *(.patch_CameraUpdate) }
	.patch_ParseTextCharacter_Wide 0x2DF8F4 + _LD_OFF : { *(.patch_ParseTextCharacter_Wide) }
	.patch_ParseTextControlCode_Wide 0x2DF9E4 + _LD_OFF : { *(.patch_ParseTextControlCode_Wide) }
	.patch_ParseTextCharacter 0x2E048C + _LD_OFF : { *(.patch_ParseTextCharacter) }
	.patch_ParseTextControlCode 0x2E057C + _LD_OFF : { *(.patch_ParseTextControlCode) }
	Play_Draw = 0x2E25F0 + _LD_OFF;
	.patch_ActorDrawContext 0x2E2CDC + _LD_OFF: { *(.patch_ActorDrawContext) }
	.patch_BlackScreenFix 0x2E2F48 + _LD_OFF : { *(.patch_BlackScreenFix) }
	.patch_HaltActors 0x2E3FB4 + _LD_OFF : { *(.patch_HaltActors) }
	PlayUpdate_PostActorUpdate = 0x2E4090 + _LD_OFF;
	WriteDungeonSceneTable = 0x2EAFB4 + _LD_OFF;
	.patch_Gfx_Update 0x310254 + _LD_OFF : { *(.patch_Gfx_Update) }
	Object_Spawn = 0x32E21C + _LD_OFF;
	.patch_SetBGMEntrance 0x33104C + _LD_OFF : { *(.patch_SetBGMEntrance) }
	Math_CosS = 0x338F60 + _LD_OFF;
	Room_StartTransition = 0x33B6BC + _LD_OFF;
	.patch_TurboTextSignalNPC 0x3469F4 + _LD_OFF : { *(.patch_TurboTextSignalNPC) }
	Player_SetEquipmentData = 0x34913C + _LD_OFF;
	Flags_SetEventChkInf = 0x34CBF8 + _LD_OFF;
	Effect_Delete = 0x34F0F4 + _LD_OFF;
	SystemArena_Malloc = 0x35010C + _LD_OFF;
	Flags_GetEventChkInf = 0x350CF4 + _LD_OFF;
	Audio_PlayFanfare = 0x35C528 + _LD_OFF;
	.patch_Audio_PlayFanfare 0x35C530 + _LD_OFF : { *(.patch_Audio_PlayFanfare) }
	Player_LinkDamage = 0x35D304 + _LD_OFF;
	Player_FireDamage = 0x35D8D8 + _LD_OFF;
	CutsceneFlags_Set = 0x366704 + _LD_OFF;
	Message_StartTextbox = 0x367C7C + _LD_OFF;
	Room_ClearPrevRoom = 0x36C520 + _LD_OFF;
	.patch_SetBGMEvent 0x36EC44 + _LD_OFF : { *(.patch_SetBGMEvent) }
	Actor_HasParent = 0x371E40 + _LD_OFF;
	Actor_OfferGetItem = 0x3724DC + _LD_OFF;
	Message_CloseTextbox = 0x3725E0 + _LD_OFF;
	Actor_Spawn = 0x3738D0 + _LD_OFF;
	Rupees_ChangeBy = 0x376A60 + _LD_OFF;
	Item_Give = 0x376A78 + _LD_OFF;
	Inventory_HasEmptyBottle = 0x377A04 + _LD_OFF;

	/* 0x377D90 */ _LD_OFF = _LD_JPN ? 0x0 : _LD_OFF;

	.patch_SleepQueryCallback 0x3FD6DC + _LD_OFF : { *(.patch_SleepQueryCallback) }
	Message_SetInstantText = 0x40B608 + _LD_OFF;
	.patch_before_GameState_Loop 0x416340 + _LD_OFF : { *(.patch_before_GameState_Loop) }
	.patch_WrapGameStateDraw 0x416398 + _LD_OFF : { *(.patch_WrapGameStateDraw) }
	GameState_Draw = 0x418B88 + _LD_OFF;

	/* 0x419E18 - 0x41A143 : Language related function, don't place anything in this range */
	/* 0x41A144 */ _LD_OFF = _LD_EUR ? 0x24 : _LD_JPN ? -0x28 : _LD_OFF;

	.patch_GameButtonInputs 0x41ABD4 + _LD_OFF : { *(.patch_GameButtonInputs) }
	.patch_ItemUsability_AnyAction 0x42E560 + _LD_OFF : { *(.patch_ItemUsability_AnyAction) }
	Play_Init = 0x4352F0 + _LD_OFF;

	/* 0x436690 */ _LD_OFF = _LD_EUR ? 0x20 : _LD_OFF;

	Load_Savefiles_Buffer = 0x447150 + _LD_OFF;
	FileSelect_LoadGame = 0x44735C + _LD_OFF;
	.patch_LoadGame 0x447380 + _LD_OFF : { *(.patch_LoadGame) }
	.patch_OverrideSceneLayer 0x4487B8 + _LD_OFF : { *(.patch_OverrideSceneLayer) }
	Play_Main = 0x45238C + _LD_OFF;
	.patch_before_Play_Update 0x452390 + _LD_OFF : { *(.patch_before_Play_Update) }
	.patch_after_Play_Update 0x452434 + _LD_OFF : { *(.patch_after_Play_Update) }
	Actor_DrawContext = 0x4618E4 + _LD_OFF;
	.patch_InitSubMainClass32A0 0x4644FC + _LD_OFF : { *(.patch_InitSubMainClass32A0) }
	.patch_ItemUsability_AnyArea 0x4762C4 + _LD_OFF : { *(.patch_ItemUsability_AnyArea) }
	CollisionCheck_DrawCollision = 0x47CACC + _LD_OFF;
	.patch_TurboTextAdvance 0x480928 + _LD_OFF : { *(.patch_TurboTextAdvance) }
	.patch_SetBGMDayNight 0x483C8C + _LD_OFF : { *(.patch_SetBGMDayNight) }
	.patch_TurboTextClose 0x488214 + _LD_OFF : { *(.patch_TurboTextClose) }

	/* 0x4A5B00 */ _LD_OFF = _LD_EUR || _LD_JPN ? 0x0 : _LD_OFF;

	. = 0x4C99A8 + _LD_OFF;
	. = ALIGN(4);
	.loader : {
		*(.loader*)
	}

	gGearUsabilityTable = 0x4D47C8 + _LD_OFF;
	gDrawItemTable = 0x4D88C8 + _LD_OFF;
	gDungeonSceneTable = 0x4DC400 + _LD_OFF;
	gMQDungeonSceneTable = 0x4DCBA8 + _LD_OFF;
	gItemUsabilityTable = 0x506C58 + _LD_OFF;
	gRandInt = 0x50C0C4 + _LD_OFF;
	gRandFloat = 0x50C0C8 + _LD_OFF;
	gActorOverlayTable = 0x50CD84 + _LD_OFF;
	gRestrictionFlags = 0x539DC4 + _LD_OFF;
	gObjectTable = 0x53CCF4 + _LD_OFF;
	gEntranceTable = 0x543BB8 + _LD_OFF;
	gSceneTable = 0x545484 + _LD_OFF;
	ControlStick_X = 0x5655C0 + _LD_OFF;
	ControlStick_Y = 0x5655C4 + _LD_OFF;
	gSaveContext = 0x587958 + _LD_OFF;
	gEffectContext = 0x58B2E0 + _LD_OFF;
	gEffectSsInfo = 0x598530 + _LD_OFF;
	gActorHeapAddress = 0x5A2E3C + _LD_OFF;
	gMainClass = 0x5BE5B8 + _LD_OFF;

	. = 0x005C7000 + _LD_OFF;
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

	gStaticContext = 0x08080010 + _LD_OFF;
	real_hid = 0x10002000 + _LD_OFF;
	Z3D_TOP_SCREEN_LEFT_1 = 0x14313890 + _LD_OFF;
	Z3D_TOP_SCREEN_LEFT_2 = 0x14359DA0 + _LD_OFF;
	Z3D_BOTTOM_SCREEN_1 = 0x143A02B0 + _LD_OFF;
	Z3D_BOTTOM_SCREEN_2 = 0x143D86C0 + _LD_OFF;
	Z3D_TOP_SCREEN_RIGHT_1 = 0x14410AD0 + _LD_OFF;
	Z3D_TOP_SCREEN_RIGHT_2 = 0x14456FE0 + _LD_OFF;

	/* 0xFFFFFFFF */
}
