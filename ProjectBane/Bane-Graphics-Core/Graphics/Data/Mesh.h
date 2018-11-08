#pragma once

#include <Core/Containers/Array.h>
#include <memory>
#include "Common.h"
#include "KieckerMath.h"
#include "../Interfaces/GraphicsResources.h"
#include "../Interfaces/PipelineState.h"


union FloatInt
{
	FloatInt() : I(0) { }
	FloatInt(float InF) : F(InF) { }
	FloatInt(int InI) : I(InI) { }
	FloatInt& operator = (float Rhs) { F = Rhs; return *this; }
	FloatInt& operator = (int Rhs) { I = Rhs; return *this; }
	FloatInt(const FloatInt& Rhs) { I = Rhs.I; }
	float F;
	int I;
};

class MeshLayout
{
public:
	MeshLayout() : Stride(0) { }
	MeshLayout(IInputLayout* Layout);

	inline void AddItem(const std::string& Name, EINPUT_ITEM_FORMAT Format)
	{
		Stride += TranslateItemFormatSize(Format);
		Inputs.Add(Name);
	}

	inline void SetLayout(IInputLayout* Layout)
	{
		Reset();
		for (auto& Item : Layout->GetDesc().InputItems)
		{
			AddItem(Item.Name, Item.ItemFormat);
		}
	}

	inline void Reset()
	{
		Stride = 0;
		Inputs.Empty();
	}

	inline MeshLayout& operator = (const MeshLayout& Rhs)
	{
		Stride = Rhs.Stride;
		Inputs = Rhs.Inputs;
		return *this;
	}

	uint32 Stride;
	TArray<std::string> Inputs;
};

class Mesh
{
	friend class Mesh;

public:

	struct Vertex
	{
		float3 Postition;
		float3 Normal;
		float3 Binormal;
		float3 Tangent;
		float2 UV;
	};

	Mesh();
	// Calls Upload()
	Mesh(const Mesh& Rhs);
	// Does not call Upload()
	Mesh(const TArray<FloatInt>& InVertices, const TArray<uint32>& InIndices);

	inline const TArray<FloatInt>& GetVertices() const { return m_Vertices; }
	inline const TArray<uint32>& GetIndices() const { return m_Indices; }
	inline MeshLayout& GetLayout() { return m_Layout; }
	inline MeshLayout GetLayout() const { return m_Layout; }

	inline IVertexBuffer* GetVertexBuffer() const { return m_VertexBuffer; }
	inline IIndexBuffer* GetIndexBuffer() const { return m_IndexBuffer; }
	inline const uint32 GetIndexCount() const { return static_cast<uint32>(m_Indices.GetCount()); }

	inline void SetName(const std::string& Name) { m_Name = Name; }
	inline std::string GetName() const { return m_Name; }

	bool LoadFromFile(const std::string& FileName);

	// none of these call Upload()
	template<class T>
	inline void SetVertexData(const TArray<T>& InVertices)
	{
		TArray<FloatInt> Res(InVertices.GetCount() * (sizeof(T) / sizeof(FloatInt)));
		memcpy((void*)Res.GetData(), (void*)InVertices.GetData(), InVertices.GetCount() * (sizeof(T)));
		SetVertices(Res);
	}

	void SetVertices(const TArray<FloatInt>& InVertices);
	void SetIndices(const TArray<uint32>& InIndices);
	void GenerateUVSphere(uint32 SegmentCount);

	void Upload(); // Call an explicit upload to gpu
	
	inline bool HasUploadedToGPU() const { return (m_VertexBuffer != nullptr && m_IndexBuffer != nullptr); }

	inline Mesh& operator = (const Mesh& Rhs)
	{
		m_VertexBuffer = Rhs.m_VertexBuffer;
		m_IndexBuffer = Rhs.m_IndexBuffer;
		m_Name = Rhs.m_Name;
		m_Vertices = Rhs.m_Vertices;
		m_Indices = Rhs.m_Indices;
		m_Layout = Rhs.m_Layout;
		return *this;
	}

	inline void Destroy()
	{
		delete m_VertexBuffer;
		m_VertexBuffer = nullptr;
		delete m_IndexBuffer;
		m_IndexBuffer = nullptr;
		m_Vertices.Empty();
		m_Indices.Empty();
	}

private:

	void AddData(float* pDatas, uint32 Size);
	void AddData(int* pDatas, uint32 Size);
	
	template<typename T>
	inline void AddVertex(T* InData)
	{
		AddData((float*)InData, sizeof(T) / sizeof(FloatInt));
	}

	template<typename T>
	inline T& GetVerticesAtIndex(uint32 Index)
	{
		return (T&)*(((uint8*)m_Vertices.data()) + (Index * sizeof(T)));
	}

	static IInputLayout* GBaseLayout;

	IVertexBuffer* m_VertexBuffer;
	IIndexBuffer* m_IndexBuffer;
	std::string m_Name; // Filename, or asset name
	MeshLayout m_Layout;
	TArray<FloatInt> m_Vertices;
	TArray<uint32> m_Indices;
};

