OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(loader_patch)

SECTIONS
{
	. = 0x100000;
	.patch_loader 0x100000 : { *(.patch_loader) }
	nninitRegion = 0x100024;
	.patch_before_GameState_Loop 0x100444 : { *(.patch_before_GameState_Loop) }
	.patch_WrapGameStateDraw 0x1004A0 : { *(.patch_WrapGameStateDraw) }
	GameState_Draw = 0x102880;
	.patch_GameButtonInputs 0x104D0C : { *(.patch_GameButtonInputs) }
	Play_Init = 0x11EE6C;
	Load_Savefiles_Buffer = 0x12E3C0;
	FileSelect_LoadGame = 0x12E5BC;
	Play_Main = 0x13A1E8;
	.patch_before_Play_Update 0x13A1F0 : { *(.patch_before_Play_Update) }
	.patch_after_Play_Update 0x13A248 : { *(.patch_after_Play_Update) }
	Actor_DrawContext = 0x1491C4;
	.patch_InitSubMainClass32A0 0x14BB44 : { *(.patch_InitSubMainClass32A0) }
	CollisionCheck_DrawCollision = 0x15E6AC;
	OceffSpot_End = 0x1C0844;
	BgCheck_GetStaticLookupIndicesFromPos = 0x2C17C4;
	CutsceneFlags_Set = 0x2CED18;
	Collider_DrawPolyImpl = 0x2D15D4;
	Actor_Spawn = 0x2F7CAC;
	Play_Draw = 0x2FC0A0;
	.patch_ActorDrawContext 0x2FC794: { *(.patch_ActorDrawContext) }
	.patch_HaltActors 0x2FDC24 : { *(.patch_HaltActors) }
	PlayUpdate_PostActorUpdate = 0x2FDD08;
	.patch_Gfx_Update 0x311BA8 : { *(.patch_Gfx_Update) }
	SystemArena_Malloc = 0x3232C0;

	. = 0x4CF2C4;
	. = ALIGN(4);
	.loader : {
		*(.loader*)
	}

	gActorOverlayTable = 0x5184AC;
	ControlStick_X = 0x573C38;
	ControlStick_Y = 0x573C3C;
	gSaveContext = 0x595FD0;
	gActorHeapAddress = 0x5B14B4;
	gMainClass = 0x5C5AA4;

	. = 0x005CE000;
	_LD_SYMBOL_TEXT_START = . ;
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
	_LD_SYMBOL_TEXT_SIZE = __text_end - __text_start;

	gStaticContext = 0x8080010;
	real_hid = 0x10002000;
	Z3D_TOP_SCREEN_LEFT_1 = 0x1430A900;
	Z3D_TOP_SCREEN_LEFT_2 = 0x14350E10;
	Z3D_BOTTOM_SCREEN_1 = 0x14397380;
	Z3D_BOTTOM_SCREEN_2 = 0x143CF790;
	Z3D_TOP_SCREEN_RIGHT_1 = 0x14407DD0;
	Z3D_TOP_SCREEN_RIGHT_2 = 0x1444E2E0;
	/* 0xFFFFFFFF */
}
