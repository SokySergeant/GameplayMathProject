#include "GameplayMathCharacter.h"
#include "Enemy/Enemy.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HelperFunctions.h"
#include "Pickup.h"
#include "Components/CollisionComponent.h"
#include "Components/HealthComponent.h"

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

	//Health component
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));

	//Collision component
	CollisionComponent = CreateDefaultSubobject<UCollisionComponent>(TEXT("Collider"));
}

void AGameplayMathCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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
}

#pragma region INPUT  

void AGameplayMathCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGameplayMathCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGameplayMathCharacter::Look);
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AGameplayMathCharacter::Attack);

		EnhancedInputComponent->BindAction(PickupAction, ETriggerEvent::Started, this, &AGameplayMathCharacter::Pickup);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Started, this, &AGameplayMathCharacter::Throw);
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

#pragma endregion

#pragma region ATTACKING

void AGameplayMathCharacter::Attack(const FInputActionValue& Value)
{
	if(!bCanAttack) return;
	
	//Get an enemy within my collider
	TObjectPtr<AEnemy> ClosestEnemy;
	for(UCollisionComponent* Comp : CollisionComponent->CollidingComps)
	{
		if(Comp->GetOwner()->IsA(AEnemy::StaticClass())) 
		{
			ClosestEnemy = Cast<AEnemy>(Comp->GetOwner());
			break;
		}
	}
	if(!ClosestEnemy) return; //Didn't find an enemy within my collider

	bCanAttack = false;
	
	//Check flags
	FVector VectorFromPlayerToEnemy = ClosestEnemy->GetActorLocation() - GetActorLocation();

	int32 TempFlags = 0;

	//Check if player is above or below or same plane
	float VerticalDist = GetActorLocation().Z - ClosestEnemy->GetActorLocation().Z;
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
	FVector2D ConstrainedEnemyForward = FVector2D(ClosestEnemy->GetActorForwardVector().X, ClosestEnemy->GetActorForwardVector().Y).GetSafeNormal();
	if(UHelperFunctions::DotProduct2D(ConstrainedDirToEnemy, ConstrainedEnemyForward) < 0)
	{
		TempFlags |= 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::Ahead);
	}else
	{
		TempFlags |= 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::Behind);
	}

	//Check if player is left or right
	FVector2D ConstrainedEnemyRight = FVector2D(ClosestEnemy->GetActorRightVector().X, ClosestEnemy->GetActorRightVector().Y).GetSafeNormal();
	if(UHelperFunctions::DotProduct2D(ConstrainedDirToEnemy, ConstrainedEnemyRight) < -LeftRightAngle)
	{
		TempFlags |= 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::Right);
	}else if(UHelperFunctions::DotProduct2D(ConstrainedDirToEnemy, ConstrainedEnemyRight) > LeftRightAngle)
	{
		TempFlags |= 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::Left);
	}

	//Check if player sees enemy
	FVector2D ConstrainedPlayerForward = FVector2D(GetActorForwardVector().X, GetActorForwardVector().Y).GetSafeNormal();
	if(UHelperFunctions::DotProduct2D(ConstrainedDirToEnemy, ConstrainedPlayerForward) > PlayerFOVValue)
	{
		TempFlags |= 1 << static_cast<int32>(EPlayerRelativeToEnemyFlags::IsSeenByPlayer);
	}

	//Update flags in enemy
	ClosestEnemy->PlayerRelativeToEnemyFlags = 0;
	ClosestEnemy->PlayerRelativeToEnemyFlags = TempFlags;
	
	//Check closest enemy flags
	//Above attack
	bool CanAttack = true;
	for (EPlayerRelativeToEnemyFlags AttackFlag : AboveAttackFlags)
	{
		if(!UHelperFunctions::CheckBit(ClosestEnemy, AttackFlag))
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
		if(!UHelperFunctions::CheckBit(ClosestEnemy, AttackFlag))
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
		if(!UHelperFunctions::CheckBit(ClosestEnemy, AttackFlag))
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
		if(!UHelperFunctions::CheckBit(ClosestEnemy, AttackFlag))
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
		if(!UHelperFunctions::CheckBit(ClosestEnemy, AttackFlag))
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
		if(!UHelperFunctions::CheckBit(ClosestEnemy, AttackFlag))
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

void AGameplayMathCharacter::FinishAttack()
{
	bCanAttack = true;
}

#pragma endregion

#pragma region THROWING

void AGameplayMathCharacter::Pickup(const FInputActionValue& Value)
{
	if(!bCanAttack) return;
	if(PickedUpActor) return; //Already holding something
	if(CollisionComponent->CollidingComps.Num() == 0) return; //No actors with collision nearby

	for(UCollisionComponent* Comp : CollisionComponent->CollidingComps)
	{
		if(Comp->GetOwner()->IsA(APickup::StaticClass())) //Found a pickup within collider
		{
			PickedUpCollisionComp = Comp;
			PickedUpActor = Comp->GetOwner();
			
			//Attach picked up actor to this
			PickedUpCollisionComp->Enabled = false;
			PickedUpActor->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
			PickedUpActor->SetActorRelativeLocation(FVector(HoldDistanceInFrontOfPlayer,0.f,0.f));

			break;
		}
	}
}

void AGameplayMathCharacter::Throw(const FInputActionValue& Value)
{
	if(!bCanAttack) return;
	if(!PickedUpActor) return; //Isn't holding anything

	//Detach picked up actor and throw
	PickedUpActor->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
	PickedUpCollisionComp->Enabled = true;
	PickedUpCollisionComp->ResetForces();
	PickedUpCollisionComp->AddForce(GetActorForwardVector() * ThrowPower);

	//Release references
	PickedUpActor = nullptr;
	PickedUpCollisionComp = nullptr;
}

#pragma endregion