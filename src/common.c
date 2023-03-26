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
    if (!gInit || gSaveContext.gameMode == 2) {
        return false;
    }
    // this is to make sure the player actor exists
    const MemInfo address_info = query_memory_permissions((int)&(PLAYER->meleeWeaponState));
    return is_valid_memory_read(&address_info);
}
