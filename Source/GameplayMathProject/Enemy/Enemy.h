#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

class UCollisionComponent;
class UHealthComponent;

UENUM(BlueprintType, Meta = (Bitflags))
enum class EPlayerRelativeToEnemyFlags
{
	Above,
	SamePlane,
	Below,
	Ahead,
	Behind,
	Left,
	Right,
	SeesPlayer,
	IsSeenByPlayer
};

UCLASS(hidecategories=(Input, Movement, Collision, Rendering, HLOD, WorldPartition, DataLayers, Replication, Physics, Networking, Actor, LevelInstance, Cooking))
class GAMEPLAYMATHPROJECT_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UHealthComponent> HealthComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCollisionComponent> CollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/GameplayMathProject.EPlayerRelativeToEnemyFlags"))
	int32 PlayerRelativeToEnemyFlags;
	
	UPROPERTY(EditAnywhere)
	bool bShowDebug = false;

protected:
	virtual void BeginPlay() override;
};
