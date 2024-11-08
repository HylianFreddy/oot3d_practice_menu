OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(loader_patch)
SECTIONS
{
	. = 0x100000;
	.patch_loader 0x100000 : {}
	.patch_CameraUpdate 0x2D7FE0 : {}
	.patch_InstantTextFirstLine 0x2DF41C : {}
	.patch_InstantTextBoxBreak 0x2DF5E4 : {}
	.patch_InstantTextRemoveOff 0x2DF648 : {}
	.patch_ActorDrawContext 0x2E27F4: {}
	.patch_BlackScreenFix 0x2E2A60 : {}
	.patch_HaltActors 0x2E3ACC : {}
	.patch_Gfx_Update 0x30FD6C : {}
	.patch_SetBGMEntrance 0x330B60 : {}
	.patch_TurboTextSignalNPC 0x34650C : {}
	.patch_PlaySound 0x35C040 : {}
	.patch_SetBGMEvent 0x36E758 : {}
	.patch_SleepQueryCallback 0x3FD6C4 : {}
	.patch_before_GameState_Loop 0x416340 : {}
	.patch_after_GameState_Update 0x416398 : {}
	.patch_GameButtonInputs 0x41ABAC : {}
	.patch_ItemUsability_AnyAction 0x42E51C : {}
	.patch_LoadGame 0x447358 : {}
	.patch_OverrideSceneSetup 0x448790 : {}
	.patch_before_GlobalContext_Update 0x452368 : {}
	.patch_after_GlobalContext_Update 0x45240C : {}
	.patch_ItemUsability_AnyArea 0x47629C : {}
	.patch_TurboTextAdvance 0x480900 : {}
	.patch_SetBGMDayNight 0x483C60 : {}
	.patch_TurboTextClose 0x4881EC : {}

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
		*(.eh_frame)
		*(.data)
		*(.bss)
		*(COMMON)
	}
}
PROVIDE(_LD_SYMBOL_TEXT_SIZE = __text_end - __text_start);
