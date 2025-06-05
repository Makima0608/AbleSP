local Ability_Task_PetAndMasterRemoveSpTag = UE4.Class(nil, "Ability_Task_PetAndMasterRemoveSpTag")

function Ability_Task_PetAndMasterRemoveSpTag:OnTaskStartBP(Context)
    local OwnerActor = Context:GetOwner()
    local MasterActor = nil
    local PetActor = nil
    if not _SP.IsValid(OwnerActor) then
        return
    end
    if OwnerActor:IsA(UE4.ASPGameMonsterBase) then
        --OnwerActor 是星兽
        PetActor = OwnerActor
        MasterActor = PetActor:GetMasterInCombatTeam()
    else
        --OwnerActor 是玩家
        MasterActor = OwnerActor
        PetActor = MasterActor:GetUnleashedPet()
    end
    if not _SP.IsValid(PetActor) or not _SP.IsValid(MasterActor) then
        return
    end
    local ActionTargets = UE4.TArray(UE4.FSPAbilityObjectInfo)
    local PetInfo = _SP.SPAbilityUtils.MakeAbilityObjectInfo(PetActor)
    local MasterInfo = _SP.SPAbilityUtils.MakeAbilityObjectInfo(MasterActor)
    ActionTargets:Add(PetInfo)
    ActionTargets:Add(MasterInfo)
    local StateTagList = {}
    for i = 1, self.TagIDList:Length(), 1 do
        local TagID = self.TagIDList:Get(i)
        table.insert(StateTagList,TagID)
    end
    _SP.SPAbilityUtils.RemoveStatTagsFromTargets(ActionTargets, StateTagList)
end
return Ability_Task_PetAndMasterRemoveSpTag