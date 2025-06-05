--[[--
* @Description: 骑乘上马表现.依赖Ability_Task_RideMountFlow
* @Author:      smbintang
* @DateTime:    2024/11/23
]]

---@type SPRideSystemUtils
local S_RideSysUtils = require("Feature.StarP.Script.System.Ride.SPRideSystemUtils")
local S_string_format = string.format
local S_tostring = tostring
local S_UE4 = UE4
local S_GetUnifiedNetworkInfo = S_UE4.USPUnifiedNetworkLibrary.GetUnifiedNetworkInfo
local S_FSPUnifiedNetworkInfo = S_UE4.FSPUnifiedNetworkInfo
local S_ESPRideSysPhase = S_UE4.ESPRideSysPhase
local S_ESPRideSysPhase_Mount = S_ESPRideSysPhase.Mount
local S_ESPRideSysPhase_Riding = S_ESPRideSysPhase.Riding
local S_EAbleAbilityTaskResult = S_UE4.EAbleAbilityTaskResult
local S_ESPRideSysPhaseChangeReason = S_UE4.ESPRideSysPhaseChangeReason
local S_IsValid = S_RideSysUtils.IsObjectValid
local S_GetPhaseContextReady = S_RideSysUtils.GetPhaseContextReady
local S_DebugVerbose = S_RideSysUtils.DebugVerbose
local S_GetPawnDebugString = S_RideSysUtils.GetPawnDebugString
local S_ContinueMountAfterReady = S_RideSysUtils.ContinueMountAfterReady
local S_UpdateMountFlowWithNoAbility = S_RideSysUtils.UpdateMountFlowWithNoAbility
local S_IsPhaseRelatedToRiding = S_RideSysUtils.IsPhaseRelatedToRiding
local S_FindRideSystemByAbilityContext = S_RideSysUtils.FindRideSystemByAbilityContext
local S_EAbleAbilityTaskResult_Interrupted = S_UE4.EAbleAbilityTaskResult.Interrupted



---@class SPAbility_Task_RideMountFlow : USPAbilityTask
---@field JumpAnim UAnimSequenceBase 上马动画
---@field AnimPlayRate float 上马动画的播放速率
---@field AnimSlotName FName 播放上马动画的槽名，用于动画蓝图
---@field AnimBlendInTime float 上马动画的淡入时间
---@field AnimBlendOutTime float 上马动画的淡出时间
---@field DurationWhenNoAnim float 当找不到可用的上马动画时上马过程的持续时间
local Ability_Task_RideMountFlow = UE4.Class(nil, "Ability_Task_RideMountFlow")

---@param Context UAbleAbilityContext
function Ability_Task_RideMountFlow:IsDoneBP(Context)
    ---@type SPAbility_Task_RideMountFlowPad
    local scratchPad = self:GetScratchPad(Context)

    if scratchPad == nil or scratchPad.Interrupted then
        return true
    end

    local sysLogic, sysData = scratchPad:GetRideSys()

    if not S_IsValid(sysLogic) or not S_IsValid(sysData) then
        return true
    end

    local curPhase = S_GetPhaseContextReady(sysLogic, sysData)
    return curPhase ~= S_ESPRideSysPhase_Mount or Context.m_CurrentTime - self.m_StartTime >= scratchPad.Duration
end

---@param Context UAbleAbilityContext
function Ability_Task_RideMountFlow:OnTaskStartBP(Context)
    ---@type SPAbility_Task_RideMountFlowPad
    local scratchPad = self:GetScratchPad(Context)
    scratchPad.Duration = 0
    local ownerActor, sysLogic, sysData, riderPawn, riddenPawn = S_FindRideSystemByAbilityContext(Context)

    local withAuthority = ownerActor ~= nil and ownerActor:HasAuthority()

    if not sysLogic or not sysData or not riderPawn or not riddenPawn then
        S_DebugVerbose(function(InLa_OwnerActor)
            _SP.Log("RideSys", "AbleTask", S_string_format("RideMountFlow:invalid targets, owner %s, sysLogic %s, riddenPawn %s",
                S_GetPawnDebugString(InLa_OwnerActor),
                S_tostring(sysLogic),
                S_tostring(riddenPawn)))
        end)(ownerActor)

        if withAuthority then
            scratchPad.Interrupted = true
            Context.m_AbilityComponent:CancelAbility(Context.m_Ability, S_EAbleAbilityTaskResult.Interrupted)
        end
        return
    end

    ---@type FSPUnifiedNetworkInfo
    local netInfo = S_FSPUnifiedNetworkInfo()
    S_GetUnifiedNetworkInfo(netInfo, riderPawn, false, nil)

    local curPhase, ready = S_GetPhaseContextReady(sysLogic, sysData)
    local isMountPhase = curPhase == S_ESPRideSysPhase_Mount

    if not isMountPhase and curPhase ~= S_ESPRideSysPhase_Riding then
        S_DebugVerbose(function(InLa_OwnerActor, InLa_Phase)
            _SP.Log("RideSys", "AbleTask", S_string_format("RideMountFlow:unmatched phase %d, owner %s",
                InLa_Phase,
                S_GetPawnDebugString(InLa_OwnerActor)))
        end)(ownerActor, curPhase)

        if withAuthority then
            scratchPad.Interrupted = true
            Context.m_AbilityComponent:CancelAbility(Context.m_Ability, S_EAbleAbilityTaskResult.Interrupted)
        end
        return
    end

    if not isMountPhase then
        S_DebugVerbose(function(InLa_OwnerActor)
            _SP.Log("RideSys", "AbleTask", S_string_format("RideMountFlow:already riding, owner %s",
                S_GetPawnDebugString(InLa_OwnerActor)))
        end)(ownerActor)
        return
    end

    if not ready then
        S_DebugVerbose(function(InLa_OwnerActor)
            _SP.Log("RideSys", "AbleTask", S_string_format("RideMountFlow:not mount ready, owner %s",
                S_GetPawnDebugString(InLa_OwnerActor)))
        end)(ownerActor)

        if withAuthority then
            scratchPad.Interrupted = true
            Context.m_AbilityComponent:CancelAbility(Context.m_Ability, S_EAbleAbilityTaskResult.Interrupted)
        end
        return
    end

    S_DebugVerbose(function(InLa_OwnerActor, InLa_Phase)
        _SP.Log("RideSys", "AbleTask", S_string_format("RideMountFlow:phase %d, owner %s",
            InLa_Phase,
            S_GetPawnDebugString(InLa_OwnerActor)))
    end)(ownerActor, curPhase)

    scratchPad:CacheRideSys(sysLogic, sysData)

    ---@type SPRideMountParams
    local mountParams = {}
    mountParams.JumpAnim = self.JumpAnim
    mountParams.AnimPlayRate = self.AnimPlayRate
    mountParams.AnimSlotName = self.AnimSlotName
    mountParams.AnimBlendInTime = self.AnimBlendInTime
    mountParams.AnimBlendOutTime = self.AnimBlendOutTime
    mountParams.DurationWhenNoAnim = self.DurationWhenNoAnim
    local success, flowParams = S_ContinueMountAfterReady(sysLogic, sysData, mountParams)
    scratchPad.Duration = (flowParams and flowParams.Duration) or 0

    if not success then
        S_DebugVerbose(function(InLa_OwnerActor)
            _SP.Log("RideSys", "AbleTask", S_string_format("RideMountFlow:fail to continue mount, owner %s",
                S_GetPawnDebugString(InLa_OwnerActor)))
        end)(ownerActor)

        if withAuthority then
            scratchPad.Interrupted = true
            Context.m_AbilityComponent:CancelAbility(Context.m_Ability, S_EAbleAbilityTaskResult.Interrupted)
        end
        return
    end

    if withAuthority then
        scratchPad:SetEventOnRidePhaseChange(
            Context, true,
            self, self.OnEventRidePhaseChange)
    end
end

---@param Context UAbleAbilityContext
---@param DeltaTime float
function Ability_Task_RideMountFlow:OnTaskTickBP(Context, DeltaTime)
    ---@type SPAbility_Task_RideMountFlowPad
    local scratchPad = self:GetScratchPad(Context)
    local sysLogic, sysData = scratchPad:GetRideSys()

    if S_IsValid(sysLogic) and S_IsValid(sysData) then
        S_UpdateMountFlowWithNoAbility(
            sysLogic, sysData,
            Context.m_CurrentTime - self.m_StartTime,
            DeltaTime)
    end
end

---@param Context UAbleAbilityContext
---@param Result EAbleAbilityTaskResult
function Ability_Task_RideMountFlow:OnTaskEndBP(Context, Result)
    ---@type SPAbility_Task_RideMountFlowPad
    local scratchPad = self:GetScratchPad(Context)
    local interrupted = scratchPad.Interrupted or Result == S_EAbleAbilityTaskResult_Interrupted
    local sysLogic, sysData = scratchPad:GetRideSys()
    scratchPad.Interrupted = false
    scratchPad:SetEventOnRidePhaseChange(Context, false, self)
    scratchPad:CacheRideSys(nil, nil)

    if interrupted and
        S_IsValid(sysLogic) and S_IsValid(sysData) and
        S_IsPhaseRelatedToRiding(S_GetPhaseContextReady(sysLogic, sysData)) then
        sysLogic:ForceDismount(sysData, S_ESPRideSysPhaseChangeReason.Interrupted, false)
    end
end

---@param InContext UAbleAbilityContext
---@param InPad SPAbility_Task_RideMountFlowPad
---@param InSysLogic USPRideSystemInterface
---@param InSysData UObject
---@param InEventData FSPRideSysPhaseChangeEventData
function Ability_Task_RideMountFlow:OnEventRidePhaseChange(
    InContext, InPad,
    InSysLogic, InSysData, InEventData)
    local curPhase = S_GetPhaseContextReady(InSysLogic, InSysData)

    if curPhase ~= S_ESPRideSysPhase_Mount and curPhase ~= S_ESPRideSysPhase_Riding then
        InPad.Interrupted = true
        InPad:SetEventOnRidePhaseChange(InContext, false, self)
        InContext.m_AbilityComponent:CancelAbility(InContext.m_Ability, S_EAbleAbilityTaskResult.Interrupted)
        return
    end
end

return Ability_Task_RideMountFlow
