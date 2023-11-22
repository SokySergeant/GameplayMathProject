#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Enemy/Enemy.h"
#include "GameplayMathCharacter.generated.h"

class UCollisionComponent;
enum class EPlayerRelativeToEnemyFlags;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AEnemy;
class UHealthComponent;

UENUM(BlueprintType)
enum EAttackType
{
	Above,
	Below,
	Left,
	Right,
	Ahead,
	Behind
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttack, AEnemy*, Enemy, EAttackType, AttackType);

UCLASS(hidecategories=(Input, Movement, Collision, Rendering, HLOD, WorldPartition, DataLayers, Replication, Physics, Networking, Actor, LevelInstance, Cooking))
class GAMEPLAYMATHPROJECT_API AGameplayMathCharacter : public ACharacter
{
	GENERATED_BODY()

	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthComponent> HealthComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCollisionComponent> CollisionComponent;

	//Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> AttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> PickupAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ThrowAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> QuitAction;

	//Enemy flag math
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<AEnemy>> Enemies;
	UPROPERTY(EditAnywhere)
	float HeightThresholdAboveBelow = 25.f;
	UPROPERTY(EditAnywhere)
	float PlayerFOVValue = 0.5f;
	UPROPERTY(EditAnywhere)
	float LeftRightAngle = 0.7f;

	//Attack
	bool bCanAttack = true;
	UFUNCTION(BlueprintCallable)
	void FinishAttack();
	
	UPROPERTY(EditAnywhere)
	TArray<EPlayerRelativeToEnemyFlags> AboveAttackFlags;
	UPROPERTY(EditAnywhere)
	TArray<EPlayerRelativeToEnemyFlags> BelowAttackFlags;
	UPROPERTY(EditAnywhere)
	TArray<EPlayerRelativeToEnemyFlags> LeftAttackFlags;
	UPROPERTY(EditAnywhere)
	TArray<EPlayerRelativeToEnemyFlags> RightAttackFlags;
	UPROPERTY(EditAnywhere)
	TArray<EPlayerRelativeToEnemyFlags> AheadAttackFlags;
	UPROPERTY(EditAnywhere)
	TArray<EPlayerRelativeToEnemyFlags> BehindAttackFlags;

	UPROPERTY(BlueprintAssignable)
	FOnAttack OnAttack;

	//Picking up and throwing
	UPROPERTY(EditAnywhere)
	float HoldDistanceInFrontOfPlayer = 100.f;
	UPROPERTY()
	TObjectPtr<AActor> PickedUpActor;
	UPROPERTY()
	TObjectPtr<UCollisionComponent> PickedUpCollisionComp;
	UPROPERTY(EditAnywhere)
	float ThrowPower = 1000.f;
	

public:
	AGameplayMathCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageAmount = 25.f;
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);
	void Pickup(const FInputActionValue& Value);
	void Throw(const FInputActionValue& Value);
	void Quit(const FInputActionValue& Value);
};
