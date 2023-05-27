// Copyright Brian Allanson, Ackk Studios


#include "ParseTargetPatternFile.h"
#include "Misc/FileHelper.h" 
#include "HAL/PlatformFilemanager.h"
#include "Containers/UnrealString.h"
#include "Math/UnrealMathUtility.h"
/*
* 
* https://forums.unrealengine.com/t/built-in-asset-type-for-plain-text/272593/2
* 
* 
* https://youtu.be/YcTa_wMjCzY
*/
ParseTargetPatternFile::ParseTargetPatternFile()
{
	data.Init(firstElement,1);
}

ParseTargetPatternFile::~ParseTargetPatternFile()
{

}

FString ParseTargetPatternFile::GetPatternData()
{
	int sel = FMath::RandRange(0, data.Num()-1);

	return data[sel].DataString;
}

FString ParseTargetPatternFile::ReadStringFromFile(FString FilePath, bool& bOutSuccess, FString& OutInfoMessage)
{

	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Failed to read file. The file might not exist: %s"), *FilePath);
		return "";
	}
	FString OutputString = "";
	
	if (!FFileHelper::LoadFileToString(OutputString, *FilePath))
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Failed to read file. The file could not be read %s"), *FilePath);
		return "";
	}


	bOutSuccess = true;
	OutInfoMessage = FString::Printf(TEXT("File read was a success - %s"), *FilePath);
	return OutputString;
}

void ParseTargetPatternFile::ParseDataFromFile(FString FilePath, bool& bOutSuccess)
{
	FString infoMessage;
	bool  result;
	FString rawData = ReadStringFromFile(FilePath,result,infoMessage);
	if (result == false)
	{
		//TODO: Log ERROR!
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("ABORTING PARSE OPERATION")));

		return;
	}
	//PARSE BELLOW:
	auto arrayOfChars = rawData.GetCharArray();
	
	FString sb = "";
	const int32 openBracket = 0;
	const int32 buildData = 2;
	int32 mode = 0;

	for (int i = 0; i < rawData.Len(); ++i)
	{
		auto c = arrayOfChars[i];

		if (c == '[')
		{
			if (data.Num() > 0)
			{
				//Main data goes here after a new group is started, the old one is finished. if first run, it won't do anything
				data[data.Num() - 1].DataString = sb;
				sb = "";
			}
			mode = openBracket;
			continue;
		}
		else if (c == ']')
		{
			data.Add(PatternSourceDataHolder(sb));
			sb = "";
			mode = buildData;
			continue;
		}
		switch (mode)
		{
		case openBracket:
			sb.AppendChar(c);
			break;
		case buildData:
			sb.AppendChar(c);
			break;
		}
	}
	data[0] = data[1]; //override placeholder until I can figure out how to remove it.
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("PARSE OPERATION SUCCESS")));

}
