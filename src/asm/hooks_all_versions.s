.arm
.text

.macro HOOK name
    .global hook_\name
hook_\name:
.endm

.section .loader
HOOK into_loader
    push {lr}
    bl loader_main
    bl nninitRegion
    pop {lr}
    bx lr

@ Place hooks in this section if they're valid for all versions (USA, EUR, JPN, KOR, TWN)
.section .asm_hooks.all_versions

HOOK into_Gfx_Update
    push {r0-r12, lr}
    bl advance_main
    pop {r0-r12, lr}
    pop {r4-r8, pc}

HOOK before_Play_Update
    push {r0-r12, lr}
    bl before_Play_Update
    pop {r0-r12, lr}
.if _KOR_TWN_
    cpy r8,r0
.else
    cpy r7,r0
.endif
    bx lr

HOOK after_Play_Update
    push {lr}
    push {r0-r12}
    bl after_Play_Update
    pop {r0-r12}
    bl Play_Draw
    push {r0-r12}
    bl after_Play_Draw
    pop {r0-r12}
    pop {pc}

HOOK HaltActors
    push {r0-r12,lr}
    bl Scene_HaltActorsEnabled
    cmp r0,#0x0
    pop {r0-r12,lr}
    bne PlayUpdate_PostActorUpdate
    cmp r0,#0x0
    bx lr

HOOK before_GameState_Loop
.if _KOR_TWN_
    push {r0-r12, lr}
    cpy r0,r4
    bl before_GameState_Loop
    pop {r0-r12, lr}
    cpy r0,r9
    bx lr
.else
    push {r0-r12, lr}
    cpy r0,r5
    bl before_GameState_Loop
    pop {r0-r12, lr}
    cpy r0,r4
    bx lr
.endif

HOOK after_GameState_Update
    push {r0-r12, lr}
    bl checkFastForward
    cmp r0,#0x0
    pop {r0-r12, lr}
    beq GameState_Draw @ handles drawing screen
    bx lr

HOOK ActorDrawContext
    push {r0-r12, lr}
    bl Actor_rDrawContext
    pop {r0-r12, lr}
    bx lr

HOOK GameButtonInputs
.if _KOR_TWN_
    push {r0-r12,lr}
    cpy r0,r1
    bl Commands_OverrideGameButtonInputs
    pop {r0-r12,lr}
    str r0,[r4,#0x4]
    bx lr
.else
    push {r0-r12,lr}
    cpy r0,r4
    bl Commands_OverrideGameButtonInputs
    pop {r0-r12,lr}
    add r0,r0,r9,lsl#0x4
    bx lr
.endif
