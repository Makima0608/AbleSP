
local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local SPBattleTalentUtil = require("Feature.StarP.Script.System.BattleTalent.SPBattleTalentUtil")

local Ability_Task_PlayScreenEffect = UE4.Class(nil, "Ability_Task_PlayScreenEffect")

local StarPAttrType = _SPFeature.ServerEnum.StarPAttrType

local Log = function(...)
    _SP.Log("Ability_Task_PlayScreenEffect", ...)
end

local Warning = function(...)
    _SP.LogWarning("Ability_Task_PlayScreenEffect", ...)
end

function Ability_Task_PlayScreenEffect:OnTaskStartBP(Context)
    -- Get ScratchPad
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    ScratchPad.UltimateUI = nil

    local ownerActor = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_Owner)
    if not ownerActor then
        Warning("Player is nil")
        return
    end

    local ultimateUI = SPBattleTalentUtil.GetUltimateUIByElementType(ownerActor, self.EffectGroupId)
    if self.IsUseScreenEffectUI then
        ultimateUI = self.ScereenEffectUIName
    end
    ScratchPad.UltimateUI = ultimateUI

    if ultimateUI ~= nil and not _SPFeature.UIManager:IsWindowOpened(ultimateUI) then
        _SP.Client.SPUIManager:AsyncOpenWindow(ultimateUI)
    end
end

function Ability_Task_PlayScreenEffect:OnTaskTickBP(Context, DeltaTime)
    local ScratchPad = self:GetScratchPad(Context)
    if not ScratchPad then
        return
    end

    if ScratchPad.UltimateUI ~= nil and _SPFeature.UIManager:IsWindowOpened(ScratchPad.UltimateUI) then
        return
    end

    local ownerActor = self:GetSingleActorFromTargetTypeBP(Context, UE4.EAbleAbilityTargetType.ATT_Owner)
    if not ownerActor then
        return
    end

    local ultimateUI = SPBattleTalentUtil.GetUltimateUIByElementType(ownerActor, self.EffectGroupId)
    if ultimateUI ~= nil and not _SPFeature.UIManager:IsWindowOpened(ultimateUI) then
        ScratchPad.UltimateUI = ultimateUI
        _SP.Client.SPUIManager:AsyncOpenWindow(ultimateUI)
    end
end

function Ability_Task_PlayScreenEffect:OnTaskEndBP(Context)
    local ScratchPad = self:GetScratchPad(Context)
    if ScratchPad then
        local ultimateUI = ScratchPad.UltimateUI
        if ultimateUI and _SPFeature.UIManager:IsWindowOpened(ultimateUI) then
            _SP.Client.SPUIManager:CloseWindow(ultimateUI)
        end
        ScratchPad.UltimateUI = nil
    end
end

function Ability_Task_PlayScreenEffect:IsSingleFrameBP()
    return false
end

function Ability_Task_PlayScreenEffect:ResetScratchPadBP(ScratchPad)
    if ScratchPad then
        ScratchPad.UltimateUI = nil
    end
end

function Ability_Task_PlayScreenEffect:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.ScreenEffect)
end

return Ability_Task_PlayScreenEffect