local AttrTypeEnum = _SPFeature.ServerEnum.StarPAttrType

---@class AblePlayParticleEffectTask : UAblePlayParticleEffectTask
local AblePlayParticleEffectTask = UE4.Class(nil, "AblePlayParticleEffectTask")
local SPBattleTalentUtil = require("Feature.StarP.Script.System.BattleTalent.SPBattleTalentUtil")

function AblePlayParticleEffectTask:GetShapeScale_Lua(Target)
    local monster
    if Target:GetSPActorType() == UE4.ESPActorType.Pet then
        monster = Target
    end

    if monster then
        if monster and monster.GetShapeScale and type(monster.GetShapeScale) == "function" then
            return monster:GetShapeScale()
        end
    end
    return 1
end

function AblePlayParticleEffectTask:GetCustomEffectTemplateByElementType_Lua(Owner, EffectId)
    return SPBattleTalentUtil.GetCustomEffectTemplateByElementType(Owner, EffectId)
end

---@param Context UAbleAbilityContext
---@param Transform FTransform
function AblePlayParticleEffectTask:GetUltimateLaserLocation_Lua(Context, Transform)
    local StartPos, Quat = SPBattleTalentUtil.GetYSYLaserStartPosAndQuat(Context:GetOwner())
    Transform.Translation = StartPos
    Transform.Rotation = Quat
    --- 使角色朝向修正大招时的特效朝向
    local Owner = Context:GetOwner()
    if _SP.IsValid(Owner) and Owner:GetSPActorType() == UE4.ESPActorType.Player then
        local MasterMeshComponent = Owner:GetComponentByClass(UE4.USkeletalMeshComponent.StaticClass())
        if MasterMeshComponent ~= nil then
            local MasterForward = MasterMeshComponent:GetRightVector()
            local Rotation = UE4.UKismetMathLibrary.Conv_VectorToRotator(MasterForward)
            Transform.Rotation = Rotation:ToQuat()
        end
    end
    return Transform
end

function AblePlayParticleEffectTask:OnPlaySFX_Lua(SFXBankName, SFXEventName, Context, UsePosition, SpawnPosition)
    if UsePosition then
        if _SPFeature and _SPFeature.SoundMgr then
            local Position = SpawnPosition
            local Rotation = UE4.FRotator(0, 0, 0)
            _SPFeature.SoundMgr:DoPlaySoundAtPosition(SFXBankName, SFXEventName, Position, Rotation)
        end
        return
    end
    if UE4.UKismetSystemLibrary.IsValid(Context) then
        _SPFeature.SoundMgr:DelayPlayAudio(0, SFXBankName, SFXEventName, _SPFeature.WwiseManager.SoundType.Default, true, false, Context, false)
    end
end

function AblePlayParticleEffectTask:GetWeaponLocation_Lua(Context, socket, Transform)

    local owner = Context:GetOwner()
    local master = owner
    local weapon
    if owner:GetSPActorType() == UE4.ESPActorType.Pet then
        master = owner:GetMaster()
    end

    if master ~= nil then
        weapon = master:GetCrtEquipWeapon()
    end

    if not _SP.IsValid(weapon) then
        return nil
    end

    local FinalWeapon = weapon
    if self.bIsDualWeapon then
        if weapon:IsA(UE4.ASPDualWeapon) then
            FinalWeapon = self.bUseLeftWeapon and weapon.LeftWeapon or weapon.RightWeapon
        end
    end

    if not _SP.IsValid(FinalWeapon) then
        _SP.Log("SPAbility", "AblePlayParticleEffectTask", "GetWeaponLocation_Lua FinalWeapon is`t Valid.")
        return nil
    end

    local WeaponSocketPos,WeaponSocketQuat = SPBattleTalentUtil.GetWeaponSocketTransform(FinalWeapon, socket)
    Transform.Translation = WeaponSocketPos
    Transform.Rotation = WeaponSocketQuat
    return Transform
end

function AblePlayParticleEffectTask:GetWeaponSocketComp_Lua(Context)

    local owner = Context:GetOwner()
    local master = owner
    local weapon
    if owner:GetSPActorType() == UE4.ESPActorType.Pet then
        master = owner:GetMaster()
    end

    if master ~= nil then
        weapon = master:GetCrtEquipWeapon()
    end

    if not _SP.IsValid(weapon) then
        return nil
    end

    local FinalWeapon = weapon
    if self.bIsDualWeapon then
        if weapon:IsA(UE4.ASPDualWeapon) then
            FinalWeapon = self.bUseLeftWeapon and weapon.LeftWeapon or weapon.RightWeapon
        end
    end

    if not _SP.IsValid(FinalWeapon) then
        _SP.Log("SPAbility", "AblePlayParticleEffectTask", "GetWeaponSocketComp_Lua FinalWeapon is`t Valid.")
        return nil
    end

    return FinalWeapon:GetWeaponMeshComponent()
end

function AblePlayParticleEffectTask:GetOwnerScale_Lua(Context)
    local owner = Context:GetOwner()
    local nScale = 1.0

    if owner:GetSPActorType() == UE4.ESPActorType.Pet then
        local nRarity = owner:GetAttributeValue(AttrTypeEnum.SPAT_BossType)
        if nRarity == _SPFeature.ServerEnum.SPPetBossType.SP_BOSS_GENERAL then
            nScale = owner.MonsterConfig.monsterScale or 1.0
        end
        if nScale <= 0 then
            nScale = 1.0
        end
    end

    _SP.Log("SPAbility", "AblePlayParticleEffectTask", "GetOwnerScale_Lua", nScale)

    return nScale
end

return AblePlayParticleEffectTask