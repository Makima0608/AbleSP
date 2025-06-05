local Ability_Task_ZiDieMiao_ReplaceMesh = UE4.Class(nil, "Ability_Task_ZiDieMiao_ReplaceMesh")

local WeaponAnimPath = "AnimBlueprint'/Game/Feature/StarP/Blueprints/Monster/Animation/MonsterABP/PartnerSkill/ABP_Weapon_HuDiemao.ABP_Weapon_HuDiemao_C'"
local NothingAnimPath = "AnimBlueprint'/Game/Feature/StarP/Blueprints/Monster/Animation/MonsterABP/ABP_Monster_Nothing_Hudiemao.ABP_Monster_Nothing_Hudiemao_C'"

function Ability_Task_ZiDieMiao_ReplaceMesh:OnTaskStartBP(Context)
    _SP.Log("Ability_Task_ZiDieMiao_ReplaceMesh", "TaskStart")
    local TargetArray = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, TargetArray)
    local PetActor = TargetArray:Get(1)
    if not _SP.IsValid(PetActor) or not PetActor:IsA(UE4.ASPGameMonsterBase) then
        local ActorName = nil
        if PetActor then
            ActorName = PetActor:GetName()
        end
        _SP.LogWarning("Ability_Task_ZiDieMiao_ReplaceMesh", "PetActor is nil or PetActor not a monster!", tostring(ActorName))
        return
    end

    if PetActor.bIsYaZhongMonster then
        if self.IsStartCombined then
            PetActor:SPEmptyOverrideMaterials()
        else
            PetActor:SetYaZhongMaterial()
        end
    end

    local MasterActor = PetActor:GetMasterInCombatTeam()
    local PetMeshCom = _SP.SPGameplayUtils.GetSkeletalMeshComponent(PetActor)
    if not _SP.IsValid(MasterActor) or not _SP.IsValid(PetMeshCom) then
        _SP.LogWarning("Ability_Task_ZiDieMiao_ReplaceMesh", "MasterActor:", tostring(MasterActor), "PetMeshCom:", tostring(PetMeshCom))
        return
    end
    local PlayerMeshCom = _SP.SPGameplayUtils.GetSkeletalMeshComponent(MasterActor)
    local FriendSkillComponent = MasterActor:GetFriendSkillComponent()
    if not _SP.IsValid(PlayerMeshCom) or not _SP.IsValid(FriendSkillComponent) then
        _SP.LogWarning("Ability_Task_ZiDieMiao_ReplaceMesh", "PlayerMeshCom:", tostring(PlayerMeshCom), "FriendSkillComponent:", tostring(FriendSkillComponent))
        return
    end
    local SkeletalMesh = nil
    local AnimClass = nil
    if self.Recover then
        -- 下身时还原mesh调用
        local OriginalPetMesh = nil
        local OriginalMasterMesh = nil
        OriginalPetMesh, OriginalMasterMesh = FriendSkillComponent:GetReplaceMesh()
        if OriginalPetMesh then
            SkeletalMesh = OriginalPetMesh
            if self.PetAnimClass and self.PetAnimClass.GeneratedClass then
                 AnimClass = self.PetAnimClass.GeneratedClass
                 _SP.Log("Ability_Task_ZiDieMiao_ReplaceMesh", " Recover Set Pet Anim Class", self.PetAnimClass.GeneratedClass:GetName())
            else
                local NothingAnimClass = UE4.UMoeBlueprintLibrary.LoadClassWithFullPath(NothingAnimPath)
                if NothingAnimClass then
                    AnimClass = NothingAnimClass
                else
                    _SP.LogWarning("Ability_Task_ZiDieMiao_ReplaceMesh", " Recover Can not Load AnimAsset")
                end
            end
        else
            _SP.LogWarning("Ability_Task_ZiDieMiao_ReplaceMesh", "Recover :Task can not get OriginalPetMesh!")
        end
        PetActor:SetSPMonsterReplaceMeshInfo(AnimClass, SkeletalMesh)
        if OriginalMasterMesh then
            PlayerMeshCom:SetSkeletalMesh(OriginalMasterMesh)
        end
        return
    end
    FriendSkillComponent:SaveReplaceMesh(PetMeshCom.SkeletalMesh, PlayerMeshCom.SkeletalMesh)
    if self.ZiDieMiaoMesh then
        SkeletalMesh = self.ZiDieMiaoMesh
        if self.PetAnimClass and self.PetAnimClass.GeneratedClass then
            AnimClass = self.PetAnimClass.GeneratedClass
            _SP.Log("Ability_Task_ZiDieMiao_ReplaceMesh", "Set Pet Anim Class", self.PetAnimClass.GeneratedClass:GetName())
        else
            local WeaponAnimClass = UE4.UMoeBlueprintLibrary.LoadClassWithFullPath(WeaponAnimPath)
            if WeaponAnimClass then
                AnimClass = WeaponAnimClass
            else
                _SP.LogWarning("Ability_Task_ZiDieMiao_ReplaceMesh","Can not Load AnimAsset")
            end
        end
    else
        _SP.LogWarning("Ability_Task_ZiDieMiao_ReplaceMesh", "Task not get self.ZiDieMiaoMesh, please Check Task Config!")
    end
    PetActor:SetSPMonsterReplaceMeshInfo(AnimClass, SkeletalMesh)
    if self.PlayerMesh then
        PlayerMeshCom:SetSkeletalMesh(self.PlayerMesh)
    end
end

return Ability_Task_ZiDieMiao_ReplaceMesh