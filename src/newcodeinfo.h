#ifndef _NEWCODEINFO_H_
#define _NEWCODEINFO_H_

#include "3ds/types.h"

#if Version_KOR || Version_TWN
    #define NEWCODE_OFFSET 0x005CE000
#else
    #define NEWCODE_OFFSET 0x005C7000
#endif

/*
 * Code size can only be known at link time, so the linker will need a way to
 * calculate it and insert the value in the compiled code.
 * This variable is fake and will not be allocated. Its symbol is defined in the
 * linker script as the total size of the code sections, so that the C code can
 * use that value through the variable's address.
 */
extern char _LD_SYMBOL_TEXT_SIZE;
#define NEWCODE_SIZE ((u32)&_LD_SYMBOL_TEXT_SIZE)

#endif //_NEWCODEINFO_H_
