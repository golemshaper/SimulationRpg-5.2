// Copyright Brian Allanson, Ackk Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HpDisplayUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class SIMULATIONRPG_API UHpDisplayUserWidget : public UUserWidget
{
	//https://en.wiki.imlint.org/How_to_create_c%2b%2b_widget
	GENERATED_BODY()
		virtual bool Initialize();
	//UPROPERTY(EditAnywhere, meta = (BindWidget))
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* HpDisplay;
	void DrawNumberAtPoint(int number, int x, int y);

};
