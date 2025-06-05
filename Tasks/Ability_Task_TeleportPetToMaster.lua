local Ability_Task_TeleportPetToMaster = UE4.Class(nil, "Ability_Task_TeleportPetToMaster")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")

function Ability_Task_TeleportPetToMaster:OnTaskStartBP(Context)
    local OwnerActor = Context:GetOwner()
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    if not _SP.IsValid(OwnerActor) then
        return
    end
    if OwnerActor:IsA(UE4.ASPGameMonsterBase) then
        --OnwerActor 是星兽
        ScratchPad.PetActor = OwnerActor
        ScratchPad.MasterActor = ScratchPad.PetActor:GetMasterInCombatTeam()
    else
        --OwnerActor 是玩家
        ScratchPad.MasterActor = OwnerActor
        ScratchPad.PetActor = ScratchPad.MasterActor:GetUnleashedPet()
    end
    if not _SP.IsValid(ScratchPad.PetActor) or not _SP.IsValid(ScratchPad.MasterActor) then
        return
    end
end

function Ability_Task_TeleportPetToMaster:GetTeleportPoint(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return nil
    end

    if not _SP.IsValid(ScratchPad.PetActor) or not _SP.IsValid(ScratchPad.MasterActor) then
        return nil
    end

    local MasterLocation = ScratchPad.MasterActor:K2_GetActorLocation()
    local TeleportVec = ScratchPad.MasterActor.Mesh:GetRightVector()
    if self.VecOffset:Size() > 0 then
        TeleportVec = self:GetTeleportVec(Context, MasterLocation)
    end
    local TargetTranceLocation = MasterLocation + TeleportVec * self.Distance

    local MonsterID = ScratchPad.PetActor.MonsterID
    local PlayerUID = nil
    if ScratchPad.MasterActor.GetPlayerInfo then
        PlayerUID = ScratchPad.MasterActor:GetPlayerInfo():GetPlayerUID()
    end

    local SPPalAIUtil = require("Feature.StarP.Script.System.AI.Util.SPPalAIUtil")
    local _, monsterRadius, _ = SPPalAIUtil.GetMonsterCapsualCfgRadiusAndHeight(MonsterID)
    local MaxRadiusCount = 3
    local MaxAdjustZ = 1000

    local adjPos, bFindAdj = _SP.SPGameplayUtils:GetLocationAdjustedByNavmesh(
        monsterRadius, TargetTranceLocation, MaxRadiusCount, MaxAdjustZ,
        UE4.ESPSpawnMonsterType.Invalid, PlayerUID, ScratchPad.PetActor,MonsterID)

    if bFindAdj then
        TargetTranceLocation = adjPos
        _SP.Log("SPAbility","Ability_Task_TeleportPetToMaster:GetTeleportPoint","Use Adjusted Location")
    else
        TargetTranceLocation = MasterLocation
        _SP.Log("SPAbility","Ability_Task_TeleportPetToMaster:GetTeleportPoint","Use Master Location")
    end

    local OwnerCapsuleComp = ScratchPad.PetActor.CapsuleComponent
    if OwnerCapsuleComp then
        TargetTranceLocation.Z = TargetTranceLocation.Z + OwnerCapsuleComp:GetScaledCapsuleHalfHeight()
    end

    return TargetTranceLocation
end

function Ability_Task_TeleportPetToMaster:GetTeleportVec(Context, PlayerLocation)
    --TODO 还未完成，后续优化
    local ScratchPad = self:GetScratchPad(Context)
    --local Transform = ScratchPad.MasterActor:GetTransform()
    local Transform = ScratchPad.MasterActor.Mesh:K2_GetComponentToWorld()
    -- local QuatRotation = UE4.FQuat(UE4.FRotator(0, 0, 90))
    -- local NewRotaion = Transform.Rotation * QuatRotation
    -- Transform.Rotation = NewRotaion
    --ocal Direction = Transform.TransformVectorNoScale(self.VecOffset:Normalize())
    local Direction = Transform.TransformVectorNoScale(Transform, self.VecOffset)
    Direction:Normalize()
    return Direction
end

---@param Context UAbleAbilityContext
---@param Result EAbleAbilityTaskResult
function Ability_Task_TeleportPetToMaster:OnTaskEndBP(Context, Result)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    if not _SP.IsValid(ScratchPad.PetActor) or not _SP.IsValid(ScratchPad.MasterActor) then
        return
    end

    --手动抬高一点，避免穿地
    local TargetPoint = self:GetTeleportPoint(Context)
    if TargetPoint ~= nil then
        ScratchPad.PetActor:K2_SetActorLocation(TargetPoint)
    end
    ScratchPad.PetActor = nil
    ScratchPad.MasterActor = nil
end

function Ability_Task_TeleportPetToMaster:IsPointInWall(Context,Point)
    --Tolerance 容差 判断是否在墙壁内
    local ScratchPad = self:GetScratchPad(Context)
    local Tolerance = 10
    local StartPoint = Point + UE4.FVector(0, 0, Tolerance)
    local EndPoint = Point - UE4.FVector(0, 0, Tolerance)
    local _, bHit = UE4.UKismetSystemLibrary.LineTraceSingle(ScratchPad.MasterActor, StartPoint, EndPoint, UE4.ECollisionChannel.ECC_WorldStatic, false, nil, UE4.EDrawDebugTrace.None, nil, true,UE4.FLinearColor(1,1,1,1), UE4.FLinearColor(1,0,0,1),3)
    return bHit
end

function Ability_Task_TeleportPetToMaster:ResetScratchPadBP(ScratchPad)
    if ScratchPad then
        ScratchPad.PetActor = nil
        ScratchPad.MasterActor = nil
    end
end

function Ability_Task_TeleportPetToMaster:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.TeleportPetToMaster)
end

return Ability_Task_TeleportPetToMaster