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

// Called when the game starts or when spawned

void AVoxelChunkActor::BeginPlay()
{
    Super::BeginPlay();

    const int32 NumVoxels = ChunkSize + 1;
    const int32 NumPoints = NumVoxels * NumVoxels * NumVoxels;
    
    TArray<float> ScalarField;
    ScalarField.SetNum(NumPoints);
    GenerateScalarField(ScalarField);

    TArray<FVector> Vertices;
    TArray<int32> Indices;

    const FVector CubeCornerOffsets[8] =
    {
        FVector(0,0,0), FVector(1,0,0), FVector(1,1,0), FVector(0,1,0),
        FVector(0,0,1), FVector(1,0,1), FVector(1,1,1), FVector(0,1,1)
    };

    const int32 EdgeToCorner[12][2] =
    {
        {0,1}, {1,2}, {2,3}, {3,0}, {4,5}, {5,6}, {6,7}, {7,4}, {0,4}, {1,5}, {2,6}, {3,7}
    };

    for (int32 z = 0; z < ChunkSize; ++z)
    {
        for (int32 y = 0; y < ChunkSize; ++y)
        {
            for (int32 x = 0; x < ChunkSize; ++x)
            {
                FVector CornerPositions[8];
                float CornerValues[8];
                for (int32 i = 0; i < 8; ++i)
                {
                    const FVector& Offset = CubeCornerOffsets[i];
                    int32 vx = x + Offset.X;
                    int32 vy = y + Offset.Y;
                    int32 vz = z + Offset.Z;

                    CornerPositions[i] = FVector(vx, vy, vz) * VoxelSize;
                    CornerValues[i] = GetScalar(ScalarField, vx, vy, vz);
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
                    Vertices.Add(TriVerts[0]);
                    Vertices.Add(TriVerts[1]); // Flip winding for correct normal
                    Vertices.Add(TriVerts[2]);

                    Indices.Add(StartIndex);
                    Indices.Add(StartIndex + 1);
                    Indices.Add(StartIndex + 2);
                }
            }
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
        int32 I0 = Indices[i];
        int32 I1 = Indices[i + 1];
        int32 I2 = Indices[i + 2];

        const FVector& V0 = Vertices[I0];
        const FVector& V1 = Vertices[I1];
        const FVector& V2 = Vertices[I2];

        FVector Normal = FVector::CrossProduct(V2 - V0, V1 - V0).GetSafeNormal();
        Normals[I0] = Normals[I1] = Normals[I2] = Normal;
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


void AVoxelChunkActor::GenerateScalarField(TArray<float>& OutField)
{
    for (int32 z = 0; z <= ChunkSize; ++z)
    {
        for (int32 y = 0; y <= ChunkSize; ++y)
        {
            for (int32 x = 0; x <= ChunkSize; ++x)
            {
                float S = FMath::PerlinNoise3D(FVector(x, y, z) * 0.1f);
                OutField[ScalarIndex(x, y, z)] = S;
            }
        }
    }
}

float AVoxelChunkActor::GetScalar(const TArray<float>& Field, int32 X, int32 Y, int32 Z) const
{
    return Field[ScalarIndex(X, Y, Z)];
}

int32 AVoxelChunkActor::ScalarIndex(int32 X, int32 Y, int32 Z) const
{
    const int32 Size = ChunkSize + 1;
    return X + Y * Size + Z * Size * Size;
}
