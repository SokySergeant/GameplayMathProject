#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HelperFunctions.generated.h"

class UCollisionComponent;
enum class EPlayerRelativeToEnemyFlags;
class AEnemy;

UCLASS()
class GAMEPLAYMATHPROJECT_API UHelperFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static float DotProduct2D(FVector2D V1, FVector2D V2);
	static float GetSmoothedValue(float CurrentVal, float TargetVal, float Speed);
	static FVector GetSmoothedValue(FVector CurrentVal, FVector TargetVal, float Speed);
	
	static bool CheckBit(AEnemy* Enemy, EPlayerRelativeToEnemyFlags Flag);

	static bool CheckIntersection(UCollisionComponent* Comp1, UCollisionComponent* Comp2);
};
