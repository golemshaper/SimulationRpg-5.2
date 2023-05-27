// Copyright Brian Allanson, Ackk Studios

#pragma once

//#include "CoreMinimal.h"
#include "PatternSourceDataHolder.h"
#include "Containers/List.h"
/**
 * https://youtu.be/YcTa_wMjCzY
 */
class SIMULATIONRPG_API ParseTargetPatternFile
{
public:
	ParseTargetPatternFile();
	~ParseTargetPatternFile();
	FString GetPatternData();
	FString ReadStringFromFile(FString FilePath, bool& bOutSuccess, FString& OutInfoMessage);
	void ParseDataFromFile(FString FilePath, bool& bOutSuccess);
	PatternSourceDataHolder firstElement; 
	TArray<PatternSourceDataHolder> data;
	
};
