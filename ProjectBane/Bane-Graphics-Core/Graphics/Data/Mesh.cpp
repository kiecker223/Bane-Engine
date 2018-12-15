#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../Interfaces/ApiRuntime.h"
#include <iostream>


MeshLayout::MeshLayout(IInputLayout* Layout)
{
	for (auto& Item : Layout->GetDesc().InputItems)
	{
		AddItem(Item.Name, Item.ItemFormat);
	}
}

IInputLayout* Mesh::GBaseLayout = nullptr;

Mesh::Mesh() :
	m_VertexBuffer(nullptr),
	m_IndexBuffer(nullptr),
	m_Name("No Named Mesh")
{
}

Mesh::Mesh(const Mesh& InMesh) :
	m_VertexBuffer(InMesh.GetVertexBuffer()),
	m_IndexBuffer(InMesh.GetIndexBuffer()),
	m_Name(InMesh.GetName()),
	m_Vertices(InMesh.GetVertices()),
	m_Indices(InMesh.GetIndices()),
	m_Layout(InMesh.m_Layout)
{
}

Mesh::Mesh(const std::vector<FloatInt>& InVertices, const std::vector<uint32>& InIndices) :
	m_VertexBuffer(nullptr),
	m_IndexBuffer(nullptr),
	m_Name("No Named Mesh"),
	m_Vertices(InVertices),
	m_Indices(InIndices)
{
}

void ProcessMesh(aiMesh* pMesh, std::vector<uint32>& OutIndices, std::vector<FloatInt>& OutVertices)
{
	for (uint32 i = 0; i < pMesh->mNumVertices; i++)
	{
		aiVector3D Pos = pMesh->mVertices[i];
		aiVector3D Norm = pMesh->mNormals[i];
		aiVector3D Tangent;
		aiVector3D Binormals;
		aiVector3D UV;
		if (pMesh->mTangents)
		{
			Tangent = pMesh->mTangents[i];
		}
		if (pMesh->mBitangents)
		{
			Binormals = pMesh->mBitangents[i];
		}
		if (pMesh->mTextureCoords && pMesh->mTextureCoords[0])
		{
			UV = pMesh->mTextureCoords[0][i];
		}

		Mesh::Vertex Vtx = { 
			float3(Pos.x, Pos.y, Pos.z), 
			float3(Norm.x, Norm.y, Norm.z), 
			float3(Binormals.x, Binormals.y, Binormals.z), 
			float3(Tangent.x, Tangent.y, Tangent.z), 
			float2(UV.x, UV.y) 
		};

		float* pFloats = reinterpret_cast<float*>(&Vtx);
		uint32 FloatsSize = sizeof(Mesh::Vertex) / sizeof(float);
		for (uint32 z = 0; z < FloatsSize; z++)
		{
			OutVertices.push_back(pFloats[z]);
		}
	}
	for (uint32 i = 0; i < pMesh->mNumFaces; i++)
	{
		aiFace Face = pMesh->mFaces[i];
		for (uint32 j = 0; j < Face.mNumIndices; j++)
		{
			OutIndices.push_back(Face.mIndices[j]);
		}
	}
}

void ProcessScene(const aiScene* pScene, aiNode* pRootNode, std::vector<uint32>& OutIndices, std::vector<FloatInt>& OutVertices)
{
	for (uint32 i = 0; i < pRootNode->mNumMeshes; i++)
	{
		ProcessMesh(pScene->mMeshes[pRootNode->mMeshes[i]], OutIndices, OutVertices);
	}
	for (uint32 i = 0; i < pRootNode->mNumChildren; i++)
	{
		ProcessScene(pScene, pRootNode->mChildren[i], OutIndices, OutVertices);
	}
}

bool Mesh::LoadFromFile(const std::string& FileName)
{
	m_Layout.AddItem("POSITION", INPUT_ITEM_FORMAT_FLOAT3);
	m_Layout.AddItem("NORMAL", INPUT_ITEM_FORMAT_FLOAT3);
	m_Layout.AddItem("BINORMAL", INPUT_ITEM_FORMAT_FLOAT3);
	m_Layout.AddItem("TANGENT", INPUT_ITEM_FORMAT_FLOAT3);
	m_Layout.AddItem("TEXCOORD", INPUT_ITEM_FORMAT_FLOAT2);

	Assimp::Importer Importer;

	const aiScene* pScene = Importer.ReadFile(FileName, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);

	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
	{
		__debugbreak();
		return false;
	}

	ProcessScene(pScene, pScene->mRootNode, m_Indices, m_Vertices);
	Upload();

	return true;
}

void Mesh::SetVertices(const std::vector<FloatInt>& InVertices)
{
	m_Vertices = InVertices;
}

void Mesh::SetIndices(const std::vector<uint32>& InIndices)
{
	m_Indices = InIndices;
}

void Mesh::GenerateUVSphere(uint32 SegmentCount)
{
	std::vector<Vertex> Vertices;
	std::vector<uint32> Indices;
	float3 TopPoint(0.f, 0.5f, 0.f);
	const float LatitudeSpacing = 1.0f / (static_cast<float>(SegmentCount) + 1.0f);
	const float LongitudeSpacing = 1.0f / (static_cast<float>(SegmentCount));

	for (uint32 y = 0; y < SegmentCount / 2 + 1; y++)
	{
		const float PercentDown = static_cast<float>(y) / static_cast<float>(SegmentCount / 2);
		if (y == 0)
		{
			Vertices.push_back({ TopPoint, normalized(TopPoint), float3(), float3(), float2(0.5f, 1.0f) });
		}
		else if (y == ((SegmentCount / 2)))
		{
			Vertices.push_back({ -TopPoint, normalized(-TopPoint), float3(), float3(), float2(0.5f, 0.0f) });
		}
		else
		{
			for (uint32 i = 0; i < SegmentCount; i++)
			{
				const float PercentAroundCrown = (static_cast<float>(i) / static_cast<float>(SegmentCount));
				float3 Point = (float3x3)(matRotY(radians(PercentAroundCrown * 360.f)) * matRotX(radians(PercentDown * 180.f))) * TopPoint;
				Vertices.push_back({ Point, normalized(Point), float3(), float3(), float2(1.0f - PercentAroundCrown, 1.0f - PercentDown) });
 				if (i == SegmentCount - 1)
 				{
 					Point = (float3x3)(matRotY(radians(360.f)) * matRotX(radians(PercentDown * 180.f))) * TopPoint;
 					Vertices.push_back({ Point, normalized(Point), float3(), float3(), float2(0.0f, 1.0f - PercentDown) });
 				}
			}
		}
	}
	uint32 LatitudeCount = SegmentCount + 1;
	for (uint32 y = 0; y < (SegmentCount / 2) - 1; y++)
	{
		if (y == 0) 
		{
			for (uint32 i = 0; i < LatitudeCount - 1; i++)
			{
				Indices.push_back(0);
				Indices.push_back(i + 1);
				Indices.push_back(i + 2);
				if (i == LatitudeCount - 2)
				{
					Indices.push_back(0);
					Indices.push_back(LatitudeCount);
					Indices.push_back(1);
				}
			}
			for (uint32 i = 0; i < (LatitudeCount); i++)
			{
				if (i == 0)
				{
					// Top triangle
					Indices.push_back((y * LatitudeCount) + LatitudeCount + LatitudeCount);
					Indices.push_back((y * LatitudeCount) + LatitudeCount);
					Indices.push_back((y * LatitudeCount) + 1);

					// Bottom triangle
					Indices.push_back((y * LatitudeCount) + LatitudeCount + LatitudeCount);
					Indices.push_back((y * LatitudeCount) + LatitudeCount + 1);
					Indices.push_back((y * LatitudeCount) + 1);
				}
				else
				{
					// Top triangle
					Indices.push_back((y * LatitudeCount) + i);
					Indices.push_back((y * LatitudeCount) + i + LatitudeCount + 1);
					Indices.push_back((y * LatitudeCount) + i + 1);

					// Bottom triangle
					Indices.push_back((y * LatitudeCount) + i);
					Indices.push_back((y * LatitudeCount) + i + LatitudeCount);
					Indices.push_back((y * LatitudeCount) + i + LatitudeCount + 1);
				}
			}
		}
		else if (y == (SegmentCount / 2) - 2)
		{
			uint32 LastPos = static_cast<uint32>(Vertices.size() - 1);
			for (uint32 i = 0; i < SegmentCount - 1; i++)
			{
				Indices.push_back(LastPos);
				Indices.push_back((LastPos) - (i + 1));
				Indices.push_back((LastPos) - (i + 2));
				if (i == SegmentCount - 2)
				{
					Indices.push_back(LastPos);
					Indices.push_back(LastPos - 1);
					Indices.push_back(LastPos - SegmentCount);
				}
			}
			
		}
		else if (y < (SegmentCount / 2) - 2)
		{
			for (uint32 i = 0; i < (LatitudeCount); i++)
			{
				if (i == 0)
				{
					// Top triangle
					Indices.push_back((y * LatitudeCount) + LatitudeCount + LatitudeCount); 
					Indices.push_back((y * LatitudeCount) + LatitudeCount); 
					Indices.push_back((y * LatitudeCount) + 1); 
 
 					// Bottom triangle
					Indices.push_back((y * LatitudeCount) + LatitudeCount + LatitudeCount); 
					Indices.push_back((y * LatitudeCount) + LatitudeCount + 1); 
					Indices.push_back((y * LatitudeCount) + 1);
				}
				else
				{
					// Top triangle
					Indices.push_back((y * LatitudeCount) + i);
					Indices.push_back((y * LatitudeCount) + i + LatitudeCount + 1);
					Indices.push_back((y * LatitudeCount) + i + 1);

					// Bottom triangle
					Indices.push_back((y * LatitudeCount) + i);
					Indices.push_back((y * LatitudeCount) + i + LatitudeCount);
					Indices.push_back((y * LatitudeCount) + i + LatitudeCount + 1);
				}
			}
		}
	}

	m_Indices = Indices;
	SetVertexData(Vertices);
	Upload();
}

void Mesh::GenerateCylinder(uint32 RadialSegments, uint32 HeightSegments)
{
	float3 TopMost = float3(0.f, 0.5f, 0.f);
	float3 BottomMost = -TopMost;

	std::vector<Vertex> Vertices;
	
	{
		Vertex Vert;
		Vert.Position = TopMost;
		Vertices.push_back(Vert);
	}
	for (uint32 y = 0; y < HeightSegments + 1; y++)
	{
		float PercentDown = (float)y / (float)HeightSegments;
		for (uint32 i = 0; i < RadialSegments; i++)
		{
			float PercentAround = (float)i / (float)RadialSegments;
			matrix RotateAround = matRotY(radians(PercentAround * 360));
			float3 Point = float3(0.f, 0.5f, -0.5f);

			float3 NewPoint = ((float3x3)RotateAround * Point) - float3(0.f, PercentDown, 0.f);

			Vertex Vert;
			Vert.Position = NewPoint;
			Vertices.push_back(Vert);
		}
	}
	{
		Vertex Vert;
		Vert.Position = BottomMost;
		Vertices.push_back(Vert);
	}

	std::vector<uint32> Indices;
	for (uint32 i = 1; i < RadialSegments; i++)
	{
		Indices.push_back(0);
		Indices.push_back(i);
		Indices.push_back(i + 1);
	}
	Indices.push_back(0);
	Indices.push_back(RadialSegments);
	Indices.push_back(1);
	for (uint32 y = 0; y < HeightSegments; y++)
	{
		for (uint32 i = 0; i < RadialSegments - 1; i++)
		{
			uint32 CurrentIdx = (y * RadialSegments + 1) + i;
			Indices.push_back(CurrentIdx);
			Indices.push_back(CurrentIdx + RadialSegments);
			Indices.push_back(CurrentIdx + RadialSegments + 1);
			Indices.push_back(CurrentIdx);
			Indices.push_back(CurrentIdx + 1);
			Indices.push_back(CurrentIdx + RadialSegments + 1);
		}
		// Here we can assume that we are at the end of the segment for the given line
		uint32 Idx = y + 1;
		Indices.push_back(Idx * RadialSegments);
		Indices.push_back(Idx * RadialSegments + 1);
		Indices.push_back(Idx * RadialSegments + (RadialSegments));
		Indices.push_back(y * RadialSegments + (RadialSegments));
		Indices.push_back(y * RadialSegments + 1);
		Indices.push_back(y * RadialSegments + RadialSegments + 1);
	}
	for (uint32 i = static_cast<uint32>(Vertices.size()) - 1; i > static_cast<uint32>(Vertices.size() - 1) - RadialSegments; i--)
	{
		Indices.push_back(static_cast<uint32>(Vertices.size()) - 1);
		Indices.push_back(i);
		Indices.push_back(i - 1);
	}
	Indices.push_back(static_cast<uint32>(Vertices.size()) - 1);
	Indices.push_back(static_cast<uint32>(Vertices.size() - 1) - RadialSegments);
	Indices.push_back(static_cast<uint32>(Vertices.size()) - 2);
	SetVertexData(Vertices);
	SetIndices(Indices);
	Upload();
}

void Mesh::Upload()
{
	IRuntimeGraphicsDevice* Device = GetApiRuntime()->GetGraphicsDevice();
	if (m_VertexBuffer)
	{
		delete m_VertexBuffer;
	}
	m_VertexBuffer = Device->CreateVertexBuffer(static_cast<uint32>(m_Vertices.size()) * sizeof(FloatInt), (uint8*)m_Vertices.data());
	if (m_IndexBuffer)
	{
		delete m_IndexBuffer;
	}
	m_IndexBuffer = Device->CreateIndexBuffer(static_cast<uint32>(m_Indices.size() * 4), (uint8*)m_Indices.data());
}

void Mesh::AddData(float* pDatas, uint32 Size)
{
	for (uint32 i = 0; i < Size; i++)
	{
		m_Vertices.push_back(pDatas[i]);
	}
}

void Mesh::AddData(int* pDatas, uint32 Size)
{
	for (uint32 i = 0; i < Size; i++)
	{
		m_Vertices.push_back(pDatas[i]);
	}
}
