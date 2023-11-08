#include "Enemy.h"

#include "HealthComponent.h"
#include "Components/CapsuleComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FString DebugString = TEXT("");
	if(PlayerRelativeToEnemyFlags & 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::Above))
	{
		DebugString += TEXT("Above ");
	}
	if(PlayerRelativeToEnemyFlags & 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::SamePlane))
	{
		DebugString += TEXT("SamePlane ");
	}
	if(PlayerRelativeToEnemyFlags & 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::Below))
	{
		DebugString += TEXT("Below ");
	}
	if(PlayerRelativeToEnemyFlags & 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::Ahead))
    {
		DebugString += TEXT("Ahead ");
    }
	if(PlayerRelativeToEnemyFlags & 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::Behind))
	{
		DebugString += TEXT("Behind ");
	}
	if(PlayerRelativeToEnemyFlags & 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::Left))
	{
		DebugString += TEXT("Left ");
	}
	if(PlayerRelativeToEnemyFlags & 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::Right))
	{
		DebugString += TEXT("Right ");
	}
	if(PlayerRelativeToEnemyFlags & 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::SeesPlayer))
	{
		DebugString += TEXT("SeesPlayer ");
	}
	if(PlayerRelativeToEnemyFlags & 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::IsSeenByPlayer))
	{
		DebugString += TEXT("IsSeenByPlayer ");
	}

	if(!bShowDebug) return;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *DebugString);
}