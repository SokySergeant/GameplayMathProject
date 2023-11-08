#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GameplayMathCharacter.generated.h"

enum class EPlayerRelativeToEnemyFlags;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AEnemy;

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
	UPROPERTY(EditAnywhere)
	float AttackRange = 200.f;
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

	//Helper functions
	float DotProduct2D(FVector2D V1, FVector2D V2);
	bool CheckBit(AEnemy* Enemy, EPlayerRelativeToEnemyFlags Flag);

public:
	AGameplayMathCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);
};
