local Ability_Task_ZhuLiYe_FireLastShot = UE4.Class(nil, "Ability_Task_ZhuLiYe_FireLastShot")

function Ability_Task_ZhuLiYe_FireLastShot:OnTaskStartBP(Context)
    _SP.Log("SPAbilityTask", "Ability_Task_ZhuLiYe_FireLastShot:OnTaskStartBP")

    -- 获取Owner对象，目前仅支持玩家角色
    local Owner = Context:GetOwner()
    local Character = Owner and Owner:Cast(UE4.ABaseMoeGameCharacter)
    if not Character then
        return
    end

    -- 获取武器对象
    local InventoryComponent = Character:SafeGetInventoryComponent()
    local CurrentWeapon = InventoryComponent and InventoryComponent:GetCrtEquipWeapon()
    local Weapon_ZhuLiYe = CurrentWeapon and CurrentWeapon:Cast(UE4.ASP_PartnerWeapon_ZhuLiYe_DaPao_Base_C)
    if not Weapon_ZhuLiYe then
        return
    end

    -- 获取技能配置Id与伤害配置Id
    local AbilityId = Context:GetAbilityId()
    local AbilityUniqueID = Context:GetAbilityUniqueID()
    -- local AbilityDataTable = {}
    -- if not _SP.SPLuaUtility:GetSkillInfo_Lua(AbilityId, AbilityDataTable) then
    --     return
    -- end
    local AbilityDataTable, isPassive = _SP.SPGameplayUtils:GetSkillData(AbilityId)
    if not AbilityDataTable then
        return
    end

    local DamageIds = AbilityDataTable.damageIds or {}
    local DamageId = tonumber(DamageIds[self.DamageIndex + 1]) or -1

    local WeaponLocation = Weapon_ZhuLiYe:K2_GetActorLocation()
    local CharacterForward = Character.Mesh:GetRightVector()

    -- 获取投射物的发射方向
    local ShootDirection = self.FireDirection
    -- 对于将发射方向配置成瞄准方向或者准星方向的情况，先获取到对应的瞄准方向
    local ShootRotationType = UE4.ESPShootRotationType.UseSelfRotation
    if self.OverrideProjectile then
        ShootRotationType = self.ShootRotationType
    elseif Weapon_ZhuLiYe.LastProjectile then
        ShootRotationType = Weapon_ZhuLiYe.LastProjectile:GetShootRotationType()
    end

    local CameraLoc = UE4.FVector()
    local CameraRot = UE4.FRotator()
    local CameraForwardVec = UE4.FVector()

    CameraLoc, CameraRot = UE4.USPGameLibrary.GetCameraLocationAndRotation()
    CameraForwardVec = UE4.UKismetMathLibrary.Conv_RotatorToVector(CameraRot)

    if ShootRotationType == UE4.ESPShootRotationType.UsePlayerAimingPoint then
        ShootDirection = UE4.USPGameLibrary.GetAimingDirection(Weapon_ZhuLiYe, WeaponLocation, Character, UE4.ESPTraceType.ActualMesh, 10000, CharacterForward)
    elseif ShootRotationType == UE4.ESPShootRotationType.UseCrosshairDirection then
        ShootDirection = UE4.USPGameLibrary.GetCrossHairDirection(Weapon_ZhuLiYe, WeaponLocation, Character, UE4.ESPTraceType.ActualMesh, CharacterForward)
        UE4.USPGameLibrary.RectifyCrossHairShootDirection(Character, ShootDirection, self.MCrossHairAngleConstraint, CharacterForward)
    end

    -- 最后将猪立叶武器的最后一发投射物（也就是猪立叶模型本身）发射出去
    if UE4.USPGameLibrary.IsDedicatedServer() or UE4.USPGameLibrary.IsStandAlone() or Character:IsLocalCharacter() then
        Weapon_ZhuLiYe:ServerShootLastProjectile(AbilityId, DamageId, AbilityUniqueID, self.OverrideProjectile, self.ProjectileInfo, self.ShootRotationType, WeaponLocation, ShootDirection, self.MCrossHairAngleConstraint, CameraLoc, CameraForwardVec)
        _SP.Log("SPAbilityTask", "Last shot of ZhuLiYe rocket of " .. Character:GetUID() .. " has been launched!")
    end
end

function Ability_Task_ZhuLiYe_FireLastShot:OnTaskEndBP(Context)
    _SP.Log("SPAbilityTask", "Ability_Task_ZhuLiYe_FireLastShot:OnTaskEndBP")
end

function Ability_Task_ZhuLiYe_FireLastShot:IsSingleFrameBP()
    return true
end

return Ability_Task_ZhuLiYe_FireLastShot