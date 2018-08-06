#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../Interfaces/ApiRuntime.h"


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
		aiVector3D Tangent = pMesh->mTangents[i];
		aiVector3D Binormals = pMesh->mBitangents[i];
		aiVector3D UV = pMesh->mTextureCoords[0][i];

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

Mesh Mesh::CreateSphere(uint32 NumPoints)
{
	UNUSED(NumPoints);
	Mesh Result;
/*	uint32 NumSphereVertices = 0;
	uint32 NumSphereFaces = 0;
	uint32 LatLines = NumPoints;
	uint32 LongLines = NumPoints;
	NumSphereVertices = ((LatLines - 2) * LongLines) + 2;
	NumSphereFaces = ((LatLines - 3)*(LongLines) * 2) + (LongLines * 2);

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;
	matrix Rotationx;
	matrix Rotationy;

	std::vector<float3> vertices(NumSphereVertices);

	Result.GetLayout().AddItem("POSITION", INPUT_ITEM_FORMAT_FLOAT3);

	float3 currVertPos = float3Set(0.0f, 0.0f, 1.0f, 0.0f);

	vertices[0].x = 0.0f;
	vertices[0].y = 0.0f;
	vertices[0].z = 1.0f;

	for (uint32 i = 0; i < LatLines - 2; ++i)
	{
		spherePitch = (i + 1) * (3.14 / (LatLines - 1));
		Rotationx = XMMatrixRotationX(spherePitch);
		for (uint32 j = 0; j < LongLines; ++j)
		{
			sphereYaw = j * (6.28 / (LongLines));
			Rotationy = XMMatrixRotationZ(sphereYaw);
			currVertPos = float33TransformNormal(float3Set(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy));
			currVertPos = float33Normalize(currVertPos);
			vertices[i*LongLines + j + 1].x = float3GetX(currVertPos);
			vertices[i*LongLines + j + 1].y = float3GetY(currVertPos);
			vertices[i*LongLines + j + 1].z = float3GetZ(currVertPos);
		}
	}

	vertices[NumSphereVertices - 1].x = 0.0f;
	vertices[NumSphereVertices - 1].y = 0.0f;
	vertices[NumSphereVertices - 1].z = -1.0f;
	Result.SetVertexData<float3>(vertices);

	std::vector<uint32> indices(NumSphereFaces * 3);

	int k = 0;
	for (uint32 l = 0; l < LongLines - 1; ++l)
	{
		indices[k] = 0;
		indices[k + 1] = l + 1;
		indices[k + 2] = l + 2;
		k += 3;
	}

	indices[k] = 0;
	indices[k + 1] = LongLines;
	indices[k + 2] = 1;
	k += 3;

	for (uint32 i = 0; i < LatLines - 3; ++i)
	{
		for (uint32 j = 0; j < LongLines - 1; ++j)
		{
			indices[k] = i * LongLines + j + 1;
			indices[k + 1] = i * LongLines + j + 2;
			indices[k + 2] = (i + 1)*LongLines + j + 1;

			indices[k + 3] = (i + 1)*LongLines + j + 1;
			indices[k + 4] = i * LongLines + j + 2;
			indices[k + 5] = (i + 1)*LongLines + j + 2;

			k += 6; // next quad
		}

		indices[k] = (i*LongLines) + LongLines;
		indices[k + 1] = (i*LongLines) + 1;
		indices[k + 2] = ((i + 1)*LongLines) + LongLines;

		indices[k + 3] = ((i + 1)*LongLines) + LongLines;
		indices[k + 4] = (i*LongLines) + 1;
		indices[k + 5] = ((i + 1)*LongLines) + 1;

		k += 6;
	}

	for (uint32 l = 0; l < LongLines - 1; ++l)
	{
		indices[k] = NumSphereVertices - 1;
		indices[k + 1] = (NumSphereVertices - 1) - (l + 1);
		indices[k + 2] = (NumSphereVertices - 1) - (l + 2);
		k += 3;
	}

	indices[k] = NumSphereVertices - 1;
	indices[k + 1] = (NumSphereVertices - 1) - LongLines;
	indices[k + 2] = NumSphereVertices - 2;
	Result.SetIndices(indices);
	*/
	return Result;
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
