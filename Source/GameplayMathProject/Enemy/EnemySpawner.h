#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class AEnemy;

UCLASS()
class GAMEPLAYMATHPROJECT_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemy> EnemyTemplate;

	UFUNCTION()
	void SpawnNewEnemy();

	UPROPERTY()
	TObjectPtr<AEnemy> Enemy;

public:
	AEnemySpawner();

protected:
	virtual void BeginPlay() override;
};
