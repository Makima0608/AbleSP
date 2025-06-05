local Ability_Task_SetPetHidden = UE4.Class(nil, "Ability_Task_SetPetHidden")

function Ability_Task_SetPetHidden:OnTaskStartBP(Context)
    _SP.Log("SPAbilityTask", "Ability_Task_SetPetHidden:OnTaskStartBP")

    local Character
    local Owner = Context:GetOwner()
    local Summon = Owner and Owner:Cast(UE4.ASPGameSummonBase)
    if _SP.IsValid(Summon) then
        -- 召唤物支持：如果该Task是在召唤物上播放，那么尝试从召唤者找到玩家角色
        local SummonMaster = Summon:GetSummonMaster()
        Character = SummonMaster and SummonMaster:Cast(UE4.ABaseMoeGameCharacter)
    else
        Character = Owner and Owner:Cast(UE4.ABaseMoeGameCharacter)
    end

    if not _SP.IsValid(Character) then
        _SP.LogWarning("SPAbilityTask", "Ability_Task_SetPetHidden", "Unable to find player character!")
        return
    end

    local UnleashPet = Character:GetUnleashedPet()
    local FriendSkillComponent = Character:GetFriendSkillComponent()

    if not _SP.IsValid(UnleashPet) then
        _SP.LogWarning("SPAbilityTask", "Ability_Task_SetPetHidden", "Unable to find unleashed pet!")
        return
    end

    if self.bHidden then
        UnleashPet:RequestStopAI()
        UnleashPet:SetMonsterIsInWorld(false)
        UnleashPet:SetInvincible(true)
        FriendSkillComponent:S2C_SetPetHiddenInGame(true)

        if self.bTeleportToTarget then
            self:TeleportToTarget(Context, UnleashPet, Character)
        end
    else
        if self.bTeleportToTarget then
            self:TeleportToTarget(Context, UnleashPet, Character)
        end

        UnleashPet:ResumeAI()
        UnleashPet:SetMonsterIsInWorld(true)
        UnleashPet:SetInvincible(false)
        FriendSkillComponent:S2C_SetPetHiddenInGame(false)
    end

    if _SP.IsValid(FriendSkillComponent) then
        FriendSkillComponent:SetEnableAutoResumePet(self.bEnableAutoResume)
    end
end

function Ability_Task_SetPetHidden:OnTaskEndBP(Context)
    _SP.Log("SPAbilityTask", "Ability_Task_SetPetHidden:OnTaskEndBP")
end

function Ability_Task_SetPetHidden:TeleportToTarget(Context, Pet, Character)
    if not Context or not Pet then
        return
    end

    local TargetLocation
    local TargetActors = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, TargetActors)
    for i = 1, TargetActors:Length() do
        local TargetActor = TargetActors:GetRef(i)
        if _SP.IsValid(TargetActor) then
            TargetLocation = TargetActor:K2_GetActorLocation()
            break
        end
    end
    if TargetLocation == nil then
        _SP.LogWarning("SPAbilityTask", "Ability_Task_SetPetHidden", "Unable to Acquire target location for teleportation!")
        return
    end

    -- 参考Ability_Task_AdjustPosition, 略微向上调整一下啾灵位置
    TargetLocation.Z = TargetLocation.Z + 50

    -- 在目标点附近寻找NavMesh立足点
    local World = _SP.GetCurrentWorld()
    local Projected = UE4.USPUtilityLibrary.ProjectPointToNavigation(World, TargetLocation, TargetLocation, nil, nil, self.NavmeshSearchRange, Pet)
    if not Projected and _SP.IsValid(Character) then
        -- 如果不能找到一个在NavMesh上的位置，则直接传送到主人所在的位置
        TargetLocation = Character:K2_GetActorLocation()
    else
        TargetLocation.Z = TargetLocation.Z + Pet:SPGetCapsuleHalfHeight()
    end
    Pet:K2_SetActorLocation(TargetLocation)

    -- 检查此时是否和其它的星兽重叠
    _SP.SPAbilityUtils.ResolvePlayerStuckInActor(Pet)
end

function Ability_Task_SetPetHidden:GetTaskRealmBP()
    return UE4.EAbleAbilityTaskRealm.ATR_Server
end

function Ability_Task_SetPetHidden:IsSingleFrameBP()
    return true
end

return Ability_Task_SetPetHidden