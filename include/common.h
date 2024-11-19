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

MemInfo query_memory_permissions(u32 address);
bool is_valid_memory_read(const MemInfo* info);
bool is_valid_memory_write(const MemInfo* info);
bool isInGame();
void setAlert(char* alertMessage, u32 alertFrames);
void pauseDisplay();
void CitraPrint(const char* message, ...);

// Math stuff
#define ABS(x) ((x) >= 0 ? (x) : -(x))
f32 sqrtf(f32 x);
f32 vecLen(Vec3f v);
f32 distXYZ(Vec3f a, Vec3f b);
f32 sins(u16 angle);
f32 coss(u16 angle);
f32 arctan(f32 x);
f32 getAngleBetween(Vec3f v1, Vec3f v2);

extern u8 gInit;

#endif //_COMMON_H_
