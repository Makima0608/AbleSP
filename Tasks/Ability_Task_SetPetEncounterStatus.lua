local Ability_Task_SetPetEncounterStatus = UE4.Class(nil, "Ability_Task_SetPetEncounterStatus")

function Ability_Task_SetPetEncounterStatus:OnTaskEndBP(Context)
    local OwnerActor = Context:GetOwner()
    if not _SP.IsValid(OwnerActor) or not OwnerActor:IsA(UE4.ASPGameMonsterBase) then
        return
    end
    if self.IsInEncounter ~= nil and OwnerActor.SetIsEncounter then
        OwnerActor:SetIsEncounter(self.IsInEncounter)
        _SP.Log("Ability_Task_SetPetEncounterStatus", "OnTaskEndBP IsInEncounter", self.IsInEncounter)
    end
end

return Ability_Task_SetPetEncounterStatus