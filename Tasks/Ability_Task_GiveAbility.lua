local Ability_Task_GiveAbility = UE4.Class(nil, "Ability_Task_GiveAbility")

function Ability_Task_GiveAbility:OnTaskStartBP(Context)
    _SP.Log("Ability_Task_GiveAbility:","OnTaskStartBP:giveability")
    local TargetArray = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, TargetArray)

    if not self.AbilityID then
        return
    end
     if _SP.IsDSorStandalone then
        for i = 1, TargetArray:Length() do
            local target = TargetArray:Get(i)
            if target and target:IsA(UE4.ASPGameCharacterBase) then
                local AbilityComponent = target:GetAbilityComponent()
                if AbilityComponent then
                    AbilityComponent:GiveAbility(self.AbilityID, target, nil, nil, true, "Ability_Task_GiveAbility:OnTaskStart")
                end
            end
        end
    end
end

function Ability_Task_GiveAbility:OnTaskEndBP(Context, Result)
    _SP.Log("Ability_Task_GiveAbility:","OnTaskEndBP:removeability")
    local TargetArray = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, TargetArray)

    if not self.AbilityID then
        return
    end
    if _SP.IsDSorStandalone then
        for i = 1, TargetArray:Length() do
            local target = TargetArray:Get(i)
            if target and target:IsA(UE4.ASPGameCharacterBase) then
                local AbilityComponent = target:GetAbilityComponent()
                if AbilityComponent then
                    AbilityComponent:RemoveAbility(self.AbilityID)
                end
            end
        end
    end
end

return Ability_Task_GiveAbility