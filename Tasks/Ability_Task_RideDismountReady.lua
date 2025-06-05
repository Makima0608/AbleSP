--[[--
* @Description: 骑乘下马
* @Author:      smbintang
* @DateTime:    2024/11/23
]]

---@type SPRideSystemUtils
local S_RideSysUtils = require("Feature.StarP.Script.System.Ride.SPRideSystemUtils")
local S_string_format = string.format
local S_tostring = tostring
local S_UE4 = UE4
local S_ESPRideSysPhase = S_UE4.ESPRideSysPhase
local S_ESPRideSysPhase_Mount = S_ESPRideSysPhase.Mount
local S_ESPRideSysPhase_Dismount = S_ESPRideSysPhase.Dismount
local S_ESPRideSysPhase_Riding = S_ESPRideSysPhase.Riding
local S_ESPRideSysPhase_Normal = S_ESPRideSysPhase.Normal
local S_EAbleAbilityTaskResult = S_UE4.EAbleAbilityTaskResult
local S_GetUnifiedNetworkInfo = S_UE4.USPUnifiedNetworkLibrary.GetUnifiedNetworkInfo
local S_FSPUnifiedNetworkInfo = S_UE4.FSPUnifiedNetworkInfo
local S_ESPRideSysPhaseChangeReason = S_UE4.ESPRideSysPhaseChangeReason
local S_ESPRideSysMountCheckResult = S_UE4.ESPRideSysMountCheckResult
local S_IsValid = S_RideSysUtils.IsObjectValid
local S_GetPhaseContextReady = S_RideSysUtils.GetPhaseContextReady
local S_SetNextDismountParams = S_RideSysUtils.SetNextDismountParams
local S_EnumDismountFlowFlags = S_RideSysUtils.EnumDismountFlowFlags
local S_DebugVerbose = S_RideSysUtils.DebugVerbose
local S_GetPawnDebugString = S_RideSysUtils.GetPawnDebugString
local S_FindRideSystemByAbilityContext = S_RideSysUtils.FindRideSystemByAbilityContext
local S_IsPhaseRelatedToRiding = S_RideSysUtils.IsPhaseRelatedToRiding
local S_EAbleAbilityTaskResult_Interrupted = S_UE4.EAbleAbilityTaskResult.Interrupted



---@class SPAbility_Task_RideDismountReady : USPAbilityTask
---@field StartFromClient bool 是否从客户端发起
---@field EndAfterDismount bool 是否把任务结束推迟到下马之后
---@field NoDefaultAnim bool 不使用表格配置的默认下马动画
---@field NoLaunch bool 不弹射
local Ability_Task_RideDismountReady = UE4.Class(nil, "Ability_Task_RideDismountReady")

---@param Context UAbleAbilityContext
function Ability_Task_RideDismountReady:IsDoneBP(Context)
    ---@type SPAbility_Task_RideDismountReadyPad
    local scratchPad = self:GetScratchPad(Context)

    if scratchPad == nil then
        return true
    end

    local sysLogic, sysData = scratchPad:GetRideSys()

    if not S_IsValid(sysLogic) or not S_IsValid(sysData) then
        return true
    end

    local curPhase, ready = S_GetPhaseContextReady(sysLogic, sysData)
    local endAfterDismount = self.EndAfterDismount
    return curPhase == S_ESPRideSysPhase_Mount or
        curPhase == S_ESPRideSysPhase_Normal or
        (not endAfterDismount and curPhase == S_ESPRideSysPhase_Dismount and ready)
end

---@param Context UAbleAbilityContext
function Ability_Task_RideDismountReady:OnTaskStartBP(Context)
    ---@type SPAbility_Task_RideDismountReadyPad
    local scratchPad = self:GetScratchPad(Context)
    local ownerActor, sysLogic, sysData, riderPawn, riddenPawn = S_FindRideSystemByAbilityContext(Context)

    local withAuthority = ownerActor ~= nil and ownerActor:HasAuthority()

    if not sysLogic or not sysData or not riderPawn or not riddenPawn then
        S_DebugVerbose(function(InLa_OwnerActor)
            _SP.Log("RideSys", "AbleTask", S_string_format("RideDismountReady:invalid targets, owner %s, sysLogic %s, riddenPawn %s",
                S_GetPawnDebugString(InLa_OwnerActor),
                S_tostring(sysLogic),
                S_tostring(riddenPawn)))
        end)(ownerActor)

        if withAuthority then
            Context.m_AbilityComponent:CancelAbility(Context.m_Ability, S_EAbleAbilityTaskResult.Interrupted)
        end
        return
    end

    ---@type FSPUnifiedNetworkInfo
    local netInfo = S_FSPUnifiedNetworkInfo()
    S_GetUnifiedNetworkInfo(netInfo, ownerActor, false, nil)

    local curPhase = S_GetPhaseContextReady(sysLogic, sysData)

    if curPhase == S_ESPRideSysPhase_Normal then
        S_DebugVerbose(function(InLa_OwnerActor)
            _SP.Log("RideSys", "AbleTask", S_string_format("RideDismountReady:already normal phase, owner %s",
                S_GetPawnDebugString(InLa_OwnerActor)))
        end)(ownerActor)
        return
    end

    if curPhase == S_ESPRideSysPhase_Mount then
        S_DebugVerbose(function(InLa_OwnerActor)
            _SP.Log("RideSys", "AbleTask", S_string_format("RideDismountReady:mount phase, owner %s",
                S_GetPawnDebugString(InLa_OwnerActor)))
        end)(ownerActor)

        if withAuthority then
            Context.m_AbilityComponent:CancelAbility(Context.m_Ability, S_EAbleAbilityTaskResult.Interrupted)
        end
        return
    end

    scratchPad:CacheRideSys(sysLogic, sysData)

    if netInfo.bWithNetOwner and (
            (netInfo.bLocalControlled and withAuthority) or
            (self.StartFromClient ~= withAuthority)) then
        -- 设置上马参数
        ---@type SPRideDismountParams
        local dismountParams = {}
        ---@type EnumSPRideDismountFlowFlags
        local flowFlags = S_EnumDismountFlowFlags.NoAbility |
            ((self.NoDefaultAnim and S_EnumDismountFlowFlags.NoDefaultAnim) or 0) |
            ((self.NoLaunch and S_EnumDismountFlowFlags.NoLaunch) or 0)

        dismountParams.FlowFlags = flowFlags
        S_SetNextDismountParams(sysLogic, sysData, dismountParams)
        -- 发起下马
        local reason = (netInfo.bLocalControlled and S_ESPRideSysPhaseChangeReason.PlayerInput) or S_ESPRideSysPhaseChangeReason.ServerRequest
        local result = sysLogic:TryDismount(sysData, reason)

        if result ~= S_ESPRideSysMountCheckResult.Success then
            S_DebugVerbose(function(InLa_OwnerActor)
                _SP.Log("RideSys", "AbleTask", S_string_format("RideDismountReady:fail to start dismount, owner %s",
                    S_GetPawnDebugString(InLa_OwnerActor)))
            end)(ownerActor)

            if withAuthority then
                Context.m_AbilityComponent:CancelAbility(Context.m_Ability, S_EAbleAbilityTaskResult.Interrupted)
            end
            return
        end

        curPhase = S_GetPhaseContextReady(sysLogic, sysData)
    end

    S_DebugVerbose(function(InLa_OwnerActor)
        _SP.Log("RideSys", "AbleTask", S_string_format("RideDismountReady:owner %s",
            S_GetPawnDebugString(InLa_OwnerActor)))
    end)(ownerActor)
end

---@param Context UAbleAbilityContext
---@param Result EAbleAbilityTaskResult
function Ability_Task_RideDismountReady:OnTaskEndBP(Context, Result)
    ---@type SPAbility_Task_RideDismountReadyPad
    local scratchPad = self:GetScratchPad(Context)
    local sysLogic, sysData = scratchPad:GetRideSys()
    scratchPad:CacheRideSys(nil, nil)

    if Result == S_EAbleAbilityTaskResult_Interrupted and
        S_IsValid(sysLogic) and S_IsValid(sysData) and
        S_IsPhaseRelatedToRiding(S_GetPhaseContextReady(sysLogic, sysData)) then
        sysLogic:ForceDismount(sysData, S_ESPRideSysPhaseChangeReason.Interrupted, false)
    end
end

return Ability_Task_RideDismountReady
