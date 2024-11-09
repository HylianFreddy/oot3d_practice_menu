OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(loader_patch)
SECTIONS
{
	. = 0x100000;
	.patch_loader 0x100000 : {}
	.patch_before_GameState_Loop 0x100444 : {}
	.patch_after_GameState_Update 0x1004A0 : {}
	.patch_GameButtonInputs 0x104DE4 : {}
	.patch_before_Play_Update 0x13A2C8 : {}
	.patch_after_Play_Update 0x13A320 : {}
	.patch_ActorDrawContext 0x2FC894: {}
	.patch_HaltActors 0x2FDD24 : {}
	.patch_Gfx_Update 0x311CA8 : {}

	. = 0x4CF3C4;
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
