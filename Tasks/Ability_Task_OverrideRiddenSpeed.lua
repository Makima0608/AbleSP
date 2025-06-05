--[[--
* @Description: 覆盖坐骑基础速度的技能task
* @Author:      smbintang
* @DateTime:    2024/08/24
]]

---@type SPRideSystemUtils
local S_RideSysUtils = require("Feature.StarP.Script.System.Ride.SPRideSystemUtils")
local S_UE4 = UE4
local S_GetRiddenSystemByPawn = S_RideSysUtils.GetRiddenSystemByPawn



---@class SPAbility_Task_OverrideRiddenSpeed
local Ability_Task_OverrideRiddenSpeed = S_UE4.Class(nil, "Ability_Task_OverrideRiddenSpeed")

---@param InContext UAbleAbilityContext
---@return USPRiddenSystemInterface, UObject
function Ability_Task_OverrideRiddenSpeed:GetRiddenSystem(InContext)
    ---@type USPActorInterface
    local OwnerActor = InContext:GetOwner()

    if not OwnerActor or OwnerActor:FindRelatedPlayerController() == nil then
        -- 跳过模拟端
        return
    end

    return S_GetRiddenSystemByPawn(OwnerActor)
end

function Ability_Task_OverrideRiddenSpeed:OnTaskStartBP(Context)
    ---@type SPAbility_Task_OverrideRiddenSpeedPad
    local scratchPad = self:GetScratchPad(Context)

    if not scratchPad then
        return
    end

    local riddenSysLogic, riddenSysData = self:GetRiddenSystem(Context)

    if riddenSysLogic == nil then
        return
    end

    local success = S_RideSysUtils.SetRiddenMaxSpeedOverride(
        riddenSysLogic, riddenSysData,
        not self.SingleFrame or self.Override,
        self.Reason, self.MaxSpeed, self.AffectedByBuff)

    scratchPad.NeedCancelOverride = not self.SingleFrame and success
end

function Ability_Task_OverrideRiddenSpeed:OnTaskEndBP(Context)
    ---@type SPAbility_Task_OverrideRiddenSpeedPad
    local scratchPad = self:GetScratchPad(Context)

    if not scratchPad or not scratchPad.NeedCancelOverride then
        return
    end

    local riddenSysLogic, riddenSysData = self:GetRiddenSystem(Context)

    if riddenSysLogic == nil then
        return
    end

    S_RideSysUtils.SetRiddenMaxSpeedOverride(
        riddenSysLogic, riddenSysData,
        false,
        self.Reason)
end


return Ability_Task_OverrideRiddenSpeed
