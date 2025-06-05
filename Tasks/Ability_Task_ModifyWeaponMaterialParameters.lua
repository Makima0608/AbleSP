---@class Ability_Task_ModifyWeaponMaterialParameters
local Ability_Task_ModifyWeaponMaterialParameters = UE4.Class(nil, "Ability_Task_ModifyWeaponMaterialParameters")

---@param Context UAbleAbilityContext
function Ability_Task_ModifyWeaponMaterialParameters:OnTaskStartBP(Context)
    self.Character = Context:GetOwner()
    local InventoryComp = self.Character:SafeGetInventoryComponent()
    self.DyMat = nil
    self.CurrentColor = nil
    _SP.Log("SPAbility", "Ability_Task_ModifyWeaponMaterialParameters is Start!")
    if InventoryComp then
        local Weapon = InventoryComp:GetCrtEquipWeapon()
        if Weapon ~= nil then
            local WeaponMesh = Weapon:GetComponentByClass(UE4.UStaticMeshComponent.StaticClass())
            if WeaponMesh == nil then
                return
            end
            local Material = WeaponMesh:GetMaterial(self.MaterialIndex)
            if Material == nil then
                return
            end
            self.CurrentColor = Material:K2_GetVectorParameterValue("BaseColorBright")
            if Material:Cast(UE4.UMaterialInstanceDynamic) then
                --如果已经创建了动态材质，则直接赋值
                self.DyMat = Material
                self.DyMat:SetVectorParameterValue("BaseColorBright", self.Color)
                return
            end
            self.DyMat = UE4.UKismetMaterialLibrary.CreateDynamicMaterialInstance(_SP.GetCurrentWorld(), Material)
            self.DyMat:SetVectorParameterValue("BaseColorBright", self.Color)
            WeaponMesh:SetMaterial(self.MaterialIndex, self.DyMat)
        end
    end
end

---@param Context UAbleAbilityContext
---@param Result EAbleAbilityTaskResult
function Ability_Task_ModifyWeaponMaterialParameters:OnTaskEndBP(Context, Result)
    _SP.Log("SPAbility", "Ability_Task_ModifyWeaponMaterialParameters is End!")
    if self.DyMat == nil or self.CurrentColor == nil then
        return
    end
    self.DyMat:SetVectorParameterValue("BaseColorBright", self.CurrentColor)
end

return Ability_Task_ModifyWeaponMaterialParameters