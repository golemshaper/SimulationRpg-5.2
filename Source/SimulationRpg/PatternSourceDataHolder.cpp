// Copyright Brian Allanson, Ackk Studios


#include "PatternSourceDataHolder.h"

PatternSourceDataHolder::PatternSourceDataHolder()
{
	GroupName = "";
	DataString = "";
}
PatternSourceDataHolder::PatternSourceDataHolder(FString nGroupName)
{
	GroupName = nGroupName;
	DataString = "";
}
PatternSourceDataHolder::~PatternSourceDataHolder()
{
}

