// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "VoxelChunkActor.generated.h"

UCLASS()
class PROCEDURALUNDERWORLD_API AVoxelChunkActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVoxelChunkActor();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;public:
	void SetChunkConfig(int32 InChunkSize, float InVoxelSize, FVector InOrigin);
	void GenerateMeshForLOD(int32 LOD);

private:
	//void GenerateVoxelChunk();

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProcMeshComp;

	UPROPERTY(EditAnywhere, Category = "Voxel")
	int32 ChunkSize = 64; // 16x16x16

	UPROPERTY(EditAnywhere, Category = "Voxel")
	float VoxelSize = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Voxel")
	FVector ChunkOrigin = FVector::ZeroVector; // 청크의 시작 위치

	// 밀도 필드 캐시
	TArray<float> DensityField;

	// 밀도 값 계산 함수 (Perlin 등으로 교체 가능)
	//float GetDensity(const FVector& WorldPosition) const;
private:
	void GenerateScalarField(TArray<float>& OutField, int32 Size);

	void GenerateScalarFieldFromBase(TArray<float>& OutField,int32 LODSize,int32 BaseSize);
	float GetScalar(const TArray<float>& Field, int32 X, int32 Y, int32 Z, int32 Size) const;
	int32 ScalarIndex(int32 X, int32 Y, int32 Z, int32 Size) const;

public:
	void ApplyLOD(int32 NewLODIndex);

private:
	UPROPERTY()
	int32 CurrentLODIndex = -1;

	UPROPERTY(EditAnywhere, Category = "LOD")
	int32 MaxLODLevel = 2; // 0,1,2 지원한다고 가정
	//LOD0일때 기본 필드
	bool bBaseFieldInitialized = false;
	TArray<float> BaseField;
};
