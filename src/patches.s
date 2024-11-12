.arm

.section .patch_loader
.global loader_patch
loader_patch:
    b hook_into_loader

.section .patch_Gfx_Update
    b hook_into_Gfx_Update

.section .patch_before_Play_Update
    bl hook_before_Play_Update

.section .patch_after_Play_Update
    b hook_after_Play_Update

.section .patch_PlaySound
    b hook_PlaySound

.section .patch_SetBGMEntrance
    b hook_SetBGMEntrance

.section .patch_SetBGMDayNight
    b hook_SetBGMDayNight

.section .patch_SetBGMEvent
    b hook_SetBGMEvent

.section .patch_TurboTextAdvance
    bl hook_TurboTextAdvance

.section .patch_TurboTextClose
    bl hook_TurboTextClose

.section .patch_TurboTextSignalNPC
    bl hook_TurboTextSignalNPC

.section .patch_InstantTextFirstLine
    bl hook_InstantTextFirstLine

.section .patch_InstantTextBoxBreak
    b hook_InstantTextBoxBreak

.section .patch_InstantTextRemoveOff
    b hook_InstantTextRemoveOff

.section .patch_ItemUsability_AnyArea
    bl hook_ItemUsability_AnyArea

.section .patch_ItemUsability_AnyAction
    bl hook_ItemUsability_AnyAction

.section .patch_SleepQueryCallback
    b hook_Gfx_SleepQueryCallback

.section .patch_OverrideSceneSetup
    bl hook_OverrideSceneSetup

.section .patch_LoadGame
    bl hook_LoadGame

.section .patch_CameraUpdate
    bl hook_CameraUpdate

.section .patch_HaltActors
    bl hook_HaltActors

.section .patch_before_GameState_Loop
    bl hook_before_GameState_Loop

.section .patch_after_GameState_Update
    bl hook_after_GameState_Update

.section .patch_ActorDrawContext
    bl hook_ActorDrawContext

.section .patch_BlackScreenFix
    bl hook_BlackScreenFix

.section .patch_GameButtonInputs
    bl hook_GameButtonInputs

.section .patch_InitSubMainClass32A0
@ pc points to current instruction +8
.if (_USA_ || _EUR_ || _JPN_)
    ldr r7,[pc,#0x1D4] @ 0x4646D8 on USA
.else @ (_KOR_ || _TWN_)
    ldr r8,[pc,#0x1D4]
.endif
    push {r0-r12, lr}
    cpy r0,r4 @ sub32A0 struct
    bl ColView_InitSubMainClass32A0
    pop {r0-r12, lr}
    add pc,pc,#0x18 @ 0x464530 on USA
