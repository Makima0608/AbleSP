local Ability_Task_StopMontage = UE4.Class(nil, "Ability_Task_StopMontage")

function Ability_Task_StopMontage:OnTaskStartBP(Context)
    local TargetArray = UE4.TArray(UE4.AActor)
    self:GetActorsForTaskBP(Context, TargetArray)
    for i = 1, TargetArray:Length() do
        local Target = TargetArray:Get(i)
        if Target.Mesh and Target.Mesh.GetAnimInstance then
            local AnimInstance = Target.Mesh:GetAnimInstance()
            if AnimInstance then
                AnimInstance:Montage_Stop(0)
            end
        end
    end
end

return Ability_Task_StopMontage