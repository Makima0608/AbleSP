---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by donglingli.
--- DateTime: 2024/6/20 14:40
---
local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local SPLuaUtility = require("Feature.StarP.Script.System.SPLuaUtility")
local Ability_Task_Gravity = UE4.Class(nil, "Ability_Task_Gravity")

function Ability_Task_Gravity:OnTaskStartBP(Context)
    local TargetArray = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, TargetArray)

    for i = 1, TargetArray:Length() do
        local Target = TargetArray:Get(i)
        local Character = Target:Cast(UE4.ASPGameMonsterBase)

        if Character then
            -- self:GravitySwitch(Context, Character)
            Character:DisableMovement("Ability_Task_Gravity")

            if self.bClearVelocity then
                local CharMovementComponent = Target.CharacterMovement
                if CharMovementComponent then
                    CharMovementComponent:StopMovementImmediately()
                end
            end
            _SP.Log("SPAbility", "Ability_Task_Gravity DisableMovement")
        end
    end
end

function Ability_Task_Gravity:OnTaskEndBP(Context, Result)
    -- if Result ~= UE4.EAbleAbilityTaskResult.Interrupted then
    --     return
    -- end
    local TargetArray = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, TargetArray)

    for i = 1, TargetArray:Length() do
        local Target = TargetArray:Get(i)
        local Character = Target:Cast(UE4.ASPGameMonsterBase)

        if Character then
            -- self:GravitySwitch(Context, Character)
            Character:RecoverMovement("Ability_Task_Gravity", self.ForceRecover)
            _SP.Log("SPAbility", "Ability_Task_Gravity RecoverMovement ", self.ForceRecover)
        end
    end
end

return Ability_Task_Gravity