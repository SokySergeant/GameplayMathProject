#include "GameplayMathCharacter.h"
#include "Enemy.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

AGameplayMathCharacter::AGameplayMathCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	//Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	//Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; 
	CameraBoom->bUsePawnControlRotation = true;

	//Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void AGameplayMathCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Get all enemies
	TArray<TObjectPtr<AActor>> EnemyActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), EnemyActors);
	for(TObjectPtr<AActor> EnemyActor : EnemyActors)
	{
		Enemies.Add(Cast<AEnemy>(EnemyActor));
	}
	
	//Input setup
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AGameplayMathCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//Check flags
	for (int i = 0; i < Enemies.Num(); i++)
	{
		if(!Enemies[i])
		{
			Enemies.RemoveAt(i);
			continue;
		}
		
		FVector VectorFromPlayerToEnemy = Enemies[i]->GetActorLocation() - GetActorLocation();
	
		int32 TempFlags = 0;

		//Check if player is above or below or same plane
		float VerticalDist = GetActorLocation().Z - Enemies[i]->GetActorLocation().Z;
		if(VerticalDist < -HeightThresholdAboveBelow)
		{
			TempFlags |= 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::Below);
		}else if(VerticalDist > HeightThresholdAboveBelow)
		{
			TempFlags |= 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::Above);
		}else
		{
			TempFlags |= 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::SamePlane);
		}

		//Check if player is ahead or behind
		FVector2D ConstrainedDirToEnemy = FVector2D(VectorFromPlayerToEnemy.X,VectorFromPlayerToEnemy.Y).GetSafeNormal();
		FVector2D ConstrainedEnemyForward = FVector2D(Enemies[i]->GetActorForwardVector().X, Enemies[i]->GetActorForwardVector().Y).GetSafeNormal();
		if(DotProduct2D(ConstrainedDirToEnemy, ConstrainedEnemyForward) < 0)
		{
			TempFlags |= 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::Ahead);
		}else
		{
			TempFlags |= 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::Behind);
		}

		//Check if player is left or right
		FVector2D ConstrainedEnemyRight = FVector2D(Enemies[i]->GetActorRightVector().X, Enemies[i]->GetActorRightVector().Y).GetSafeNormal();
		if(DotProduct2D(ConstrainedDirToEnemy, ConstrainedEnemyRight) < -LeftRightAngle)
		{
			TempFlags |= 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::Right);
		}else if(DotProduct2D(ConstrainedDirToEnemy, ConstrainedEnemyRight) > LeftRightAngle)
		{
			TempFlags |= 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::Left);
		}

		//Check if enemy sees player

		//Check if player sees enemy
		FVector2D ConstrainedPlayerForward = FVector2D(GetActorForwardVector().X, GetActorForwardVector().Y).GetSafeNormal();
		if(DotProduct2D(ConstrainedDirToEnemy, ConstrainedPlayerForward) > PlayerFOVValue)
		{
			TempFlags |= 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::IsSeenByPlayer);
		}

		//Update flags in enemy
		Enemies[i]->PlayerRelativeToEnemyFlags = 0;
		Enemies[i]->PlayerRelativeToEnemyFlags = TempFlags;
	} 
}

#pragma region INPUT  

void AGameplayMathCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGameplayMathCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGameplayMathCharacter::Look);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AGameplayMathCharacter::Attack);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}
}

void AGameplayMathCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AGameplayMathCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(-LookAxisVector.Y);
}

void AGameplayMathCharacter::Attack(const FInputActionValue& Value)
{
	//Find closest enemy
	TObjectPtr<AEnemy> ClosestEnemy;
	float ClosestDistance = 10000.f;
	for (TObjectPtr<AEnemy> Enemy : Enemies)
	{
		float TempDist = FVector::Distance(GetActorLocation(), Enemy->GetActorLocation());
		if(TempDist < ClosestDistance)
		{
			ClosestDistance = TempDist;
			ClosestEnemy = Enemy;
		}
	}

	//Don't continue if closest enemy is too far away
	if(ClosestDistance > AttackRange) return;

	if(!bCanAttack) return;
	bCanAttack = false;
	
	//Check closest enemy flags
	//Above attack
	bool CanAttack = true;
	for (EPlayerRelativeToEnemyFlags AttackFlag : AboveAttackFlags)
	{
		if(!CheckBit(ClosestEnemy, AttackFlag))
		{
			CanAttack = false;
			break;
		}
	}
	if(CanAttack) 
	{
		OnAttack.Broadcast(ClosestEnemy, EAttackType::Above);
		return;
	}

	//Below attack
	CanAttack = true;
	for (EPlayerRelativeToEnemyFlags AttackFlag : BelowAttackFlags)
	{
		if(!CheckBit(ClosestEnemy, AttackFlag))
		{
			CanAttack = false;
			break;
		}
	}
	if(CanAttack) 
	{
		OnAttack.Broadcast(ClosestEnemy, EAttackType::Below);
		return;
	}

	//Left attack
	CanAttack = true;
	for (EPlayerRelativeToEnemyFlags AttackFlag : LeftAttackFlags)
	{
		if(!CheckBit(ClosestEnemy, AttackFlag))
		{
			CanAttack = false;
			break;
		}
	}
	if(CanAttack) 
	{
		OnAttack.Broadcast(ClosestEnemy, EAttackType::Left);
		return;
	}

	//Right attack
	CanAttack = true;
	for (EPlayerRelativeToEnemyFlags AttackFlag : RightAttackFlags)
	{
		if(!CheckBit(ClosestEnemy, AttackFlag))
		{
			CanAttack = false;
			break;
		}
	}
	if(CanAttack) 
	{
		OnAttack.Broadcast(ClosestEnemy, EAttackType::Right);
		return;
	}

	//Ahead attack
	CanAttack = true;
	for (EPlayerRelativeToEnemyFlags AttackFlag : AheadAttackFlags)
	{
		if(!CheckBit(ClosestEnemy, AttackFlag))
		{
			CanAttack = false;
			break;
		}
	}
	if(CanAttack) 
	{
		OnAttack.Broadcast(ClosestEnemy, EAttackType::Ahead);
		return;
	}

	//Behind attack
	CanAttack = true;
	for (EPlayerRelativeToEnemyFlags AttackFlag : BehindAttackFlags)
	{
		if(!CheckBit(ClosestEnemy, AttackFlag))
		{
			CanAttack = false;
			break;
		}
	}
	if(CanAttack) 
	{
		OnAttack.Broadcast(ClosestEnemy, EAttackType::Behind);
		return;
	}
}

#pragma endregion

void AGameplayMathCharacter::FinishAttack()
{
	bCanAttack = true;
}

#pragma region HELPERFUNCTIONS

float AGameplayMathCharacter::DotProduct2D(FVector2D V1, FVector2D V2)
{
	return V1.X * V2.X + V1.Y * V2.Y;
}

bool AGameplayMathCharacter::CheckBit(AEnemy* Enemy, EPlayerRelativeToEnemyFlags Flag)
{
	return Enemy->PlayerRelativeToEnemyFlags & 1 << static_cast<int32>(Flag);
}

#pragma endregion