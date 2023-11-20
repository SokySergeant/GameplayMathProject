#include "Enemy.h"
#include "Components/CapsuleComponent.h"
#include "GameplayMathProject/Components/CollisionComponent.h"
#include "GameplayMathProject/Components/HealthComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	CollisionComponent = CreateDefaultSubobject<UCollisionComponent>(TEXT("Collider"));
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemy::DamageEnemy(float Damage)
{
	HealthComponent->UpdateHealthBy(-Damage);
}

void AEnemy::DoVoiceline()
{
	float NoiseAmount = FMath::PerlinNoise1D(Seed);
	UE_LOG(LogTemp, Warning, TEXT("%f"), NoiseAmount);
}

