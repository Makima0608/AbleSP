---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by bladeyuan.
--- DateTime: 2024/6/18 23:44
---
local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local SPLuaUtility = require("Feature.StarP.Script.System.SPLuaUtility")
local SPBattleTalentUtil = require("Feature.StarP.Script.System.BattleTalent.SPBattleTalentUtil")
local Ability_Task_LaserWeapon = UE4.Class(nil, "Ability_Task_LaserWeapon")

local TempVector = UE4.FVector(0, 0, 0)
local MAX_HITEFFECT_COUNT = 5

local function Log(...)
    _SP.Log("SPAbility", "[Ability_Task_LaserWeapon]", ...)
end

local function TaskTargetValid(Target)
    if Target and Target:IsA(UE4.ASPGameMonsterBase) then
        -- 判断是否为正在被骑乘的星兽
        -- local pawn = Target:FindRelatedPlayerPawn()
        return Target:IsRidden()
    elseif Target and Target:IsA(UE4.AMoeGameCharacter) then
        return true
    end
    return false
end

function Ability_Task_LaserWeapon:GetTransformToWeaponCross(Context, DeltaTime)
    local Target = Context:GetOwner()
    local Target_mount = Context:GetOwner()
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    if Target:IsA(UE4.ASPGameMonsterBase) then
        -- 判断是否有乘骑Owner
        local pawn = Target:FindRelatedPlayerPawn()
        if not pawn or not UE4.UKismetSystemLibrary.IsValid(pawn) then
            return
        else
            ---@comment 骑乘状态下要将Target替换成Character
            Target = pawn
        end
    end
    if Target:IsA(UE4.AMoeGameCharacter) then
        local weapon = Target:GetCrtEquipWeapon()
        if _SP.IsValid(weapon) then
            local Location = self.Location
            local weaponStartPos = weapon:GetStartPosition()
            local CameraLocation = weapon.CameraLocation
            local CameraRotation = weapon.CameraRotation
            if Target:IsOwnedByLocalPlayerController() then
                -- 1P 更新FireTransform
                if weapon.UpdateFireTransform then
                    weapon:UpdateFireTransform()
                end
                local CameraManager = UE4.UGameplayStatics.GetPlayerCameraManager(_SP.GetCurrentWorld(), 0)
                CameraLocation = CameraManager:GetCameraLocation()
                CameraRotation = CameraManager:GetCameraRotation()
            end
            local CameraEndPosition = CameraLocation + CameraRotation:GetForwardVector() * weapon.WeaponRange
            -- ecoliwu 此处location做特别调整因为ABP在DS上不同步动画，骨骼没更新，所以
            -- 位置不对，需要调整为武器StartPos的位置上做偏移
            -- local SpawnTransform = UE4.USPAbilityFunctionLibrary.GetAbilityTargetTypeLocationTransform(Context, Location)
            local SpawnTransform = UE4.FTransform()

            local forwardVec, rightVec, upVec = self:GetOwnerRotationDirection(Context, Target)--SpawnTransform.Rotation:ToRotator()
            local fixedLocationOffset = rightVec * Location.m_Offset.X
            + forwardVec * Location.m_Offset.Y
            + upVec * Location.m_Offset.Z
            SpawnTransform.Translation = weaponStartPos + fixedLocationOffset

            local DoCameraHit = true
            -- 发射点朝向相机终点的朝向
            local weaponRotation = self:GetWeaponRotation(weaponStartPos, CameraEndPosition)

            --烁金杖大招
            local weaponQuat = weapon:GetStartQuat()
            if self.IsUltimateWeapon == true then
                weaponStartPos, weaponQuat = SPBattleTalentUtil.GetYSYLaserStartPosAndQuat(Target)
                SpawnTransform.Translation = weaponStartPos
                weaponRotation = weaponQuat
                DoCameraHit = false
            end

            -- eocliwu: 此配置计算方式有问题，屏蔽
            -- local CameraMgr = UE4.UGameplayStatics.GetPlayerCameraManager(_SP.GetCurrentWorld(), 0)
            -- if self.UseCrosshair and CameraMgr:GetCameraRotation().Pitch > self.CrossValidMinAngle then
            --     --使用准星方向(摄像机方向计算武器旋转)
            --     local CameraForward = CameraMgr:GetCameraRotation():GetForwardVector()
            --     local CrossEndLocation = CameraMgr:GetCameraLocation() + CameraForward *
            --     (weapon.WeaponRange + UE4.UKismetMathLibrary.Vector_Distance(weaponStartPos,CameraMgr:GetCameraLocation()))
            --     local Rot = UE4.UKismetMathLibrary.FindLookAtRotation(weaponStartPos, CrossEndLocation)
            --     weaponRotation = Rot
            --     _SP.LogDebug("SPAbility", "[Ability_Task_LaserWeapon]", "GetTransformToWeaponCross",
            --     "根据准星调整武器旋转", weaponRotation)
            -- end

            -- 发射点到相机终点的距离
            local LaserRange = UE4.UKismetMathLibrary.Vector_Distance(weaponStartPos, CameraEndPosition)

            -- calculate end pos
            local endPos = weaponStartPos + weaponRotation:GetForwardVector() * LaserRange
            -- Hit query
            local bShowDebug = UE4.USPAbilityFunctionLibrary.IsVerboseDebug()
            local DebugTrace = nil
            if bShowDebug then
                DebugTrace = UE4.EDrawDebugTrace.ForOneFrame
            else
                DebugTrace = UE4.EDrawDebugTrace.None
            end
            local ignoreActors = UE4.TArray(UE4.AActor)
            ignoreActors:Add(Target)

            local CameraHitActor = nil
            local CameraHitResult = nil

            local ObjectTypes = _SP.SPAbilityUtils.GetObjectTypesPresent(Context, self.CollisionChannel.Present, self.CollisionChannel.Channels)
            if DoCameraHit then
                --相机准星射线检测
                local bCameraHit = false
                CameraHitResult, bCameraHit = UE4.UKismetSystemLibrary.LineTraceSingleForObjects(
                        self,
                        CameraLocation,
                        CameraEndPosition,
                        ObjectTypes,
                        false,
                        ignoreActors,
                        DebugTrace,
                        nil,
                        true,
                        UE4.FLinearColor(1,1,1,1),
                        UE4.FLinearColor(1,0,0,1),
                        5.0)
                --确保碰撞点在武器前方
                if UE4.UKismetMathLibrary.Vector_Distance(CameraHitResult.ImpactPoint, CameraLocation)
                    <
                   UE4.UKismetMathLibrary.Vector_Distance(CameraLocation, weaponStartPos) then
                    bCameraHit = false
                end

                if bCameraHit then
                    CameraHitActor = CameraHitResult.Actor
                    Log("CameraHitActor: ", CameraHitActor:GetName())
                    if not self.IsPenetrate then
                        endPos = CameraHitResult.ImpactPoint
                        Log("CameraHit End Pos : ", endPos)
                    end
                end
            end

            --激光射线检测
            local HitResult, bHit = UE4.UKismetSystemLibrary.LineTraceSingleForObjects(
                self,
                weaponStartPos,
                endPos,
                ObjectTypes,
                false,
                ignoreActors,
                DebugTrace,
                nil,
                true,
                UE4.FLinearColor(1,1,1,1),
                UE4.FLinearColor(1,0,0,1),
                5.0)
            local targetPos
            -- 有Hit修正特效的EndPos
            if not bHit then
                targetPos = endPos
                -- self:PlayTrailingEffect(ScratchPad, weaponStartPos, endPos)
                -- self:StopHitEffect(ScratchPad, true)
                --_SP.Log("Ability_Task_LaserWeapon", "============")
                Log("not Hit TargetPos == EndPos")
            else
                if CameraHitActor and CameraHitActor == HitResult.Actor then
                    --激光射线于相机准星射线击中对象一致
                    targetPos = CameraHitResult.ImpactPoint
                    Log("CameraHitActor == HitResult.Actor")
                else
                    targetPos = HitResult.ImpactPoint
                    Log("targetPos = HitResult.ImpactPoint, Actor = ", HitResult.Actor:GetName())
                end
                -- self:StopTrailingEffect(ScratchPad, true)
                -- self:PlayHitEffect(ScratchPad, HitResult.ImpactPoint, HitResult.ImpactNormal)
            end
            self:PlayTrailingEffect(ScratchPad, weaponStartPos, targetPos, Target)

            local dirToImpactPoint = UE4.UKismetMathLibrary.Normal(targetPos - SpawnTransform.Translation)
            local rotation = UE4.UKismetMathLibrary.Conv_VectorToRotator(dirToImpactPoint)
            SpawnTransform.Rotation = rotation:ToQuat()
            -- effect dis
            local Distance = UE4.UKismetMathLibrary.Vector_Distance(SpawnTransform.Translation, targetPos)
            self.Distance = Distance
            local ScaleX
            if self.ParticleNormalizeLength ~= 0 then
                ScaleX = math.min(Distance / self.ParticleNormalizeLength, self.ParticleMaxLength / self.ParticleNormalizeLength)
            else
                ScaleX = 1.0
            end
            --_SP.Log("Ability_Task_LaserWeapon", "Log: ", self.Distance, ScaleX, _SPFeature.MoeGameUtils.TableToString(targetPos))
            SpawnTransform.Scale3D.X = self:CalculateParticleScaleFactor(Context, ScaleX, DeltaTime)
            if ScratchPad.SpawnTransform and self.IsNeedThick then
                SpawnTransform.Scale3D.Y = ScratchPad.SpawnTransform.Scale3D.Y
                SpawnTransform.Scale3D.Z = ScratchPad.SpawnTransform.Scale3D.Z
            end
            ---@comment 骑乘状态需要用到坐骑原始socket的位置
            if Target_mount ~= nil and Target_mount:IsA(UE4.ASPGameMonsterBase) and self.Location and self.Location.m_Socket then
                local AttachComponent = Target_mount:GetComponentByClass(UE4.USceneComponent:StaticClass())
                if AttachComponent then
                    local socketTransform = AttachComponent:GetSocketTransform(Location.m_Socket)
                    SpawnTransform.Translation = socketTransform.Translation
                end
            end
            ScratchPad.SpawnTransform = SpawnTransform
        end
    end
end

function Ability_Task_LaserWeapon:GetWeaponRotation(StartPos, EndPos)
    local DirToEnd = UE4.UKismetMathLibrary.Normal(EndPos - StartPos)
    local Rotation = UE4.UKismetMathLibrary.Conv_VectorToRotator(DirToEnd)
    return Rotation:ToQuat()
end

function Ability_Task_LaserWeapon:OnTaskStartBP(Context)
    if not TaskTargetValid(Context:GetOwner()) then
        return
    end
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    if self.AddBuffWhenDamage then
        ScratchPad.AddBuffActors = {}
    end
    ScratchPad.IntervalTimer = self.Interval
    ScratchPad.QueryScale = self.QueryScale

    self:GetTransformToWeaponCross(Context)
    if _SP.IsDSorStandalone then
        self:DoQuery(Context)
        self:DoDamage(Context)
    end
    if not _SP.IsDS then
        self:PlayParticleEffect(Context)
        self:RefreshParticle(Context)
    end
end

function Ability_Task_LaserWeapon:OnTaskTickBP(Context, deltaTime)
    if not TaskTargetValid(Context:GetOwner()) then
        return
    end
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    local bDoDamge = false
    ScratchPad.IntervalTimer = ScratchPad.IntervalTimer - deltaTime
    if ScratchPad.IntervalTimer <= 0 then
        ScratchPad.IntervalTimer = self.Interval
        bDoDamge = true
    end
    if self.IsNeedThick then
        ScratchPad.QueryScale = ScratchPad.QueryScale + self.AddThickValue * deltaTime
        -- 激光变粗不需要变X轴
        -- ScratchPad.SpawnTransform.Scale3D.X = ScratchPad.SpawnTransform.Scale3D.X + self.AddThickValue * deltaTime
        ScratchPad.SpawnTransform.Scale3D.Y = ScratchPad.SpawnTransform.Scale3D.Y + self.AddThickValue * deltaTime
        ScratchPad.SpawnTransform.Scale3D.Z = ScratchPad.SpawnTransform.Scale3D.Z + self.AddThickValue * deltaTime
    end
    self:GetTransformToWeaponCross(Context, deltaTime)
    if _SP.IsDSorStandalone then
        self:DoQuery(Context)
        if bDoDamge then
            self:DoDamage(Context)
        end
    end
    if not _SP.IsDS then
        if ScratchPad.StartTime < self.SpawnFactorTime then
            ScratchPad.StartTime = ScratchPad.StartTime + deltaTime
        end
        self:RefreshParticle(Context)
        if bDoDamge then
            ---@comment 判断穿透特效
            self:PlayPenetrationEffect(Context)
        end
    end
end

function Ability_Task_LaserWeapon:OnTaskEndBP(Context, Result)
    if not TaskTargetValid(Context:GetOwner()) then
        return
    end
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    ScratchPad.IntervalTimer = 0

    if not _SP.IsDS then
        self:StopParticleEffect(Context)
        -- self:StopHitEffect(ScratchPad, false)
        self:StopAllHitEffect(ScratchPad, false)
        self:StopTrailingEffect(ScratchPad, false)
    end
end

function Ability_Task_LaserWeapon:DoQuery(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    local Owner = Context:GetOwner()
    local Transform = ScratchPad.SpawnTransform and ScratchPad.SpawnTransform or UE4.USPAbilityFunctionLibrary.GetAbilityTargetTypeLocationTransform(Context, self.Location)
    local TraceStart = Transform.Translation
    local Orientation = Transform.Rotation:ToRotator()

    local overrideRange = {
        HalfExtents = UE4.FVector(0, 0, 0)
    }
    overrideRange.HalfExtents.X = (self.Distance and self.Distance/2 or self.ShapeRange.HalfExtents.X/2) * ScratchPad.QueryScale.X
    overrideRange.HalfExtents.Y = self.ShapeRange.HalfExtents.Y * ScratchPad.QueryScale.Y
    overrideRange.HalfExtents.Z = self.ShapeRange.HalfExtents.Z * ScratchPad.QueryScale.Z
    local ObjectTypes = _SP.SPAbilityUtils.GetObjectTypesPresent(Context, self.DamageChannel.Present, self.DamageChannel.Channels)
    -- local HitResults = SPAbilityUtils.DetectOthersByTickTrace(
    --     Context,
    --     Owner,
    --     self.CollisionShape,
    --     ObjectTypes,
    --     TraceStart,
    --     Orientation,
    --     overrideRange,
    --     true,
    --     true
    -- )

    local HitResults = UE4.TArray(UE4.FHitResult)
    UE4.USPAbilityFunctionLibrary.DoCollisionDetect(Context, Owner, HitResults, self.CollisionShape, ObjectTypes, TraceStart, Orientation, true, overrideRange.HalfExtents, self.ShapeRange.Radius, self.ShapeRange.ConeRadius,
    self.ShapeRange.ConeLength, self.ShapeRange.HalfHeight, self.ShapeRange.CylinderAngle, self.ShapeRange.CylinderInnerRadius, self.ShapeRange.CylinderOuterRadius, self.ShapeRange.CylinderHeight, _SP.IsDSorStandalone and _SP.DS._bShowDebugCollision)

    ScratchPad.QueryResults = {}
    for i = 1, HitResults:Length() do
        table.insert(ScratchPad.QueryResults, HitResults:Get(i))
    end
end

function Ability_Task_LaserWeapon:DoDamage(Context)
    if not _SP.IsDS and not _SP.IsStandalone then
        return
    end

    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    if not ScratchPad.QueryResults or #ScratchPad.QueryResults == 0 then
        return
    end

    local AbilityId = Context:GetAbilityId()
    local AbilityUniqueID = Context:GetAbilityUniqueID()
    -- local AbilityDataTable = {}
    -- local Ret = SPLuaUtility:GetSkillInfo_Lua(AbilityId, AbilityDataTable)
    local AbilityDataTable, isPassive = _SP.SPGameplayUtils:GetSkillData(AbilityId)
    if not AbilityDataTable then
        return
    end

    local DamageIds = AbilityDataTable.damageIds or {}
    local DamageId = tonumber(DamageIds[self.DamageIndex + 1]) or -1
    local Owner = Context:GetOwner()
    local Instigator = Context:GetInstigator() or Owner

    for _, Target in ipairs(ScratchPad.QueryResults) do
        local DamageEvent = UE4.FDamageEvent()
        if UE4.UKismetSystemLibrary.IsValid(Target.Actor) then
            -- Log("DoDamage take damage to ", Target.Actor:GetName())
            _SP.SPDamageSystem:DoDamageByDamageId(AbilityId, DamageId, AbilityUniqueID, Target.Actor, DamageEvent, Instigator:GetController(), Instigator)
            self:DoBuffLogic(Context, Owner, Target.Actor)
        end
    end
end

function Ability_Task_LaserWeapon:DoBuffLogic(Context, SourceActor, TargetActor)
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

------------------------------
---Only Effect In Client Start
------------------------------

function Ability_Task_LaserWeapon:CalculateParticleScaleFactor(Context, ScaleX, DeltaTime)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return ScaleX
    end
    if ScratchPad.bInGrow and ScratchPad.StartTime and self.SpawnFactorTime > 0 and ScratchPad.StartTime < self.SpawnFactorTime then
        if not DeltaTime then
            DeltaTime = 0
        end
        local currScale
        if ScaleX <= ScratchPad.LastFrameScale then
            currScale = ScaleX
            --- Stop Factor
            ScratchPad.bInGrow = false
        else
            currScale = ScratchPad.LastFrameScale + (DeltaTime / self.SpawnFactorTime) * ScaleX
            ScratchPad.LastFrameScale = currScale
        end
        return currScale
    end
    ScratchPad.bInGrow = false
    return ScaleX
end

local function DistanceSquare(a, b)
    local line = b - a
    return line.X * line.X + line.Y * line.Y + line.Z * line.Z
end

function Ability_Task_LaserWeapon:PlayPenetrationEffect(Context)
    if self.HitEffectTemplate then
        local ignoreActors = UE4.TArray(UE4.AActor)
        local Target = Context:GetOwner()
        ignoreActors:Add(Target)
        if Target:IsA(UE4.ASPGameMonsterBase) then
            -- 判断是否有乘骑Owner
            local pawn = Target:FindRelatedPlayerPawn()
            if not pawn or not UE4.UKismetSystemLibrary.IsValid(pawn) then
                return
            else
                ignoreActors:Add(pawn)
            end
        end
        local ScratchPad = self:GetScratchPad(Context)
        if not ScratchPad then
            return
        end
        -- self:StopAllHitEffect(ScratchPad, true)
        local Owner = Context:GetOwner()
        local Transform = ScratchPad.SpawnTransform and ScratchPad.SpawnTransform or UE4.USPAbilityFunctionLibrary.GetAbilityTargetTypeLocationTransform(Context, self.Location)
        local TraceStart = Transform.Translation
        local Orientation = Transform.Rotation:ToRotator()

        local overrideRange = {
            HalfExtents = UE4.FVector(0, 0, 0)
        }
        overrideRange.HalfExtents.X = self.Distance and self.Distance/2 or self.ShapeRange.HalfExtents.X/2
        overrideRange.HalfExtents.Y = self.ShapeRange.HalfExtents.Y
        overrideRange.HalfExtents.Z = self.ShapeRange.HalfExtents.Z

        local ObjectTypes = _SP.SPAbilityUtils.GetObjectTypesPresent(Context, self.PenetrationEffectChannel.Present, self.PenetrationEffectChannel.Channels)
        -- local HitResults = SPAbilityUtils.BoxSweepTrace(
        --     Context,
        --     Owner,
        --     ObjectTypes,
        --     TraceStart,
        --     Orientation,
        --     overrideRange,
        --     true,
        --     true,
        --     nil,
        --     ignoreActors,
        --     true
        -- )

        local HitResults = UE4.TArray(UE4.FHitResult)
        UE4.USPAbilityFunctionLibrary.DoCollisionDetect(Context, Owner, HitResults, SPAbilityUtils.EAbilityCollisionShape.Box, ObjectTypes, TraceStart, Orientation, true, overrideRange.HalfExtents, self.ShapeRange.Radius, self.ShapeRange.ConeRadius,
        self.ShapeRange.ConeLength, self.ShapeRange.HalfHeight, self.ShapeRange.CylinderAngle, self.ShapeRange.CylinderInnerRadius, self.ShapeRange.CylinderOuterRadius, self.ShapeRange.CylinderHeight, _SP.IsDSorStandalone and _SP.DS._bShowDebugCollision, false, true, true)

        Log("PlayPenetrationEffect HitResult Length = ", HitResults:Length())
        local hitResult, targetHitResult, dis
        -- for i = 1, HitResults:Length() do
        --     hitResult = HitResults:Get(i)
        --     -- -- @comment 采用局部Vector变量节省构建开销
        --     -- TempVector.X = -hitResult.ImpactNormal.X
        --     -- TempVector.Y = -hitResult.ImpactNormal.Y
        --     -- TempVector.Z = -hitResult.ImpactNormal.Z
        --     -- UE4.UGameplayStatics.SpawnEmitterAtLocation(_SP.GetCurrentWorld(), self.HitEffectTemplate, hitResult.ImpactPoint, TempVector:ToRotator())
        --     -- if hitResult.Actor.CanBeDamaged then
        --     --     if hitResult.Actor:CanBeDamaged(Owner) then
        --     --         self:PlayHitEffect(ScratchPad, hitResult.ImpactPoint, hitResult.ImpactNormal)
        --     --     end
        --     -- else
        --     --     self:PlayHitEffect(ScratchPad, hitResult.ImpactPoint, hitResult.ImpactNormal)
        --     -- end
        --     -- UE4.UKismetSystemLibrary.DrawDebugSphere(self, hitResult.ImpactPoint, 100, 12, UE4.FLinearColor(1, 0.3, 0, 1), 2, 5)
        --     --找到最近的点
        --     dis = DistanceSquare(TraceStart, hitResult.ImpactPoint)
        --     if not targetHitResult then
        --         targetHitResult = hitResult
        --     elseif dis < DistanceSquare(TraceStart, targetHitResult.ImpactPoint) then
        --         targetHitResult = hitResult
        --     end
        -- end
        local resultTable = HitResults:ToTable()
        table.sort(resultTable, function(a, b)
            return DistanceSquare(TraceStart, a.ImpactPoint) < DistanceSquare(TraceStart, b.ImpactPoint)
        end)

        self:StopAllHitEffect(ScratchPad, true)
        -- if targetHitResult then
        --     self:PlayHitEffect(ScratchPad, targetHitResult.ImpactPoint, targetHitResult.ImpactNormal)
        --     -- UE4.UKismetSystemLibrary.DrawDebugSphere(self, hitResult.ImpactPoint, 100, 12, UE4.FLinearColor(1, 0.3, 0, 1), 2, 5)
        -- -- else
        -- --     self:StopHitEffect(ScratchPad, true)
        -- end
        local cout = math.min(MAX_HITEFFECT_COUNT, #resultTable)
        for i = 1, cout do
            hitResult = resultTable[i]
            self:PlayHitEffect(ScratchPad, hitResult.ImpactPoint, hitResult.ImpactNormal, Target)
        end
    end
end

function Ability_Task_LaserWeapon:GetCustomEffectTemplate(Target, UseCustomEffect, EffectId, EffectTemplate)
    if UseCustomEffect then
        local CustomEffectTemplate = SPBattleTalentUtil.GetCustomEffectTemplateByElementType(Target, EffectId)
        if CustomEffectTemplate then
            return CustomEffectTemplate
        end
    end
    return EffectTemplate
end

function Ability_Task_LaserWeapon:SpawnOrModifyPSC(Context, ScratchPad)
    local Target = Context:GetOwner()
    local SpawnTransform = ScratchPad.SpawnTransform
    if not SpawnTransform then
        return
    end

    if ScratchPad.SpawnedEffect then
        ScratchPad.SpawnedEffect:K2_SetWorldTransform(SpawnTransform)
    else
        -- spawn new PSC
        local EffectTemplate = self.EffectTemplate
        if Target:IsA(UE4.AMoeGameCharacter) then
            EffectTemplate = self:GetCustomEffectTemplate(Target, self.UseCustomEffect, self.EffectId, EffectTemplate)
        end

        local SpawnedEffect
        local Location = self.Location
        if EffectTemplate then
            if self.AttachToSocket then
                local AttachComponent = Target:GetComponentByClass(UE4.USceneComponent:StaticClass())
                SpawnedEffect = UE4.UGameplayStatics.SpawnEmitterAttached(EffectTemplate, AttachComponent,
                Location.m_Socket, SpawnTransform.Translation,
                SpawnTransform.Rotation:ToRotator(),
                SpawnTransform.Scale3D,
                UE4.EAttachLocation.KeepWorldPosition)
            else
                SpawnedEffect = UE4.UGameplayStatics.SpawnEmitterAtLocation(
                Target:GetWorld(), EffectTemplate, SpawnTransform)
            end
        end
        if SpawnedEffect then
            SpawnedEffect.TranslucencySortPriority = self.RenderSortPriority
            ScratchPad.SpawnedEffect = SpawnedEffect
        end
    end
end

function Ability_Task_LaserWeapon:PlayParticleEffect(Context)
    if not self.EffectTemplate then
        return
    end

    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    ScratchPad.SpawnedEffect = nil

    self:SpawnOrModifyPSC(Context, ScratchPad)
end

function Ability_Task_LaserWeapon:StopParticleEffect(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    if ScratchPad.SpawnedEffect then
        if self.DetachOnStop then
            ScratchPad.SpawnedEffect:K2_DetachFromComponent(UE4.EAttachmentRule.KeepWorld, UE4.EAttachmentRule.KeepWorld, UE4.EAttachmentRule.KeepWorld, true)
        end
        UE4.USPGameLibrary.DeactivateParticleSystem(ScratchPad.SpawnedEffect)
        ScratchPad.SpawnedEffect = nil
    end
end

function Ability_Task_LaserWeapon:RefreshParticle(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    if ScratchPad.SpawnedEffect then
        self:SpawnOrModifyPSC(Context, ScratchPad)
    end
end

-- function Ability_Task_LaserWeapon:PlayHitEffect(ScratchPad, HitPos, HitImpactNormal)
--     if self.HitEffectTemplate then
--         -- @comment 采用局部Vector变量节省构建开销
--         TempVector.X = -HitImpactNormal.X
--         TempVector.Y = -HitImpactNormal.Y
--         TempVector.Z = -HitImpactNormal.Z
--         if not ScratchPad.SpawnedHitEffect or not _SP.IsValid(ScratchPad.SpawnedHitEffect) then
--             ScratchPad.SpawnedHitEffect = UE4.UGameplayStatics.SpawnEmitterAtLocation(_SP.GetCurrentWorld(), self.HitEffectTemplate, HitPos, TempVector:ToRotator())
--         else
--             ScratchPad.SpawnedHitEffect:K2_SetWorldLocation(HitPos)
--             ScratchPad.SpawnedHitEffect:K2_SetWorldRotation(TempVector:ToRotator())
--             ScratchPad.SpawnedHitEffect:SetVisibility(true)
--             ScratchPad.SpawnedHitEffect:Activate(true)
--         end
--     end
-- end

-- function Ability_Task_LaserWeapon:StopHitEffect(ScratchPad, bHide)
--     if ScratchPad.SpawnedHitEffect and _SP.IsValid(ScratchPad.SpawnedHitEffect) then
--         if bHide then
--             ScratchPad.SpawnedHitEffect:SetVisibility(false)
--         else
--             UE4.USPGameLibrary.DeactivateParticleSystem(ScratchPad.SpawnedHitEffect)
--             ScratchPad.SpawnedHitEffect = nil
--         end
--     end
-- end

function Ability_Task_LaserWeapon:StopAllHitEffect(ScratchPad, bHide)
    for _, psc in pairs(ScratchPad.HitEffectCache) do
        if _SP.IsValid(psc) then
            if bHide then
                psc:SetVisibility(false)
            else
                UE4.USPGameLibrary.DeactivateParticleSystem(psc)
                psc:Destroy()
            end
        end
    end
    if not bHide then
        ScratchPad.HitEffectCache = {}
    end
    ScratchPad.ActivateEffectIndex = 1
end

function Ability_Task_LaserWeapon:GetHitEffectFromCache(ScratchPad, Pos, Rotation, Target)
    local psc = ScratchPad.HitEffectCache[ScratchPad.ActivateEffectIndex]
    if psc and _SP.IsValid(psc) then
        psc:SetVisibility(true)
        ScratchPad.ActivateEffectIndex = ScratchPad.ActivateEffectIndex + 1
        return psc
    else
        -- craete new PSC
        local HitEffectTemplate = self.HitEffectTemplate
        HitEffectTemplate = self:GetCustomEffectTemplate(Target, self.UseHitCustomEffect, self.HitEffectId, HitEffectTemplate)
        ScratchPad.HitEffectCache[ScratchPad.ActivateEffectIndex] = UE4.UGameplayStatics.SpawnEmitterAtLocation(_SP.GetCurrentWorld(), HitEffectTemplate, Pos, Rotation)
        ScratchPad.ActivateEffectIndex = ScratchPad.ActivateEffectIndex + 1
        return ScratchPad.HitEffectCache[ScratchPad.ActivateEffectIndex]
    end
end

function Ability_Task_LaserWeapon:PlayHitEffect(ScratchPad, Pos, Rotation, Target)
    -- @comment 采用局部Vector变量节省构建开销
    TempVector.X = -Rotation.X
    TempVector.Y = -Rotation.Y
    TempVector.Z = -Rotation.Z
    local psc = self:GetHitEffectFromCache(ScratchPad, Pos, TempVector:ToRotator(), Target)
    if psc and _SP.IsValid(psc) then
        psc:K2_SetWorldLocation(Pos)
        psc:K2_SetWorldRotation(Rotation:ToRotator())
        psc:Activate(true)
    end
end

function Ability_Task_LaserWeapon:PlayTrailingEffect(ScratchPad, StarPos, EndPos, Target)
    if self.TraillingEffectTemplate then
        local dirToImpactPoint = UE4.UKismetMathLibrary.Normal(EndPos - StarPos)
        local rotation = UE4.UKismetMathLibrary.Conv_VectorToRotator(dirToImpactPoint)
        if not ScratchPad.TraillingEffect or not _SP.IsValid(ScratchPad.TraillingEffect) then
            local TraillingEffectTemplate = self.TraillingEffectTemplate
            TraillingEffectTemplate = self:GetCustomEffectTemplate(Target, self.UseTraillingCustomEffect, self.TraillingEffectId, TraillingEffectTemplate)
            ScratchPad.TraillingEffect = UE4.UGameplayStatics.SpawnEmitterAtLocation(_SP.GetCurrentWorld(), TraillingEffectTemplate, EndPos, rotation)
        else
            ScratchPad.TraillingEffect:K2_SetWorldLocation(EndPos)
            ScratchPad.TraillingEffect:K2_SetWorldRotation(rotation)
            ScratchPad.TraillingEffect:SetVisibility(true)
        end
    end
end

function Ability_Task_LaserWeapon:StopTrailingEffect(ScratchPad, bHide)
    if ScratchPad.TraillingEffect then
        if bHide then
            ScratchPad.TraillingEffect:SetVisibility(false)
        else
            UE4.USPGameLibrary.DeactivateParticleSystem(ScratchPad.TraillingEffect)
            ScratchPad.TraillingEffect = nil
        end
    end
end

------------------------------
---Only Effect In Client End
------------------------------

function Ability_Task_LaserWeapon:ResetScratchPadBP(ScratchPad)
    if ScratchPad then
        ScratchPad.SpawnedEffect = nil
        ScratchPad.QueryResults = nil
        ScratchPad.IntervalTimer = 0
        ScratchPad.AddBuffActors = nil
        ScratchPad.SpawnedHitEffect = nil
        ScratchPad.TraillingEffect = nil
        ScratchPad.StartTime = 0
        ScratchPad.LastFrameScale = 0
        ScratchPad.bInGrow = true
        ScratchPad.HitEffectCache = {}
        ScratchPad.ActivateEffectIndex = 1
        if ScratchPad.SpawnTransform and ScratchPad.SpawnTransform.Scale3D and self.IsNeedThick then
            ScratchPad.SpawnTransform.Scale3D.Y = 1
            ScratchPad.SpawnTransform.Scale3D.Z = 1
        end
    end
end

function Ability_Task_LaserWeapon:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.LaserEye)
end

---@return direction: Forward Right Up
function Ability_Task_LaserWeapon:GetOwnerRotationDirection(Context, TargetActor)
    local OwnerRotation = TargetActor:K2_GetActorRotation()

    local OwnerMeshComp = TargetActor:GetComponentByClass(UE4.USkeletalMeshComponent.StaticClass())
    if OwnerMeshComp then
        OwnerRotation = OwnerMeshComp:K2_GetComponentRotation()
        return OwnerRotation:GetRightVector(), -OwnerRotation:GetForwardVector(), OwnerRotation:GetUpVector()
    end

    return OwnerRotation:GetForwardVector(), OwnerRotation:GetRightVector(), OwnerRotation:GetUpVector()
end

return Ability_Task_LaserWeapon