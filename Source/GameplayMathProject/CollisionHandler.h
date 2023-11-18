#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollisionHandler.generated.h"

class UCollisionComponent;

UCLASS()
class GAMEPLAYMATHPROJECT_API ACollisionHandler : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UCollisionComponent>> CollisionComps;

	//Debug
	UPROPERTY(EditAnywhere)
	bool ShowDebug = true;
	UPROPERTY(EditAnywhere)
	float DebugThickness = 1.f;

public:
	ACollisionHandler();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void CheckIntersections();
	void DrawShapes();

};
