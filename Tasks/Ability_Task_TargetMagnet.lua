local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local CurveType = {
    Speed = 0,
    Acceleration = 1,
    Alpha = 2,
}
local MinDistanceToTarget = 1

---@class Ability_Task_TargetMagnet    目标吸附
local Ability_Task_TargetMagnet = UE4.Class(nil, "Ability_Task_TargetMagnet")
local SPMonsterChannel = UE4.USPGameLibrary.GetCollisionChannelBySPTraceType(UE4.ESPTraceType.WildMonster)

---@param Context UAbleAbilityContext
function Ability_Task_TargetMagnet:OnTaskStartBP(Context)
    local TargetArray = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, TargetArray)

    for i = 1, TargetArray:Length() do
        local Target = TargetArray:Get(i)
        local Character = Target:Cast(UE4.ASPGameMonsterBase)
        if not Character then
            Character = Target:Cast(UE4.ASPGameCharacterBase)
        end

        if Character then
            self:ApproachTarget(Context, Character)
        end
    end
end

---@param Context UAbleAbilityContext
---@param Result EAbleAbilityTaskResult
function Ability_Task_TargetMagnet:OnTaskEndBP(Context, Result)
    --在task结束时需要将碰撞修改回去
    local TargetArray = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, TargetArray)
    for i = 1, TargetArray:Length() do
        local Target = TargetArray:Get(i)
        local Character = Target:Cast(UE4.ASPGameMonsterBase)
        if not Character then
            Character = Target:Cast(UE4.ASPGameCharacterBase)
        end

        if _SP.IsValid(Character) then
            if self.SpeedCoefficientChanged and self.OriginalCoefficient and self.OriginalCoefficient > 0 then
                if Character.ChangeWalkingSpeedCoefficient then
                    Character:ChangeWalkingSpeedCoefficient(self.OriginalCoefficient)
                end
            end
            if self.CanPassThroughTarget then
                if Character.CapsuleComponent then
                    Character.CapsuleComponent:SetCollisionResponseToChannel(SPMonsterChannel, self.CurrentCollisionResponse)
                end
            end
        end
    end

    if not Result or Result == UE4.EAbleAbilityTaskResult.Successful then
        return
    end

    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad or not ScratchPad.Handle then
        return
    end

    ScratchPad.Handle:Abort()
    ScratchPad.Handle = nil
end

---@param Context UAbleAbilityContext
---@param Character AMoeGameCharacter
function Ability_Task_TargetMagnet:ApproachTarget(Context, Character)
    if not Context or not Character then
        return
    end

    local TargetActorIndex = Context:GetIntParameter("MagnetTargetIndex")
    local Targets = Context:GetTargetActors()
    local TargetNum = Targets:Length()
    local TargetActor = TargetActorIndex > 0 and TargetActorIndex <= TargetNum and Targets:Get(TargetActorIndex)
    if not TargetActor then
        _SP.LogWarning("SPAbility", "Unable to acquire the target actor to approach! MagnetTargetIndex: " .. TargetActorIndex .. " TargetArrayLength: " .. TargetNum)
        return nil
    end

    local FollowTarget = self.FollowTarget
    local OffsetPitch = self.RotateOffset.Pitch
    local OffsetYaw = self.RotateOffset.Yaw
    local OffsetRoll = self.RotateOffset.Roll
    local IgnorePitch = self.IgnoreRotationPitch
    local IgnoreYaw = self.IgnoreRotationYaw
    local IgnoreRoll = self.IgnoreRotationRoll
    self.TargetLocationOffsetZ = 0.0
    local Time = self:GetDuration()
    --该参数表示距离目标点多少距离停下，应是一个较小的误差值，1~5即可
    local ToleranceDistance = MinDistanceToTarget
    local MovementTargetLocation
    local MaxMovementDistance = self.MaxMovementDistance
    local bUseVertical = self:CheckUseVertical(Character, TargetActor)
    _SP.Log("SPAbility", "Ability_Task_TargetMagnet", "bUseVertical ", bUseVertical)

    MovementTargetLocation, MaxMovementDistance = self:FindMovementTargetLocation(Context, TargetActor, Character)
    if not MovementTargetLocation then
        _SP.Log("SPAbility", "Ability_Task_TargetMagnet:ApproachTarget No moving target location could be found")
        return
    end

    if not self:IsInputValid(Character, MovementTargetLocation) then
        return
    end
    --新增DebugDraw配置
    if self.DrawTargetLocationSphere then
        UE4.UKismetSystemLibrary.DrawDebugSphere(_SP.GetCurrentWorld(), MovementTargetLocation ,50, 12, self.TargetLocationSphereColor, 10)
    end
    local Params = UE4.USPCharacterMovementLibrary.GetSpeedCurveParameter(self.HMovementCurve, self.HMovementValue, nil, 0, Time)
    --替換爲新的参数接口，用来适配新的曲线移动模式
    local Config = UE4.USPCharacterMovementLibrary.GetMovementAdsorptiveConfig(bUseVertical, OffsetPitch, OffsetYaw, OffsetRoll, IgnorePitch, IgnoreYaw, IgnoreRoll, ToleranceDistance, Time, self.VelocityMode, MaxMovementDistance)
    Config.bFollowTarget = FollowTarget or false
    Config.FollowRange = self.FollowRange or 10
    Config.TargetDistance = self.TargetDistance
    Config.bCanPassThroughTarget = self.CanPassThroughTarget
    Config.MaxPassThroughTargetCapsuleRadius = self.MaxPassThroughTargetCapsuleRadius

    local Handle
    --单点追踪目前支持三种曲线模式，根据曲线模式配置来选择
    if self.CurveUsage == CurveType.Speed then
        if Character.ChangeWalkingSpeedCoefficient then
            --修改速度系数
            self.OriginalCoefficient = Character.WalkingSpeedCoefficient or 1
            local finalSpeedCoefficient= 1 + self.SpeedScale
            if finalSpeedCoefficient ~= self.OriginalCoefficient then
                self.SpeedCoefficientChanged = true
                Character:ChangeWalkingSpeedCoefficient(finalSpeedCoefficient)
            end
        end
        if FollowTarget then
            Handle = Character:MoveAdsorptiveTargetActorByVelocityCurve(TargetActor, MovementTargetLocation, self.TargetLocationOffsetZ, Params, Config)                    --速度模式（持续追踪）
        else
            Handle = Character:MoveAdsorptiveTargetLocationByVelocityCurve(MovementTargetLocation, Params, Config)      --速度模式
        end
    elseif self.CurveUsage == CurveType.Acceleration then
        if FollowTarget then
            Handle = Character:MoveAdsorptiveTargetActorByAccelerationCurve(TargetActor, MovementTargetLocation, self.TargetLocationOffsetZ, Params, Config)                --加速度模式（持续追踪）
        else
            Handle = Character:MoveAdsorptiveTargetLocationByAccelerationCurve(MovementTargetLocation, Params, Config)  --加速度模式
        end
    elseif self.CurveUsage == CurveType.Alpha then
        if FollowTarget then
            Handle = Character:MoveAdsorptiveTargetActorByDistanceRateCurve(TargetActor, MovementTargetLocation, self.TargetLocationOffsetZ, Params, Config)                --路程进度模式（持续追踪）
        else
            Handle = Character:MoveAdsorptiveTargetLocationByDistanceRateCurve(MovementTargetLocation, Params, Config)  --路程进度模式
        end
    else
        _SP.Log("SPAbility", "Ability_Task_TargetMagnet", "ApproachTarget 无法到达的错误代码" )
    end

    _SP.Log("SPAbility", "Start approach target task for character " .. Character:GetUID())

    local ScratchPad = self:GetScratchPad(Context)
    if ScratchPad then
        ScratchPad.Handle = Handle
    end
end

function Ability_Task_TargetMagnet:FindMovementTargetLocation(Context, TargetActor, Character)
    if not Context or not Character then
        return nil
    end

    if self.EnableCollisionCheckXY and self:CheckCollision(Character, TargetActor) then
        return
    end

    if not TargetActor.SPGetCapsuleRadius then
        return
    end

    local MaxMovementDistance = self.MaxMovementDistance
    local TargetCapsuleRadius = TargetActor:SPGetCapsuleRadius()
    local TargetLocation = TargetActor:SP_GetActorLocation()
    --吸附后与目标的预期距离，玩家中心点应该移动到距离目标ExpectDistance的距离上
    local ExpectDistance = self.TargetDistance + TargetCapsuleRadius
    --玩家与目标中心连线的方向向量，指向目标
    local TargetVec = TargetLocation - Character:SP_GetActorLocation()

    --如果自己比对方矮且对方不是地面单位就打同等身高下的对应坐标
    local IsCapsuleSmaller = self:IsCapsuleSmallerThanTarget(Character, TargetActor)
    local CharFloorLocation,TargetFloorLocation = self:GetFloorLocation(Character, TargetActor)
    if IsCapsuleSmaller and TargetFloorLocation then
        if CharFloorLocation and TargetFloorLocation then
            TargetVec = TargetFloorLocation - CharFloorLocation
            --UE4.UKismetSystemLibrary.DrawDebugSphere(_SP.GetCurrentWorld(), TargetFloorLocation ,50, 12, UE4.FLinearColor(0, 1, 0, 1), 10)
            --UE4.UKismetSystemLibrary.DrawDebugSphere(_SP.GetCurrentWorld(), CharFloorLocation ,50, 12, UE4.FLinearColor(1, 0, 0, 1), 10)
        end
    end
    local TargeInPassThroughVec = UE4.FVector(TargetVec.X,TargetVec.Y,TargetVec.Z)
    local TargetVecLength = TargetVec:Size()
    --如果未勾选穿过目标，且玩家和目标的距离比吸附后与目标的预期距离还要小，则不产生吸附
    if not self.CanPassThroughTarget and TargetVecLength < ExpectDistance then
        return
    end

    TargetVec:Normalize()
    --将MovementTargetLocation直接设为目标中心，在目标与玩家连线方向上，前面ExpectDistance距离的那个点上
    local MovementTargetLocation = TargetLocation - TargetVec * ExpectDistance

    -- 如果目标比角色高，就打目标脚底+玩家半身身高的位置
    if IsCapsuleSmaller and TargetFloorLocation then
        MovementTargetLocation.Z = TargetFloorLocation.Z + Character:SPGetCapsuleHalfHeight()
        --UE4.UKismetSystemLibrary.DrawDebugSphere(_SP.GetCurrentWorld(), MovementTargetLocation ,50, 12, UE4.FLinearColor(0, 0, 1, 1), 10)
    end
    if self.CanPassThroughTarget and TargetCapsuleRadius <= self.MaxPassThroughTargetCapsuleRadius then
        --如果可以穿过目标，则需要临时关闭碰撞
        if Character.CapsuleComponent then
            --记录当前对MonsterChannel的response
            self.CurrentCollisionResponse = Character.CapsuleComponent:GetCollisionResponseToChannel(SPMonsterChannel)
            Character.CapsuleComponent:SetCollisionResponseToChannel(SPMonsterChannel, UE4.ECollisionResponse.ECR_Ignore)
        end
        TargeInPassThroughVec.Z = 0
        local TargetVecInPassThroughLength = TargeInPassThroughVec:Size()
        if MaxMovementDistance < TargetVecInPassThroughLength then
            --如果目标点距离大于self.MaxMovementDistance 则使用最新的distance
            MaxMovementDistance = TargetVecInPassThroughLength
        end
        TargeInPassThroughVec:Normalize()
        local NewTargetLocation = TargetLocation  + TargeInPassThroughVec * ExpectDistance
        --选取目标点后重新从上往下打射线，选取贴近地面
        MovementTargetLocation = self:FindTargetFloorLocation(NewTargetLocation, Character)
    end

    MovementTargetLocation.Z = MovementTargetLocation.Z + self.TargetLocationOffsetZ

    return MovementTargetLocation, MaxMovementDistance
end

---@return boolean
function Ability_Task_TargetMagnet:IsSingleFrameBP()
    return false
end

---@param ScratchPad USPAbilityTaskScratchPad
function Ability_Task_TargetMagnet:ResetScratchPadBP(ScratchPad)
    if ScratchPad then
        ScratchPad.Handle = nil
    end
end

---@param Context UAbleAbilityContext
function Ability_Task_TargetMagnet:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.TargetMagnet)
end

function Ability_Task_TargetMagnet:FindTargetFloorLocation(TargetLocation, Character)
    local StartLocation = TargetLocation + UE4.FVector(0, 0, 200)
    local EndLocation = StartLocation + UE4.FVector(0, 0, -1000)
    local DebugTrace = UE4.EDrawDebugTrace.None
    local HitResult, bHit = UE4.UKismetSystemLibrary.LineTraceSingle(Character, StartLocation, EndLocation, UE4.ECollisionChannel.ECC_WorldStatic, false, nil, DebugTrace, nil, true,UE4.FLinearColor(1,1,1,1), UE4.FLinearColor(1,0,0,1),3)
    if bHit then
        return HitResult.ImpactPoint
    end
    return TargetLocation
end

function Ability_Task_TargetMagnet:CheckUseVertical(Char, Target)
    if not _SP.IsValid(Char) or not _SP.IsValid(Target) then
        return false
    end
    if self.CanWarpZ then
        local CharRootPosition, TargetRootPosition = self:GetRootPosition(Char, Target)
        if CharRootPosition and TargetRootPosition then
            if self.EnableCollisionCheckZ and self:CheckCollision(Char, Target) then
                return false
            end
            local CharToTargetZDiff = TargetRootPosition.Z - CharRootPosition.Z
            if CharToTargetZDiff <= self.MaxUpDistanceToWarpZ and CharToTargetZDiff >= self.MaxDownDistanceToWarpZ then
                CharToTargetZDiff = math.abs(math.ceil(CharToTargetZDiff)) > 1.0 and CharToTargetZDiff or -1.0
                self.TargetLocationOffsetZ = self.OffsetToWarpZ * UE4.UKismetMathLibrary.SignOfFloat(-CharToTargetZDiff)
                return true
            end
        end
    end
    return false
end

function Ability_Task_TargetMagnet:CheckCollision(Char, Target)
    if not _SP.IsValid(Char) or not _SP.IsValid(Target) then
        return false
    end
    local QueryObjectsArray = UE4.TArray(UE4.EObjectTypeQuery)
    QueryObjectsArray:Add(UE4.UMoeBlueprintLibrary.ConvertToObjectType(UE4.ECollisionChannel.ECC_WorldStatic))
    local IgnoreActors = UE4.TArray(UE4.AActor)
    local DrawDebugType = UE4.EDrawDebugTrace.None
    if UE4.USPAbilityFunctionLibrary.IsVerboseDebug() then
        DrawDebugType = UE4.EDrawDebugTrace.ForDuration
    end
    -- generate a capsule collision check if we can translate to the target
    local CheckHitResult, bHasHit = UE4.UKismetSystemLibrary.LineTraceSingle(
        Char, --[[WorldContextObject]]
        Char:SP_GetActorLocation(), --[[Start]]
        Target:SP_GetActorLocation(), --[[End]]
        UE4.ECollisionChannel.ECC_WorldStatic, --[[TraceChannel]]
        false, --[[bTraceComplex]]
        IgnoreActors, --[[ActorsToIgnore]]
        DrawDebugType, --[[DrawDebugType]]
        nil, --[[OutHits]]
        true, --[[bIgnoreSelf]]
        UE4.FLinearColor.Red,
        UE4.FLinearColor.Green,
        10 --[[DrawTime]]
    )
    return bHasHit
end

function Ability_Task_TargetMagnet:GetRootPosition(Char, Target)
    if not _SP.IsValid(Char) or not _SP.IsValid(Target) then
        return nil, nil
    end
    local CharCapsuleComponent = Char.CapsuleComponent
    local TargetCapsuleComponent = Target.CapsuleComponent
    if _SP.IsValid(CharCapsuleComponent) and _SP.IsValid(TargetCapsuleComponent) then
        local CharHalfHeight = CharCapsuleComponent:GetScaledCapsuleHalfHeight()
        local TargetHalfHeight = TargetCapsuleComponent:GetScaledCapsuleHalfHeight()
        local CharCapsuleLocation = CharCapsuleComponent:K2_GetComponentLocation()
        local TargetCapsuleLocation = TargetCapsuleComponent:K2_GetComponentLocation()
        local CharRootPosition = CharCapsuleLocation - UE4.FVector(0, 0, CharHalfHeight)
        local TargetRootPosition = TargetCapsuleLocation - UE4.FVector(0, 0, TargetHalfHeight)
        return CharRootPosition, TargetRootPosition
    end
    return nil, nil
end

function Ability_Task_TargetMagnet:GetFloorLocation(Char, Target)
    if not _SP.IsValid(Char) or not _SP.IsValid(Target) then
        return nil, nil
    end
    local CharMoveCom = Char:GetComponentByClass(UE4.UCharacterMovementComponent.StaticClass())
    local TargetMoveCom = Target:GetComponentByClass(UE4.UCharacterMovementComponent.StaticClass())
    if not _SP.IsValid(CharMoveCom) or not _SP.IsValid(TargetMoveCom) then
        return nil, nil
    end
    local CharFloorLocation
    local TargetFloorLocation
    if CharMoveCom.K2_FindFloor ~= nil and TargetMoveCom.K2_FindFloor ~= nil then
        --后续moveCom父类可能要更换，新的父类需要liangyaohua实现对应接口才能使用
        local CharCapsuleLocation = Char.CapsuleComponent:K2_GetComponentLocation()
        local CharFindFloorResult = UE4.FFindFloorResult()
        CharMoveCom:K2_FindFloor(CharCapsuleLocation, CharFindFloorResult)
        if CharFindFloorResult.bBlockingHit and CharFindFloorResult.bWalkableFloor then
            CharFloorLocation = CharFindFloorResult.HitResult.ImpactPoint
            local TargetFindFloorResult = UE4.FFindFloorResult()
            local TargetCapsuleLocation = Target.CapsuleComponent:K2_GetComponentLocation()
            TargetMoveCom:K2_FindFloor(TargetCapsuleLocation, TargetFindFloorResult)
            if CharFloorLocation and TargetFindFloorResult.bBlockingHit and TargetFindFloorResult.bWalkableFloor then
                TargetFloorLocation = TargetFindFloorResult.HitResult.ImpactPoint
                if TargetFloorLocation then
                    return CharFloorLocation,TargetFloorLocation
                end
            end
        end
    end
    return nil, nil
end

function Ability_Task_TargetMagnet:IsCapsuleSmallerThanTarget(Character, TargetActor)
    local TargetCapsuleHalfHeight = TargetActor:SPGetCapsuleHalfHeight()
    local CharacterCapsuleHalfHeight = Character:SPGetCapsuleHalfHeight()
    if (TargetCapsuleHalfHeight > CharacterCapsuleHalfHeight) then
        return true
    end
    return false
end

function Ability_Task_TargetMagnet:IsInputValid(Character, MovementTargetLocation)
    local CharInputComponent = Character:GetCharInputComponent():Cast(UE4.USPGameCharInputComponent)
    if CharInputComponent then
        local InputVector = CharInputComponent:GetCurrentJoystickInput()
        if self.DrawInputVectorLine then
            UE4.UKismetSystemLibrary.DrawDebugLine(_SP.GetCurrentWorld(), Character:K2_GetActorLocation(), Character:K2_GetActorLocation() + InputVector * 100 ,self.InputVectorLineColor, 10, 5)
        end
        local InputMagnitude = InputVector:Size2D()
        if InputMagnitude > 0 then
            if InputMagnitude <= self.JoystickDeadzoneMagnitude then
                return false
            end
            local DirToTarget = UE4.UMoeGameLibrary.GetSafeNormal2D(MovementTargetLocation - Character:SP_GetActorLocation())
            if self.DrawTargetDirectionLine then
                UE4.UKismetSystemLibrary.DrawDebugLine(_SP.GetCurrentWorld(), Character:K2_GetActorLocation(), Character:K2_GetActorLocation() + DirToTarget * 100 ,self.TargetDirectionLineColor, 10, 5)
            end
            --local CharForwardVec = UE4.UKismetMathLibrary.GetForwardVector(Character:GetControlRotation())
            local AngleToTarget = UE4.USPGameLibrary.CalculateHorizontalAngleBetweenVectors(DirToTarget, InputVector)
            AngleToTarget = math.abs(AngleToTarget)
            if AngleToTarget > self.JoystickInvalidAngle then
                return false
            end
            -- 这里直接用输入向量转角度判断实际上与角色朝向无关，相当于限制了输入角度，先注释掉
            -- local MoveInput = UE4.UMoeGameLibrary.GetSafeNormal2D(InputVector)
            -- local InputAngle = UE4.UKismetMathLibrary.RadiansToDegrees(UE4.UKismetMathLibrary.Atan2(MoveInput.Y, MoveInput.X))
            -- InputAngle = math.abs(InputAngle)
            -- if InputAngle > self.JoystickInvalidAngle then
            --     return false
            -- end
        end
    end
    return true
end

return Ability_Task_TargetMagnet