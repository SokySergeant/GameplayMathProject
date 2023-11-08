#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
}

void UHealthComponent::UpdateHealthBy(float Hp)
{
	CurrentHealth += Hp;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);

	if(CurrentHealth == 0.f)
	{
		Die();
	}
}

void UHealthComponent::Die()
{
	if(bIsDead) return;
	bIsDead = true;
	OnDeath.Broadcast();
}
