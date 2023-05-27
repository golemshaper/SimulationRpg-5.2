// Copyright Brian Allanson, Ackk Studios


#include "SoldierActor.h"

// Sets default values
ASoldierActor::ASoldierActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASoldierActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASoldierActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

