// Copyright Brian Allanson, Ackk Studios

#pragma once

#include "CoreMinimal.h"
#include <map>
#include "UObject/NoExportTypes.h"
#include "BPAStateMachineUnreal.generated.h"

/**
 * 
 */
UCLASS()
class SIMULATIONRPG_API UBPAStateMachineUnreal : public UObject
{
	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStateSMDelegate);

	TMap<int, UBPAStateMachineUnreal::FStateSMDelegate> updateStateMap;
	TMap<int, UBPAStateMachineUnreal::FStateSMDelegate> enterStateMap;
	TMap<int, UBPAStateMachineUnreal::FStateSMDelegate> exitStateMap;
	
	//needed so the garbage collector doesn't yeet the states in the TMap
	UPROPERTY()
	TArray<UBPAStateMachineUnreal::FStateSMDelegate> storedCopyOfAllDelegates;
	//--


	int currentState;
	float timeInState;
	float timeMultiplier = 1.0f;
	//---
	void UpdateTick(float DeltaTime);
	void MapState(int state, FStateSMDelegate updateDel);
	void MapState(int state, FStateSMDelegate updateDel, FStateSMDelegate enterDel);
	void MapState(int state, FStateSMDelegate updateDel, FStateSMDelegate enterDel, FStateSMDelegate exitDel);
	void MapStateEnter(int state, FStateSMDelegate enterDel);
	void MapStateExit(int state, FStateSMDelegate exitDel);
	UBPAStateMachineUnreal::FStateSMDelegate GetStateUpdateDelegate(int state);
	void SetState(int state);
	int GetState();
	float GetTimeInState();
	void SetTimeInState(float nTime);
	void UnbindAll();
};
