#ifndef _COMMON_H_
#define _COMMON_H_

#include "3ds/svc.h"
#include "lib/printf.h"
#include "z3d/z3Dvec.h"
#include <stdarg.h>

#define UNSUPPORTED_WARNING ( \
    Version_KOR ? "Unsupported on KOR" : \
    Version_TWN ? "Unsupported on TWN" : \
    "" \
)

extern MemInfo query_memory_permissions(u32 address);
extern bool is_valid_memory_read(const MemInfo* info);
extern bool is_valid_memory_write(const MemInfo* info);
extern bool isInGame();
extern void drawAlert();
extern void setAlert(char* alertMessage, u32 alertFrames);
extern void pauseDisplay();
void CitraPrint(const char* message, ...);
f32 sins(u16 angle);
f32 coss(u16 angle);

extern u8 gInit;

#endif //_COMMON_H_
