#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

class UCollisionComponent;

UCLASS()
class GAMEPLAYMATHPROJECT_API APickup : public AActor
{
	GENERATED_BODY()

public:
	APickup();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCollisionComponent> CollisionComponent;

protected:
	virtual void BeginPlay() override;
};
