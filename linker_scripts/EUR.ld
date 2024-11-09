OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(loader_patch)
SECTIONS
{
	. = 0x100000;
	.patch_loader 0x100000 : {}
	.patch_CameraUpdate 0x2D84C8 : {}
	.patch_InstantTextFirstLine 0x2E049C : {}
	.patch_InstantTextBoxBreak 0x2E0664 : {}
	.patch_InstantTextRemoveOff 0x2E06C8 : {}
	.patch_ActorDrawContext 0x2E2CDC: {}
	.patch_BlackScreenFix 0x2E2F48 : {}
	.patch_HaltActors 0x2E3FB4 : {}
	.patch_Gfx_Update 0x310254 : {}
	.patch_SetBGMEntrance 0x331048 : {}
	.patch_TurboTextSignalNPC 0x3469F4 : {}
	.patch_PlaySound 0x35C528 : {}
	.patch_SetBGMEvent 0x36EC40 : {}
	.patch_SleepQueryCallback 0x3FD6C4 : {}
	.patch_before_GameState_Loop 0x416340 : {}
	.patch_after_GameState_Update 0x416398 : {}
	.patch_GameButtonInputs 0x41ABF8 : {}
	.patch_ItemUsability_AnyAction 0x42E568 : {}
	.patch_LoadGame 0x4473A0 : {}
	.patch_OverrideSceneSetup 0x4487D8 : {}
	.patch_before_GlobalContext_Update 0x4523B0 : {}
	.patch_after_GlobalContext_Update 0x452454 : {}
	.patch_ItemUsability_AnyArea 0x4762E4 : {}
	.patch_TurboTextAdvance 0x480948 : {}
	.patch_SetBGMDayNight 0x483CA8 : {}
	.patch_TurboTextClose 0x488234 : {}

	. = 0x4C99A8;
	. = ALIGN(4);
	.loader : {
		*(.loader*)
	}

	. = 0x005C7000;
	.text : {
		__text_start = . ;
		*(.text)
		*(.text.*)
		__text_end  = . ;
		*(.rodata)
		*(.rodata.*)
		*(.data)
		*(.bss)
		*(COMMON)
	}

	/DISCARD/ : {
		*(.eh_frame)
	}
}
PROVIDE(_LD_SYMBOL_TEXT_SIZE = __text_end - __text_start);
