#include "common.h"
#include "z3D/z3D.h"
#include <math.h>

#define ABS(x) ((x) >= 0 ? (x) : -(x))

MemInfo query_memory_permissions(u32 address) {
    MemInfo memory_info = {};
    PageInfo page_info = {};
    svcQueryMemory(&memory_info, &page_info, address);
    return memory_info;
}

bool is_valid_memory_read(const MemInfo* info) {
    return (info->perm & MEMPERM_READ) != 0;
}

bool is_valid_memory_write(const MemInfo* info) {
    return (info->perm & MEMPERM_WRITE) != 0;
}

bool isInGame() {
    return gInit && gGlobalContext->state.main == Play_Main && gGlobalContext->state.running && PLAYER;
}

void CitraPrint(const char* message, ...) {
    va_list args;
    va_start(args, message);
    char buf[128];
    int length = vsnprintf(buf, 128, message, args);
    svcOutputDebugString(buf, length);
    va_end(args);
}

f32 sqrtf(f32 x) {
    f32 n = (1 + x) * 0.5;

    while (n * n < x * 0.999f || n * n > x * 1.001f) {
        n = (n + x / n) * 0.5;
    }

    return n;
}

f32 vecLen(Vec3f v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

f32 distXYZ(Vec3f a, Vec3f b) {
    return vecLen((Vec3f){
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
    });
}

f32 sins(u16 angle) {
    // Taylor expansion up to x^7. Use symmetries for larger angles.
    if (angle <= 0x4000) {
        f32 theta = angle * 0.0000958737992429, theta2 = theta * theta, result = theta;
        theta *= theta2 * 0.166666666667;
        result -= theta;
        theta *= theta2 * 0.05;
        result += theta;
        theta *= theta2 * 0.0238095238095;
        result -= theta;
        return result;
    } else if (angle <= 0x8000) {
        return sins(0x8000 - angle);
    }
    return -sins(angle - 0x8000);
}

f32 coss(u16 angle) {
    return sins(angle + 0x4000);
}

f32 arctan(f32 x) {
    // arctan approximation function
    return 8*x/(3+sqrtf(25+(80*x*x/3)));
}

f32 getAngle(f32 x, f32 y) {
    if (x == 0.0) {
        return (y >= 0 ? M_PI_2 : -M_PI_2);
    }
    f32 a = arctan(y/x);
    if (x < 0) {
        a += M_PI;
    }
    return a;
}
