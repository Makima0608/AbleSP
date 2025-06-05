local Ability_Task_SwapPosition = UE4.Class(nil, "Ability_Task_SwapPosition")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPPetInteractionUtils = require("Feature.StarP.Script.System.Comp.PetInteraction.SPPetInteractionUtils")

local UPPER_OFFSET_FOR_WATER_DETECTION = 600.0

function Ability_Task_SwapPosition:OnTaskStartBP(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    ScratchPad.InstigatorActor = self:GetSingleActorFromTargetTypeBP(Context, self.TriggerActorType)
    ScratchPad.TargetActor = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_TargetActor)
    if not _SP.IsValid(ScratchPad.InstigatorActor) or not _SP.IsValid(ScratchPad.TargetActor) then
        return
    end
    -- 进行传送
    local InstigatorTPResult = false
    local TargetActorTPResult = false
    local InstigatorOriginalLocation = ScratchPad.InstigatorActor:K2_GetActorLocation():Copy()
    local TargetActorOriginalLocation = ScratchPad.TargetActor:K2_GetActorLocation():Copy()
    local InstigatorTPLocation = TargetActorOriginalLocation:Copy()
    local TargetActorTPLocation = InstigatorOriginalLocation:Copy()
    InstigatorTPLocation = self:AdjustTPLocationFor(self.TriggerActorTPBottomAlignment,ScratchPad.InstigatorActor,ScratchPad.TargetActor,InstigatorTPLocation)
    TargetActorTPLocation = self:AdjustTPLocationFor(self.TargetActorTPBottomAlignment,ScratchPad.TargetActor,ScratchPad.InstigatorActor,TargetActorTPLocation)

    InstigatorTPResult, InstigatorTPLocation = self:TPForActor_Unsafe(ScratchPad.InstigatorActor, InstigatorTPLocation,InstigatorOriginalLocation,ScratchPad.TargetActor)
    TargetActorTPResult,TargetActorTPLocation = self:TPForActor_Unsafe(ScratchPad.TargetActor,TargetActorTPLocation,TargetActorOriginalLocation,ScratchPad.InstigatorActor)

    self:ToggleServerAuthorityForPlayerMovement(ScratchPad.InstigatorActor,true)
    self:ToggleServerAuthorityForPlayerMovement(ScratchPad.TargetActor,true)

    local InstigatorUID = self:GetUID(ScratchPad.InstigatorActor)
    local TargetActorUID = self:GetUID(ScratchPad.TargetActor)
    SPPetInteractionUtils.LogInfo("[Ability_Task_SwapPosition] Instigator " .. tostring(ScratchPad.InstigatorActor:GetName()) .. " " .. tostring(InstigatorUID) ..
            ": TPResult = " .. tostring(InstigatorTPResult) .. ", PreLocation " .. tostring(InstigatorOriginalLocation) .. ", TPLocation " .. tostring(InstigatorTPLocation) .. "; " ..
            " TargetActor " .. tostring(ScratchPad.TargetActor:GetName()) .. " " .. tostring(TargetActorUID) ..
            ": TPResult = " .. tostring(TargetActorTPResult) .. ", PreLocation " .. tostring(TargetActorOriginalLocation) .. ", TPLocation " .. tostring(TargetActorTPLocation))
end

---@param Context UAbleAbilityContext
---@param Result EAbleAbilityTaskResult
function Ability_Task_SwapPosition:OnTaskEndBP(Context,Result)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    if not _SP.IsValid(ScratchPad.InstigatorActor) or not _SP.IsValid(ScratchPad.TargetActor) then
        return
    end

    self:ToggleServerAuthorityForPlayerMovement(ScratchPad.InstigatorActor,false)
    self:ToggleServerAuthorityForPlayerMovement(ScratchPad.TargetActor,false)
end

function Ability_Task_SwapPosition:ToggleServerAuthorityForPlayerMovement(Actor,bServerAuthority)
    if not _SP.IsValid(Actor) then
        return
    end
    -- 仅对玩家生效
    if not Actor:Cast(UE4.ASPGameCharacterBase) then
        return
    end
    local MoveComp = Actor.GetMovementComponent and Actor:GetMovementComponent() or nil
    if _SP.IsValid(MoveComp) then
        if MoveComp.SetAlwaysCorrectClient then
            MoveComp:SetAlwaysCorrectClient(bServerAuthority)
            SPPetInteractionUtils.LogInfo("[Ability_Task_SwapPosition] SetAlwaysCorrectClient " .. tostring(bServerAuthority) .. ", Actor:GetName=".. tostring(Actor:GetName()))
        end
    end
end

function Ability_Task_SwapPosition:GetUID(Actor)
    if Actor.SPGetUID then
        return Actor:SPGetUID()
    end
    return -1
end

---@return FVector ResultLocation
function Ability_Task_SwapPosition:AdjustTPLocationFor(bAdjust,ActorA,ActorB,RefLocation)
    if not bAdjust then
        return RefLocation:Copy()
    end

    local HalfHeightA = self:GetHalfHeight(ActorA)
    local HalfHeightB = self:GetHalfHeight(ActorB)

    -- TPLocationA = RefZ - HalfHeightB + HalfHeightA
    local RefZ = RefLocation.Z
    local ResultLocation = RefLocation:Copy()
    ResultLocation.Z = RefZ - HalfHeightB + HalfHeightA
    return ResultLocation
end

function Ability_Task_SwapPosition:GetHalfHeight(Actor)
    local DefaultHalfHeight = 50
    return Actor.SPGetCapsuleHalfHeight and Actor:SPGetCapsuleHalfHeight() or DefaultHalfHeight
end

---@return boolean,FVector IsSuccess,ResultLocation
---@param TPActor AActor 被TP的对象
---@param TargetLocation FVector 目标位置
---@param DefaultLocation FVector 传送失败默认位置
---@param TargetActor AActor 目标对象
function Ability_Task_SwapPosition:TPForActor_Unsafe(TPActor,TargetLocation,DefaultLocation,TargetActor)
    local Result = false
    local Destination = TargetLocation:Copy()
    if TPActor:IsA(UE4.ASPGameCharacterBase) then
        Result,Destination = self:GetSuitablePositionForPlayer(TPActor,TargetActor,Destination)
    elseif TPActor:IsA(UE4.ASPGameMonsterBase) then
        Result,Destination = self:GetSuitablePositionForMonster(TPActor,Destination)
    else
        Result,Destination = self:GetSuitablePositionForDefaultType(TPActor,Destination)
    end
    if not Result then
        Destination = DefaultLocation
    end
    Result = UE4.USPUtilityLibrary.ActorTeleportTo(TPActor, Destination,TPActor:K2_GetActorRotation(), false, false)
    if not Result then
        SPPetInteractionUtils.LogInfo("[Ability_Task_SwapPosition] fail pos, TPForActor_Unsafe - ActorTeleportTo, Actor:GetName=".. tostring(TPActor:GetName()) .. ", Destination=" .. tostring(Destination) .. ", TargetLocation=" .. tostring(TargetLocation))
    end
    return Result,Destination
end

---@return boolean,FVector 表明操作是否成功
function Ability_Task_SwapPosition:GetSuitablePositionForDefaultType(Actor,RefPosition)
    return true,RefPosition
end

---@return boolean,FVector 表明操作是否成功
function Ability_Task_SwapPosition:GetSuitablePositionForMonster(Monster,RefPosition)
    local ResultLocation = nil
    local GetLocationSucceed = false
    local MonsterRadius = Monster:SPGetCapsuleRadius()
    local MonsterHalfHeight = self:GetHalfHeight(Monster)
    ResultLocation,GetLocationSucceed = _SP.SPGameplayUtils:GetLocationAdjustedByNavmesh(MonsterRadius, RefPosition, self.MonsterMaxSearchCount, self.MonsterMaxToleranceZ, self.MonsterType, Monster:GetOwnerPlayerUID(), Monster)
    if GetLocationSucceed then
        ResultLocation = _SP.SPGameplayUtils:AdjustedActorPositionWitchWaterCheck(ResultLocation, Monster.MonsterID, MonsterRadius, MonsterHalfHeight, "Ability_Task_SwapPosition")
    else
        SPPetInteractionUtils.LogInfo("[Ability_Task_SwapPosition] fail pos, GetSuitablePositionForMonster - GetLocationAdjustedByNavmesh fail, Actor:GetName=".. tostring(Monster:GetName())
        .. ", RefPosition=" .. tostring(RefPosition) .. ", GetLocationSucceed=" .. tostring(GetLocationSucceed)
        .. ", MonsterRadius=" .. tostring(MonsterRadius) .. ", MonsterHalfHeight=" .. tostring(MonsterHalfHeight))
    end
    return GetLocationSucceed,ResultLocation
end

---@param Player AActor 被传送的对象
---@param TargetActor AActor 目标对象
---@param RefPosition FVector 参考坐标
---@return boolean,FVector 表明操作是否成功
---@comment 角色传送到合适点位, 需要判断水中
function Ability_Task_SwapPosition:GetSuitablePositionForPlayer(Player,TargetActor,RefPosition)
    local PlayerHalfHeight = Player:SPGetCapsuleHalfHeight()
    local LowerOffset = 0.0 - PlayerHalfHeight
    local UpperOffset = 0.0 + PlayerHalfHeight
    local AdditionalUpperOffset = 0.0
    local TargetLocation = RefPosition:Copy()
    local TargetInSwimming = false
    if TargetActor:Cast(UE4.ASPGameMonsterBase) then
        local MonsterHalfHeight = TargetActor:SPGetCapsuleHalfHeight()
        -- 星兽如果在水中需要特殊处理
        if _SP.SPGameplayUtils.CheckMonsterIsSwimming(TargetActor) then
            TargetInSwimming = true
            AdditionalUpperOffset = UPPER_OFFSET_FOR_WATER_DETECTION
        end
        UpperOffset = math.max(UpperOffset,0.0 + MonsterHalfHeight)
        LowerOffset = math.min(LowerOffset,0.0 - MonsterHalfHeight)
    end
    -- 从下方往上方进行水面检测
    local StartPosition = RefPosition + UE4.FVector(0,0,RefPosition.Z + LowerOffset)
    local EndPosition = RefPosition + UE4.FVector(0,0,RefPosition.Z + UpperOffset + AdditionalUpperOffset)
    local bWaterCheckResult = false
    local WaterSurfacePos = UE4.FVector(0,0,0)
    bWaterCheckResult,WaterSurfacePos = _SP.SPGameplayUtils.CheckWaterSurfaceAndReturnPos(Player,StartPosition,EndPosition)
    if bWaterCheckResult then
        -- 如果存在水面, 需要将角色拉至水面上方一个身位
        TargetLocation = WaterSurfacePos + PlayerHalfHeight
        SPPetInteractionUtils.LogInfo("[Ability_Task_SwapPosition] Detect WaterSurface, Suitable Position for Player is " .. tostring(TargetLocation))
    else
        -- 如果目标对象处于游泳状态但是无水, 则不传送
        if TargetInSwimming then
            SPPetInteractionUtils.LogWarning("[Ability_Task_SwapPosition] Target is in Swimming but cannot detect water surface, fail to get suitable position for player")
            return false,RefPosition
        end
    end
    return true,TargetLocation
end

function Ability_Task_SwapPosition:ResetScratchPadBP(ScratchPad)
    if ScratchPad then
        ScratchPad.InstigatorActor = nil
        ScratchPad.TargetActor = nil
    end
end

function Ability_Task_SwapPosition:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.TemplatePad)
end

function Ability_Task_SwapPosition:StopMovementForActor(Actor)
    if not _SP.IsValid(Actor) or Actor:Cast(UE4.APawn) == nil then
        return
    end
    local MoveComp = Actor:GetMovementComponent()
    if MoveComp then
        MoveComp:StopMovementImmediately()
    end
end

return Ability_Task_SwapPosition