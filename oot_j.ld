OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(loader_patch)
SECTIONS
{
	. = 0x100000;
	.patch_loader 0x100000 : {
		*(.patch_loader)
	}

	.patch_CameraUpdate 0x2D7FE0 : {
	    * (.patch_CameraUpdate)
	}

	.patch_InstantTextFirstLine 0x2DFFB4 : {
		*(.patch_InstantTextFirstLine)
	}

	.patch_InstantTextBoxBreak 0x2E017C : {
		*(.patch_InstantTextBoxBreak)
	}

	.patch_InstantTextRemoveOff 0x2E01E0 : {
		*(.patch_InstantTextRemoveOff)
	}

	.patch_ActorDrawContext 0x2E27F4: {
	    * (.patch_ActorDrawContext)
	}

	.patch_BlackScreenFix 0x2E2A60 : {
	    * (.patch_BlackScreenFix)
	}

	.patch_Gfx_Update 0x30FD6C : {
		*(.patch_Gfx_Update)
	}

	.patch_SetBGMEntrance 0x330B60 : {
		*(.patch_SetBGMEntrance)
	}

	.patch_TurboTextSignalNPC 0x34650C : {
		*(.patch_TurboTextSignalNPC)
	}

	.patch_PlaySound 0x35C040 : {
		*(.patch_PlaySound)
	}

	.patch_SetBGMEvent 0x36E758 : {
		*(.patch_SetBGMEvent)
	}

	.patch_SleepQueryCallback 0x3FD6C4 : {
		*(.patch_SleepQueryCallback)
	}

	.patch_before_GameState_Loop 0x416340 : {
		*(.patch_before_GameState_Loop)
	}

	.patch_after_GameState_Update 0x416398 : {
		*(.patch_after_GameState_Update)
	}

	.patch_GameButtonInputs 0x41ABAC : {
		*(.patch_GameButtonInputs)
	}

	.patch_ItemUsability_Shield 0x42E2C8 : {
		*(.patch_ItemUsability_Shield)
	}

	.patch_LoadGame 0x447358 : {
		*(.patch_LoadGame)
	}

	.patch_OverrideSceneSetup 0x448790 : {
		*(.patch_OverrideSceneSetup)
	}

	.patch_before_GlobalContext_Update 0x452368 : {
		*(.patch_before_GlobalContext_Update)
	}

	.patch_after_GlobalContext_Update 0x45240C : {
		*(.patch_after_GlobalContext_Update)
	}

	.patch_Actor_UpdateAll 0x461300 : {
		*(.patch_Actor_UpdateAll)
	}

	.patch_ItemUsability 0x476CF4 : {
		*(.patch_ItemUsability)
	}

	.patch_TurboTextAdvance 0x480900 : {
		*(.patch_TurboTextAdvance)
	}

	.patch_SetBGMDayNight 0x483C60 : {
		*(.patch_SetBGMDayNight)
	}

	.patch_TurboTextClose 0x4881EC : {
		*(.patch_TurboTextClose)
	}

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
		*(.rodata)
		*(.eh_frame)
		*(.data)
		*(.bss)
		*(COMMON)
		__text_end  = . ;
	}
}
