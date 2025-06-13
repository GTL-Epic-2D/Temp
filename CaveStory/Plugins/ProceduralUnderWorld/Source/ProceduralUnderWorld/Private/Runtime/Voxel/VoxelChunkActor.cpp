// Fill out your copyright notice in the Description page of Project Settings.


#include "Runtime/Voxel/VoxelChunkActor.h"
#include "Core/MCUtils.h"
AVoxelChunkActor::AVoxelChunkActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    
    ProcMeshComp = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProcMesh"));
    SetRootComponent(ProcMeshComp);
    ProcMeshComp->bUseAsyncCooking = true;
    

}
void AVoxelChunkActor::SetChunkConfig(int32 InChunkSize, float InVoxelSize, FVector InOrigin)
{
	ChunkSize = InChunkSize;
	VoxelSize = InVoxelSize;
	ChunkOrigin = InOrigin;
}

// Called when the game starts or when spawned

void AVoxelChunkActor::BeginPlay()
{
    Super::BeginPlay();
}
void AVoxelChunkActor::GenerateMeshForLOD(int32 LOD)
{
	const int32 LODSize = ChunkSize >> LOD;
	if (LODSize <= 1)
		return;
	const float LODVoxelSize = VoxelSize * (ChunkSize / (float)LODSize);
	const int32 BaseSize = ChunkSize;
	const int32 BaseNumPoints = (BaseSize + 1) * (BaseSize + 1) * (BaseSize + 1);

	// LOD0 기준 스칼라 필드 생성
	if (!bBaseFieldInitialized)
	{
		BaseField.SetNum(BaseNumPoints);
		GenerateScalarField(BaseField, BaseSize);
		bBaseFieldInitialized = true;
	}

	TArray<float> ScalarField;
	GenerateScalarFieldFromBase(ScalarField, LODSize, BaseSize);

	TArray<FVector> Vertices;
	TArray<int32> Indices;

	const FVector CubeCornerOffsets[8] = {
		FVector(0,0,0), FVector(1,0,0), FVector(1,1,0), FVector(0,1,0),
		FVector(0,0,1), FVector(1,0,1), FVector(1,1,1), FVector(0,1,1)
	};
	const int32 EdgeToCorner[12][2] = {
		{0,1}, {1,2}, {2,3}, {3,0}, {4,5}, {5,6}, {6,7}, {7,4}, {0,4}, {1,5}, {2,6}, {3,7}
	};

	for (int32 z = 0; z < LODSize; ++z)
		for (int32 y = 0; y < LODSize; ++y)
			for (int32 x = 0; x < LODSize; ++x)
			{
				FVector CornerPositions[8];
				float CornerValues[8];

				for (int32 i = 0; i < 8; ++i)
				{
					const FVector& Offset = CubeCornerOffsets[i];
					int32 vx = x + Offset.X;
					int32 vy = y + Offset.Y;
					int32 vz = z + Offset.Z;

					CornerPositions[i] = FVector(vx, vy, vz) * LODVoxelSize;
					CornerValues[i] = GetScalar(ScalarField, vx, vy, vz, LODSize);
				}

				int32 CaseIndex = MCUtils::ComputeCaseIndex(CornerValues);
				TArray<int32> EdgeIndices;
				MCUtils::GetTriangleEdges(CaseIndex, EdgeIndices);

				for (int32 i = 0; i + 2 < EdgeIndices.Num(); i += 3)
				{
					FVector TriVerts[3];
					for (int32 j = 0; j < 3; ++j)
					{
						int32 Edge = EdgeIndices[i + j];
						int32 C0 = EdgeToCorner[Edge][0];
						int32 C1 = EdgeToCorner[Edge][1];

						float V0 = CornerValues[C0];
						float V1 = CornerValues[C1];
						float T = (0 - V0) / (V1 - V0 + KINDA_SMALL_NUMBER);
						T = FMath::Clamp(T, 0.f, 1.f);
						TriVerts[j] = FMath::Lerp(CornerPositions[C0], CornerPositions[C1], T);
					}
					int32 StartIndex = Vertices.Num();
					Vertices.Append(TriVerts, 3);
					Indices.Add(StartIndex);
					Indices.Add(StartIndex + 1);
					Indices.Add(StartIndex + 2);
				}
			}

	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
	TArray<FColor> Colors;

	Normals.SetNum(Vertices.Num());
	UVs.SetNum(Vertices.Num());
	Tangents.SetNum(Vertices.Num());

	for (int32 i = 0; i < Indices.Num(); i += 3)
	{
		const FVector& V0 = Vertices[Indices[i]];
		const FVector& V1 = Vertices[Indices[i + 1]];
		const FVector& V2 = Vertices[Indices[i + 2]];
		FVector Normal = FVector::CrossProduct(V2 - V0, V1 - V0).GetSafeNormal();
		Normals[Indices[i]] = Normals[Indices[i + 1]] = Normals[Indices[i + 2]] = Normal;
	}

	for (int32 i = 0; i < Vertices.Num(); ++i)
	{
		UVs[i] = FVector2D(Vertices[i].X, Vertices[i].Y);
		FVector Tangent = FVector::CrossProduct(FVector(0, 0, 1), Normals[i]);
		if (Tangent.IsNearlyZero()) Tangent = FVector(1, 0, 0);
		Tangents[i] = FProcMeshTangent(Tangent, false);
		Colors.Add(FColor::White);
	}

	ProcMeshComp->CreateMeshSection(0, Vertices, Indices, Normals, UVs, Colors, Tangents, false);
	ProcMeshComp->bUseComplexAsSimpleCollision = false;
	ProcMeshComp->bRenderCustomDepth = false;
}

void AVoxelChunkActor::GenerateScalarField(TArray<float>& OutField, int32 Size)
{
	for (int32 z = 0; z <= Size; ++z)
	{
		for (int32 y = 0; y <= Size; ++y)
		{
			for (int32 x = 0; x <= Size; ++x)
			{
				float S = FMath::PerlinNoise3D((FVector(x, y, z) * VoxelSize + ChunkOrigin) * 0.001f);
				OutField[ScalarIndex(x, y, z, Size)] = S;
			}
		}
	}
}
void AVoxelChunkActor::GenerateScalarFieldFromBase(TArray<float>& OutField,int32 LODSize,int32 BaseSize)
{
	const int32 Count = (LODSize + 1) * (LODSize + 1) * (LODSize + 1);
	OutField.SetNum(Count);

	for (int32 z = 0; z <= LODSize; ++z)
		for (int32 y = 0; y <= LODSize; ++y)
			for (int32 x = 0; x <= LODSize; ++x)
			{
				float fx = x * (BaseSize / (float)LODSize);
				float fy = y * (BaseSize / (float)LODSize);
				float fz = z * (BaseSize / (float)LODSize);
				int32 ix = FMath::Clamp(FMath::RoundToInt(fx), 0, BaseSize);
				int32 iy = FMath::Clamp(FMath::RoundToInt(fy), 0, BaseSize);
				int32 iz = FMath::Clamp(FMath::RoundToInt(fz), 0, BaseSize);

				float Sample = BaseField[ScalarIndex(ix, iy, iz, BaseSize)];
				OutField[ScalarIndex(x, y, z, LODSize)] = Sample;
			}
}


float AVoxelChunkActor::GetScalar(const TArray<float>& Field, int32 X, int32 Y, int32 Z, int32 Size) const
{
    return Field[ScalarIndex(X, Y, Z, Size)];
}

int32 AVoxelChunkActor::ScalarIndex(int32 X, int32 Y, int32 Z, int32 Size) const
{
    return X + Y * (Size + 1) + Z * (Size + 1) * (Size + 1);
}

void AVoxelChunkActor::ApplyLOD(int32 NewLODIndex)
{
	if (NewLODIndex == CurrentLODIndex || !ProcMeshComp)
		return;

	// 이전 Section 제거
	ProcMeshComp->ClearAllMeshSections();

	// 현재 LOD Section 다시 생성
	GenerateMeshForLOD(NewLODIndex);

	CurrentLODIndex = NewLODIndex;
}
