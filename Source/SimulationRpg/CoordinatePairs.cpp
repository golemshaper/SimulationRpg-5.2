// Copyright Brian Allanson, Ackk Studios


#include "CoordinatePairs.h"

CoordinatePairs::CoordinatePairs()
{
	Set(0, 0);
}
CoordinatePairs::CoordinatePairs(int x, int y)
{
	X = x;
	Y = y;
}
CoordinatePairs::~CoordinatePairs()
{
}

void CoordinatePairs::Set(int x, int y)
{
	X = x;
	Y = y;
}
bool CoordinatePairs::IsEqual(CoordinatePairs checkMe)
{
	if (X == checkMe.X && Y == checkMe.Y)
	{
		return true;
	}
	return false;
}

