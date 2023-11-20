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
	
	//Gravity increment
	CurrentGravityAmount -= GravityAmount * DeltaTime;
	CurrentGravityAmount = FMath::Clamp(CurrentGravityAmount, -MaxGravity, 0.f);

	//Bouncing off things
	for(UCollisionComponent* Comp : CollidingComps)
	{
		if(!Comp->AffectsPhysics) continue; //Ignore this object if it shouldn't affect physics
		
		//Collided with something underneath self, reset gravity
		if(Comp->ColliderLocation().Z < ColliderLocation().Z)
		{
			CurrentGravityAmount = 0.f;
		}

		//Bounce in opposite direction of where collision happened
		FVector IntersectionPoint = UHelperFunctions::GetIntersectionPoint(this, Comp);
		FVector DirToSelf = (ColliderLocation() - IntersectionPoint).GetSafeNormal();
		float Dot = UHelperFunctions::DotProduct(DirToSelf, FVector(0,0,1));

		//Different bounciness value if collided with a wall or a floor
		AddForce(DirToSelf * (Dot > 0.2f ? UpwardsBounciness : SidewaysBounciness) * DeltaTime);
			
		//Add friction for each object being touched
		ForceToApply = UHelperFunctions::GetSmoothedValue(ForceToApply, FVector(0,0,0), Friction * DeltaTime);
	}

	//Force deadzone (so that the object comes to rest at a more normal rate instead of micro bouncing for too long)
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
