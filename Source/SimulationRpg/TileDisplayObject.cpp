// Copyright Brian Allanson, Ackk Studios


#include "TileDisplayObject.h"

// Sets default values
ATileDisplayObject::ATileDisplayObject()
{

	// RootComponent initialization 
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	//https://forums.unrealengine.com/t/error-unknown-cook-failure-ue-4-12/367544/2
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SMNeutralSpace = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SMNeutralSpace"));
	//SMNeutralSpace->SetMobility(EComponentMobility::Movable);
	//SMNeutralSpace->SetupAttachment(RootScene);
	//SMNeutralSpace->AttachToComponent(RootScene, FAttachmentTransformRules::KeepWorldTransform);
	SMNeutralSpace->SetupAttachment(RootScene);

	SMPlayerTerritory = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SMPlayerTerritory"));
//	SMPlayerTerritory->SetMobility(EComponentMobility::Movable);
	//SMPlayerTerritory->SetupAttachment(RootScene);
	//SMPlayerTerritory->AttachToComponent(RootScene, FAttachmentTransformRules::KeepWorldTransform);
	SMPlayerTerritory->SetupAttachment(RootScene);


	SMEnemyTerritory = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SMEnemyTerritory"));
//	SMEnemyTerritory->SetMobility(EComponentMobility::Movable);
	//SMEnemyTerritory->SetupAttachment(RootScene);
	//SMEnemyTerritory->AttachToComponent(RootScene, FAttachmentTransformRules::KeepWorldTransform);
	SMEnemyTerritory->SetupAttachment(RootScene);

}

// Called when the game starts or when spawned
void ATileDisplayObject::BeginPlay()
{
	Super::BeginPlay();
	SetTileGraphic(1);
}

void ATileDisplayObject::SetTileGraphic(int id)
{
    switch (id)
	{
		case 0:
			SMNeutralSpace->SetHiddenInGame(false);
			SMPlayerTerritory->SetHiddenInGame(true);
			SMEnemyTerritory->SetHiddenInGame(true);
			break;
		case 1:
			SMNeutralSpace->SetHiddenInGame(true);
			SMPlayerTerritory->SetHiddenInGame(false);
			SMEnemyTerritory->SetHiddenInGame(true);
			break;
		case 2:
			SMNeutralSpace->SetHiddenInGame(true);
			SMPlayerTerritory->SetHiddenInGame(true);
			SMEnemyTerritory->SetHiddenInGame(false);
			break;
    }
}

