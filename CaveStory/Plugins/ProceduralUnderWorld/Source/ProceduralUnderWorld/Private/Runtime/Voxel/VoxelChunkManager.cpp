// Fill out your copyright notice in the Description page of Project Settings.


#include "Runtime/Voxel/VoxelChunkManager.h"
#include "Runtime/Voxel/VoxelChunkActor.h"
AVoxelChunkManager::AVoxelChunkManager()
{
	PrimaryActorTick.bCanEverTick = true;
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
void AVoxelChunkManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector CameraLocation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
	//UE_LOG(LogTemp, Display, TEXT("Num: %d"),SpawnedChunks.Num());
	for (AVoxelChunkActor* Chunk : SpawnedChunks)
	{
		if (!IsValid(Chunk)) continue;

		float Distance = FVector::Dist(CameraLocation, Chunk->GetActorLocation());
		//UE_LOG(LogTemp, Display, TEXT("Dist: %f"), Distance);
		int32 DesiredLOD = 0;
		if (Distance > 4000.f) DesiredLOD = 2;
		else if (Distance > 2000.f) DesiredLOD = 1;

		Chunk->ApplyLOD(DesiredLOD);
	}
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
					Chunk->SetChunkConfig(ChunkSize, VoxelSize,Location);
					//Chunk->InitializeChunk();
					SpawnedChunks.Add(Chunk);
				}
			}
		}
	}
}