// Copyright Brian Allanson, Ackk Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
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
};
