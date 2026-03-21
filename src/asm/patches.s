.arm

.macro PATCH name
    .section .patch_\name
    .global \name\()_patch
\name\()_patch:
.endm

PATCH loader
    bl hook_into_loader

PATCH Gfx_Update
    b hook_into_Gfx_Update

PATCH before_Play_Update
    bl hook_before_Play_Update

PATCH after_Play_Update
    b hook_after_Play_Update

PATCH Audio_PlayFanfare
    bl hook_Audio_PlayFanfare

PATCH SetBGMEntrance
    bl hook_SetBGMEntrance

PATCH SetBGMDayNight
    bl hook_SetBGMDayNight

PATCH SetBGMEvent
    bl hook_SetBGMEvent

PATCH TurboTextAdvance
    bl hook_TurboTextAdvance

PATCH TurboTextClose
    bl hook_TurboTextClose

PATCH TurboTextSignalNPC
    bl hook_TurboTextSignalNPC

PATCH ParseTextCharacter
    bl hook_ParseTextCharacter

PATCH ParseTextCharacter_Wide
    bl hook_ParseTextCharacter

PATCH ParseTextControlCode
    bl hook_ParseTextControlCode

PATCH ParseTextControlCode_Wide
    bl hook_ParseTextControlCode

PATCH ItemUsability_AnyArea
    bl hook_ItemUsability_AnyArea

PATCH ItemUsability_AnyAction
    bl hook_ItemUsability_AnyAction

PATCH SleepQueryCallback
    bl hook_SleepQueryCallback

PATCH OverrideSceneSetup
    bl hook_OverrideSceneSetup

PATCH LoadGame
    bl hook_LoadGame

PATCH CameraUpdate
    bl hook_CameraUpdate

PATCH HaltActors
    bl hook_HaltActors

PATCH before_GameState_Loop
    bl hook_before_GameState_Loop

PATCH after_GameState_Update
    bl hook_after_GameState_Update

PATCH ActorDrawContext
    bl hook_ActorDrawContext

PATCH BlackScreenFix
    bl hook_BlackScreenFix

PATCH GameButtonInputs
    bl hook_GameButtonInputs

PATCH InitSubMainClass32A0
@ pc points to current instruction +8
.if !_KOR_TWN_
    ldr r7,[pc,#0x1D4] @ 0x4646D8 on USA
.else
    ldr r8,[pc,#0x1D4]
.endif
    push {r0-r12, lr}
    cpy r0,r4 @ sub32A0 struct
    bl ColView_InitSubMainClass32A0
    pop {r0-r12, lr}
    add pc,pc,#0x18 @ 0x464530 on USA
