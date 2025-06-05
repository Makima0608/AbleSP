local SPAbilityUtils = require("Feature.StarP.Script.System.Ability.SPAbilityUtils")

local function Log(...)
    _SP.Log("SPAbility", "[ShootProjectileTask]", ...)
end

---@class ShootProjectileTask : UAbleAbilityTask
local ShootProjectileTask = UE4.Class(nil, "ShootProjectileTask")

local EAimTargetLocationType = {
    AimTowardSTP = 0,
    AimTowardTargetLocation = 1
}

function ShootProjectileTask:GetProjectileAimLocation_lua(Owner)
    local AimLocation = UE4.FVector()
    if not _SP.IsValid(Owner) then
        _SP.LogWarning("SPAbilityTask", "ShootProjectileTask:GetProjectileAimLocation_lua Owner is not valid" )
       return AimLocation
    end

    if self.m_AimTargetLocationType == EAimTargetLocationType.AimTowardSTP then
        --获取Monster所在的关卡偏移量
        local Locationoffset = UE4.FVector(0, 0, 0)
        if Owner.GetMonsterDungeonLoc then
            Locationoffset = Owner:GetMonsterDungeonLoc()
        end
        --获取表中配置的位置点
        local VirtualPoint = nil
        if self.STPUID > 0 then
            VirtualPoint = SPAbilityUtils.GetSTPConfig(self.STPUID, true)
        else
            VirtualPoint = SPAbilityUtils.GetSTPConfig(self.STPID, false)
        end

        if VirtualPoint == nil then
            _SP.Log("SPAbilityTask", "ShootProjectileTask:GetProjectileAimLocation_lua" , "Unable to find matching STP points, Please check the ability configuration.")
            return UE4.FVector()
        end

        local locParam = string.split(VirtualPoint.pos, ',')
        AimLocation = UE4.FVector(locParam[1], locParam[2], locParam[3]) + Locationoffset
    end

    _SP.Log("SPAbilityTask", "ShootProjectileTask:GetProjectileAimLocation_lua AimLocation" , AimLocation)
    return AimLocation
end

---OnCalcMaxDamageCount
---计算最大伤害次数
---@param Context UAbleAbilityContext
function ShootProjectileTask:OnCalcMaxDamageCount(Context)
    local Owner = Context:GetOwner()
    local AbilityId = Context:GetAbilityId()
    local AbilityUniqueID = Context:GetAbilityUniqueID()

    SPAbilityUtils.DispatchMaxDamageCount(AbilityId, AbilityUniqueID, Owner, self.m_AmountToSpawn)
end

function ShootProjectileTask:GetTaskRealmBP()
    if self.m_ProjectileBaseInfo.NetPolicy ~= UE4.EProjectileNetPolicy.Independent then
        if self.m_ShootRotationType == UE4.ESPShootRotationType.UseCrosshairDirection or self.m_ShootRotationType == UE4.ESPShootRotationType.UsePlayerAimingPoint or (self.m_ShootRotationType == UE4.ESPShootRotationType.UseSelfRotation and self.m_ProjectileBaseInfo.NetPolicy == UE4.EProjectileNetPolicy.LocalOnly) then
                return UE4.EAbleAbilityTaskRealm.ATR_Client;
            end
        end

        return self.m_TaskRealm;
end
return ShootProjectileTask
