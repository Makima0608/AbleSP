local Ability_Task_Event = UE4.Class(nil, "Ability_Task_Event")

local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")
local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")

function Ability_Task_Event:OnTaskStartBP(Context)
    if self.EventName == "" then
        return
    end
    _SP.EventManager:DispatchEvent(self.EventName)
end

function Ability_Task_Event:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.TemplatePad)
end

return Ability_Task_Event