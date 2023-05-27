// Copyright Brian Allanson, Ackk Studios

#pragma once


/**
 * 
 */
class SIMULATIONRPG_API CoordinatePairs
{
public:
	CoordinatePairs();
	CoordinatePairs(int x, int y);
	~CoordinatePairs();
	int32 X;
	int32 Y;
	void Set(int x, int y);
	bool IsEqual(CoordinatePairs checkMe);
};
