
#include "SkateCore/NGSkateAnimInstance.h"

void UNGSkateAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
    if (!TryGetPawnOwner()) return;
    // Normally would use interfaces, but time is running short
    OwningPawn = CastChecked<ANGSkateCharacter>(TryGetPawnOwner());
    
}

void UNGSkateAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!OwningPawn) return;

    bOnSkate = OwningPawn->OnSkate;
    bWantsToJump = OwningPawn->WantsToJump;
    bSkateAccelerating = OwningPawn->SkateAccelerating;
    

 

}
