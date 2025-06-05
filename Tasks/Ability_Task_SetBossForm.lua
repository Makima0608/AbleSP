local Ability_Task_SetBossForm = UE4.Class(nil, "Ability_Task_SetBossForm")

function Ability_Task_SetBossForm:OnTaskStartBP(Context)
    local owner = Context:GetOwner()

    if not _SP.IsValid(owner) == nil or owner.SetBossForm == nil then
        return
    end

    local bIsMonster = owner:GetSPActorType() == UE4.ESPActorType.Pet or false
    if bIsMonster == true then
        local baseData = owner:GetMonsterBaseData()
        -- 判断目标是否配置了形态属性，如果未配置则不设置
        local weaknessPoints = _MOE.Config.SPPetForm:GetDataByKey(baseData.MonsterID)
        if weaknessPoints == nil or weaknessPoints.petForm[self.BossForm] == nil then
            return
        end
    end

    owner:SetBossForm(self.BossForm)
end

return Ability_Task_SetBossForm