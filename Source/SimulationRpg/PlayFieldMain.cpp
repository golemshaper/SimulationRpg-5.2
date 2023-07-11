// Fill out your copyright notice in the Description page of Project Settings.

#include "HudSrpg.h"
#include "PlayFieldMain.h"

// Sets default values
APlayFieldMain::APlayFieldMain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayFieldMain::BeginPlay()
{
	Super::BeginPlay();

	UWorld* const World = GetWorld();
	
	//Find the actor that handles control for the local player.
	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);

	//SETUP STATES HERE
	StateMachine = NewObject<UBPAStateMachineUnreal>();
	UBPAStateMachineUnreal::FStateSMDelegate OnDoNothing;
	OnDoNothing.AddDynamic(this, &APlayFieldMain::EmptyFunction);

	//PLAYER UPDATE
	UBPAStateMachineUnreal::FStateSMDelegate OnPlayerUpdate;
	UBPAStateMachineUnreal::FStateSMDelegate OnPlayerTurnEnter;
	OnPlayerUpdate.AddDynamic(this, &APlayFieldMain::PlayerUpdate);
	OnPlayerTurnEnter.AddDynamic(this, &APlayFieldMain::PlayerTurnEnter);
	StateMachine->MapState(State::playerTurn, OnPlayerUpdate, OnPlayerTurnEnter);
	//DROP SKY PLAYER
	UBPAStateMachineUnreal::FStateSMDelegate OnSpawnHeroOnSkyDrop;
	UBPAStateMachineUnreal::FStateSMDelegate OnDropFromSkyPlayerUnit;
	OnSpawnHeroOnSkyDrop.AddDynamic(this, &APlayFieldMain::SpawnHeroOnSkyDrop);
	OnDropFromSkyPlayerUnit.AddDynamic(this, &APlayFieldMain::DropFromSkyPlayerUnit);
	StateMachine->MapState(State::dropSkyPlayer, OnDropFromSkyPlayerUnit, OnSpawnHeroOnSkyDrop);
	//HERO ATTACK
	UBPAStateMachineUnreal::FStateSMDelegate OnHerosAttack;
	UBPAStateMachineUnreal::FStateSMDelegate OnHerosAttackUpdate;

	OnHerosAttack.AddDynamic(this, &APlayFieldMain::HerosAttack);
	OnHerosAttackUpdate.AddDynamic(this, &APlayFieldMain::HerosAttackUpdate);
	StateMachine->MapState(State::heroAttackPhase, OnHerosAttackUpdate, OnHerosAttack);
	//DROP SKY ENEMY
	UBPAStateMachineUnreal::FStateSMDelegate OnDropFromSkyEnemyUnit;
	UBPAStateMachineUnreal::FStateSMDelegate OnSpawnEnemyOnSkyDrop;
	OnDropFromSkyEnemyUnit.AddDynamic(this, &APlayFieldMain::DropFromSkyEnemyUnit);
	OnSpawnEnemyOnSkyDrop.AddDynamic(this, &APlayFieldMain::SpawnEnemyOnSkyDrop);
	StateMachine->MapState(State::dropSkyEnemy, OnDropFromSkyEnemyUnit, OnSpawnEnemyOnSkyDrop);
	//ENEMY ATTACK
	UBPAStateMachineUnreal::FStateSMDelegate OnEnemiesAttack;
	UBPAStateMachineUnreal::FStateSMDelegate OnEnemiesAttackUpdate;

	OnEnemiesAttack.AddDynamic(this, &APlayFieldMain::EnemiesAttack);
	OnEnemiesAttackUpdate.AddDynamic(this, &APlayFieldMain::EnemiesAttackUpdate);
	StateMachine->MapState(State::enemyAttackPhase, OnEnemiesAttackUpdate, OnEnemiesAttack);
	//ENEMY UPDATE
	UBPAStateMachineUnreal::FStateSMDelegate OnEnemyUpdate;
	OnEnemyUpdate.AddDynamic(this, &APlayFieldMain::EnemyUpdate);
	StateMachine->MapState(State::enemyTurn, OnEnemyUpdate, OnDoNothing);
	//SET INITIAL STATE
	StateMachine->SetState(State::playerTurn);
	
	StateMachine->timeMultiplier = 3.0f;
	//Smoothly transition to our actor on begin play.
	//OurPlayerController->SetViewTargetWithBlend(this , 2.f);
	// 
	
	//Create playfield:
	
	int halfField = (cellSize / 2)* multiplier;
	
	for (int i = 0; i < cellSize; ++i)
	{
		for (int j = 0; j < cellSize; ++j)
		{
			FVector location = FVector((i* multiplier)- halfField,(j* multiplier)- halfField,0);
			AActor* spawnedActor = World->SpawnActor<AActor>(gridObjectTemplate, location, FRotator().ZeroRotator);
			if(spawnedActor){
				spawnedActor->SetActorScale3D(FVector(0.25,0.25,0.25));
				cells[i][j].cellGraphicID = arrayOfGridObjects.Num();
				arrayOfGridObjects.Add(spawnedActor);
			}
			cells[i][j].CellData = 0;
			cells[i][j].unitGraphicID = -1;

		}
	}
	//Cursor
	
	AActor* nCursor = World->SpawnActor<AActor>(cursorObjectTemplate, FVector(0, 0, 0), FRotator(0,0,0));
	cursorObject = nCursor;


	//Example of referencing cell graphics:
	int cellGfxID = cells[0][0].cellGraphicID;
	arrayOfGridObjects[cellGfxID]->SetActorScale3D(FVector(1, 1, 1));
	HeroLastCoordinate.Set(halfField,halfField);
	PrevCoordinate.Set(-1, -1);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("CELL TEMP DATA %f"), playerFill));


	bool aiParseSuccess;
	aiParser.ParseDataFromFile(FPaths::ProjectContentDir()+"/TextAssets/AiTargetPatterns24x24.txt", aiParseSuccess);

	//MOVE THIST TO RESET GAME LATER
	
	
	ResetBoard();
}
void APlayFieldMain::ResetBoard()
{
	TotalTurnCount = 0;
	PlayerScore = 0;
	PlayerKillScore = 0;
	EnemyScore = 0;
	EnemyKillScore = 0;
	GetEnemyIdealPositionsFromPattern(aiParser.GetPatternData());
	//TODO: Repool old Hero models
	//TODO: Repool old Enemy models
	MarkAllTerritory();
	RenderMarkedTerritory();
	CountTerritoryScore();
	for (int i = 0; i < cellSize; ++i)
	{
		for (int j = 0; j < cellSize; ++j)
		{
			cells[i][j].CellData = 0;
			cells[i][j].TempData = 0;
			cells[i][j].atk = 0;
			cells[i][j].hp = 0;
			cells[i][j].unitGraphicID = -1; //TODO, ONLY ASSIGN NEW ID if -1, OTHERWISE USE THE CELS CURRENT ID, AND DESTROY ACTOR IN THAT PALCE AND SPAWN A NEW ACTOR
		}
	}
}
void APlayFieldMain::CountTerritoryScore()
{
	PlayerScore = 0;
	EnemyScore = 0;
	NeutralScore = 0;
	for (int i = 0; i < cellSize; ++i)
	{
		for (int j = 0; j < cellSize; ++j)
		{
			if (cells[i][j].CellData == heroPiece)
			{
				PlayerScore++;
			}
			if (cells[i][j].CellData == enemyPiece)
			{
				EnemyScore++;
			}

			if (cells[i][j].CellData == neutralPiece)
			{
				NeutralScore++;
			}

			if (cells[i][j].CellData == fillPieceEnemy)
			{
				EnemyScore++;
			}
			if (cells[i][j].TempData == fillPieceHero)
			{
				PlayerScore++;
			}
		}
	}
}


void APlayFieldMain::CursorPosSet()
{
	if (cursorObject == nullptr)
	{
		return;
	}
	//https://stackoverflow.com/questions/58925092/set-actor-world-location-from-mouse-position-math
	FVector WorldLocation = FVector(0, 0, 0);
	FVector WorldDirection = FVector(0, 0, 0);
	float DistanceAboveGround = 0.0f;

	Cast<APlayerController>(GetController())->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	FVector PlaneOrigin(0.0f, 0.0f, DistanceAboveGround);

	FVector ActorWorldLocation = FMath::LinePlaneIntersection(
		WorldLocation,
		WorldLocation + WorldDirection,
		PlaneOrigin,
		FVector::UpVector);

	cursorObject->SetActorLocation(ActorWorldLocation);

	

}

FVector2D APlayFieldMain::GetClosestCellToCursor()
{
	float previousDistance = 9999.99f;

	int objectID = 0;
	//find the closest object location in flat array
	for (int i = 0; i < arrayOfGridObjects.Num(); ++i)
	{
		float curDistance = FVector::Distance(arrayOfGridObjects[i]->GetActorLocation(), cursorObject->GetActorLocation());

		if (curDistance < previousDistance)
		{
			previousDistance = curDistance;
			objectID = i;
		}
	}
	//find the object ID that matches the object
	for (int i = 0; i < cellSize; ++i)
	{
		for (int j = 0; j < cellSize; ++j)
		{
			if (cells[i][j].cellGraphicID == objectID)
			{
				return FVector2D(i, j);
			}
		}
	}
	return FVector2D(0, 0);


	
}
void APlayFieldMain::RenderMarkedTerritory()
{
	

	if (lastTurn == totalTurnNumber)
	{
		
		return;
	}
	lastTurn = totalTurnNumber;
	
	for (int i = 0; i < cellSize; ++i)
	{
		for (int j = 0; j < cellSize; ++j)
		{
			bool hasAUnitOnTheTile = false;
			ATileDisplayObject* tileDisplay = Cast<ATileDisplayObject>(arrayOfGridObjects[cells[i][j].cellGraphicID]);
			if (tileDisplay != nullptr)tileDisplay->SetTileGraphic(0);
			//POTENTIALL LIGHT UP HERO OR ENEMY GRIDS, OR SKIP THEM
			if (cells[i][j].CellData != neutralPiece)
			{
				if (cells[i][j].CellData == heroPiece)
				{
					//MARK ground the hero is standing on.
					if (tileDisplay != nullptr)tileDisplay->SetTileGraphic(1);
					hasAUnitOnTheTile = true;

				}
				if (cells[i][j].CellData == enemyPiece)
				{
					//MARK ground the enemy is standing on.
					if (tileDisplay != nullptr)tileDisplay->SetTileGraphic(2);
					hasAUnitOnTheTile = true;
				}
				
				continue; //only mark up the empty space, not the space a unit occupies
			}
			//MARK TERRITORY

			if (cells[i][j].TempData == fillPieceHero)
			{
				//MARK TERRITORY
				if (tileDisplay != nullptr)tileDisplay->SetTileGraphic(1);
				
			}
			if (cells[i][j].TempData == fillPieceEnemy)
			{
				//MARK TERRITORY
				if (tileDisplay != nullptr)tileDisplay->SetTileGraphic(2);
			}

			
		}
	}
}
void APlayFieldMain::DrawHPNumbers()
{
	//DRAW HP
	AHudSrpg* hud = Cast<AHudSrpg>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (hud != nullptr)
	{
		hud->ClearHp();
	}
	for (int i = 0; i < cellSize; ++i)
	{
		for (int j = 0; j < cellSize; ++j)
		{
			bool hasAUnitOnTheTile = false;
			if (cells[i][j].CellData != neutralPiece)
			{
				hasAUnitOnTheTile = true;
			}
			if (hasAUnitOnTheTile && hud != nullptr)
			{
				int unitID = cells[i][j].unitGraphicID;

				hud->DrawHp(arrayOfWarriorModels[unitID]->GetActorLocation(), cells[i][j].hp);
			}
		}

	}
}
void APlayFieldMain::MarkAllTerritory()
{
	for (int i = 0; i < cellSize; ++i)
	{
		for (int j = 0; j < cellSize; ++j)
		{
			this->playerFill = 0;
			this->enemyFill = 0;

			this->FloodFill(i, j, fillPieceHero, 0);
			if (enemyFill > playerFill)
			{
				//redraw, but in the enemy color
				this->FloodFill(i, j, fillPieceEnemy, fillPieceHero);
			}
			
			//neutral space
			this->FloodFill(1, 1, 0, fillPieceHero);
			this->FloodFill(1, cellSize - 1, 0, fillPieceHero);
			this->FloodFill(cellSize - 1, 1, 0, fillPieceHero);
			this->FloodFill(cellSize - 1, cellSize - 1, 0, fillPieceHero);
			//neutral space 2
			this->FloodFill(1, 1, 0, fillPieceEnemy);
			this->FloodFill(1, cellSize - 1, 0, fillPieceEnemy);
			this->FloodFill(cellSize - 1, 1, 0, fillPieceEnemy);
			this->FloodFill(cellSize - 1, cellSize - 1, 0, fillPieceEnemy);
			
		}
	}
}
void APlayFieldMain::FloodFill(int x, int y, int fill, int old)
{
	if ((x <= 0) || (x >= cellSize - 1)) return;
	if ((y <= 0) || (y >= cellSize - 1)) return;

	if (this->cells[x][y].TempData == old)
	{
		this->cells[x][y].TempData = fill;

		this->FloodFill(x + 1, y, fill, old);
		this->FloodFill(x, y + 1, fill, old);
		this->FloodFill(x - 1, y, fill, old);
		this->FloodFill(x, y - 1, fill, old);
	}
	else
	{
		//find out who's territory it is using these variables
		if (this->cells[x][y].TempData == heroPiece)
		{
			this->playerFill++;
		}
		if (this->cells[x][y].TempData == enemyPiece)
		{
			this->enemyFill++;
		}
	}
	
}
void APlayFieldMain::GetEnemyIdealPositionsFromPattern(FString input)
{

	for (int i = 0; i < cellSize; ++i)
	{
		for (int j = 0; j < cellSize; ++j)
		{
			AI_CellOutout[i][j].CellData = 0;
		}
	}

	TArray<FString> StrAry;
	input.ParseIntoArray(StrAry, TEXT("\n"), true);


	for (int i = 0; i < StrAry.Num(); ++i)
	{
		FString s = StrAry[i];
		for (int j = 0; j < s.Len(); ++j)
		{
			if (s[j] == 'X')
			{
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("FOUND ENEMY DATA")));
				AI_CellOutout[i][j].CellData = enemyPiece;
			}
			if (s[j] == 'O')
			{
				//Used for saved data at a later point in time
				AI_CellOutout[i][j].CellData = heroPiece;
			}
		}
	}

}

void APlayFieldMain::PlaceEnemyByPattern()
{
	//float curDistance = FVector::Distance(arrayOfGridObjects[i]->GetActorLocation(), cursorObject->GetActorLocation());
	FVector lastPlayedPos = FVector(HeroLastCoordinate.X * multiplier, HeroLastCoordinate.Y * multiplier, 0);
	float shortestDist = 999999.0f;
	CoordinatePairs playAtPosition;
	playAtPosition.Set(0, 0);
	bool foundALocation = false;
	//AI_CellOutout instead of potentialPattern from the OG version of the game I think.
	for (int i = 0; i < cellSize; ++i)
	{
		for (int j = 0; j < cellSize; ++j)
		{
			
			if (i == LastKilledEnemy.X && j == LastKilledEnemy.Y)
			{
				continue; //dont play in the place where the enemy just died
			}
			if (AI_CellOutout[i][j].CellData == heroPiece)
			{
				continue; //skip it if the player is standing here (pattern)
			}
			if (cells[i][j].CellData == heroPiece)
			{
				continue; //skip it if the player is standing here(real map)
			}
			if (cells[i][j].CellData == enemyPiece)
			{
				continue; //skip it if the enemy is standing here(real map)
			}
			//Note that this is the enemy piece IN THE PATTERN, not on the real map!!
			if (AI_CellOutout[i][j].CellData == enemyPiece)
			{
				FVector thisCelPos = FVector(i * multiplier, j * multiplier, 0);
				
				//closest to player location or the last enemy position if it was hijacked.
				float nDistance = FVector::Distance(thisCelPos, lastPlayedPos);
				if (nDistance < shortestDist)
				{
					shortestDist = nDistance;
					playAtPosition.Set (i,j);
					foundALocation = true;
					
				}
			}
		}
	}
	if (foundALocation == false)
	{
		auto randomFreeplay = PickRandomFreeCell(playAtPosition.X, playAtPosition.Y,100);
		playAtPosition.X = randomFreeplay.X;
		playAtPosition.Y = randomFreeplay.Y;
	}
	//-----------------------------------------------------------------
	float chanceToDeviate = FMath::RandRange(0.0f,100.0f);
	// For a story event, set the enemy to a disorganizedone, and always set this to zero.
		//Each mission should be able to have custom values for chance to diviate and chance to pick at random!
	if (chanceToDeviate < 12 && chanceToDeviate>5)
	{
		if (playAtPosition.X < cellSize - 1 && playAtPosition.X>0)
		{
			if (playAtPosition.Y < cellSize - 1 && playAtPosition.Y > 0)
			{
				//safe to deviate here.
				TArray<int32> potentialX;
				TArray<int32> potentialY;
				potentialX.Add(playAtPosition.X);
				potentialY.Add(playAtPosition.Y);

				if (cells[playAtPosition.X + 1][ playAtPosition.Y].CellData == neutralPiece)
				{
					potentialX.Add(playAtPosition.X + 1);
				}
				if (cells[playAtPosition.X - 1][ playAtPosition.Y].CellData == neutralPiece)
				{
					potentialX.Add(playAtPosition.X - 1);
				}
				if (cells[playAtPosition.Y][playAtPosition.Y + 1].CellData == neutralPiece)
				{
					potentialY.Add(playAtPosition.Y + 1);
				}
				if (cells[playAtPosition.X][playAtPosition.Y - 1].CellData == neutralPiece)
				{
					potentialY.Add(playAtPosition.Y - 1);
				}
				int selectNX = FMath::RandRange(0, potentialX.Num() - 1);
				int selectNY = FMath::RandRange(0, potentialY.Num() - 1);
				playAtPosition.X = potentialX[selectNX];
				playAtPosition.Y = potentialY[selectNY];
				
			}
		}
	}
	if (chanceToDeviate <= 5)
	{
		//random play!
		auto randomPlay = PickRandomFreeCell(playAtPosition.X, playAtPosition.Y, 100);
		playAtPosition.X = randomPlay.X;
		playAtPosition.Y = randomPlay.Y;
	}
	//-----------------------------------------------------------------
	cells[playAtPosition.X][playAtPosition.Y].CellData = enemyPiece;
	cells[playAtPosition.X][playAtPosition.Y].hp = 8;
	cells[playAtPosition.X][playAtPosition.Y].atk = 1;
	int32 celID = cells[playAtPosition.X][playAtPosition.Y].cellGraphicID;
	arrayOfGridObjects[celID]->SetActorScale3D(FVector(1, 1, 1));
	EnemyLastCoordinate.Set(playAtPosition.X, playAtPosition.Y);
	ATileDisplayObject* tileDisplay = Cast<ATileDisplayObject>(arrayOfGridObjects[cells[playAtPosition.X][playAtPosition.Y].cellGraphicID]);
	if (tileDisplay != nullptr)tileDisplay->SetTileGraphic(2);

}

CoordinatePairs APlayFieldMain::PickRandomFreeCell(int fallbackX, int fallbackY, int numberOfTries)
{
	CoordinatePairs result = CoordinatePairs(fallbackX, fallbackY);

	while (numberOfTries > 0)
	{
		numberOfTries--;
		result.X = FMath::RandRange(0,cellSize - 1);
		result.Y = FMath::RandRange(0, cellSize - 1);
		if (cells[result.X][result.Y].CellData == neutralPiece)
		{
			return result;
		}
	}
	return result;
}

// Called every frame
void APlayFieldMain::Tick(float DeltaTime)
{
	dt = DeltaTime;
	Super::Tick(DeltaTime);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("STATE:%i"),StateMachine->GetState()));

	StateMachine->UpdateTick(DeltaTime);

	bool skipAttackAnimations = GetSkipButton();
	if (skipAttackAnimations && StateMachine->GetState()!=State::playerTurn)
	{
		//update twice for each turn played until the state is the player turn state. This will skip every animation, and skip the little pause
		for (int i = 0; i < TotalTurnCount * 2; i++)
		{
			StateMachine->UpdateTick(DeltaTime);
		}
	}



	CursorUpdate();


	//FLOOD FILL
	MarkAllTerritory();
	RenderMarkedTerritory();
	//DRAW HUD:
	DrawHPNumbers();
}


//STATES:
void APlayFieldMain::PlayerTurnEnter()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("PLAYER TURN START")));

	TotalTurnCount++;
}
void APlayFieldMain::CursorUpdate()
{
	this->CursorPosSet();
	//RESET SCALE
	for (int i = 0; i < cellSize; ++i)
	{
		for (int j = 0; j < cellSize; ++j)
		{
			if (cells[i][j].CellData != 0) {
				arrayOfGridObjects[cells[i][j].cellGraphicID]->SetActorScale3D(FVector(1, 1, 1));
				continue;
			}
			arrayOfGridObjects[cells[i][j].cellGraphicID]->SetActorScale3D(FVector(0.25, 0.25, 0.25));
			//RESET TEMP DATA
			cells[i][j].TempData = cells[i][j].CellData;
			//cells[i][j].TempData = 0;
		}
	}
	//HIGHLIGHT CLOSEST TO CURSOR
	FVector2D selectGridVec = this->GetClosestCellToCursor();
	int cellGfxID = cells[selectGridVec.IntPoint().X][selectGridVec.IntPoint().Y].cellGraphicID;
	arrayOfGridObjects[cellGfxID]->SetActorScale3D(FVector(1.25, 1.25, 1.25));
}
int APlayFieldMain::SpawnAHero(FVector location, bool isHero, int cellGfxID)
{
	workingVectorA = location;
	workingVectorB = location;
	workingVectorA.Z += 1000;

	UWorld* const World = GetWorld();
	AActor* spawnedActor = nullptr;
	if (isHero)
	{
		spawnedActor = World->SpawnActor<AActor>(basicWarriorTemplate, workingVectorA, FRotator().ZeroRotator);
	}
	else
	{
		spawnedActor = World->SpawnActor<AActor>(basicWarriorEnemyTemplate, workingVectorA, FRotator().ZeroRotator);
	}
	if (cellGfxID != -1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("SHOULD SPAWN IN ANOTHER LOC INSTEAD %i"), cellGfxID));
		//use an old array location instead of adding!
		arrayOfWarriorModels[cellGfxID] = spawnedActor;
		return cellGfxID;
	}
	
	arrayOfWarriorModels.Add(spawnedActor);
	return arrayOfWarriorModels.Num()-1;
}
void APlayFieldMain::PlayerUpdate()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("PLAYER TURN UPDATE")));


	if (TotalTurnCount >= MaxNumberOfTurns)
	{
		//TOOD: NOTIFY PLAYER VICTORY AND END THE GAME.
		//Then RETURN
	}

	//MOVE THE CURSOR TO THE MOUSE
	this->CursorPosSet();

	//RESET SCALE
	for (int i = 0; i < cellSize; ++i)
	{
		for (int j = 0; j < cellSize; ++j)
		{
			if (cells[i][j].CellData != 0) {
				arrayOfGridObjects[cells[i][j].cellGraphicID]->SetActorScale3D(FVector(1, 1, 1));
				continue;
			}
			arrayOfGridObjects[cells[i][j].cellGraphicID]->SetActorScale3D(FVector(0.25, 0.25, 0.25));
			//RESET TEMP DATA
			cells[i][j].TempData = cells[i][j].CellData;
			//cells[i][j].TempData = 0;
		}
	}
	//HIGHLIGHT CLOSEST TO CURSOR
	FVector2D selectGridVec = this->GetClosestCellToCursor();
	int cellGfxID = cells[selectGridVec.IntPoint().X][selectGridVec.IntPoint().Y].cellGraphicID;
	arrayOfGridObjects[cellGfxID]->SetActorScale3D(FVector(1.25, 1.25, 1.25));

	//MOUSE DOWN:
	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);
	FKey key = EKeys::LeftMouseButton;
	bool down = OurPlayerController->IsInputKeyDown(key);

	//TODO: LIMIT ONCE UNTIL CURSOR MOVES
	bool endTurn = false;
	HeroLastCoordinate.Set(selectGridVec.IntPoint().X, selectGridVec.IntPoint().Y);
	if (down && !HeroLastCoordinate.IsEqual(PrevCoordinate))
	{
		if (cells[selectGridVec.IntPoint().X][selectGridVec.IntPoint().Y].CellData == 0)
		{
			totalTurnNumber++;
			limitOnce = true;
			cells[selectGridVec.IntPoint().X][selectGridVec.IntPoint().Y].CellData = heroPiece;
			cells[selectGridVec.IntPoint().X][selectGridVec.IntPoint().Y].hp = 8;
			cells[selectGridVec.IntPoint().X][selectGridVec.IntPoint().Y].atk = 1;
			cells[selectGridVec.IntPoint().X][selectGridVec.IntPoint().Y].TempData = 99;
			PrevCoordinate.Set(HeroLastCoordinate.X, HeroLastCoordinate.Y);
			endTurn = true;
		}
	}
	if (endTurn)
	{
		StateMachine->SetState(State::dropSkyPlayer);
	}
}
void APlayFieldMain::SpawnHeroOnSkyDrop()
{
	//Get location of grid from last player coordinate
	int celGfxID = cells[HeroLastCoordinate.X][HeroLastCoordinate.Y].cellGraphicID;
	FVector loc = arrayOfGridObjects[celGfxID]->GetActorLocation();
	int nID = SpawnAHero(loc,true, cells[HeroLastCoordinate.X][HeroLastCoordinate.Y].unitGraphicID);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("SPAWN GFX ID %i"),nID));
	cells[HeroLastCoordinate.X][HeroLastCoordinate.Y].unitGraphicID = nID;
	// 
	//on drop start
}
void APlayFieldMain::DropFromSkyPlayerUnit()
{
	bool skipAttackAnimations = GetSkipButton();
	if (skipAttackAnimations)StateMachine->SetTimeInState(1.0f);

	FVector currentVec = FMath::Lerp(workingVectorA, workingVectorB, StateMachine->GetTimeInState());
	int unitID = cells[HeroLastCoordinate.X][HeroLastCoordinate.Y].unitGraphicID;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("%iSPAWN GFX ID %i,%i"), unitID, HeroLastCoordinate.X, HeroLastCoordinate.Y));

	arrayOfWarriorModels[unitID]->SetActorLocation(currentVec);
	if (StateMachine->GetTimeInState() >= 1.0f)
	{
		arrayOfWarriorModels[unitID]->SetActorLocation(workingVectorB);
		StateMachine->SetState(State::heroAttackPhase);
	}
	
}
void APlayFieldMain::EnemyUpdate()
{
	//GET RID OF THIS! It's for testing until a turn system is added
	if (StateMachine->GetTimeInState() >= 0.5f)
	{
		PlaceEnemyByPattern();
	
		StateMachine->SetState(State::dropSkyEnemy);
	}
	
}
void APlayFieldMain::HerosAttack()
{
	//FIRE ONCE
	//TODO YIELD TO A COROUTINE OR SUBSTATE THAT ANIMATES THE ATTACK PHASES
	//StartCoroutine(ProccessAttacks(heroPiece, enemyPiece, allSpawnedEnemies, false, State.enemyTurn));
	ProccessAttacksReset(heroPiece);
	
	//StateMachine->SetState(State::enemyTurn);
}
void APlayFieldMain::HerosAttackUpdate()
{
	ProccessAttacks(heroPiece,enemyPiece, (int)State::enemyTurn, 2.0f*dt);
}

void APlayFieldMain::SpawnEnemyOnSkyDrop()
{
	//Get location of grid from last player coordinate
	int celGfxID = cells[EnemyLastCoordinate.X][EnemyLastCoordinate.Y].cellGraphicID;
	FVector loc = arrayOfGridObjects[celGfxID]->GetActorLocation();
	int nID = SpawnAHero(loc, false, cells[EnemyLastCoordinate.X][EnemyLastCoordinate.Y].unitGraphicID);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("SPAWN GFX ID %i"), nID));
	cells[EnemyLastCoordinate.X][EnemyLastCoordinate.Y].unitGraphicID = nID;
}
void APlayFieldMain::DropFromSkyEnemyUnit()
{
	bool skipAttackAnimations = GetSkipButton();
	if (skipAttackAnimations)StateMachine->SetTimeInState(1.0f);
	FVector currentVec = FMath::Lerp(workingVectorA, workingVectorB, StateMachine->GetTimeInState());
	int unitID = cells[EnemyLastCoordinate.X][EnemyLastCoordinate.Y].unitGraphicID;
	arrayOfWarriorModels[unitID]->SetActorLocation(currentVec);
	
	if (StateMachine->GetTimeInState() >= 1.0f)
	{
		arrayOfWarriorModels[unitID]->SetActorLocation(workingVectorB);

		StateMachine->SetState(State::enemyAttackPhase);
	}
}

void APlayFieldMain::EnemiesAttack()
{
	//FIRE ONCE
	ProccessAttacksReset(enemyPiece);
	
}
void APlayFieldMain::EnemiesAttackUpdate()
{
	ProccessAttacks(enemyPiece,heroPiece, (int)State::playerTurn, 2.0f * dt);

}
void APlayFieldMain::EmptyFunction()
{
	//DO NOTHING
}
void APlayFieldMain::ProccessAttacksReset(int idOfAttackerTeam)
{
	AttackSubState = 0;
	AttackListIndex = 0;
	attackStateTimer = 0.0f;
	CurrentAttackerFlatIndex = 0;
	PopulatedCells.Empty();
	for (int i = 0; i < cellSize; ++i)
	{
		for (int j = 0; j < cellSize; ++j)
		{
			if (cells[i][j].CellData == idOfAttackerTeam)
			{
				PopulatedCells.Add(CoordinatePairs(i, j));
			}
		}
	}
}

bool APlayFieldMain::GetSkipButton()
{
	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);
	FKey key = EKeys::RightMouseButton;
	return OurPlayerController->IsInputKeyDown(key);
}
void APlayFieldMain::SquashAnimation(AActor* attacker, FVector initScale, FVector maxSize, float progress)
{
	progress = progress * 2.0f;
	float input = progress;
	FVector result = initScale;
	if (progress > 1.0f)
	{
		//scale back down to init size
		input = progress - 1.0f;
		result = FMath::Lerp(maxSize,initScale, input);

	}
	else
	{
		//scale up to squash size
		result = FMath::Lerp(initScale, maxSize, input);
	}
	attacker->SetActorScale3D(result);
}
void APlayFieldMain::ProccessAttacks(int32 idOfAttackerTeam, int32 idOfDefenderTeam, int stateOnEnd,float DeltaTime)
{
	const int start = 0;
	const int lookAtTarget = 1;
	const int animateAttack = 2;
	const int animateHurt = 3;
	const int incrementAttackIndex = 4;
	const int cleanupRotation = 5;
	const int incrementTiles = 6;

	int x = PopulatedCells[CurrentAttackerFlatIndex].X;
	int y = PopulatedCells[CurrentAttackerFlatIndex].Y;
	int unitGfxID_Attacker = 0;
	int unitGfxID_Defender = 0;
	CoordinatePairs currentAttackTarget = CoordinatePairs(0,0);
	FRotator StartRot = FRotator().ZeroRotator;
	FRotator DummyRot = FRotator().ZeroRotator;



	bool skipAttackAnimations = GetSkipButton();
	if (skipAttackAnimations)
	{
		//skip animations
		attackStateTimer = 1.0f;
	}

	switch (AttackSubState)
	{
		case start:
			attackStateTimer = 0.0f;
			unitGfxID_Attacker = cells[x][y].unitGraphicID;
			StartRot = arrayOfWarriorModels[unitGfxID_Attacker]->GetActorRotation();
			AttackSubState = lookAtTarget;
		break;
		case lookAtTarget:
			attackStateTimer += DeltaTime;
			if (AttackListIndex > 3) //size of array minus 1
			{
				//CLEANUP ROTATION, AND INCREMENT TO NEXT ACTOR AFTER THAT
				attackStateTimer = 0.0f;
				unitGfxID_Attacker = cells[x][y].unitGraphicID;
				StartRot = arrayOfWarriorModels[unitGfxID_Attacker]->GetActorRotation();
				AttackSubState = cleanupRotation; // ran out of attack targets on this unit. Bring in the next unit.
				return;
			}
			//TODO: Look at target coordinate at current cell + attackAngles [current attack angle]
			unitGfxID_Attacker = cells[x][y].unitGraphicID;
			currentAttackTarget = attack_list_normal[AttackListIndex];
			currentAttackTarget.X += x;
			currentAttackTarget.Y += y;

			unitGfxID_Defender = cells[currentAttackTarget.X][currentAttackTarget.Y].unitGraphicID;
		
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Black, FString::Printf(TEXT("(%i) <-UNIT ->Attack Index->(%i) LOOKING AT TARGET: %i,%i "), unitGfxID_Attacker, AttackListIndex, attack_list_normal[AttackListIndex].X, attack_list_normal[AttackListIndex].Y));

			//BOUNDS CHECK
			if (currentAttackTarget.X < 0)
			{
				currentAttackTarget.X = 0;
			}
			if (currentAttackTarget.Y < 0)
			{
				currentAttackTarget.Y = 0;
			}
			if (currentAttackTarget.Y > cellSize)
			{
				currentAttackTarget.Y = cellSize;
			}
			if (currentAttackTarget.X > cellSize)
			{
				currentAttackTarget.X = cellSize;
			}
			//-----------------------------------------
			
			if (cells[currentAttackTarget.X][currentAttackTarget.Y].CellData == idOfDefenderTeam)
			{

				FVector playerPos = arrayOfWarriorModels[unitGfxID_Attacker]->GetActorLocation();
				FVector enemyPos = arrayOfWarriorModels[unitGfxID_Defender]->GetActorLocation();
				//FRotator StartRot = FRotator().ZeroRotator;
				FRotator TargetRot = FRotationMatrix::MakeFromX(playerPos - enemyPos).Rotator();
				attackStateTimer += 2.0f* DeltaTime;
				if (attackStateTimer > 1.0f)attackStateTimer = 1.0f;


				StartRot = arrayOfWarriorModels[unitGfxID_Attacker]->GetActorRotation();

				FRotator NewRotation = FQuat::Slerp(StartRot.Quaternion(), TargetRot.Quaternion(), DeltaTime).Rotator();

				arrayOfWarriorModels[unitGfxID_Attacker]->SetActorRotation(NewRotation);
				if (attackStateTimer >= 1.0f)
				{
					arrayOfWarriorModels[unitGfxID_Attacker]->SetActorRotation(TargetRot.Quaternion());
					//Finished rotating. move to attack animation state
					attackStateTimer = 0.0f;
					AttackSubState = animateAttack;
					return;
				}
			}
			else
			{
				AttackListIndex++;
				attackStateTimer = 0.0f;
			}

			break;
		case animateAttack:
			attackStateTimer += 2 * DeltaTime;
			//animate squash
			unitGfxID_Attacker = cells[x][y].unitGraphicID; //must be assigned in every state that uses it!
			SquashAnimation(arrayOfWarriorModels[unitGfxID_Attacker],FVector(1,1,1),FVector(0.65f,0.65f,1.35f), attackStateTimer);

			//Animate Attack
			//TODO: ANIMATE ATTACK SQUASH AND STRETCH
			if (attackStateTimer >= 1.0f)
			{

				attackStateTimer = 0.0f;
				AttackSubState = animateHurt;
			}
			break;
		case animateHurt:

			attackStateTimer += 2 * DeltaTime;

			//animate squash
			currentAttackTarget = attack_list_normal[AttackListIndex];
			currentAttackTarget.X += x;
			currentAttackTarget.Y += y;
			unitGfxID_Defender = cells[currentAttackTarget.X][currentAttackTarget.Y].unitGraphicID;
			SquashAnimation(arrayOfWarriorModels[unitGfxID_Defender], FVector(1, 1, 1), FVector(1.35f, 1.35f, 0.65f), attackStateTimer);

			if (attackStateTimer >= 1.0f)
			{
				//HURT THE ACTOR
				cells[currentAttackTarget.X][currentAttackTarget.Y].hp -= cells[x][y].atk;
				//TODO IF DEAD, DESTROY AND SPAWN EXPLOSION
				if (cells[currentAttackTarget.X][currentAttackTarget.Y].hp <= 0)
				{
					//arrayOfWarriorModels[unitGfxID_Defender]->SetActorHiddenInGame(true);
					arrayOfWarriorModels[unitGfxID_Defender]->Destroy();
					cells[currentAttackTarget.X][currentAttackTarget.Y].CellData = neutralPiece;
					cells[currentAttackTarget.X][currentAttackTarget.Y].TempData = neutralPiece;
				}

				attackStateTimer = 0.0f;
				AttackSubState = incrementAttackIndex;
			}

			
			
			break;
		case incrementAttackIndex:

			//increment current attackAngles[x] x value.
			//then set to state 1 if not at the end of the attackAngles

			AttackListIndex++;
			if (AttackListIndex > 4)
			{
				//done attacking with this unit, go to the next unit
				attackStateTimer = 0.0f;
				unitGfxID_Attacker = cells[x][y].unitGraphicID; //must be assigned in every state that uses it!
				StartRot = arrayOfWarriorModels[unitGfxID_Attacker]->GetActorRotation();
				AttackSubState = cleanupRotation;
				return;
				
			}
			else
			{
				// this unit may have more targets to attack
				attackStateTimer = 0.0f;
				AttackSubState = start;
			}

			break;
		case cleanupRotation:
			
			unitGfxID_Attacker = cells[x][y].unitGraphicID; //must be assigned in every state that uses it!

			attackStateTimer += 1.0f * DeltaTime;
			//if (attackStateTimer > 1.0f)attackStateTimer = 1.0f;
			DummyRot = FQuat::Slerp(StartRot.Quaternion(), FRotator().ZeroRotator.Quaternion(), attackStateTimer).Rotator();

			arrayOfWarriorModels[unitGfxID_Attacker]->SetActorRotation(DummyRot);


			if (attackStateTimer >= 1.0f)
			{
				AttackSubState = incrementTiles;
			}

			break;
		case incrementTiles:
			//move on to the next unit, and set state to case 0.
			//if at the end of the board, set main state to stateOnEnd and stop proccessing attacks.
			
			attackStateTimer = 0.0f;
			CurrentAttackerFlatIndex++;
			AttackListIndex = 0;
			
			if (CurrentAttackerFlatIndex >= PopulatedCells.Num())
			{
				StateMachine->SetState(stateOnEnd);
				return;
			}
			AttackSubState = start;
			
			break;
	}
}


