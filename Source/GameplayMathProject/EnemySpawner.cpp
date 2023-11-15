#include "EnemySpawner.h"
#include "Enemy.h"
#include "HealthComponent.h"
#include "Engine/World.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();


	SpawnNewEnemy();
}

void AEnemySpawner::SpawnNewEnemy()
{
	FVector SpawnLoc = GetActorLocation();
	FRotator SpawnRot = FRotator(0.f,FMath::RandRange(-1,2) * 90.f,0.f); //Random Z rotation that's snapped to the cardinal directions
	AEnemy* SpawnedEnemy = GetWorld()->SpawnActor<AEnemy>(EnemyTemplate, SpawnLoc, SpawnRot);
	
	//Bind event to spawn new enemy when old one dies
	Cast<UHealthComponent>(SpawnedEnemy->GetComponentByClass(UHealthComponent::StaticClass()))->OnDeath.AddDynamic(this, &AEnemySpawner::SpawnNewEnemy);
}
