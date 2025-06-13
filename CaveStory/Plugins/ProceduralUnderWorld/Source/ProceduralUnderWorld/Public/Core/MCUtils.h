// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class PROCEDURALUNDERWORLD_API MCUtils
{
public:
	// 삼각형 구성을 위한 정적 유틸리티 함수
	static void GetTriangleEdges(int32 CaseIndex, TArray<int32>& OutEdges);

	// 코너 8개 밀도값으로 CaseIndex 구하기
	static int32 ComputeCaseIndex(const float CornerValues[8]);

	// 정점 위치 보간 등 추가 유틸 함수들 선언 가능
};
