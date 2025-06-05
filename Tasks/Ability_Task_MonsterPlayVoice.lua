local Ability_Task_MonsterPlayVoice = UE4.Class(nil, "Ability_Task_MonsterPlayVoice")
---@type SPGameAudioManager
local SPGameAudioManager = require("Feature.StarP.Script.System.Sound.SPGameAudioManager")

function Ability_Task_MonsterPlayVoice:OnTaskStartBP(Context)
    local OwnerActor = Context:GetOwner()
    if OwnerActor == nil then
        _SP.LogWarning("AbilityTaskMonsterPlayVoice", "The OwnerActor is nil")
        return
    end
    local OwnerMonster = OwnerActor:Cast(UE4.ASPGameMonsterBase)
    if OwnerMonster == nil then
        _SP.LogWarning("AbilityTaskMonsterPlayVoice", "The OwnerMonster is nil")
        return
    end

    local BaseData = OwnerMonster:GetMonsterBaseData()
    if BaseData == nil then
        _SP.LogWarning("AbilityTaskMonsterPlayVoice", "The BaseData is nil")
        return
    end
    local MonsterID = BaseData.MonsterID
    local VoiceType = self.VoiceType
    SPGameAudioManager:PlayMonsterVoice(OwnerMonster, MonsterID, VoiceType)
end

function Ability_Task_MonsterPlayVoice:OnTaskEndBP(Context)
    if not self.StopWithAble then
        return
    end

    local OwnerActor = Context:GetOwner()
    if OwnerActor == nil then
        _SP.LogWarning("AbilityTaskMonsterPlayVoice", "The OwnerActor is nil")
        return
    end
    local OwnerMonster = OwnerActor:Cast(UE4.ASPGameMonsterBase)
    if OwnerMonster == nil then
        _SP.LogWarning("AbilityTaskMonsterPlayVoice", "The OwnerMonster is nil")
        return
    end

    local BaseData = OwnerMonster:GetMonsterBaseData()
    if BaseData == nil then
        _SP.LogWarning("AbilityTaskMonsterPlayVoice", "The BaseData is nil")
        return
    end
    local MonsterID = BaseData.MonsterID
    SPGameAudioManager:StopMonsterVoice(OwnerMonster, MonsterID)
end

return Ability_Task_MonsterPlayVoice