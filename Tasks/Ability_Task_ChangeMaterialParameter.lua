local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local SPLuaUtility = require("Feature.StarP.Script.System.SPLuaUtility")

local Ability_Task_ChangeMaterialParameter = UE4.Class(nil, "Ability_Task_ChangeMaterialParameter")

local IsValid = UE4.UKismetSystemLibrary.IsValid



function Ability_Task_ChangeMaterialParameter:OnTaskStartBP(Context)
    local owner = Context:GetOwner()
    if owner == nil then
        return
    end

    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    local Target = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_Self)
    if not Target then
        return
    end


    --启用了溶解
    if self.DissolveSwitch and self.DissolveMaterialInstance then
        local CharMeshComp = owner:GetComponentByClass(UE4.USkeletalMeshComponent.StaticClass())
        local materials = CharMeshComp:GetMaterials()
        _SP.LogDebug("SPAbility", "[Ability_Task_ChangeMaterialParamter]", "AddOverlapping", "星兽材质数量", materials:Length())
        for i = 1, materials:Length() do
            CharMeshComp:SetMaterial(i-1,self.DissolveMaterialInstance)
            local material = materials:Get(i)
            local dyMat = CharMeshComp:CreateDynamicMaterialInstance(i, material)
            --dyMat:SetScalarParameterValue("Dissolve_Effect_Switch",tonumber(self.DissolveSwitch))
            dyMat:SetScalarParameterValue("OffsetPos",self.Dissolve)
            _SP.LogDebug("SPAbility", "[Ability_Task_ChangeMaterialParamter]", "AddOverlapping", "Ability_Task_ChangeMaterialParamter", "替换并修改星兽的材质")
        end
    end

    --启用了菲尼尔
    if self.MaterialElementIndex >= 0 and self.FresnelMaterialInstance and self.FresnelOpener then
        -- Add Material
        local Character = Target:Cast(UE4.ASPGameMonsterBase)
        if Character ~= nil then
            local MaterialComponent = Character:GetComponentByClass(UE4.USPMaterialManagerComponent:StaticClass())
            if MaterialComponent then
                local Lifetime = self:GetDuration()
                local dyMat = UE4.UKismetMaterialLibrary.CreateDynamicMaterialInstance(_SP.GetCurrentWorld(), self.FresnelMaterialInstance)
                dyMat:SetVectorParameterValue("Color",self.Color)
                dyMat:SetScalarParameterValue("Exponentln",self.Exponentln)
                MaterialComponent:AddMaterial(dyMat, self.Priority, Lifetime, true)
                ScratchPad.CacheMaterialInstance = self.FresnelMaterialInstance
                _SP.LogDebug("SPAbility", "[Ability_Task_ChangeMaterialParamter]", "AddOverlapping", "CacheMaterialInstance:", "修改材质效果")
            end
        end

    end

    local classPath="Blueprint'/TimeOfDay/StarP/BP_Starp_Base.BP_Starp_Base_C'"
    local loadedClass = UE4.UClass.Load(classPath)
    local world = _SP.GetCurrentWorld()
    local Actors = UE4.UGameplayStatics.GetAllActorsOfClass(world, loadedClass)

    --色差分离 偏移值 强度(chromatic aberration),intensity,start offset
    if Actors:Length() > 0 then
        local starp = Actors:Get(1)
        starp.postProcessTOD.settings.ChromaticAberrationStartOffset = self.ChromaticAberrationStartOffset
        starp.postProcessTOD.settings.SceneFringeIntensity = self.ChromaticAberrationIntensity
        starp.postProcessTOD.settings.VignetteIntensity = self.VigetteIntensity
        _SP.LogDebug("SPAbility", "[Ability_Task_ChangeMaterialParamter]", "AddOverlapping", "获取到了starp蓝图", "修改postProcessTOD后处理参数")

    end

end

function Ability_Task_ChangeMaterialParameter:OnTaskTickBP(Context, DeltaTime)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    local Target = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_Self)
    if not Target then
        return
    end

    local CharMeshComp = Target:GetComponentByClass(UE4.USkeletalMeshComponent.StaticClass())
    local overlayMaterial = CharMeshComp:GetOverlayMaterial()

    if overlayMaterial ~= nil then
        ScratchPad.ElapsedTime = ScratchPad.ElapsedTime + DeltaTime
        local GradientValue = self.GradientCurve:GetFloatValue(ScratchPad.ElapsedTime)
        overlayMaterial:SetScalarParameterValue("Noise_int", GradientValue)

        local FreEmiIntValue = self.Fre_Emi_int_Curve:GetFloatValue(ScratchPad.ElapsedTime)
        overlayMaterial:SetScalarParameterValue("Fre_Emi_int", FreEmiIntValue)

        _SP.LogDebug("SPAbility", "[Ability_Task_ChangeMaterialParamter]", "AddOverlapping", "OnTaskTickBP", "读取曲线修改材质参数")
    end


end

function Ability_Task_ChangeMaterialParameter:ResetScratchPadBP(ScratchPad)
    if ScratchPad then
        ScratchPad.Characters = {}
    end
end


function Ability_Task_ChangeMaterialParameter:OnTaskEndBP(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    if ScratchPad then
        ScratchPad.CacheMaterialInstance = nil
        --ScratchPad.MatInstance = nil
        ScratchPad.ElapsedTime = 0
    end
end

-- function Ability_Task_CollisionDamageAdsorbent:OnAbilityEditorTickBP(Context,DeltaTime)

-- end

function Ability_Task_ChangeMaterialParameter:IsSingleFrameBP()
    return self.SingleFrame
end

function Ability_Task_ChangeMaterialParameter:GetTaskScratchPadClassBP(Context)
    local getClass = SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.ChangeMaterialParameter)
    return getClass
end


return Ability_Task_ChangeMaterialParameter