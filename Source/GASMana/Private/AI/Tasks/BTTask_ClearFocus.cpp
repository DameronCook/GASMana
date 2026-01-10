#include "AI/Tasks/BTTask_ClearFocus.h"
#include "AI/AIC_NPC.h"

EBTNodeResult::Type UBTTask_ClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIC_NPC* AIController = Cast<AAIC_NPC>(OwnerComp.GetAIOwner()))
	{
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
