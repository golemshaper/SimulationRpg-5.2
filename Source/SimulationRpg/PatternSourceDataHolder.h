// Copyright Brian Allanson, Ackk Studios

#pragma once

#include "Containers/UnrealString.h"

/**
 * 
 */
class SIMULATIONRPG_API PatternSourceDataHolder
{
public:
	PatternSourceDataHolder();
	PatternSourceDataHolder(FString nGroupName);
	~PatternSourceDataHolder();

	 FString GroupName;
	 FString DataString;
};
