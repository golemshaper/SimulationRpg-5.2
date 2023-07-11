// Copyright Brian Allanson, Ackk Studios

#include "Engine/Canvas.h"
#include "Kismet/GameplayStatics.h"
#include "HudSrpg.h"

void AHudSrpg::DrawHUD()
{
	Super::DrawHUD();
	
	// Font'/Game/Fonts/CompositeVerdana.CompositeVerdana'
	UObject* obj_ptr = StaticLoadObject(UFont::StaticClass(), NULL,
		TEXT("/Game/Fonts/CompositeVerdana"));
	UFont* font_ptr = Cast<UFont>(obj_ptr);
	this->DrawText(TEXT("Todo: Modify HudSrpg.cpp to take actor vectors and lives and draw like this to screen!"), FColor::White, 100.0f, 100.0f, font_ptr, 1);
	

	for (int i = 0; i < coordinates.Num(); ++i)
	{
		FVector2D ScreenLocation = Project(coordinates[i]);
		//this->DrawText(TEXT("X"), FColor::White, ScreenLocation.X, ScreenLocation.Y, font_ptr, 1.0f);
		this->DrawText(FString::FromInt(hpArray[i]), FColor::White, ScreenLocation.X, ScreenLocation.Y, font_ptr, 1.0f);
	}

}

void AHudSrpg::ClearHp()
{
	coordinates.Empty();
	hpArray.Empty();
}

void AHudSrpg::DrawHp(FVector pair, int nHp)
{
	coordinates.Add(pair);
	hpArray.Add(nHp);
}

FVector2D AHudSrpg::Project(FVector WorldLocation)
{
	const APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (OurPlayerController == nullptr)return FVector2D(0, 0);
	FVector2D ScreenLocation;
	OurPlayerController->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation);

	return ScreenLocation;
}