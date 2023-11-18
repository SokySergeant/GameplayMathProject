#include "CollisionHandler.h"
#include "HelperFunctions.h"
#include "Components/CollisionComponent.h"
#include "Kismet/GameplayStatics.h"

ACollisionHandler::ACollisionHandler()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACollisionHandler::BeginPlay()
{
	Super::BeginPlay();

	//Get all collision components in scene
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);
	for(TObjectPtr<AActor> Actor : Actors)
	{
		if(UCollisionComponent* CollisionComp = Cast<UCollisionComponent>(Actor->GetComponentByClass(UCollisionComponent::StaticClass())))
		{
			CollisionComps.Add(CollisionComp);
		}
	}
}

void ACollisionHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckIntersections();

	if(!ShowDebug) return;
	DrawShapes();
}

void ACollisionHandler::CheckIntersections()
{
	for(UCollisionComponent* Comp1 : CollisionComps)
	{
		for(UCollisionComponent* Comp2 : CollisionComps)
		{
			if(Comp1 == Comp2) continue; //Don't collide with self

			bool IsColliding = UHelperFunctions::CheckIntersection(Comp1, Comp2);
			Comp1->SetIsColliding(Comp2, IsColliding);
			Comp2->SetIsColliding(Comp1, IsColliding);
		}
	}
}

void ACollisionHandler::DrawShapes()
{
	for(UCollisionComponent* Comp : CollisionComps)
	{
		FColor Color = Comp->IsColliding() ? FColor::Red : FColor::Blue;
		
		switch (Comp->CollisionType)
		{
			case ECollisionType::Sphere:
				DrawDebugSphere(GetWorld(), Comp->ColliderLocation(), Comp->CollisionInfo.Radius, 16, Color, false, -1, 0, DebugThickness);
				break;
			case ECollisionType::AABB:
				DrawDebugBox(GetWorld(), Comp->ColliderLocation(), Comp->CollisionInfo.CubeHalfSize, Color, false, -1, 0, DebugThickness);
				break;
			
			default:
				break;
		}
	}
}
