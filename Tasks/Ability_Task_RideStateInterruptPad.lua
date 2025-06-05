--[[--
* @Description: 骑乘跳跃打断武器开火的技能task的实例数据
* @Author:      dylanyzliu
* @DateTime:    2024/08/05
]]

local S_UE4 = UE4

---@class SPAbility_Task_RideStateInterruptPad
---@field OwnerAbility SPAbility_Task_RideStateInterrupt
---@field RiddenPawn USPRiddenInterface|APawn
local Ability_Task_RideStateInterruptPad = UE4.Class(nil, "Ability_Task_RideStateInterruptPad")

function Ability_Task_RideStateInterruptPad:OnBehaviorOccur(...)
    if self.OwnerAbility ~= nil then
        self.OwnerAbility:OnBehaviorOccur(...)
    end
end

return Ability_Task_RideStateInterruptPad
