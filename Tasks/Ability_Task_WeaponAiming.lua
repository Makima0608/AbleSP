---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by brendanwang.
--- DateTime: 2024/12/19 17:19
---

---@class Ability_Task_WeaponAiming : UAbleAbilityTask 武器瞄准
local Ability_Task_WeaponAiming = UE4.Class(nil, "Ability_Task_WeaponAiming")

function Ability_Task_WeaponAiming:OnTaskStartBP(Context)
    local Character = Context:GetOwner()
    if _SP.IsValid(Character) then
        self.CurrentWeapon = Character:GetCrtEquipWeapon() or nil
        if _SP.IsValid(self.CurrentWeapon) then
            self.CurrentWeapon:RealOpenAim()
        end
    end
end

function Ability_Task_WeaponAiming:OnTaskEndBP(Context, Result)
    if not self.IsSingleFrame and self.IsStopAtEnd then
        if _SP.IsValid(self.CurrentWeapon) then
            self.CurrentWeapon:RealCloseAim()
        end
    end
end

return Ability_Task_WeaponAiming