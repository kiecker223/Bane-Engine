#pragma once

#include <vector>
#include <memory>
#include "Core/Common.h"
#include "Core/KieckerMath.h"
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

	ForceInline void AddItem(const std::string& Name, EINPUT_ITEM_FORMAT Format)
	{
		Stride += TranslateItemFormatSize(Format);
		Inputs.push_back(Name);
	}

	ForceInline void SetLayout(IInputLayout* Layout)
	{
		Reset();
		for (auto& Item : Layout->GetDesc().InputItems)
		{
			AddItem(Item.Name, Item.ItemFormat);
		}
	}

	ForceInline void Reset()
	{
		Stride = 0;
		Inputs.clear();
	}

	ForceInline MeshLayout& operator = (const MeshLayout& Rhs)
	{
		Stride = Rhs.Stride;
		Inputs = Rhs.Inputs;
		return *this;
	}

	uint Stride;
	std::vector<std::string> Inputs;
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
	Mesh(const std::vector<FloatInt>& InVertices, const std::vector<uint>& InIndices);

	ForceInline const std::vector<FloatInt>& GetVertices() const { return m_Vertices; }
	ForceInline const std::vector<uint>& GetIndices() const { return m_Indices; }
	ForceInline MeshLayout& GetLayout() { return m_Layout; }

	ForceInline IVertexBuffer* GetVertexBuffer() const { return m_VertexBuffer; }
	ForceInline IIndexBuffer* GetIndexBuffer() const { return m_IndexBuffer; }
//	ForceInline const uint GetVertexCount() const { return m_Vertices.size(); }
	ForceInline const uint GetIndexCount() const { return static_cast<uint>(m_Indices.size()); }

	ForceInline void SetName(const std::string& Name) { m_Name = Name; }
	ForceInline std::string GetName() const { return m_Name; }

	bool LoadFromFile(const std::string& FileName);

	// none of these call Upload()
	template<class T>
	ForceInline void SetVertexData(const std::vector<T>& InVertices)
	{
		std::vector<FloatInt> Res(InVertices.size() * (sizeof(T) / sizeof(FloatInt)));
		memcpy((void*)Res.data(), (void*)InVertices.data(), InVertices.size() * (sizeof(T)));
		SetVertices(Res);
	}
	void SetVertices(const std::vector<FloatInt>& InVertices);
	void SetIndices(const std::vector<uint>& InIndices);

	void Upload(); // Call an explicit upload to gpu
	
	ForceInline bool HasUploadedToGPU() const { return (m_VertexBuffer != nullptr && m_IndexBuffer != nullptr); }

	ForceInline Mesh& operator = (const Mesh& Rhs)
	{
		m_VertexBuffer = Rhs.m_VertexBuffer;
		m_IndexBuffer = Rhs.m_IndexBuffer;
		m_Name = Rhs.m_Name;
		m_Vertices = Rhs.m_Vertices;
		m_Indices = Rhs.m_Indices;
		m_Layout = Rhs.m_Layout;
		return *this;
	}

	ForceInline void Destroy()
	{
		delete m_VertexBuffer;
		m_VertexBuffer = nullptr;
		delete m_IndexBuffer;
		m_IndexBuffer = nullptr;
		m_Vertices.clear();
		m_Indices.clear();
	}

	static Mesh CreateSphere(uint NumPoints);

private:

	void AddData(float* pDatas, uint Size);
	void AddData(int* pDatas, uint Size);
	
	template<typename T>
	ForceInline void AddVertex(T* InData)
	{
		AddData((float*)InData, sizeof(T) / sizeof(FloatInt));
	}

	template<typename T>
	ForceInline T& GetVerticesAtIndex(uint Index)
	{
		return (T&)*(((uint8*)m_Vertices.data()) + (Index * sizeof(T)));
	}

	static IInputLayout* GBaseLayout;

	IVertexBuffer* m_VertexBuffer;
	IIndexBuffer* m_IndexBuffer;
	std::string m_Name; // Filename, or asset name
	MeshLayout m_Layout;
	std::vector<FloatInt> m_Vertices;
	std::vector<uint> m_Indices;
};

