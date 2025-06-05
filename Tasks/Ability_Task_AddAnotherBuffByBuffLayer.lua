local Ability_Task_AddAnotherBuffByBuffLayer = UE4.Class(nil, "Ability_Task_AddAnotherBuffByBuffLayer")

function Ability_Task_AddAnotherBuffByBuffLayer:OnTaskStartBP(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    local OwnerActor = Context:GetOwner()
    if OwnerActor and self.AddAnotherBuffId ~= 0 then
        local buffLayer = 0
        local abilityComponent = OwnerActor:GetAbilityComponent()
        if self.BuffId ~= 0 and abilityComponent and abilityComponent.FindBuff then
            local buffInstance = abilityComponent:FindBuff(self.BuffId)
            if buffInstance then
                buffLayer = buffInstance:GetLayer()
            end
        end
        ScratchPad.buffLayer = buffLayer
        if buffLayer ~= 0 then
            local Instigator = _SP.SPAbilityUtils.GetAbilityObjectInfoActor(Context:GetInstigator())
            abilityComponent:AddBuff(self.AddAnotherBuffId, OwnerActor, Instigator, buffLayer, 0, "Ability_Task_HealByBuffLayer:AddBuffTo")
        end
    end
end

function Ability_Task_AddAnotherBuffByBuffLayer:OnTaskEndBP(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    local OwnerActor = Context:GetOwner()
    if OwnerActor then
        local abilityComponent = OwnerActor:GetAbilityComponent()
        if self.IsNeedRemoveBuff and abilityComponent and self.BuffId ~= 0 and ScratchPad.buffLayer ~= 0 then
            abilityComponent:RemoveBuff(self.BuffId, OwnerActor, ScratchPad.buffLayer, false)
        end
    end
end

function Ability_Task_AddAnotherBuffByBuffLayer:ResetScratchPadBP(ScratchPad)
    if ScratchPad then
        ScratchPad.buffLayer = 0
    end
end

return Ability_Task_AddAnotherBuffByBuffLayer