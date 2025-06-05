--[[--
* @Description: 骑乘上马准备.当控制对象切换时完成准备
* @Author:      smbintang
* @DateTime:    2024/11/23
]]

---@type SPRideSystemUtils
local S_RideSysUtils = require("Feature.StarP.Script.System.Ride.SPRideSystemUtils")
local S_string_format = string.format
local S_tostring = tostring
local S_pairs = pairs
local S_UE4 = UE4
local S_ESPRideSysPhase = S_UE4.ESPRideSysPhase
local S_ESPRideSysPhase_Mount = S_ESPRideSysPhase.Mount
local S_ESPRideSysPhase_Normal = S_ESPRideSysPhase.Normal
local S_EAbleAbilityTaskResult = S_UE4.EAbleAbilityTaskResult
local S_GetUnifiedNetworkInfo = S_UE4.USPUnifiedNetworkLibrary.GetUnifiedNetworkInfo
local S_FSPUnifiedNetworkInfo = S_UE4.FSPUnifiedNetworkInfo
local S_ESPRideSysPhaseChangeReason = S_UE4.ESPRideSysPhaseChangeReason
local S_ESPRideSysMountCheckResult = S_UE4.ESPRideSysMountCheckResult
local S_IsValid = S_RideSysUtils.IsObjectValid
local S_GetPhaseContextReady = S_RideSysUtils.GetPhaseContextReady
local S_SetNextMountParams = S_RideSysUtils.SetNextMountParams
local S_EnumMountFlowFlags = S_RideSysUtils.EnumMountFlowFlags
local S_DebugVerbose = S_RideSysUtils.DebugVerbose
local S_GetPawnDebugString = S_RideSysUtils.GetPawnDebugString
local S_IsPhaseRelatedToRiding = S_RideSysUtils.IsPhaseRelatedToRiding
local S_CheckMountConditions = S_RideSysUtils.CheckMountConditions
local S_FindRideSystemByAbilityContext = S_RideSysUtils.FindRideSystemByAbilityContext
local S_EAbleAbilityTaskResult_Interrupted = S_UE4.EAbleAbilityTaskResult.Interrupted
local S_FSPRideMountTarget = S_UE4.FSPRideMountTarget



---@class SPAbility_Task_RideMountReady : USPAbilityTask
---@field StartFromClient bool 是否从客户端发起上马
---@field EnableStaminaCost bool 是否开启精力消耗
local Ability_Task_RideMountReady = UE4.Class(nil, "Ability_Task_RideMountReady")

---@param Context UAbleAbilityContext
function Ability_Task_RideMountReady:IsDoneBP(Context)
    ---@type SPAbility_Task_RideMountReadyPad
    local scratchPad = self:GetScratchPad(Context)

    if scratchPad == nil or scratchPad.Interrupted then
        return true
    end

    local sysLogic, sysData = scratchPad:GetRideSys()

    if not S_IsValid(sysLogic) or not S_IsValid(sysData) then
        return true
    end

    local curPhase, ready = S_GetPhaseContextReady(sysLogic, sysData)
    return (curPhase == S_ESPRideSysPhase_Mount and ready) or
        (curPhase ~= S_ESPRideSysPhase_Normal and curPhase ~= S_ESPRideSysPhase_Mount)
end

---@param Context UAbleAbilityContext
function Ability_Task_RideMountReady:OnTaskStartBP(Context)
    ---@type SPAbility_Task_RideMountReadyPad
    local scratchPad = self:GetScratchPad(Context)
    local ownerActor, sysLogic, sysData, riderPawn, riddenPawn = S_FindRideSystemByAbilityContext(Context)

    local withAuthority = ownerActor ~= nil and ownerActor:HasAuthority()

    if not sysLogic or not sysData or not riderPawn or not riddenPawn then
        S_DebugVerbose(function(InLa_OwnerActor)
            _SP.Log("RideSys", "AbleTask", S_string_format("RideMountReady:invalid targets, owner %s, sysLogic %s, riddenPawn %s",
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

    if not isMountPhase and curPhase ~= S_ESPRideSysPhase_Normal then
        S_DebugVerbose(function(InLa_OwnerActor, InLa_Phase)
            _SP.Log("RideSys", "AbleTask", S_string_format("RideMountReady:unmatched phase %d, owner %s",
                InLa_Phase,
                S_GetPawnDebugString(InLa_OwnerActor)))
        end)(ownerActor, curPhase)

        if withAuthority then
            scratchPad.Interrupted = true
            Context.m_AbilityComponent:CancelAbility(Context.m_Ability, S_EAbleAbilityTaskResult.Interrupted)
        end
        return
    end

    if isMountPhase and ready then
        S_DebugVerbose(function(InLa_OwnerActor)
            _SP.Log("RideSys", "AbleTask", S_string_format("RideMountReady:already mount, owner %s",
                S_GetPawnDebugString(InLa_OwnerActor)))
        end)(ownerActor)
        return
    end

    scratchPad:CacheRideSys(sysLogic, sysData)

    if not isMountPhase and netInfo.bWithNetOwner and (
            (netInfo.bLocalControlled and withAuthority) or
            (self.StartFromClient ~= withAuthority)) then
        -- 检查上马条件
        ---@type SPRideMountConditionOptions
        local checkOptions = {
            IgnoreLocationConditions = true,
            IgnorePlayerState = false,
        }
        local result = S_CheckMountConditions(sysLogic, sysData, riddenPawn, checkOptions)

        if result ~= S_ESPRideSysMountCheckResult.Success then
            S_DebugVerbose(function(InLa_OwnerActor)
                _SP.Log("RideSys", "AbleTask", S_string_format("RideMountReady:fail to check mount, owner %s",
                    S_GetPawnDebugString(InLa_OwnerActor)))
            end)(ownerActor)

            if withAuthority then
                scratchPad.Interrupted = true
                Context.m_AbilityComponent:CancelAbility(Context.m_Ability, S_EAbleAbilityTaskResult.Interrupted)
            end
            return
        end
        -- 设置上马参数
        ---@type SPRideMountParams
        local mountParams = {}
        mountParams.FlowFlags = S_EnumMountFlowFlags.ManualStart |
            S_EnumMountFlowFlags.NoAbility |
            ((not self.EnableStaminaCost and S_EnumMountFlowFlags.NoStaminaCost) or 0)

        S_SetNextMountParams(sysLogic, sysData, mountParams)
        -- 发起上马
        local reason = (netInfo.bLocalControlled and S_ESPRideSysPhaseChangeReason.PlayerInput) or S_ESPRideSysPhaseChangeReason.ServerRequest
        ---@type FSPRideMountTarget
        local mountTarget = S_FSPRideMountTarget()
        mountTarget.RidablePawn = riddenPawn
        sysLogic:MountWithNoCheck(sysData, reason, mountTarget)

        curPhase, ready = S_GetPhaseContextReady(sysLogic, sysData)
    end

    S_DebugVerbose(function(InLa_OwnerActor, InLa_Phase, InLa_Ready)
        _SP.Log("RideSys", "AbleTask", S_string_format("RideMountReady:phase %d, ready %s, owner %s",
            InLa_Phase,
            S_tostring(InLa_Ready),
            S_GetPawnDebugString(InLa_OwnerActor)))
    end)(ownerActor, curPhase, ready)

    if withAuthority then
        scratchPad:SetEventOnRidePhaseChange(
            Context, true,
            self, self.OnEventRidePhaseChange)
    end
end

---@param Context UAbleAbilityContext
---@param Result EAbleAbilityTaskResult
function Ability_Task_RideMountReady:OnTaskEndBP(Context, Result)
    ---@type SPAbility_Task_RideMountReadyPad
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
---@param InPad SPAbility_Task_RideMountReadyPad
---@param InSysLogic USPRideSystemInterface
---@param InSysData UObject
---@param InEventData FSPRideSysPhaseChangeEventData
function Ability_Task_RideMountReady:OnEventRidePhaseChange(
    InContext, InPad,
    InSysLogic, InSysData, InEventData)
    local curPhase = S_GetPhaseContextReady(InSysLogic, InSysData)

    if curPhase ~= S_ESPRideSysPhase_Mount then
        local logicContext = InEventData.LogicContext
        S_DebugVerbose(function(InLa_LogicContext, InLa_Phase)
            _SP.Log("RideSys", "AbleTask", S_string_format("RideMountReady:leave mount when listening on server, phase %d, rider %s",
                InLa_Phase,
                S_GetPawnDebugString(InLa_LogicContext.RiderPawn)))
        end)(logicContext, curPhase)
        InPad.Interrupted = true
        InPad:SetEventOnRidePhaseChange(InContext, false, self)
        InContext.m_AbilityComponent:CancelAbility(InContext.m_Ability, S_EAbleAbilityTaskResult.Interrupted)
        return
    end
end

return Ability_Task_RideMountReady
