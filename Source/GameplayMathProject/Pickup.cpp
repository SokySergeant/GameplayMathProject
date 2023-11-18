#include "Pickup.h"
#include "Components/CollisionComponent.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<UCollisionComponent>(TEXT("Collider"));
}

void APickup::BeginPlay()
{
	Super::BeginPlay();
	
}