#include "common.h"
#include "z3D/z3D.h"

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
    return gInit && gSaveContext.gameMode != 2 && gGlobalContext->state.running && PLAYER;
}

void CitraPrint(const char* message, ...) {
    va_list args;
    va_start(args, message);
    char buf[128];
    int length = vsnprintf(buf, 128, message, args);
    svcOutputDebugString(buf, length);
    va_end(args);
}
