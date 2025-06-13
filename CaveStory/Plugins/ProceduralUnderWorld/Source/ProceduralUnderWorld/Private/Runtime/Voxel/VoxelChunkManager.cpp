// Fill out your copyright notice in the Description page of Project Settings.


#include "Runtime/Voxel/VoxelChunkManager.h"
#include "Runtime/Voxel/VoxelChunkActor.h"
AVoxelChunkManager::AVoxelChunkManager()
{
	PrimaryActorTick.bCanEverTick = false;
	if (!ChunkActorClass)
	{
		ChunkActorClass = AVoxelChunkActor::StaticClass();
	}
}

void AVoxelChunkManager::BeginPlay()
{
	Super::BeginPlay();
	GenerateChunks();
}

void AVoxelChunkManager::GenerateChunks()
{
	if (!ChunkActorClass) return;

	const float ChunkWorldSize = ChunkSize * VoxelSize;

	for (int32 z = 0; z < ChunkCountZ; ++z)
	{
		for (int32 y = 0; y < ChunkCountY; ++y)
		{
			for (int32 x = 0; x < ChunkCountX; ++x)
			{
				FVector Location = GetActorLocation() + FVector(x, y, z) * ChunkWorldSize;
				FTransform SpawnTransform(Location);

				AVoxelChunkActor* Chunk = GetWorld()->SpawnActor<AVoxelChunkActor>(
					ChunkActorClass,
					SpawnTransform
				);
				if (Chunk)
				{
					UE_LOG(LogTemp, Display, TEXT("SetChunkConfig"));
					Chunk->SetChunkConfig(ChunkSize, VoxelSize);
					Chunk->InitializeChunk();
					SpawnedChunks.Add(Chunk);
				}
			}
		}
	}
}