--[[--
* @Description: 骑乘跳跃打断武器开火的技能task的实例数据
* @Author:      dylanyzliu
* @DateTime:    2024/08/05
]]

---@type SPRideSystemUtils
local S_RideSysUtils = require("Feature.StarP.Script.System.Ride.SPRideSystemUtils")
local S_UE4 = UE4
---@class SPAbility_Task_RideStateInterrupt
local Ability_Task_RideStateInterrupt = UE4.Class(nil, "Ability_Task_RideStateInterrupt")
local S_USPGameLibrary = S_UE4.USPGameLibrary
local S_ReductionStateIDWithOffset = S_USPGameLibrary.ReductionStateIDWithOffset
local S_EStateIdOffsetType = S_UE4.EStateIdOffsetType
local S_GetRiddenSystemByPawn = S_RideSysUtils.GetRiddenSystemByPawn



function Ability_Task_RideStateInterrupt:OnTaskStartBP(Context)
    ---@type SPAbility_Task_RideStateInterruptPad
    local scratchPad = self:GetScratchPad(Context)

    if not scratchPad then
        return
    end
    ---@type USPActorInterface
    local OwnerActor = Context:GetOwner()
    ---@type USPActorInterface
    local playerPawn = OwnerActor and OwnerActor:FindRelatedPlayerPawn()

    if not playerPawn or playerPawn:FindRelatedPlayerController() == nil then
        -- 跳过模拟端
        return
    end

    local riddenSysLogic, riddenSysData = S_GetRiddenSystemByPawn(OwnerActor)

    if not riddenSysLogic or
        not riddenSysLogic:IsRiddenFromRidden(riddenSysData) or
        riddenSysLogic:GetDriverPawnFromRidden(riddenSysData) ~= playerPawn then
        -- 跳过乘客
        return
    end

    if self.RiddenBehaviors:Contains(S_RideSysUtils.EnumBehavior.Move) and S_RideSysUtils.GetWantsToMove(riddenSysLogic, riddenSysData) then
        self:InterruptRiderState(riddenSysLogic:GetDriverPawnFromRidden(riddenSysData), S_RideSysUtils.EnumBehavior.Move)
        return
    end

    scratchPad.OwnerAbility = self
    scratchPad.RiddenPawn = riddenSysLogic:GetRiddenPawnFromRidden(riddenSysData)
    S_RideSysUtils.ListenBehaviorOccur(riddenSysLogic, riddenSysData, scratchPad, scratchPad.OnBehaviorOccur)
end

function Ability_Task_RideStateInterrupt:OnTaskEndBP(Context)
    ---@type SPAbility_Task_RideStateInterruptPad
    local scratchPad = self:GetScratchPad(Context)

    if not scratchPad then
        return
    end

    local riddenPawn = scratchPad.RiddenPawn
    scratchPad.OwnerAbility = nil
    scratchPad.RiddenPawn = nil

    local riddenSysLogic, riddenSysData = S_GetRiddenSystemByPawn(riddenPawn)

    if riddenSysLogic then
        S_RideSysUtils.UnListenBehaviorOccur(riddenSysLogic, riddenSysData, scratchPad, scratchPad.OnBehaviorOccur)
    end
end

---@param InSystemContext FSPRiddenSystemContext
---@param InBehavior EnumSPRideBehavior
function Ability_Task_RideStateInterrupt:OnBehaviorOccur(InSystemContext, InBehavior)
    ---@type ASPGameCharacterBase
    local riderPawn = InSystemContext.SysLogic:GetDriverPawnFromRidden(InSystemContext.SysData)

    self:InterruptRiderState(riderPawn, InBehavior)
end

---@param InRiderPawn ASPGameCharacterBase
---@param InBehavior EnumSPRideBehavior
function Ability_Task_RideStateInterrupt:InterruptRiderState(InRiderPawn, InBehavior)
    local curActionState = InRiderPawn:GetActionState_New()
    local curSPActionState = S_ReductionStateIDWithOffset(curActionState, S_EStateIdOffsetType.SP)
    local riderActionStates = self.RiderActionStates
    local riddenBehaviors = self.RiddenBehaviors

    ---tododylan 配置表，加参数
    if riddenBehaviors:Contains(InBehavior) and riderActionStates:Contains(curSPActionState) then
        InRiderPawn:TryLeaveActionState_New(curSPActionState, S_EStateIdOffsetType.SP, true)
    end
end

return Ability_Task_RideStateInterrupt
