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
	void SetChunkConfig(int32 InChunkSize, float InVoxelSize);
	void InitializeChunk();
	
private:
	//void GenerateVoxelChunk();

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProcMeshComp;

	UPROPERTY(EditAnywhere, Category = "Voxel")
	int32 ChunkSize = 64; // 16x16x16

	UPROPERTY(EditAnywhere, Category = "Voxel")
	float VoxelSize = 50.0f;

	// 밀도 필드 캐시
	TArray<float> DensityField;

	// 밀도 값 계산 함수 (Perlin 등으로 교체 가능)
	//float GetDensity(const FVector& WorldPosition) const;
private:
	void GenerateScalarField(TArray<float>& OutField);
	float GetScalar(const TArray<float>& Field, int32 X, int32 Y, int32 Z) const;
	int32 ScalarIndex(int32 X, int32 Y, int32 Z) const;
};
