// Copyright Brian Allanson, Ackk Studios


#include "BPAStateMachineUnreal.h"

UBPAStateMachineUnreal::FStateSMDelegate UBPAStateMachineUnreal::GetStateUpdateDelegate(int state)
{
	return updateStateMap[state];
}

void UBPAStateMachineUnreal::SetState(int state)
{
	if (currentState == state)
	{
		return;
	}
	if (exitStateMap.Contains(currentState) == true)
	{
		exitStateMap[currentState].Broadcast();
	}

	currentState = state;
	timeInState = 0.0f;

	if (enterStateMap.Contains(state) == false)
	{
		return;
	}
	//element found;
	enterStateMap[state].Broadcast();
}

int UBPAStateMachineUnreal::GetState()
{
	return currentState;
}

float UBPAStateMachineUnreal::GetTimeInState()
{
	return timeInState;
}

void UBPAStateMachineUnreal::SetTimeInState(float nTime)
{
	timeInState = nTime;
}

void UBPAStateMachineUnreal::UpdateTick(float DeltaTime)
{
	
	timeInState += timeMultiplier * DeltaTime;
	updateStateMap[currentState].Broadcast();
}

void UBPAStateMachineUnreal::MapState(int state, FStateSMDelegate updateDel)
{
	storedCopyOfAllDelegates.Add(updateDel);
	updateStateMap.Add(state, updateDel);
}

void UBPAStateMachineUnreal::MapState(int state, FStateSMDelegate updateDel, FStateSMDelegate enterDel)
{
	storedCopyOfAllDelegates.Add(updateDel);
	storedCopyOfAllDelegates.Add(enterDel);


	updateStateMap.Add(state, updateDel);
	enterStateMap.Add(state, enterDel);
}

void UBPAStateMachineUnreal::MapState(int state, FStateSMDelegate updateDel, FStateSMDelegate enterDel, FStateSMDelegate exitDel)
{

	storedCopyOfAllDelegates.Add(updateDel);
	storedCopyOfAllDelegates.Add(enterDel);
	storedCopyOfAllDelegates.Add(exitDel);


	updateStateMap.Add(state, updateDel);
	enterStateMap.Add(state, enterDel);
	exitStateMap.Add(state, exitDel);
}

void UBPAStateMachineUnreal::MapStateEnter(int state, FStateSMDelegate enterDel)
{
	storedCopyOfAllDelegates.Add(enterDel);

	enterStateMap.Add(state, enterDel);
}

void UBPAStateMachineUnreal::MapStateExit(int state, FStateSMDelegate exitDel)
{
	storedCopyOfAllDelegates.Add(exitDel);

	exitStateMap.Add(state, exitDel);
}

void UBPAStateMachineUnreal::UnbindAll()
{
	enterStateMap.Empty();
	updateStateMap.Empty();
	exitStateMap.Empty();
	storedCopyOfAllDelegates.Empty();
}
