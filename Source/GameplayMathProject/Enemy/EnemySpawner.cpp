#include "EnemySpawner.h"
#include "Enemy.h"
#include "Engine/World.h"
#include "GameplayMathProject/Components/HealthComponent.h"

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
	
	if(Enemy) //Move old enemy if one exists
	{
		Enemy->SetActorLocation(SpawnLoc, false, nullptr, ETeleportType::ResetPhysics);
		Enemy->SetActorRotation(SpawnRot, ETeleportType::ResetPhysics);
		Enemy->HealthComponent->Setup();
	}else //No old enemy exists, spawn one
	{
		Enemy = GetWorld()->SpawnActor<AEnemy>(EnemyTemplate, SpawnLoc, SpawnRot);
		
		//Bind event to respawn enemy when it dies
		Cast<UHealthComponent>(Enemy->GetComponentByClass(UHealthComponent::StaticClass()))->OnDeath.AddDynamic(this, &AEnemySpawner::SpawnNewEnemy);
	}
}
