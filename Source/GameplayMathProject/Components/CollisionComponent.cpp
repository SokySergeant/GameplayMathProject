#include "CollisionComponent.h"

#include "GameplayMathProject/HelperFunctions.h"

UCollisionComponent::UCollisionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCollisionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Simulate physics
	if(!SimulatePhysics) return;
	
	//Gravity
	CurrentGravityAmount -= GravityAmount * DeltaTime;
	CurrentGravityAmount = FMath::Clamp(CurrentGravityAmount, -MaxGravity, 0.f);

	//Bouncing off things
	for(UCollisionComponent* Comp : CollidingComps)
	{
		//Collided with something underneath myself, reset gravity
		if(Comp->ColliderLocation().Z < ColliderLocation().Z)
		{
			CurrentGravityAmount = 0.f;
		}

		//TODO Instead of direction from collider to self, it needs to be direction from intersection point to self
		FVector DirToSelf = (ColliderLocation() - Comp->ColliderLocation()).GetSafeNormal();
		AddForce(DirToSelf * Bounciness * DeltaTime);

		ForceToApply = UHelperFunctions::GetSmoothedValue(ForceToApply, FVector(0,0,0), Friction * DeltaTime);
	}

	//Force deadzone
	if(ForceToApply.Length() < Deadzone)
	{
		ForceToApply = FVector(0,0,0);
	}

	//Apply forces
	FVector NewLoc = ColliderLocation() + FVector(0,0,CurrentGravityAmount) + ForceToApply;
	GetOwner()->SetActorLocation(NewLoc);
	
	//Force falloff
	ForceToApply = UHelperFunctions::GetSmoothedValue(ForceToApply, FVector(0,0,0), ForceFalloff * DeltaTime);
}

void UCollisionComponent::SetIsColliding(UCollisionComponent* OtherColComp, bool IsColliding)
{
	int AddedIndex = -1;
	int RemovedAmount = -1;

	//Add or remove incoming component from array
	if(IsColliding)
	{
		AddedIndex = CollidingComps.AddUnique(OtherColComp);
	}else
	{
		RemovedAmount = CollidingComps.Remove(OtherColComp);
	}

	//Broadcast OnCollisionChanged if the incoming component was added or removed from the array
	if(AddedIndex > 0 && RemovedAmount > 0)
	{
		OnCollisionChanged.Broadcast(OtherColComp, IsColliding);
	}
}

void UCollisionComponent::ResetForces()
{
	ForceToApply = FVector(0,0,0);
	CurrentGravityAmount = 0.f;
}

void UCollisionComponent::AddForce(FVector Force)
{
	ForceToApply += Force;
}
