---@class Ability_Task_SacrificePet
local Ability_Task_SacrificePet = UE4.Class(nil, "Ability_Task_SacrificePet")

local Log = function(...)
    _SP.Log("Ability_Task_SacrificePet", ...)
end

local Warning = function(...)
    _SP.LogWarning("Ability_Task_SacrificePet", ...)
end

function Ability_Task_SacrificePet:OnTaskStartBP(Context)
    Log("OnTaskStartBP")

    ---@type SPGameCharacter
    local player = Context:GetOwner()
    if not player then
        Warning("Player is nil")
        return
    end
    local unleashedPet = player:GetUnleashedPet()
    local petAttribute = _SP.SPSubSystemUtil.GetSPActorAttributeData(unleashedPet)
    if not petAttribute then
        Warning("petAttribute is nil")
        return
    end
    local petCrtHP = petAttribute:GetAttributeValue(_SPFeature.ServerEnum.StarPAttrType.SPAT_Hp)
    petAttribute:DSAddAttributeValue(_SPFeature.ServerEnum.StarPAttrType.SPAT_Hp, -(petCrtHP - self.PetLeftHP), _SPFeature.ServerEnum.StarPAttrOPType.SPAT_OP_Base)

    local captureComp = player:GetCaptureComponent()
    if(captureComp ~=nil and captureComp.ForceDoLeashSafelyInDS_Lua ~= nil) then
        captureComp:ForceDoLeashSafelyInDS_Lua(true,"SacrificePet")
    end
end

function Ability_Task_SacrificePet:OnTaskTickBP(Context)
end

function Ability_Task_SacrificePet:OnTaskEndBP(Context)
end

return Ability_Task_SacrificePet
