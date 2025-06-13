// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MCUtils.h"
#include "Core/MarchingCubesTable.h"
inline int32 MCUtils::ComputeCaseIndex(const float CornerValues[8])
{
	int32 CaseIndex = 0;
	for (int32 i = 0; i < 8; ++i)
	{
		if (CornerValues[i] < 0.0f) // 음수 → 내부
		{
			CaseIndex |= (1 << i);
		}
	}
	return CaseIndex;
}
inline void MCUtils::GetTriangleEdges(int32 CaseIndex, TArray<int32>& OutEdges)
{
	OutEdges.Reset();
	for (int32 i = 0; i < 16; ++i)
	{
		int32 Edge = MarchingCubesTable[CaseIndex][i];
		if (Edge == -1)
			break;
		OutEdges.Add(Edge);
	}
}