// Copyright Brian Allanson, Ackk Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CoordinatePairs.h"
#include "HudSrpg.generated.h"

/**
 * 
 */
UCLASS()
class SIMULATIONRPG_API AHudSrpg : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;
	TArray<FVector> coordinates;
	TArray<int> hpArray;
	void ClearHp();
	void DrawHp(FVector pair, int nHp);
	FVector2D Project(FVector WorldLocation);
	//interface with editor:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Font)
		UFont* workingFont;
};
