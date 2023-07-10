// Copyright Brian Allanson, Ackk Studios

#include "Engine/Canvas.h"
#include "HudSrpg.h"

void AHudSrpg::DrawHUD()
{
	Super::DrawHUD();
	
	/*FText SomeText = "Mot...";
	FLinearColor TheFontColour = FLinearColor(1.0f, 1.0f, 1.0f);
	FCanvasTextItem NewText(FVector2D(12,12 ), SomeText, TheFont, TheFontColour);

	//Text Scale
	NewText.Scale.Set(1, 1);

	//Draw
	Canvas->DrawItem(NewText);*/

	// Font'/Game/Fonts/CompositeVerdana.CompositeVerdana'
	UObject* obj_ptr = StaticLoadObject(UFont::StaticClass(), NULL,
		TEXT("/Game/Fonts/CompositeVerdana"));
	UFont* font_ptr = Cast<UFont>(obj_ptr);
	this->DrawText(TEXT("Todo: Modify HudSrpg.cpp to take actor vectors and lives and draw like this to screen!"), FColor::White, 100.0f, 100.0f, font_ptr, 1);

}
