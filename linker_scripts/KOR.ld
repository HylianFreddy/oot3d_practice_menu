OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(loader_patch)
SECTIONS
{
	. = 0x100000;
	.patch_loader 0x100000 : {}
	.patch_before_GameState_Loop 0x100444 : {}
	.patch_after_GameState_Update 0x1004A0 : {}
	.patch_GameButtonInputs 0x104D0C : {}
	.patch_before_GlobalContext_Update 0x13A1F0 : {}
	.patch_after_GlobalContext_Update 0x13A248 : {}
	.patch_ActorDrawContext 0x2FC794: {}
	.patch_HaltActors 0x2FDC24 : {}
	.patch_Gfx_Update 0x311BA8 : {}

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
		__text_end  = . ;
		*(.rodata)
		*(.eh_frame)
		*(.data)
		*(.bss)
		*(COMMON)
	}
}
PROVIDE(_LD_SYMBOL_TEXT_SIZE = __text_end - __text_start);
