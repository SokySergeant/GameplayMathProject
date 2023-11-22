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
	
	UFUNCTION(BlueprintCallable)
	void DamageEnemy(float Damage);

	//Voiceline
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> VoicelineSound;
	UPROPERTY()
	TObjectPtr<UAudioComponent> SpawnedAudioComponent;
	
	UPROPERTY(EditAnywhere)
	float Position;
	UPROPERTY(EditAnywhere)
	float Frequency = 1.f;
	UPROPERTY(EditAnywhere)
	float Aplitude = 1.f;

	float CurrentTime = 0.f;
	UPROPERTY(EditAnywhere)
	float AllowedTime = 1.6f;
	
	UFUNCTION(BlueprintCallable)
	void StartVoiceline();
	FTimerHandle VoicelineTimerHandle;
	void DoVoiceline();

protected:
	virtual void BeginPlay() override;
};
