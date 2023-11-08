#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathDelegate);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYMATHPROJECT_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

	bool bIsDead = false;

	UPROPERTY(EditAnywhere)
	float MaxHealth = 100.f;
	UPROPERTY(VisibleAnywhere)
	float CurrentHealth = 0.f;

public:
	UHealthComponent();

	UPROPERTY(BlueprintAssignable)
	FOnDeathDelegate OnDeath;
	void Die();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void UpdateHealthBy(float Hp);

	UFUNCTION(BlueprintCallable)
	float GetHealthPercentage() {return CurrentHealth / MaxHealth;};
};
