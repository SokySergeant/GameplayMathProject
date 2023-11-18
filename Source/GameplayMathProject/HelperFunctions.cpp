#include "HelperFunctions.h"
#include "Components/CollisionComponent.h"
#include "Enemy/Enemy.h"

float UHelperFunctions::DotProduct2D(FVector2D V1, FVector2D V2)
{
	return V1.X * V2.X + V1.Y * V2.Y;
}

bool UHelperFunctions::CheckBit(AEnemy* Enemy, EPlayerRelativeToEnemyFlags Flag)
{
	return Enemy->PlayerRelativeToEnemyFlags & 1 << static_cast<int32>(Flag);
}

float UHelperFunctions::GetSmoothedValue(float CurrentVal, float TargetVal, float Speed)
{
	return CurrentVal + (TargetVal - CurrentVal) * Speed;
}

FVector UHelperFunctions::GetSmoothedValue(FVector CurrentVal, FVector TargetVal, float Speed)
{
	return CurrentVal + (TargetVal - CurrentVal) * Speed;
}

bool UHelperFunctions::CheckIntersection(UCollisionComponent* Comp1, UCollisionComponent* Comp2)
{
	//Sphere to sphere
	if(Comp1->CollisionType == ECollisionType::Sphere && Comp2->CollisionType == ECollisionType::Sphere)
	{
		float Dist = FVector::Distance(Comp1->GetOwner()->GetActorLocation(), Comp2->GetOwner()->GetActorLocation());
		float Radiuses = Comp1->CollisionInfo.Radius + Comp2->CollisionInfo.Radius;

		return Radiuses > Dist;
	}

	//Cube to sphere
	if(Comp1->CollisionType == ECollisionType::Sphere && Comp2->CollisionType == ECollisionType::AABB || Comp1->CollisionType == ECollisionType::AABB && Comp2->CollisionType == ECollisionType::Sphere)
	{
		UCollisionComponent* SphereComp = Comp1->CollisionType == ECollisionType::Sphere ? Comp1 : Comp2;
		UCollisionComponent* CubeComp = Comp1->CollisionType == ECollisionType::AABB ? Comp1 : Comp2;

		//Positive faces refers to the faces with the higher numbers in their location value in the cube, i.e. between two opposing planes of the cube A and B where their X locations are A:(x = 10) and B:(x = 5), A would be the positive and B would be the negative 
		FVector PositiveFaces = CubeComp->ColliderLocation() + FVector(CubeComp->CollisionInfo.CubeHalfSize);
		FVector NegativeFaces = CubeComp->ColliderLocation() - FVector(CubeComp->CollisionInfo.CubeHalfSize);

		//Find distance between sphere center and closest face (either positive or negative)
		float XDistance = FMath::Abs(SphereComp->ColliderLocation().X - FMath::Max(NegativeFaces.X, FMath::Min(SphereComp->ColliderLocation().X, PositiveFaces.X)));
		float YDistance = FMath::Abs(SphereComp->ColliderLocation().Y - FMath::Max(NegativeFaces.Y, FMath::Min(SphereComp->ColliderLocation().Y, PositiveFaces.Y)));
		float ZDistance = FMath::Abs(SphereComp->ColliderLocation().Z - FMath::Max(NegativeFaces.Z, FMath::Min(SphereComp->ColliderLocation().Z, PositiveFaces.Z)));

		return SphereComp->CollisionInfo.Radius > XDistance && SphereComp->CollisionInfo.Radius > YDistance && SphereComp->CollisionInfo.Radius > ZDistance;
	}

	//Cube to cube
	if(Comp1->CollisionType == ECollisionType::AABB && Comp2->CollisionType == ECollisionType::AABB)
	{
		FVector Comp1Loc = Comp1->GetOwner()->GetActorLocation();
		FVector Comp2Loc = Comp2->GetOwner()->GetActorLocation();
		
		float XDistance = FMath::Abs(Comp1Loc.X - Comp2Loc.X);
		float YDistance = FMath::Abs(Comp1Loc.Y - Comp2Loc.Y);
		float ZDistance = FMath::Abs(Comp1Loc.Z - Comp2Loc.Z);
		
		float MaxXWithinBounds = Comp1->CollisionInfo.CubeHalfSize.X + Comp2->CollisionInfo.CubeHalfSize.X;
		float MaxYWithinBounds = Comp1->CollisionInfo.CubeHalfSize.Y + Comp2->CollisionInfo.CubeHalfSize.Y;
		float MaxZWithinBounds = Comp1->CollisionInfo.CubeHalfSize.Z + Comp2->CollisionInfo.CubeHalfSize.Z;

		return MaxXWithinBounds > XDistance && MaxYWithinBounds > YDistance && MaxZWithinBounds > ZDistance;
	}

	return false;
}
