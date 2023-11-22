#include "Enemy.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameplayMathProject/Components/CollisionComponent.h"
#include "GameplayMathProject/Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"

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

void AEnemy::StartVoiceline()
{
	CurrentTime = 0.f;

	SpawnedAudioComponent = UGameplayStatics::SpawnSoundAttached(VoicelineSound, RootComponent);
	Position = UGameplayStatics::GetTimeSeconds(GetWorld());
	GetWorld()->GetTimerManager().SetTimer(VoicelineTimerHandle, this, &AEnemy::DoVoiceline, GetWorld()->GetDeltaSeconds(), true);
}

void AEnemy::DoVoiceline()
{
	CurrentTime += GetWorld()->GetTimerManager().GetTimerElapsed(VoicelineTimerHandle);
	
	Position += Frequency * GetWorld()->GetDeltaSeconds();
	float Pitch = FMath::PerlinNoise1D(Position) + 1.f; // +1 to remap from -1,1 to 0,2
	
	SpawnedAudioComponent->SetPitchMultiplier(Pitch * Aplitude);

	if(CurrentTime > AllowedTime)
	{
		GetWorld()->GetTimerManager().ClearTimer(VoicelineTimerHandle);
	}
}
