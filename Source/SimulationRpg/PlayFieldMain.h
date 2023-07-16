// Fill out your copyright notice in the Description page of Project Settings.
/*
*TODO: 
-Add a reset function, and map it to the R key.
-Add score counters
-Add day limit/end of battle win/lost screen.
*/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "TileDisplayObject.h"
#include "ParseTargetPatternFile.h"
#include "CoordinatePairs.h"
#include "Cell.h"
#include "BPAStateMachineUnreal.h"
#include "CanvasItem.h"
#include "Engine/Canvas.h"

#include "PlayFieldMain.generated.h"

UCLASS()
class SIMULATIONRPG_API APlayFieldMain : public APawn
{
	GENERATED_BODY()
		
	
public:	

	// Sets default values for this actor's properties
	APlayFieldMain();
	/*
	* TODO: Put this in a Warrior Actor class (you need to write that)
	* And then replace the warrior template and warrior TArray with that new class. Then, send the HP data to that actor, and draw it somehow!
	UPROPERTY(EditAnywhere, Category = "GameSRPG")
		class UHpDisplayUserWidget* HpWidget;
		*/
	UPROPERTY()
		class UBPAStateMachineUnreal* StateMachine;
	UPROPERTY(EditAnywhere, Category = "GameSRPG")
		TSubclassOf<class AActor> gridObjectTemplate;
	UPROPERTY(EditAnywhere, Category = "GameSRPG")
		TSubclassOf<class AActor> basicWarriorTemplate;
	UPROPERTY(EditAnywhere, Category = "GameSRPG")
		TSubclassOf<class AActor> basicWarriorEnemyTemplate;
	//Cursor references
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameSRPG")
		class AActor* cursorObject;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameSRPG")
		TSubclassOf<class AActor> cursorObjectTemplate;
	//
	UPROPERTY(EditAnywhere)
		class UMaterialInterface* PlayerTerritoryMat;
	UPROPERTY(EditAnywhere)
		class UMaterialInterface* EnemyTerritoryMat;
	UPROPERTY(EditAnywhere)
		class UMaterialInterface* NeutralTerritoryMat;
	//https://unrealcpp.com/change-material-mesh/
	// 
	// 
	//camera references
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameSRPG")
		class UCameraComponent* mainCamera;
	UPROPERTY(EditAnywhere, Category = "GameSRPG")
		TSubclassOf<class UCameraComponent> mainCamera;*/

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void CursorPosSet();
	void FloodFill(int x, int y, int fill, int old);
	void MarkAllTerritory();
	void RenderMarkedTerritory();
	void DrawHPNumbers();

	FVector2D GetClosestCellToCursor();
	FVector workingVectorA;
	FVector workingVectorB;
	
	void PlaceEnemyByPattern();
	CoordinatePairs PickRandomFreeCell(int fallbackX, int fallbackY, int numberOfTries);
	void ResetBoard();
	void CountTerritoryScore();
	void CursorUpdate();
	int SpawnAHero(FVector location, bool isHero, int cellGfxID);
	//--
	ParseTargetPatternFile aiParser; //enemyAIPatterns
	void GetEnemyIdealPositionsFromPattern(FString input);
	Cell AI_CellOutout[24][24]; //modified by  GetEnemyIdealPositionsFromPattern
	//--
	//CELLS:
	const int multiplier = 100; //world scale 
	const int cellSize = 24; //Warning: The cell size is currently hard-coded in the cells array in the header.
	Cell cells[24][24];
	//---

	CoordinatePairs PrevCoordinate;
	CoordinatePairs HeroLastCoordinate;
	CoordinatePairs EnemyLastCoordinate;
	CoordinatePairs LastKilledEnemy;
	
	TArray<AActor*> arrayOfGridObjects;
	TArray<AActor*> arrayOfWarriorModels;
	TArray<CoordinatePairs> PopulatedCells;

	CoordinatePairs attack_list_normal[4] = { CoordinatePairs(1,0),CoordinatePairs(0,1) ,CoordinatePairs(-1,0) ,CoordinatePairs(0,-1) };

	bool limitOnce;
	//FILL
	int playerFill = 0;
	int enemyFill = 0;
	int neutralFill = 0;
	int32 lastTurn = -1;
	int32 PlayerScore;
	int32 PlayerKillScore;
	int32 EnemyScore;
	int32 NeutralScore;
	int32 EnemyKillScore;
	int32 TotalTurnCount;
	int32 MaxNumberOfTurns = 150;
	int32 totalTurnNumber = 0; //not used for score, but to check if it can render again
	float attackStateTimer = 0.0f;
	int32 AttackSubState = 0;
	int32 AttackListIndex = 0;
	int32 CurrentAttackerFlatIndex = 0; //attack X
	//CONST 
	const int neutralPiece = 0; 
	const int heroPiece = 1;
	const int enemyPiece = 2; 
	//TERRITORY TEMP DATA CONSTS
	const int fillPieceHero = 3;
	const int fillPieceEnemy = 4;
	float dt = 0.0f;
	enum State { playerTurn = 0, enemyTurn = 1, dropSkyPlayer = 2, dropSkyEnemy = 3, heroAttackPhase = 4, enemyAttackPhase = 5};
	//STATE
	UFUNCTION()
	void PlayerTurnEnter();
	UFUNCTION()
	void PlayerUpdate();
	UFUNCTION()
	void SpawnHeroOnSkyDrop();
	UFUNCTION()
	void DropFromSkyPlayerUnit();
	UFUNCTION()
	void HerosAttack();
	UFUNCTION()
	void HerosAttackUpdate();
	UFUNCTION()
	void SpawnEnemyOnSkyDrop();
	UFUNCTION()
	void DropFromSkyEnemyUnit();
	UFUNCTION()
	void EnemiesAttack();
	UFUNCTION()
	void EnemiesAttackUpdate();
	UFUNCTION()
	void EnemyUpdate();
	UFUNCTION()
	void EmptyFunction();
	
	void ProccessAttacksReset(int idOfAttackerTeam);
	void ProccessAttacks(int32 idOfAttackerTeam, int32 idOfDefenderTeam, int stateOnEnd, float DeltaTime);
	void SquashAnimation(AActor* attacker,FVector initScale, FVector maxSize, float progress);
	bool GetSkipButton();
	
	
};
