
local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local SPLuaUtility = require("Feature.StarP.Script.System.SPLuaUtility")
local SPGameLuaUtility = require("Feature.StarP.Script.System.SPLuaUtility")

local Ability_Task_LaserAttach = UE4.Class(nil, "Ability_Task_LaserAttach")

local function Log(...)
    _SP.Log("SPAbility", "[Ability_Task_LaserAttach]", ...)
end

function Ability_Task_LaserAttach:OnTaskStartBP(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    if self.AddBuffWhenDamage then
        ScratchPad.AddBuffActors = {}
    end
    ScratchPad.IntervalTimer = self.Interval
    ScratchPad.SpawnTransform = nil
    self.IsDoDamage = false
    self.IsPlayedHitParticle = false
    self.CurrentTime = 0

    self:DoQuery(Context)

    if not _SP.IsDS then
        self:PlayParticleEffect(Context)
        self:RefreshParticle(Context)
        if self.IsDoDamage then
            self.IsPlayedHitParticle = true
            self:PlayHitParticleEffect(Context)
            self:RefreshHitParticle(Context)
        end
    end
end

function Ability_Task_LaserAttach:OnTaskTickBP(Context, deltaTime)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    if not self.IsDoDamage then
        self.CurrentTime = self.CurrentTime + deltaTime
        if self.CurrentTime >= self.AttachTime then
            self.IsDoDamage = true
        end
    end

    self:DoQuery(Context)
    if _SP.IsDSorStandalone and self.IsDoDamage then
        ScratchPad.IntervalTimer = ScratchPad.IntervalTimer - deltaTime
        if ScratchPad.IntervalTimer <= 0 then
            ScratchPad.IntervalTimer = self.Interval
            self:DoDamage(Context)
        end
    end
    if not _SP.IsDS then
        self:RefreshParticle(Context)
        if self.IsDoDamage then
            if not self.IsPlayedHitParticle then
                self:PlayHitParticleEffect(Context)
                self.IsPlayedHitParticle = true
            end
            self:RefreshHitParticle(Context)
        end
    end
end

function Ability_Task_LaserAttach:OnTaskEndBP(Context, Result)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    ScratchPad.IntervalTimer = 0
    self.IsDoDamage = false
    self.IsPlayedHitParticle = false
    self.CurrentTime = 0

    if not _SP.IsDS then
        self:StopParticleEffect(Context)
        self:StopHitParticleEffect(Context)
    end
end

function Ability_Task_LaserAttach:DoQuery(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    local SpawnTransform = UE4.USPAbilityFunctionLibrary.GetAbilityTargetTypeLocationTransform(Context, self.QueryLocation)
    ScratchPad.SpawnTransform = SpawnTransform
    local TraceStart = SpawnTransform.Translation

    local Orientation = SpawnTransform.Rotation:ToRotator()

    ScratchPad.QueryResultActorArray = {}
    ScratchPad.QueryHitResultPointArray = {}

    local TargetArray = Context:GetTargetActors()
    local test = TargetArray:Length()
    if TargetArray and TargetArray:Length() > 0 then
        local Distance = nil
        local TargetLocation = nil
        local targetNum = TargetArray:Length()
        --计算距离剔除不符合激光距离的目标
        for i = targetNum, 1, -1 do
            TargetLocation = TargetArray:Get(i):K2_GetActorLocation()
            Distance = UE4.UKismetMathLibrary.Vector_Distance(TraceStart, TargetLocation)
            if self.ParticleMaxLength < Distance then
                TargetArray:Remove(i)
            end
        end
        targetNum = TargetArray:Length()
        if targetNum > 0 then
            for i = 1, targetNum do
                ScratchPad.QueryResultActorArray[i] = TargetArray:Get(i)
                ScratchPad.QueryHitResultPointArray[i] = TargetArray:Get(i):K2_GetActorLocation()
            end
        end
    end
end

function Ability_Task_LaserAttach:DoDamage(Context)
    if not _SP.IsDS and not _SP.IsStandalone then
        return
    end

    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    if not ScratchPad.QueryResultActorArray or #ScratchPad.QueryResultActorArray == 0 then
        return
    end

    local AbilityId = Context:GetAbilityId()
    local AbilityUniqueID = Context:GetAbilityUniqueID()
    local AbilityDataTable, isPassive = _SP.SPGameplayUtils:GetSkillData(AbilityId)
    if not AbilityDataTable then
        return
    end

    local DamageIds = AbilityDataTable.damageIds or {}
    local DamageId = tonumber(DamageIds[self.DamageIndex + 1]) or -1
    local Owner = Context:GetOwner()
    local Instigator = Context:GetInstigator() or Owner

    local DamageEvent = UE4.FDamageEvent()
    for _,v in pairs(ScratchPad.QueryResultActorArray) do
        if UE4.UKismetSystemLibrary.IsValid(v) then
            local DamageInfos = {
                {
                    DamagedActor = v,
                    DamageEvent = DamageEvent,
                }
            }

            SPGameLuaUtility:DoDamageByDamageId(
                    AbilityId,
                    DamageId,
                    DamageInfos,
                    AbilityUniqueID,
                    Instigator:GetController(),
                    Instigator
            )

            self:DoBuffLogic(Context, Instigator, v)
        end
    end

end

function Ability_Task_LaserAttach:DoBuffLogic(Context, SourceActor, TargetActor)
    if not self.AddbuffRepeat then
        local ScratchPad = self:GetScratchPad(Context)
        if not ScratchPad or not ScratchPad.AddBuffActors then
            return
        end
        if ScratchPad.AddBuffActors[TargetActor] ~= nil then
            return
        end
        local addSuccess = SPLuaUtility:AddBuffByDamageSkill_Lua(Context:GetAbilityId(), self.BuffID, self.BuffLayer, self.IndexBuffFromSkill, SourceActor, TargetActor)
        if addSuccess then
            ScratchPad.AddBuffActors[TargetActor] = true
        end
    else
        SPLuaUtility:AddBuffByDamageSkill_Lua(Context:GetAbilityId(), self.BuffID, self.BuffLayer, self.IndexBuffFromSkill, SourceActor, TargetActor)
    end

end

function Ability_Task_LaserAttach:CalculateParticleScale(Context, Transform, PointIndex)
    local CalculatedScale = 1
    if self.ParticleNormalizeLength <= 0 then
        return CalculatedScale
    end
    local ScratchPad = self:GetScratchPad(Context)
    if ScratchPad and ScratchPad.QueryHitResultPointArray[PointIndex] then
        local targetLocation = ScratchPad.QueryHitResultPointArray[PointIndex]
        local Distance = UE4.UKismetMathLibrary.Vector_Distance(Transform.Translation, targetLocation)
        if self.ParticleNormalizeLength ~= 0 then
            CalculatedScale = math.min(Distance / self.ParticleNormalizeLength, self.ParticleMaxLength / self.ParticleNormalizeLength)
            if not self.IsDoDamage then
                CalculatedScale = CalculatedScale * self.CurrentTime / self.AttachTime
            end
            return CalculatedScale
        end
    end
    return self.ParticleMaxLength / self.ParticleNormalizeLength
end

function Ability_Task_LaserAttach:SpawnOrModifyPSC(Context, ScratchPad)
    local Target = Context:GetOwner()

    for i = 1, #ScratchPad.QueryHitResultPointArray do
        -- calculate scale
        local SpawnTransform = UE4.USPAbilityFunctionLibrary.GetAbilityTargetTypeLocationTransform(Context, self.ParticleLocation)
        local ModifiedScale = self:CalculateParticleScale(Context, SpawnTransform)
        SpawnTransform.Scale3D.X = self.CanEditScaleX and ModifiedScale or self.Scale
        SpawnTransform.Scale3D.Y = self.CanEditScaleY and ModifiedScale or self.Scale
        SpawnTransform.Scale3D.Z = self.CanEditScaleZ and ModifiedScale or self.Scale
        if _SP.IsValid(ScratchPad.SpawnedEffectArray[i]) then
            local dirToImpactPoint = UE4.UKismetMathLibrary.Normal(ScratchPad.QueryHitResultPointArray[i] - SpawnTransform.Translation)
            local rotation = UE4.UKismetMathLibrary.Conv_VectorToRotator(dirToImpactPoint)
            SpawnTransform.Rotation = rotation:ToQuat()
            --just modify transform and particle length
            ScratchPad.SpawnedEffectArray[i]:K2_SetWorldTransform(SpawnTransform)
        else
            -- spawn new PSC
            local EffectTemplate = self.EffectTemplate
            local SpawnedEffect
            local Location = self.QueryLocation
            if EffectTemplate then
                if self.AttachToSocket then
                    local AttachComponent = Target:GetComponentByClass(UE4.USceneComponent:StaticClass())
                    SpawnedEffect = UE4.UGameplayStatics.SpawnEmitterAttached(EffectTemplate, AttachComponent,
                    Location.m_Socket, SpawnTransform.Translation,
                    SpawnTransform.Rotation:ToRotator(),
                    SpawnTransform.Scale3D,
                    UE4.EAttachLocation.KeepWorldPosition)
                else
                    SpawnedEffect = UE4.UGameplayStatics.SpawnEmitterAtLocation(Target:GetWorld(), EffectTemplate, SpawnTransform.Translation, SpawnTransform.Rotation:ToRotator())
                end
                SpawnedEffect.TranslucencySortPriority = self.RenderSortPriority
            end
            if SpawnedEffect then
                ScratchPad.SpawnedEffectArray[i] = SpawnedEffect
            end
        end
    end
end

function Ability_Task_LaserAttach:PlayParticleEffect(Context)
    if not self.EffectTemplate then
        return
    end

    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    if not ScratchPad.QueryResultActorArray or #ScratchPad.QueryResultActorArray == 0 then
        return
    end

    ScratchPad.SpawnedEffectArray = {}

    self:SpawnOrModifyPSC(Context, ScratchPad)
end

function Ability_Task_LaserAttach:StopParticleEffect(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    if ScratchPad.SpawnedEffectArray then
        for i = 1, #ScratchPad.SpawnedEffectArray do
            if self.DetachOnStop then
                ScratchPad.SpawnedEffectArray[i]:K2_DetachFromComponent(UE4.EAttachmentRule.KeepWorld, UE4.EAttachmentRule.KeepWorld, UE4.EAttachmentRule.KeepWorld, true)
            end
            UE4.USPGameLibrary.DeactivateParticleSystem(ScratchPad.SpawnedEffectArray[i])
        end
        ScratchPad.SpawnedEffectArray = nil
    end
end


function Ability_Task_LaserAttach:RefreshParticle(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    if not ScratchPad.QueryResultActorArray or #ScratchPad.QueryResultActorArray == 0 then
        return
    end

    if ScratchPad.SpawnedEffectArray then
        self:SpawnOrModifyPSC(Context, ScratchPad)
    end
end

function Ability_Task_LaserAttach:ResetScratchPadBP(ScratchPad)
    if ScratchPad then
        ScratchPad.SpawnedEffectArray = nil
        ScratchPad.QueryResultActorArray = nil
        ScratchPad.IntervalTimer = 0
        ScratchPad.AddBuffActors = nil
        ScratchPad.SpawnTransform = nil
        ScratchPad.QueryHitResultPointArray = nil
        ScratchPad.SpawnedHitBodyEffectArray = nil
    end
end

function Ability_Task_LaserAttach:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.LaserAttach)
end

function Ability_Task_LaserAttach:PlayHitParticleEffect(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    ScratchPad.SpawnedHitBodyEffectArray = {}

    self:SpawnOrModifyHitPSC(Context, ScratchPad)
end

function Ability_Task_LaserAttach:SpawnOrModifyHitPSC(Context, ScratchPad)
    if not self.HitBodyEffectTemplate then
        return
    end
    local SpawnTransform = ScratchPad.SpawnTransform
    local Target = Context:GetOwner()
    local HitEffectTemplate
    -- 先判断击中的Actor类型，决定使用的特效
    local HitActor = nil
    for i = 1, #ScratchPad.QueryResultActorArray do
        HitActor = ScratchPad.QueryResultActorArray[i]
        if HitActor and (HitActor:Cast(UE4.ASPGameCharacterBase) or HitActor:Cast(UE4.ASPGameMonsterBase)) then
            -- 击中玩家或星兽
            HitEffectTemplate = self.HitBodyEffectTemplate
            if ScratchPad.QueryHitResultPointArray[i] then
                local SpawnLocation = ScratchPad.QueryHitResultPointArray[i]

                -- 根据QueryRotation旋转受击特效
                local SpawnRotation = UE4.FRotator(0, 0, 0)
                SpawnRotation.Pitch = self.BodyEffectRotatePitch and -SpawnTransform.Rotation:ToRotator().Pitch or 0
                SpawnRotation.Yaw = self.BodyEffectRotateYaw and SpawnTransform.Rotation:ToRotator().Yaw or 0
                SpawnRotation.Roll = self.BodyEffectRotateRoll and SpawnTransform.Rotation:ToRotator().Roll or 0

                local SpawnScale = UE4.FVector(1, 1, 1)
                local SpawnTransForm = UE4.FTransform(SpawnRotation:ToQuat(), SpawnLocation)

                if _SP.IsValid(ScratchPad.SpawnedHitBodyEffectArray[i]) then
                    -- 特效存在，更新位置
                    ScratchPad.SpawnedHitBodyEffectArray[i]:K2_SetWorldTransform(SpawnTransForm)
                else
                    -- 特效不存在，生成特效
                    local SpawnedHitEffect = UE4.UGameplayStatics.SpawnEmitterAtLocation(
                    Target:GetWorld(), HitEffectTemplate, SpawnLocation,
                    SpawnRotation, SpawnScale)
                    if _SP.IsValid(SpawnedHitEffect) then
                        ScratchPad.SpawnedHitBodyEffectArray[i] = SpawnedHitEffect
                    end
                end
            else
                -- 没有impactpoint，销毁HitEffect
                self:StopHitParticleEffect(Context)
            end
        end
    end
end

function Ability_Task_LaserAttach:RefreshHitParticle(Context)
    -- Tick中刷新HitEffect，需要根据ImpactPoint更新可视性以及位置
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    if ScratchPad.SpawnedHitBodyEffectArray then
        self:SpawnOrModifyHitPSC(Context, ScratchPad)
    end
end

function Ability_Task_LaserAttach:StopHitParticleEffect(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    if ScratchPad.SpawnedHitBodyEffectArray then
        for _, v in pairs(ScratchPad.SpawnedHitBodyEffectArray) do
            UE4.USPGameLibrary.DeactivateParticleSystem(v)
        end
        ScratchPad.SpawnedHitBodyEffectArray = nil
    end
end

-- 为了特效位置位于激光中央
function Ability_Task_LaserAttach:RevisedImpactPoint(ImpactPoint, StartPoint, Orientation)
    if not ImpactPoint then
        return
    end
    local Direction = UE4.UKismetMathLibrary.Conv_RotatorToVector(Orientation)
    local t = 0
    if Direction.Z ~= 0 then
        t = (ImpactPoint.Z - StartPoint.Z) / Direction.Z
    end
    local FinalPoint = UE4.FVector(
        StartPoint.X + t * Direction.X,
        StartPoint.Y + t * Direction.Y,
        StartPoint.Z + t * Direction.Z
    )
    return FinalPoint
end

return Ability_Task_LaserAttach