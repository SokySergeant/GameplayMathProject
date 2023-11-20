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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCollisionComponent> CollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 50.f;

protected:
	virtual void BeginPlay() override;
};
