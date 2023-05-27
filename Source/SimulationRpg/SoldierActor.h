// Copyright Brian Allanson, Ackk Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HpDisplayUserWidget.h"
#include "SoldierActor.generated.h"

UCLASS()
class SIMULATIONRPG_API ASoldierActor : public AActor
{
	GENERATED_BODY()
		UPROPERTY(EditAnywhere, Category = "GameSRPG")
		class UHpDisplayUserWidget* HpWidget;
public:	
	// Sets default values for this actor's properties
	ASoldierActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
