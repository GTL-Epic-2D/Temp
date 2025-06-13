// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelChunkManager.generated.h"

class AVoxelChunkActor;

UCLASS()
class PROCEDURALUNDERWORLD_API AVoxelChunkManager : public AActor
{
	GENERATED_BODY()

public:
	AVoxelChunkManager();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Voxel")
	TSubclassOf<AVoxelChunkActor> ChunkActorClass;

	UPROPERTY(EditAnywhere, Category = "Voxel")
	int32 ChunkCountX = 4;

	UPROPERTY(EditAnywhere, Category = "Voxel")
	int32 ChunkCountY = 4;

	UPROPERTY(EditAnywhere, Category = "Voxel")
	int32 ChunkCountZ = 2;

	UPROPERTY(EditAnywhere, Category = "Voxel")
	int32 ChunkSize = 64;

	UPROPERTY(EditAnywhere, Category = "Voxel")
	float VoxelSize = 50.0f;

	UPROPERTY()
	TArray<AVoxelChunkActor*> SpawnedChunks;

	void GenerateChunks();
};