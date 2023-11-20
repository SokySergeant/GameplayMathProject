#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CollisionComponent.generated.h"

enum class ECollisionType;

UENUM()
enum class ECollisionType
{
	Sphere,
	AABB
};

USTRUCT()
struct FCollisionComponentInfo
{
	GENERATED_BODY()

	//Sphere
	UPROPERTY(EditAnywhere)
	float Radius = 0.f;

	//Cube
	UPROPERTY(EditAnywhere)
	FVector CubeHalfSize;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCollisionChanged, UCollisionComponent*, OtherColComp, bool, IsColliding);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYMATHPROJECT_API UCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	//Info
	UPROPERTY(EditAnywhere)
	ECollisionType CollisionType;
	UPROPERTY(EditAnywhere)
	FCollisionComponentInfo CollisionInfo;
	UFUNCTION()
	FVector ColliderLocation() {return GetOwner()->GetActorLocation();};

	//Colliding
	UPROPERTY(BlueprintAssignable)
	FOnCollisionChanged OnCollisionChanged;
	void SetIsColliding(UCollisionComponent* OtherColComp, bool IsColliding);
	
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UCollisionComponent>> CollidingComps;

	UFUNCTION()
	bool IsColliding() {return CollidingComps.Num() > 0;};

	//Physics
	UPROPERTY(EditAnywhere)
	bool Enabled = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool SimulatePhysics = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Whether this collider can affect other colliders with simulated physics on"))
	bool AffectsPhysics = true;

	UPROPERTY(EditAnywhere)
	float GravityAmount = 9.81f;
	UPROPERTY(EditAnywhere)
	float CurrentGravityAmount = 0.f;
	UPROPERTY(EditAnywhere)
	float MaxGravity = 30.f;
	
	UPROPERTY(EditAnywhere)
	FVector ForceToApply;
	UPROPERTY(EditAnywhere)
	float ForceFalloff = 0.5f;
	UPROPERTY(EditAnywhere)
	float SidewaysBounciness = 100.f;
	UPROPERTY(EditAnywhere)
	float UpwardsBounciness = 1.f;
	UPROPERTY(EditAnywhere)
	float Deadzone = 1.f;
	UPROPERTY(EditAnywhere)
	float Friction = 0.5f;

	UFUNCTION()
	void ResetForces();
	UFUNCTION()
	void AddForce(FVector Force);
	
	//Overriden functions
	UCollisionComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
