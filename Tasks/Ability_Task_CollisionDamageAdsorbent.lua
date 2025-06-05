local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local SPLuaUtility = require("Feature.StarP.Script.System.SPLuaUtility")
local FocusActorType = require("Feature.StarP.Script.System.Comp.Interact.SPPlayerFocusActorType")
local SPGameLuaUtility = require("Feature.StarP.Script.System.SPLuaUtility")

local Ability_Task_CollisionDamageAdsorbent = UE4.Class(nil, "Ability_Task_CollisionDamageAdsorbent")

local IsValid = UE4.UKismetSystemLibrary.IsValid
local table_insert = table.insert

local function Log(...)
    _SP.Log("SPAbility", "[Ability_Task_CollisionDamageAdsorbent]", ...)
end

function Ability_Task_CollisionDamageAdsorbent:OnTaskStartBP(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    local Target = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_Self)
    if not Target then
        return
    end
    ScratchPad.Target = Target
    ScratchPad.AbilityId = Context:GetAbilityId()
    ScratchPad.AbilityUniqueID = Context:GetAbilityUniqueID()
    ScratchPad.AdsorbentIntervalControlTime = 0
    if _SP.IsClient and not _SP.IsServer then
        self:DoCollisionAndEffect(Context)
        return
    end

    if _SP.IsDSorStandalone then
        -- 初始化伤害配置
        self:InitDamageConfig(ScratchPad)
    end
    if self.AddBuffWhenDamage then
        ScratchPad.AddBuffActors = {}
    end
    -- 计算最大伤害次数
    self:CalcMaxDamageCount(Context)

    -- check once when component spawn
    self:DoCollisionAndEffect(Context, true)
    self:DoDamage(Context)
end

function Ability_Task_CollisionDamageAdsorbent:OnTaskEndBP(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    self:ClearScratchPad(ScratchPad)
end

function Ability_Task_CollisionDamageAdsorbent:OnTaskTickBP(Context, DeltaTime)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    if self.EnableIntervalControlTime then
        ScratchPad.AdsorbentIntervalControlTime = ScratchPad.AdsorbentIntervalControlTime + DeltaTime
        if ScratchPad.AdsorbentIntervalControlTime > self.AttractiveFrequency then
            ScratchPad.AdsorbentIntervalControlTime = 0
            --在客户端也需要执行牵引
            if _SP.IsClient and not _SP.IsServer then
                self:DoCollisionAndEffect(Context)
                return
            end

            self:DoCollisionAndEffect(Context)
            ScratchPad.Timespan = ScratchPad.Timespan + DeltaTime
            self:DoDamage(Context)
        end
    else
        --在客户端也需要执行牵引
        if _SP.IsClient and not _SP.IsServer then
            self:DoCollisionAndEffect(Context)
            return
        end

        self:DoCollisionAndEffect(Context)
        ScratchPad.Timespan = ScratchPad.Timespan + DeltaTime
        self:DoDamage(Context)
    end

    -- if self:CheckCollision(Context) then
    --     self:PostCollision(Context)
    --     self:Adsorbent(Context)
    -- end
end

---CalcMaxDamageCount
---计算最大伤害次数
---@param Context UAbleAbilityContext
function Ability_Task_CollisionDamageAdsorbent:CalcMaxDamageCount(Context)
    local Owner = Context:GetOwner()
    local AbilityId = Context:GetAbilityId()
    local AbilityUniqueID = Context:GetAbilityUniqueID()

    SPAbilityUtils.DispatchMaxDamageCount(AbilityId, AbilityUniqueID, Owner, self.DamageMaxCount, self.DamageInterval, self:GetDuration())
end

function Ability_Task_CollisionDamageAdsorbent:ClearScratchPad(ScratchPad)
    -- ScratchPad.CollisionComponent = nil
    -- ScratchPad.OverlappingComponents = {}
    ScratchPad.CacheDamageActorsInfo = {}
    ScratchPad.Timespan = 0
    ScratchPad.AddBuffActors = {}
    ScratchPad.TraceStart = nil
    ScratchPad.TraceRotation = nil
    ScratchPad.AborbentMap = {}
    ScratchPad.TeamMap = {}
    ScratchPad.CollisionResults = {}
    ScratchPad.AbilityId = nil
    ScratchPad.AbilityUniqueID = nil
    ScratchPad.Target = nil
    ScratchPad.DamageId = nil
    ScratchPad.SummonMaster = nil
    -- ScratchPad.HitResultCount = 0

end

function Ability_Task_CollisionDamageAdsorbent:ResetScratchPadBP(ScratchPad)
    if ScratchPad then
        -- _SP.LogDebug("SPAbility", "[Ability_Task_CollisionDamage]", "ResetScratchPadBP ")
        self:ClearScratchPad(ScratchPad)
    end
end

---InitDamageConfig
---初始化伤害配置
function Ability_Task_CollisionDamageAdsorbent:InitDamageConfig(ScratchPad)
    local DamageId = 0
    local AbilityId = ScratchPad.AbilityId
    local abilityData = _SP.SPGameplayUtils:GetSkillData(AbilityId)
    if abilityData then
        local DamageIds = abilityData.damageIds or {}
        DamageId = tonumber(DamageIds[self.DamageIndex + 1])
        if DamageId == nil then
            DamageId = -1
        end
    end

    ScratchPad.DamageId = DamageId
    -- ScratchPad.DamageConfig = _SP.SPConfigManager:GetConfigById("SPDamageConfigTable", "SPDamageConfig", DamageId)
    -- local HitId = ScratchPad.DamageConfig and ScratchPad.DamageConfig.hitId or 0
    -- ScratchPad.HitConfig = _SP.SPConfigManager:GetConfigById("SPHitConfigTable", "SPHitConfig", HitId)
end

function Ability_Task_CollisionDamageAdsorbent:DoCollisionAndEffect(Context, bStart)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    -- 碰撞检测前的处理
    self:PreCollisionAction(ScratchPad)
    local TraceStart = nil
    local Orientation = nil
    local Owner = ScratchPad.Target
    if not Owner then
        return
    end
    if not self.bTickCollisionChange and not bStart then
        -- read location and rotation form the ScratchPad
        TraceStart = ScratchPad.TraceStart or Owner:K2_GetActorLocation()
        Orientation = ScratchPad.TraceRotation or Owner:K2_GetActorRotation()
    else
        local Transform = UE4.USPAbilityFunctionLibrary.GetAbilityTargetTypeLocationTransform(Context, self.QueryLocation)
        TraceStart = Transform.Translation
        Orientation = Transform.Rotation:ToRotator()
    end

    local ObjectTypes = _SP.SPAbilityUtils.GetObjectTypesPresent(Context, self.CollisionChannelPresent, self.CollisionObjectChannels)

    local HitResults = UE4.TArray(UE4.FHitResult)
    UE4.USPAbilityFunctionLibrary.DoCollisionDetect(Context, Owner, HitResults, self.CollisionShape, ObjectTypes, TraceStart, Orientation, false, self.ShapeRange.HalfExtents, self.ShapeRange.Radius, self.ShapeRange.ConeRadius,
    self.ShapeRange.ConeLength, self.ShapeRange.HalfHeight, self.ShapeRange.CylinderAngle, self.ShapeRange.CylinderInnerRadius, self.ShapeRange.CylinderOuterRadius, self.ShapeRange.CylinderHeight, _SP.IsDSorStandalone and _SP.DS._bShowDebugCollision)

    -- 配置过滤
    UE4.USPAbilityFunctionLibrary.DoCollisionFilterByHitResult(self.Filter.m_Filters, Context, HitResults)
    ScratchPad.CollisionResults = {}
    ScratchPad.CollisionResults = HitResults:ToTable()
    -- 碰撞效果
    if #ScratchPad.CollisionResults  > 0 then
        self:CollisionEffect(ScratchPad, Context)
    end
end

function Ability_Task_CollisionDamageAdsorbent:PreCollisionAction(ScratchPad)
    -- 清理玩家ABP状态
    if ScratchPad and self.bChangeAbpInput then
        local target = nil
        for _, HitResult in pairs(ScratchPad.CollisionResults) do
            target = HitResult.Actor
            self:ChangeSpecialInputFlagToCharacter(target, false)
        end
    end
end

function Ability_Task_CollisionDamageAdsorbent:CollisionEffect(ScratchPad, Context)
    ---Interrupt
    local CollisionResults = ScratchPad.CollisionResults
    self:CollisionEffect_Interrupt(Context, CollisionResults)
    local targetActor = nil
    for _, HitResult in ipairs(CollisionResults) do
        targetActor = HitResult.Actor
        ---Adsorbent
        self:CollisionEffect_Adsorbent(targetActor, ScratchPad, Context)
        --- ABP State
        if self.bChangeAbpInput then
            self:ChangeSpecialInputFlagToCharacter(targetActor, true)
        end
    end
end

function Ability_Task_CollisionDamageAdsorbent:CollisionEffect_Interrupt(Context, HitResults)
    if self:CheckCollision(Context, HitResults) then
        self:PostCollision(Context)
    end
end

function Ability_Task_CollisionDamageAdsorbent:CollisionEffect_Adsorbent(TargetActor, ScratchPad, Context)
    if self.AdsorBentCurve == nil then
        return
    end
    local owner = Context:GetOwner()
    if owner == nil then
        return
    end
    if IsValid(TargetActor) and TargetActor ~= owner then
        -- 判断死亡
        if TargetActor.GetIsDead and TargetActor:GetIsDead() then
            return
        end
        -- 判断是否同阵营
        if not ScratchPad.TeamMap[TargetActor] then
            ScratchPad.TeamMap[TargetActor] = UE4.USPGameLibrary.IsInDifferentTeam(owner, TargetActor)
        end
        local bCanAbsorb = ScratchPad.TeamMap[TargetActor]
        -- 判断召唤物
        -- 如果释放碰撞检测的是召唤物，确保召唤物自己的主人不会被自己打,不能用IsInDifferentTeam来做，因为野生星灵跟他的召唤物也会当做不同队
        if bCanAbsorb == true and owner.GetSPActorType and TargetActor.GetSPActorType then
            if ScratchPad.SummonMaster and ScratchPad.SummonMaster == TargetActor then
                bCanAbsorb = false
            elseif not ScratchPad.SummonMaster and ScratchPad.SummonMaster ~= false and owner:GetSPActorType() == UE4.ESPActorType.Summon then
                ScratchPad.SummonMaster = _SP.SPAbilityUtils.FindFinalSummonMaster(owner)
                if ScratchPad.SummonMaster and ScratchPad.SummonMaster == TargetActor then
                    bCanAbsorb = false
                end
            else
                ScratchPad.SummonMaster = false
            end
        end
        if bCanAbsorb then
            local sourceLocation = nil
            if self.IsUsAbleLocation then
                sourceLocation = UE4.USPAbilityFunctionLibrary.GetAbilityTargetTypeLocationTransform(Context, self.QueryLocation).Translation
            else
                sourceLocation = owner:K2_GetActorLocation()
            end
            local monsterLocation = TargetActor:K2_GetActorLocation()
            local distance = UE4.UKismetMathLibrary.Vector_Distance(sourceLocation, monsterLocation)
            local mapDistance = UE4.UKismetMathLibrary.MapRangeClamped(distance, 0, self.ShapeRange.Radius, 0, 1)
            local adsorbentParam = self.AdsorBentCurve:GetFloatValue(mapDistance)
            if self.Adsorbent2D then
                sourceLocation.z = monsterLocation.z
            end

            local adsorbentPower = UE4.UKismetMathLibrary.Normal(sourceLocation - monsterLocation) * adsorbentParam
            adsorbentPower = self:IncreaseAdsorbentPower(TargetActor, adsorbentPower, ScratchPad)

            -- if TargetActor.IsSPActorDead and TargetActor:IsSPActorDead() then
            --     return
            -- end

            if TargetActor.CharacterMovement then
                TargetActor.CharacterMovement:AddImpulse(adsorbentPower, true)
            end
        end
    end
end

function Ability_Task_CollisionDamageAdsorbent:IsSingleFrameBP()
    return self.SingleFrame
end

function Ability_Task_CollisionDamageAdsorbent:GetTaskRealmBP()
    return UE4.EAbleAbilityTaskRealm.ATR_ClientAndServer
end

function Ability_Task_CollisionDamageAdsorbent:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.CollisionDamage)
end

function Ability_Task_CollisionDamageAdsorbent:DoDamage(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    local DamageId = ScratchPad.DamageId
    local Owner = ScratchPad.Target
    local Instigator = Context:GetInstigator() or Owner

    local DamageInfos = {}
    local CurrentTimespan = ScratchPad.Timespan
    local Actor = nil
    local cacheCount = nil
    local cacheTime = nil
    for _, HitResult in pairs(ScratchPad.CollisionResults) do
        Actor = HitResult.Actor
        if ScratchPad.SummonMaster and ScratchPad.SummonMaster == Actor then
            goto continue
        end
        if ScratchPad.CacheDamageActorsInfo[Actor] == nil then
            ScratchPad.CacheDamageActorsInfo[Actor] = {
                Timespan = CurrentTimespan,
                Count = 1,
            }
            cacheCount = 0
            cacheTime = 0
        else
            cacheCount = ScratchPad.CacheDamageActorsInfo[Actor].Count
            cacheTime = ScratchPad.CacheDamageActorsInfo[Actor].Timespan
        end
        local DeltaTime = CurrentTimespan - cacheTime
        local TimePass = self:IsSingleFrameBP() or (cacheTime <= 0) or (DeltaTime >= self.DamageInterval)
        local CountPass = self.DamageMaxCount < 0 or cacheCount < self.DamageMaxCount
        if TimePass and CountPass then
            ScratchPad.CacheDamageActorsInfo[Actor].Timespan = CurrentTimespan
            ScratchPad.CacheDamageActorsInfo[Actor].Count = ScratchPad.CacheDamageActorsInfo[Actor].Count + 1
            local DamageEvent = UE4.FDamageEvent()
            local DamageInfo = {
                DamagedActor = Actor,
                DamageEvent = DamageEvent,
            }
            table.insert(DamageInfos, DamageInfo)
            -- self:UpdateDamageInfo(DamageId, Actor)
            -- self:PlayHitEffect(CacheInfo, ScratchPad.AbilityId, DamageId, Actor, Owner)
            -- self:IsHead(CacheInfo, Actor)
            if self.AddBuffWhenDamage then
                self:DoBuffLogic(Context,Owner,Actor)
            end
        end
        ::continue::
    end

    if #DamageInfos > 0 then
        Log("ecoliwu Test")
        SPGameLuaUtility:DoDamageByDamageId(
            ScratchPad.AbilityId,
            DamageId,
            DamageInfos,
            ScratchPad.AbilityUniqueID,
            Instigator:GetController(),
            Instigator
        )
    end
end

-- function Ability_Task_CollisionDamageAdsorbent:PlayHitEffect(CacheInfo, SkillId, DamageId, DamagedActor, Owner)

--     if not SPLuaUtility:CanBeDamaged_Lua(SkillId, DamageId, DamagedActor, Owner) then
--         _SP.Log("Can not BeDamaged")
--         return
--     end

--     if self.effectAsset and CacheInfo.HitResult and Owner.LuaMulticast_PlayEffect then
--         if _SP.IsStandalone then
--             UE4.UGameplayStatics.SpawnEmitterAtLocation(_SP.GetCurrentWorld(), self.effectAsset, CacheInfo.HitResult.Location, CacheInfo.HitResult.ImpactNormal, UE4.FVector(1, 1, 1), true)
--         end
--         Owner:LuaMulticast_PlayEffect(self.effectAsset, CacheInfo.HitResult.Location, CacheInfo.HitResult.ImpactNormal:ToRotator(), UE4.FVector(1, 1, 1), true)
--     end
-- end

-- function Ability_Task_CollisionDamageAdsorbent:UpdateDamageInfo(DamageId, HitActor)
--     local dmgConfig = _SP.SPConfigManager:GetConfigById("SPDamageConfigTable", "SPDamageConfig", DamageId)
--     if not dmgConfig then
--         _SP.LogError("SP伤害表中找不到配置DamageID=.",DamageId)
--     end

--     -- update Hit Reaction and Effects
--     if dmgConfig and (not self.hitId or self.hitId ~= dmgConfig.hitId) then
--         self.hitId = dmgConfig.hitId
--         local effectName = ""
--         if self.hitId then
--             local hitConfig = _SP.SPConfigManager:GetConfigById("SPHitConfigTable", "SPHitConfig", self.hitId)
--             if hitConfig then
--                 effectName = hitConfig.hitVFX
--             end
--         end
--         -- self.effectAsset = _SPFeature.AssetMgr:LoadAssetObject(effectName)
--         --self.effectAsset = UE4.LoadObject(effectName)

--         local HitAssetSubsystem = UE4.USPGameLibrary.GetHitAssetConfigSubsystem(_SP.GetCurrentWorld())
--         if HitAssetSubsystem and IsValid(HitActor) then
--             local HitEffectId = effectName
--             local HitActorType = self:GetSPActorType(HitActor)
--             _SP.LogWarning("SPAbility", "Ability_Task_CollisionDamage HitEffectId ", HitEffectId, "HitActorType", HitActorType)
--             self.effectAsset = HitAssetSubsystem:GetSPHitEffectAssetById(HitEffectId, HitActorType)
--         end
--     end
-- end

-- function Ability_Task_CollisionDamageAdsorbent:GetSPActorType(HitActor)
--     local SPActorType = 0
--     local HitActorType = _SP.SPGameplayUtils:GetActorType(HitActor)
--     if FocusActorType.Player == HitActorType then
--         SPActorType = 1
--     elseif FocusActorType.Monster == HitActorType then
--         SPActorType = 2
--     elseif FocusActorType.Choppable == HitActorType then
--         if HitActor.Category == "Wood" then
--             SPActorType = 4
--         elseif HitActor.Category == "Rock" then
--             SPActorType = 5
--         end
--     end

--     return SPActorType
-- end

function Ability_Task_CollisionDamageAdsorbent:DoBuffLogic(Context,SourceActor,TargetActor)
    if not self.AddbuffRepeat then
        local ScratchPad = self:GetScratchPad(Context)
        if not ScratchPad or not ScratchPad.AddBuffActors then
            return
        end
        if ScratchPad.AddBuffActors[TargetActor] ~= nil then
            return
        end
        local addSuccess = SPLuaUtility:AddBuffByDamageSkill_Lua(Context:GetAbilityId(),self.BuffID,self.BuffLayer,self.IndexBuffFromSkill,SourceActor,TargetActor)
        if addSuccess then
            ScratchPad.AddBuffActors[TargetActor] = true
        end
    else
        SPLuaUtility:AddBuffByDamageSkill_Lua(Context:GetAbilityId(),self.BuffID,self.BuffLayer,self.IndexBuffFromSkill,SourceActor,TargetActor)
    end

end

function Ability_Task_CollisionDamageAdsorbent:CheckCollision(Context, HitResults)
    if not self.ShouldInterrupt then
        return false
    end
    if #HitResults > 0 then
        return true
    end
    return false
end

function Ability_Task_CollisionDamageAdsorbent:PostCollision(Context)
    local owner = Context:GetOwner()
    if owner == nil then
        return
    end

    local abilityComponent = owner:GetAbilityComponent()
    if abilityComponent == nil then
        return
    end

    local abilityId = Context:GetAbilityId()
    abilityComponent:InterruptedAbility(abilityId, "interrupt on collisionDamageAdsorbent post collision")
    if self.ComboAbilityId ~= 0 then
        abilityComponent:TryActivateAbility(self.ComboAbilityId, owner, Context:GetInstigator(), false)
    end
end

-- function Ability_Task_CollisionDamageAdsorbent:Adsorbent(Context)
--     local owner = Context:GetOwner()
--     if owner == nil then
--         return
--     end

--     local ScratchPad = self:GetScratchPad(Context)
--     if not ScratchPad then
--         return
--     end

--     if self.AdsorBentCurve == nil then
--         return
--     end

--     for Actor, _ in pairs(ScratchPad.OverlappingComponents) do
--         if IsValid(Actor) and Actor ~= owner then

--             if Actor.GetIsDead and Actor:GetIsDead() then
--                 return
--             end

--             local bCanAbsorb = UE4.USPGameLibrary.IsInDifferentTeam(owner,Actor)

--             -- 如果释放碰撞检测的是召唤物，确保召唤物自己的主人不会被自己打,不能用IsInDifferentTeam来做，因为野生星灵跟他的召唤物也会当做不同队
--             if bCanAbsorb == true and owner.GetSPActorType and Actor.GetSPActorType then
--                 if owner:GetSPActorType() == UE4.ESPActorType.Summon then
--                     local SummonMaster = _SP.SPAbilityUtils.FindFinalSummonMaster(owner)
--                     if Actor == SummonMaster then
--                         bCanAbsorb = false
--                     end
--                 end
--             end

--             if bCanAbsorb then
--                 local sourceLocation = nil
--                 if self.IsUsAbleLocation then
--                     sourceLocation = UE4.USPAbilityFunctionLibrary.GetAbilityTargetTypeLocationTransform(Context, self.QueryLocation).Translation
--                 else
--                     sourceLocation = owner:K2_GetActorLocation()
--                 end
--                 local monsterLocation = Actor:K2_GetActorLocation()
--                 local distance = UE4.UKismetMathLibrary.Vector_Distance(sourceLocation, monsterLocation)
--                 local mapDistance = UE4.UKismetMathLibrary.MapRangeClamped(distance,0,self.ShapeRange.Radius,0,1)
--                 local adsorbentParam = self.AdsorBentCurve:GetFloatValue(mapDistance)
--                 if self.Adsorbent2D then
--                     sourceLocation.z = monsterLocation.z
--                 end

--                 local adsorbentPower = UE4.UKismetMathLibrary.Normal(sourceLocation - monsterLocation) * adsorbentParam
--                 adsorbentPower = self:IncreaseAdsorbentPower(Actor, adsorbentPower, ScratchPad)

--                 if Actor.IsSPActorDead and Actor:IsSPActorDead() then
--                     return
--                 end

--                 if Actor.CharacterMovement then
--                     Actor.CharacterMovement:AddImpulse(adsorbentPower,true)
--                 end
--             end
--         end
--     end
-- end

function Ability_Task_CollisionDamageAdsorbent:ChangeSpecialInputFlagToCharacter(TargetActor, bIsAdd)
    if not TargetActor then
        return
    end
    local Character = TargetActor:Cast(UE4.ACharacter)
    if not IsValid(Character) then
        return
    end

    ---@type USkeletalMeshComponent
    local SkeletalComponent = Character:GetComponentByClass(UE4.USkeletalMeshComponent:StaticClass())
    if not IsValid(SkeletalComponent) then
        return
    end

    ---@type USPGameCharacterAnimInstance
    local AnimInstance = SkeletalComponent:GetAnimInstance()
    if not AnimInstance or not AnimInstance:IsA(UE4.USPGameCharacterAnimInstance) then
        return
    end
    AnimInstance.bSpecialInputValue = bIsAdd

end

function Ability_Task_CollisionDamageAdsorbent:IncreaseAdsorbentPower(TargetActor, OriginValue, ScratchPad)
    if TargetActor and TargetActor.GetSPActorType then
        local ActorType = TargetActor:GetSPActorType()
        local value = ScratchPad.AborbentMap[ActorType]
        if ActorType and not value then
            if self.SpecificAborbentMultiplier:Length() > 0 then
                if not ScratchPad.AborbentMap[ActorType] then
                    ScratchPad.AborbentMap[ActorType] = self.SpecificAborbentMultiplier:Find(ActorType) or -1
                end
            end
            value = ScratchPad.AborbentMap[ActorType]
        end
        if value and value ~= -1 then
            OriginValue = OriginValue * value
        end
    end

    return OriginValue
end


return Ability_Task_CollisionDamageAdsorbent