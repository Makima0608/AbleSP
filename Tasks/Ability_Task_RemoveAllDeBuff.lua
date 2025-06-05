local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local SPLuaUtility = require("Feature.StarP.Script.System.SPLuaUtility")
---@type SPPalAIUtil
local SPAbilityEnums = require("Feature.StarP.Script.System.Ability.SPAbilityEnums")


local Ability_Task_RemoveAllDeBuff = UE4.Class(nil, "Ability_Task_RemoveAllDeBuff")
local IsValid = UE4.UKismetSystemLibrary.IsValid


function Ability_Task_RemoveAllDeBuff:OnTaskStartBP(Context)
    local Target = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_Self)
    if not Target then
        return
    end

    self:RemoveAllDebuff(Context)
    self:DetectOthersByTickTrace(Context,true)
    self:RemoveMateDebuff(Context)
end

function Ability_Task_RemoveAllDeBuff:RemoveAllDebuff(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    local Owner = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_Self)
    if not Owner then
        return
    end

    local AbilityComp = Owner:GetAbilityComponent()
    if AbilityComp ~= nil then
        local buffInstArray = AbilityComp:AllBuffs()
        for i = 1, buffInstArray:Length() do
            local BuffId = buffInstArray:GetRef(i)
            local buffInstance = AbilityComp:FindBuff(BuffId)
            local buffData = buffInstance:GetBuffDataTable_Lua()
            --检测出负面buff
            if buffData.EffectType == -1 then
                AbilityComp:RemoveBuff(BuffId,Owner,buffInstance:GetLayer(),true)
                _SP.LogDebug("SPAbility", "[Ability_Task_RemoveAllDeBuff]", "RemoveAllyAllDebuff()",
                "移除了范围内一位友方或者自己的全部Debuff","移除了范围内一位友方或者自己的全部Debuff:")
            end
        end
    end
    --SPAbilityUtils.GetRelationType

    -- local buffInstance = AbilityComp:FindBuff(id)
    -- AbilityComp:RemoveBuff()
    --local PetData = _SP.Client.SPGamePetManager:GetPetInfo(BackpackType, uuid)

    -- local buffData = instance:GetBuffDataTable_Lua()
    -- --移除负面buff
    -- if buffData.EffectType then
    -- AbilityComponent:RemoveBuff(self.BuffID)
    --虚化
    -- MoeUGCCharPropComponent:SetPropOpacity
end

function Ability_Task_RemoveAllDeBuff:DetectOthersByTickTrace(Context, bStart)
    local ScratchPad=self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    local Owner = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_Self)
    local CharMeshComp = Owner:GetComponentByClass(UE4.USkeletalMeshComponent.StaticClass())

    local TraceStart = Owner:K2_GetActorLocation()
    local Orientation = Owner:K2_GetActorRotation()
    --local Scale = Owner:GetActorScale3D()

    if CharMeshComp then
        Orientation=CharMeshComp:K2_GetComponentRotation()
    end

    local ObjectTypes = _SP.SPAbilityUtils.GetObjectTypesPresent(Context, self.CollisionChannelPresent, self.CollisionObjectChannels)

    -- local HitResults = SPAbilityUtils.DetectOthersByTickTrace(
    --     Context,
    --     Owner,
    --     self.CollisionShape,
    --     ObjectTypes,
    --     TraceStart,
    --     Orientation,
    --     self.ShapeRange
    -- )
    local HitResults = UE4.TArray(UE4.FHitResult)
    UE4.USPAbilityFunctionLibrary.DoCollisionDetect(Context, Owner, HitResults, self.CollisionShape, ObjectTypes, TraceStart, Orientation, false, self.ShapeRange.HalfExtents, self.ShapeRange.Radius, self.ShapeRange.ConeRadius,
    self.ShapeRange.ConeLength, self.ShapeRange.HalfHeight, self.ShapeRange.CylinderAngle, self.ShapeRange.CylinderInnerRadius, self.ShapeRange.CylinderOuterRadius, self.ShapeRange.CylinderHeight, _SP.IsDSorStandalone and _SP.DS._bShowDebugCollision)

    local HitResultCount = HitResults:Length()
    -- _SP.LogDebug("SPAbility", "[Ability_Task_CollisionDamage]", "HitResults:Length ", HitResults:Length())
    if ScratchPad.HitResultCount ~= 0 then
        if HitResultCount == 0 then
            ScratchPad.OverlappingComponents = {}
        end
        -- for Actor, _ in pairs(ScratchPad.OverlappingComponents) do
        --     local hasActor = false
        --     for i = 1, HitResultCount do
        --         if Actor == HitResults:Get(i).Actor then
        --             hasActor = true
        --             break
        --         end
        --     end
        --     if not hasActor then
        --         -- _SP.LogDebug("SPAbility", "[Ability_Task_CollisionDamage]", "DetectOthersByTickTrace Actor Name: ", GetDisplayName(Actor))
        --         ScratchPad.OverlappingComponents[Actor] = nil
        --     end
        -- end
    end

    for i = 1, HitResultCount do
        self:AddOverlapping(Context,HitResults:Get(i).Actor, HitResults:Get(i))
    end

    ScratchPad.HitResultCount = HitResultCount
end

function Ability_Task_RemoveAllDeBuff:AddOverlapping(Context, OverlappedActor, HitResult)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad or not IsValid(OverlappedActor) then
        return
    end

    local Owner=Context:GetOwner();
    if not Owner then
        return
    end

    if OverlappedActor == Owner then
        return
    end

    if ScratchPad.OverlappingComponents[OverlappedActor] == nil then
        ScratchPad.OverlappingComponents[OverlappedActor] = {
            HitResult = HitResult;
        }
    end
end

function Ability_Task_RemoveAllDeBuff:ResetScratchPadBP(ScratchPad)
    if ScratchPad then
        ScratchPad.OverlappingComponents = {}
        ScratchPad.Timespan = 0
        ScratchPad.TraceStart = nil
        ScratchPad.HitResultCount = 0
    end
end


function Ability_Task_RemoveAllDeBuff:RemoveMateDebuff(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    if not ScratchPad.OverlappingComponents then
        ScratchPad.OverlappingComponents = {}
    end

    local AbilityId = Context:GetAbilityId()
    -- local AbilityDataTable = {}
    -- local Ret = SPLuaUtility:GetSkillInfo_Lua(AbilityId, AbilityDataTable)
    local AbilityDataTable, isPassive = _SP.SPGameplayUtils:GetSkillData(AbilityId)
    if not AbilityDataTable then
        return
    end

    local Owner = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_Self)

    for Actor, CacheInfo in pairs(ScratchPad.OverlappingComponents) do
        if IsValid(Actor) then
            local CacheTimespan = CacheInfo.Timespan or 0
            local TimePass = self:IsSingleFrameBP() or (CacheTimespan <= 0)

            if TimePass then
                local bContinue = false
                --近战不打场景尝试性单独筛选处理
                if self.IsIgnoreWorldStatic then
                    if CacheInfo.HitResult then
                        local HitComponent = CacheInfo.HitResult.Component
                        if (IsValid(HitComponent)) then
                            local objectType = HitComponent:GetCollisionObjectType()
                            if objectType == UE4.ECollisionChannel.ECC_WorldStatic then
                                bContinue = true
                            end
                        end
                    end
                end

                if not bContinue then
                    --移除debuff
                    local RelationType = SPAbilityUtils.GetRelationType(Owner, Actor)
                    if RelationType == SPAbilityEnums.SPAbilityRelationType.Ally then
                         _SP.LogDebug("SPAbility", "[Ability_Task_RemoveAllDeBuff]", "CheckRangeObject()",
                         "检测到了范围内的友方对象","检测到了范围内的友方对象")
                        self:RemoveAllDebuff(Context)
                end
        end
        end
    end
end
end

function Ability_Task_RemoveAllDeBuff:OnTaskTickBP(Context, DeltaTime)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

end


function Ability_Task_RemoveAllDeBuff:OnTaskEndBP(Context)
    local Target = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_Self)
    if not Target then
        return
    end

end

function Ability_Task_RemoveAllDeBuff:GetTaskRealmBP()
    return UE4.EAbleAbilityTaskRealm.ATR_ClientAndServer
end

function Ability_Task_RemoveAllDeBuff:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.RemoveAllDebuff)
end


return Ability_Task_RemoveAllDeBuff