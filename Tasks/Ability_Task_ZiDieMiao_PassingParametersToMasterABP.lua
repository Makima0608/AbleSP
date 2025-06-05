local Ability_Task_ZiDieMiao_PassingParametersToMasterABP = UE4.Class(nil, "Ability_Task_ZiDieMiao_PassingParametersToMasterABP")

local ESPAbilityABPParameters = {
    JumpSkillStart = 0,
    JumpSkillEnd = 1,
    PartnerSkillStart = 2,
    PartnerSKillEnd = 3,
}
local ESPAbilityZiDieMiaoAttackType = {
    None = 0,
    Dash = 1,
    Smash = 2,

}

function Ability_Task_ZiDieMiao_PassingParametersToMasterABP:OnTaskStartBP(Context)
    local PetActor = Context:GetOwner()
    if not _SP.IsValid(PetActor) then
        _SP.LogWarning("SPAbility", "Ability_Task_ZiDieMiao_PassingParametersToMasterABP nil Owner")
        return
    end
    local MasterActor = PetActor:GetMasterInCombatTeam()
    if not _SP.IsValid(MasterActor) then
        _SP.LogWarning("SPAbility", "Ability_Task_ZiDieMiao_PassingParametersToMasterABP nil Player")
        return
    end
    local Weapon = self:GetMasterWeapon(MasterActor)
    local AnimStateComp = MasterActor:GetComponentByClass(UE4.USPAnimStateComponent.StaticClass())
    if not _SP.IsValid(AnimStateComp) then
        _SP.LogWarning("SPAbility", "Ability_Task_ZiDieMiao_PassingParametersToMasterABP nil AnimStateComp")
        return
    end
    if self.Parameter == ESPAbilityABPParameters.JumpSkillStart then
        AnimStateComp:SetPartnerSkillStates(self.Result, not self.Result, false)
        self:SetAttackType(Weapon, ESPAbilityZiDieMiaoAttackType.Smash)
        _SP.Log("SPAbility", "Ability_Task_ZiDieMiao_PassingParametersToMasterABP JumpSkillStart Set:", self.Result, "JumpSkillEnd Set:", not self.Result)
    elseif self.Parameter == ESPAbilityABPParameters.JumpSkillEnd then
        if self.ResetParameter then
            self:SetAttackType(Weapon, ESPAbilityZiDieMiaoAttackType.None)
            AnimStateComp:SetPartnerSkillStates(false, false, false)
            _SP.Log("SPAbility", "Ability_Task_ZiDieMiao_PassingParametersToMasterABP JumpSkillStart Set:", false, "JumpSkillEnd Set:", false)
            return
        end
        AnimStateComp:SetPartnerSkillStates(not self.Result, self.Result, false)
        _SP.Log("SPAbility", "Ability_Task_ZiDieMiao_PassingParametersToMasterABP JumpSkillStart Set:", not self.Result, "JumpSkillEnd Set:", self.Result)
    elseif self.Parameter == ESPAbilityABPParameters.PartnerSkillStart then
        --冲刺暂时讨论只需要一个变量控制
        AnimStateComp:SetPartnerSkillStates(false, false, self.Result)
        if self.Result then
            self:SetAttackType(Weapon, ESPAbilityZiDieMiaoAttackType.Dash)
        else
            self:SetAttackType(Weapon, ESPAbilityZiDieMiaoAttackType.None)
        end

        _SP.Log("SPAbility", "Ability_Task_ZiDieMiao_PassingParametersToMasterABP PartnerSkillStart Set:", self.Result)
    end
    return
end

function Ability_Task_ZiDieMiao_PassingParametersToMasterABP:GetMasterWeapon(MasterActor)
    local CharInventoryComp = MasterActor:SafeGetInventoryComponent()
    if _SP.IsValid(CharInventoryComp) then
        local Weapon = CharInventoryComp:GetCrtEquipWeapon()
        if _SP.IsValid(Weapon) and Weapon:IsA(UE4.ASPPetMeleeWeapon_Ride) then
            return Weapon
        end
    end
    return nil
end

function Ability_Task_ZiDieMiao_PassingParametersToMasterABP:SetAttackType(Weapon, Type)
    if Weapon then
        Weapon:SetAttackType(Type)
    end
end


return Ability_Task_ZiDieMiao_PassingParametersToMasterABP