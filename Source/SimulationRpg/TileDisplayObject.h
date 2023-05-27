// Copyright Brian Allanson, Ackk Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileDisplayObject.generated.h"

UCLASS()
class SIMULATIONRPG_API ATileDisplayObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileDisplayObject();
	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameSRPG")
		UStaticMeshComponent* SMNeutralSpace;
		*/
	UPROPERTY(VisibleAnywhere)
		USceneComponent* RootScene;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameSRPG")
		UStaticMesh* NeutralSpace;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameSRPG")
		UStaticMesh* PlayerTerritory;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameSRPG")
		UStaticMesh* EnemyTerritory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameSRPG")
		UStaticMeshComponent* SMNeutralSpace;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameSRPG")
		UStaticMeshComponent* SMPlayerTerritory;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameSRPG")
		UStaticMeshComponent* SMEnemyTerritory;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	void SetTileGraphic(int id);

};
