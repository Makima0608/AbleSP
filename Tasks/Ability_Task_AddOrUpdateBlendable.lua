
local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local Ability_Task_AddOrUpdateBlendable = UE4.Class(nil, "Ability_Task_AddOrUpdateBlendable")

function Ability_Task_AddOrUpdateBlendable:IsSingleFrameBP()
    return false
end

function Ability_Task_AddOrUpdateBlendable:GetTaskRealmBP()
    return UE4.EAbleAbilityTaskRealm.ATR_Client
end

---@param Context UAbleAbilityContext
function Ability_Task_AddOrUpdateBlendable:OnTaskStartBP(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    local Actor = Context:GetSelfActor()
    if not Actor then
        return
    end
    local Character = Actor:Cast(UE4.ASPGameCharacterBase)
    if Character ~= nil then
        local PostProcessComponent = Character:GetComponentByClass(UE4.UPostProcessComponent:StaticClass())
        if PostProcessComponent then
            local linearColor = UE4.FLinearColor(Character:K2_GetActorLocation().X, Character:K2_GetActorLocation().Y, Character:K2_GetActorLocation().Z, 0)
            local dyMat = nil
            if ScratchPad.PostMaterialInstance then
                dyMat = ScratchPad.PostMaterialInstance
            end
            dyMat = UE4.UKismetMaterialLibrary.CreateDynamicMaterialInstance(_SP.GetCurrentWorld(), self.PostRadialBlur)
            dyMat:SetVectorParameterValue("WorldPositon",linearColor)
            dyMat:SetScalarParameterValue("Radius",self.Radius)
            dyMat:SetScalarParameterValue("SampleNumber",self.SampleNumber)
            dyMat:SetScalarParameterValue("Stretch",self.Stretch)
            dyMat:SetScalarParameterValue("BlendrFade",self.BlendrFade)
            PostProcessComponent:AddOrUpdateBlendable(dyMat, self.Weight)
            ScratchPad.PostMaterialInstance = dyMat
        end
    end
end

function Ability_Task_AddOrUpdateBlendable:OnTaskTickBP(Context, DeltaTime)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    local Actor = Context:GetSelfActor()
    if not Actor then
        return
    end
    local dyMat = ScratchPad.PostMaterialInstance
    local Character = Actor:Cast(UE4.ASPGameCharacterBase)
    if Character ~= nil then
        local PostProcessComponent = Character:GetComponentByClass(UE4.UPostProcessComponent:StaticClass())
        if PostProcessComponent then
            local linearColor = UE4.FLinearColor(Character:K2_GetActorLocation().X, Character:K2_GetActorLocation().Y, Character:K2_GetActorLocation().Z, 0)
            dyMat:SetVectorParameterValue("WorldPositon",linearColor)
            PostProcessComponent:AddOrUpdateBlendable(dyMat, self.Weight)
            ScratchPad.PostMaterialInstance = dyMat
        end
    end
end

function Ability_Task_AddOrUpdateBlendable:OnTaskEndBP(Context, Result)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end
    local Actor = Context:GetSelfActor()
    if not Actor then
        return
    end
    local dyMat = ScratchPad.PostMaterialInstance
    local Character = Actor:Cast(UE4.ASPGameCharacterBase)
    if Character ~= nil then
        local PostProcessComponent = Character:GetComponentByClass(UE4.UPostProcessComponent:StaticClass())
        if PostProcessComponent then
            PostProcessComponent:AddOrUpdateBlendable(dyMat, 0)
        end
    end
end

function Ability_Task_AddOrUpdateBlendable:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.AddOrUpdateBlendable)
end

return Ability_Task_AddOrUpdateBlendable