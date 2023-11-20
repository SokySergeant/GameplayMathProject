#include "HelperFunctions.h"
#include "Components/CollisionComponent.h"
#include "Enemy/Enemy.h"

float UHelperFunctions::DotProduct(FVector V1, FVector V2)
{
	return V1.X * V2.X + V1.Y * V2.Y + V1.Z * V2.Z;
}

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
		float Dist = FVector::Distance(Comp1->ColliderLocation(), Comp2->ColliderLocation());
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
		FVector Comp1Loc = Comp1->ColliderLocation();
		FVector Comp2Loc = Comp2->ColliderLocation();
		
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

FVector UHelperFunctions::GetIntersectionPoint(UCollisionComponent* Comp1, UCollisionComponent* Comp2)
{
	FVector Result = FVector(0,0,0);
	
	if(!CheckIntersection(Comp1, Comp2)) return Result; //The collision components are not intersecting
	
	//Sphere to sphere
	if(Comp1->CollisionType == ECollisionType::Sphere && Comp2->CollisionType == ECollisionType::Sphere)
	{
		FVector Dir = (Comp2->ColliderLocation() - Comp1->ColliderLocation()).GetSafeNormal();
		float Dist = Dir.Length();

		Result = Comp1->ColliderLocation() + (Dir * (Dist / 2.f));

		return Result;
	}

	//Cube to sphere
	if(Comp1->CollisionType == ECollisionType::Sphere && Comp2->CollisionType == ECollisionType::AABB || Comp1->CollisionType == ECollisionType::AABB && Comp2->CollisionType == ECollisionType::Sphere)
	{
		UCollisionComponent* SphereComp = Comp1->CollisionType == ECollisionType::Sphere ? Comp1 : Comp2;
		UCollisionComponent* CubeComp = Comp1->CollisionType == ECollisionType::AABB ? Comp1 : Comp2;

		//Positive faces refers to the faces with the higher numbers in their location value in the cube, i.e. between two opposing planes of the cube A and B where their X locations are A:(x = 10) and B:(x = 5), A would be the positive and B would be the negative 
		FVector PositiveFaces = CubeComp->ColliderLocation() + FVector(CubeComp->CollisionInfo.CubeHalfSize);
		FVector NegativeFaces = CubeComp->ColliderLocation() - FVector(CubeComp->CollisionInfo.CubeHalfSize);

		//Constrain sphere position to cube faces
		//X
		if(SphereComp->ColliderLocation().X > PositiveFaces.X)
		{
			Result.X = PositiveFaces.X;
		}else if(SphereComp->ColliderLocation().X < NegativeFaces.X)
		{
			Result.X = NegativeFaces.X;
		}else
		{
			Result.X = SphereComp->ColliderLocation().X;
		}

		//Y
		if(SphereComp->ColliderLocation().Y > PositiveFaces.Y)
		{
			Result.Y = PositiveFaces.Y;
		}else if(SphereComp->ColliderLocation().Y < NegativeFaces.Y)
		{
			Result.Y = NegativeFaces.Y;
		}else
		{
			Result.Y = SphereComp->ColliderLocation().Y;
		}

		//Z
		if(SphereComp->ColliderLocation().Z > PositiveFaces.Z)
		{
			Result.Z = PositiveFaces.Z;
		}else if(SphereComp->ColliderLocation().Z < NegativeFaces.Z)
		{
			Result.Z = NegativeFaces.Z;
		}else
		{
			Result.Z = SphereComp->ColliderLocation().Z;
		}
		
		return Result;
	}

	//Cube to cube
	if(Comp1->CollisionType == ECollisionType::AABB && Comp2->CollisionType == ECollisionType::AABB)
	{

	}
	
	return Result;
}
