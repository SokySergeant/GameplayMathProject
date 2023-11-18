#include "HealthComponent.h"
#include "GameplayMathProject/HelperFunctions.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Setup();
}

void UHealthComponent::Setup()
{
	bIsDead = false;
	TargetHealth = MaxHealth;
	CurrentHealth = MaxHealth;
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	//Smooth health value
	CurrentHealth = UHelperFunctions::GetSmoothedValue(CurrentHealth, TargetHealth, CurrentHealthSmoothingValue);
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