---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by brendanwang.
--- DateTime: 2024/12/17 21:28
---

---@class Ability_Task_SetStopFireData 设置停火数据
local Ability_Task_SetStopFireData = UE4.Class(nil, "Ability_Task_SetStopFireData")

function Ability_Task_SetStopFireData:OnTaskStartBP(Context)
    local Character = self:GetPlayer(Context)
    if _SP.IsValid(Character) then
        local crtWeapon = Character:GetCrtEquipWeapon() or nil
        if _SP.IsValid(crtWeapon) then
            if crtWeapon.SetStopFireStateData then
                crtWeapon:SetStopFireStateData()
            end
            if crtWeapon.StopPetAnimation then
                crtWeapon:StopPetAnimation()
            end
        end
    end
end

function Ability_Task_SetStopFireData:GetPlayer(Context)
    local Character = Context:GetOwner()
    if _SP.IsValid(Character) then
        if Character:Cast(UE4.ASPGameMonsterBase) then
            return Character:FindRelatedPlayerPawn()
        end
    end
    return Character
end


return Ability_Task_SetStopFireData