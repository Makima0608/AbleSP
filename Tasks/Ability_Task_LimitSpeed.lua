---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by .
--- DateTime: 2024/8/12 17:16
---
local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local SPLuaUtility = require("Feature.StarP.Script.System.SPLuaUtility")

local Ability_Task_LimitSpeed = UE4.Class(nil, "Ability_Task_LimitSpeed")

local function Log(...)
    _SP.Log("SPAbility", "[Ability_Task_LaserWeapon]", ...)
end



function Ability_Task_LimitSpeed:OnTaskStartBP(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    local Target = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_Self)
    if not Target then
        return
    end

    self:LimitSpeed(Context)
end

function Ability_Task_LimitSpeed:OnTaskTickBP(Context, deltaTime)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    self:LimitSpeed(Context)

end

function Ability_Task_LimitSpeed:LimitSpeed(Context)
    local Target = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_Self)
    if not Target then
        return
    end

    if Target:IsA(UE4.ASPGameMonsterBase) then
        local monster = Target:Cast(UE4.ASPGameMonsterBase)
        local baseData = monster:GetMonsterBaseData()
        baseData.MoveSpeed = self.MaxWalkSpeed
        monster:UpdateMonsterMoveSpeed()
        _SP.LogDebug("SPAbility", "[Ability_Task_LimitSpeed]", "OnTaskStartBP",
        "更新怪物移动速度:", self.MaxWalkSpeed)
    end
end

function Ability_Task_LimitSpeed:OnTaskEndBP(Context, Result)
end

function Ability_Task_LimitSpeed:ResetScratchPadBP(ScratchPad)
    if ScratchPad then
        -- ScratchPad.SpawnedEffect = nil
        -- ScratchPad.QueryResults = nil
        -- ScratchPad.IntervalTimer = 0
        -- ScratchPad.AddBuffActors = nil
    end
end

function Ability_Task_LimitSpeed:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.LimitSpeed)
end

return Ability_Task_LimitSpeed