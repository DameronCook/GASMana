#include "AI/Tasks/BTTask_ClearFocus.h"
#include "ManaCharacterAnimInstance.h"
#include "AI/AIC_NPC.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GASMana/GASManaCharacter.h"

EBTNodeResult::Type UBTTask_ClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIC_NPC* AIController = Cast<AAIC_NPC>(OwnerComp.GetAIOwner()))
	{
		AIController->ClearFocus(EAIFocusPriority::Gameplay);

		if (const AGASManaCharacter* Char = Cast<AGASManaCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor")))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Getting character");
			if (UManaCharacterAnimInstance* AnimInstance = Cast<UManaCharacterAnimInstance>(Char->GetMesh()->GetAnimInstance()))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Setting should strafe to false");
				AnimInstance->SetShouldStrafe(false);
			}

			
			if (Char->GetAbilitySystemComponent())
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Removing blocking tag!");
				const FGameplayTag BlockTag = FGameplayTag::RequestGameplayTag(FName("Character.IsBlocking"));
				Char->GetAbilitySystemComponent()->RemoveLooseGameplayTag(BlockTag);
			}
		}
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
