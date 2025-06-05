
local Ability_Task_GenerateRandomNumber = UE4.Class(nil, "Ability_Task_GenerateRandomNumber")

local SPAbility = require("Feature.StarP.Script.System.Ability.SPAbilityClasses")
local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")

function Ability_Task_GenerateRandomNumber:GetTaskRealmBP()
    return UE4.EAbleAbilityTaskRealm.ATR_Server
end

function Ability_Task_GenerateRandomNumber:OnTaskStartBP(Context)
    math.randomseed(os.time())

    local randomNumber = math.random(self.MinNumber, self.MaxNumber)

    Context:SetIntParameter(self.RandomNumberID, randomNumber)

    local log = string.format("%s: [%d]", self.RandomNumberID, randomNumber)
    _SP.Log("SPAbility", "Ability_Task_GenerateRandomNumber", "Generate Random Number", log)
end

function Ability_Task_GenerateRandomNumber:OnTaskEndBP(Context)

end

function Ability_Task_GenerateRandomNumber:IsSingleFrameBP()
    return true
end

function Ability_Task_GenerateRandomNumber:GetTaskScratchPadClassBP(Context)
    return SPAbilityUtils.LoadScratchPadClass(SPAbility.ScratchPadClass.TemplatePad)
end

return Ability_Task_GenerateRandomNumber