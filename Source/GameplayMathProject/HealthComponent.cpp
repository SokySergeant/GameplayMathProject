#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	TargetHealth = MaxHealth;
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	//Lerp health value
	CurrentHealth = GetSmoothedValue(CurrentHealth, TargetHealth, CurrentHealthSmoothingValue);
	if(CurrentHealth < 0.01f)
	{
		Die();
	}
}

void UHealthComponent::UpdateHealthBy(float Hp)
{
	TargetHealth += Hp;
	TargetHealth = FMath::Clamp(TargetHealth, 0.f, MaxHealth);
}

void UHealthComponent::Die()
{
	if(bIsDead) return;
	bIsDead = true;
	OnDeath.Broadcast();
}

float UHealthComponent::GetSmoothedValue(float CurrentVal, float TargetVal, float Speed)
{
	return CurrentVal + (TargetVal - CurrentVal) * Speed;
}
