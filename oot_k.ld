OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(loader_patch)
SECTIONS
{
	. = 0x100000;
	.patch_loader 0x100000 : {
		*(.patch_loader)
	}

	.patch_before_GameState_Loop 0x100444 : {
		*(.patch_before_GameState_Loop)
	}

	.patch_after_GameState_Update 0x1004A0 : {
		*(.patch_after_GameState_Update)
	}

	.patch_GameButtonInputs 0x104D0C : {
		*(.patch_GameButtonInputs)
	}

	.patch_before_GlobalContext_Update 0x13A1f0 : {
		*(.patch_before_GlobalContext_Update)
	}

	.patch_after_GlobalContext_Update 0x13A248 : {
		*(.patch_after_GlobalContext_Update)
	}

	.patch_Actor_UpdateAll 0x148C5C : {
		*(.patch_Actor_UpdateAll)
	}

	.patch_Gfx_Update 0x311BA8 : {
		*(.patch_Gfx_Update)
	}

	. = 0x4CF2C4;
	. = ALIGN(4);
	.loader : {
		*(.loader*)
	}

	. = 0x005CE000;
	.text : {
		__text_start = . ;
		*(.text)
		*(.text.*)
		*(.rodata)
		*(.data)
		*(.bss)
		*(COMMON)
		__text_end  = . ;
	}
}
