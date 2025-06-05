local Ability_Task_PetNotifyOwnerCastAble = UE4.Class(nil, "Ability_Task_PetNotifyOwnerCastAble")

function Ability_Task_PetNotifyOwnerCastAble:OnTaskStartBP(Context)
    local PetActor = Context:GetOwner()
    if not _SP.IsValid(PetActor) or self.AbilityID <= 0 then
        return
    end
    local MasterActor = PetActor:GetMasterInCombatTeam()
    if not _SP.IsValid(MasterActor) then
        return
    end
    local AbilityComponent = MasterActor:GetAbilityComponent()
    local FriendComponent = MasterActor:GetFriendSkillComponent()
    if _SP.IsValid(AbilityComponent) then
        _SP.Log("Ability_Task_PetNotifyOwnerCastAble", "Owner TryActivateAbility:", self.AbilityID)
        if self.bForceInterrupt then
            local currentActiveAbleId = AbilityComponent:GetActivatingAbility()
            if currentActiveAbleId == self.AbilityId then
                _SP.Log("Ability_Task_PetNotifyOwnerCastAble", "ActivateAbility:", self.AbilityID, " already Activated")
            else
                AbilityComponent:InterruptActiveAbility("Ability_Task_PetNotifyOwnerCastAble")
            end
        end
        AbilityComponent:GiveAbility(self.AbilityID, MasterActor)
        local Result = AbilityComponent:TryActivateAbility(self.AbilityID, MasterActor, MasterActor)

        if not Result then
            _SP.LogWarning("Ability_Task_PetNotifyOwnerCastAble", "Owner TryActivateAbility  result:", Result)
        else
            if _SP.IsValid(FriendComponent) and _SP.IsDSorStandalone then
                FriendComponent:SetListenedOwnerAbility(self.AbilityID)
            end
        end

    end
end

return Ability_Task_PetNotifyOwnerCastAble