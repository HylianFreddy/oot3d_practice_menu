.arm
.text

.global hook_into_Gfx_Update
hook_into_Gfx_Update:
    push {r0-r12, lr}
    bl advance_main
    pop {r0-r12, lr}
    pop {r4-r8, pc}

.global hook_sfxprecrash
hook_sfxprecrash:
    push {r0-r12, lr}
    bl crash
    pop {r0-r12, lr}
    cmp r0,#0x0
    addeq lr,lr,#0x10 @ skip crash
    addne r0,r0,#0x14 @ continue normally
    bx lr

.global hook_sfxpostcrash
hook_sfxpostcrash:
    push {r0-r12, lr}
    bl check_crashed
    cmp r0,#0x1
    pop {r0-r12, lr}
.if _JP_==1
    beq 0x3750a8
    b 0x2dc9f8
.else
    beq 0x375590
    b 0x2dcee0
.endif

.global hook_before_GlobalContext_Update
hook_before_GlobalContext_Update:
    push {r0-r12, lr}
    bl before_GlobalContext_Update
    pop {r0-r12, lr}
    cpy r7,r0
    bx lr

.global hook_after_GlobalContext_Update
hook_after_GlobalContext_Update:
    push {r0-r12, lr}
    bl after_GlobalContext_Update
    pop {r0-r12, lr}
.if _JP_==1
    b 0x2E2108
.else
# both USA and EUR
    b 0x2E25F0
.endif

.section .loader
.global hook_into_loader
hook_into_loader:
    push {r0-r12, lr}
    bl loader_main
    pop {r0-r12, lr}
    bl 0x100028
    b  0x100004

.global hook_PlaySound
hook_PlaySound:
    push {r1-r12, lr}
    bl Cheats_RemoveBGM
    pop {r1-r12, lr}
    push {r3-r7, lr}
.if _JP_==1
    b 0x35C044
.else
    b 0x35C52C
.endif

.global hook_SetBGMEntrance
hook_SetBGMEntrance:
    push {r1-r12, lr}
    bl Cheats_RemoveBGM
    pop {r1-r12, lr}
    push {r4-r6, lr}
.if _JP_==1
    b 0x330B64
.else
    b 0x33104C
.endif

.global hook_SetBGMDayNight
hook_SetBGMDayNight:
    push {r1-r12, lr}
    bl Cheats_RemoveBGM
    pop {r1-r12, lr}
    push {r4-r6, lr}
.if _USA_==1
    b 0x483C8C
.endif
.if _EUR_==1
    b 0x483CAC
.endif
.if _JP_==1
    b 0x483C64
.endif

.global hook_SetBGMEvent
hook_SetBGMEvent:
    push {r0, r2-r12, lr}
    cpy r0,r1
    bl Cheats_RemoveBGM
    cpy r1,r0
    pop {r0, r2-r12, lr}
    push {r4-r11, lr}
.if _JP_==1
    b 0x36E75C
.else
    b 0x36EC44
.endif

.global hook_InstantTextFirstLine
hook_InstantTextFirstLine:
    cmp r9,#0x0
    bgt NoInstantText
    push {r0-r12, lr}
    bl Cheats_IsInstantText
    cmp r0,#0x1
    pop {r0-r12, lr}
    bne NoInstantText
    push {r0-r12, lr}
    ldr r0,[r5,#0x0]
    ldr r1,[r0,#0x20]
    cpy r0,r5
    blx r1
    strb r11,[r4,#0x24]
    pop {r0-r12, lr}
NoInstantText:
    cmp r10,#0xFF
    bx lr

.global hook_InstantTextBoxBreak
hook_InstantTextBoxBreak:
    push {r0-r12, lr}
    bl Cheats_IsInstantText
    cmp r0,#0x1
    pop {r0-r12, lr}
.if _JP_==1
    bne 0x2E09F8
.else
    bne 0x2E0EE0
.endif
    push {r0-r12, lr}
    ldr r0,[r5,#0x0]
    ldr r1,[r0,#0x20]
    cpy r0,r5
    blx r1
    strb r11,[r4,#0x24]
    pop {r0-r12, lr}
.if _JP_==1
    bne 0x2E09F8
.else
    bne 0x2E0EE0
.endif

.global hook_InstantTextRemoveOff
hook_InstantTextRemoveOff:
    push {r0-r12, lr}
    bl Cheats_IsInstantText
    cmp r0,#0x1
    pop {r0-r12, lr}
.if _JP_==1
    beq 0x2E09EC
    ldr r0,[r5,#0x0]
    b 0x2E01E4
.else
    beq 0x2E0ED4
    ldr r0,[r5,#0x0]
    b 0x2E06CC
.endif

.global hook_TurboTextAdvance
hook_TurboTextAdvance:
    push {r0-r12, lr}
    bl Cheats_IsTurboText
    cmp r0,#0x0
    pop {r0-r12, lr}
    cmpeq r0,#0x0
    bx lr

.global hook_TurboTextClose
hook_TurboTextClose:
    push {r0-r12, lr}
    bl Cheats_IsTurboText
    cmp r0,#0x0
    pop {r0-r12, lr}
    cmpeq r0,#0x0
    bx lr

.global hook_TurboTextSignalNPC
hook_TurboTextSignalNPC:
    movne r4,#0x1
    push {r0-r12, lr}
    bl Cheats_IsTurboText
    cmp r0,#0x0
    pop {r0-r12, lr}
    movne r4,#0x1
    bx lr

.global hook_ItemUsability
hook_ItemUsability:
    push {r0-r12, lr}
    bl Cheats_UsableItems
    pop {r0-r12, lr}
    add sp,sp,#0x14
    bx lr

.global hook_ItemUsability_Shield
hook_ItemUsability_Shield:
    push {r0-r12, lr}
    bl Cheats_areItemsForcedUsable
    cmp r0,#0x0
    pop {r0-r12, lr}
.if _USA_==1
    bne 0x42E3DC
    cmp r0,#0x0
    b 0x42E2F4
.endif
.if _EUR_==1
    bne 0x42E400
    cmp r0,#0x0
    b 0x42E2E8
.endif
.if _JP_==1
    bne 0x42E3B4
    cmp r0,#0x0
    b 0x42E2CC
.endif

.global hook_Gfx_SleepQueryCallback
hook_Gfx_SleepQueryCallback:
    push {r0-r12, lr}
    bl Gfx_SleepQueryCallback
    pop {r0-r12, lr}
    add r0,r0,#0x9C
    b 0x3FD6C8

.global hook_OverrideSceneSetup
hook_OverrideSceneSetup:
    push {r0-r12, lr}
    bl Warps_OverrideSceneSetup
    pop {r0-r12, lr}
    mov r0,#0x2
    bx lr

.global hook_LoadGame
hook_LoadGame:
    add r0, r5, r4
    push {r0-r12, lr}
    bl Settings_LoadGame
    pop {r0-r12, lr}
    bx lr
