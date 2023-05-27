// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"

class SIMULATIONRPG_API Cell
{
public:
	int CellData;
	int TempData=0;
	int unitGraphicID;
	int cellGraphicID;
	int hp;
	int atk;
	Cell();
	~Cell();
};
