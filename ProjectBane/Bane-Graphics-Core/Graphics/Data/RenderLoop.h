#pragma once

#include <vector>
#include "Mesh.h"
#include "LightData.h"
#include "Material.h"
#include "CameraData.h"


typedef struct ALIGN_FOR_GPU_BUFFER MESH_RENDER_DATA {
	float4x4 Model;
	MATERIAL_PARAMETERS Parameters;
} MESH_RENDER_DATA;

typedef struct ALIGN_FOR_GPU_BUFFER CAMERA_CONSTANT_BUFFER_DATA {
	float4x4 View;
	float4x4 Projection;
	float3 Position;
	float ZResolution;
	float FarPlane;
} CAMERA_CONSTANT_BUFFER_DATA;


typedef struct MESH_ITEM {
	Mesh* pMesh;
	Material* pMaterial;
} MESH_ITEM;

typedef struct SKYBOX_DATA {
	const ITextureCube* TexCube;
	float3 AmbientLight; // Possibly remove when making GI?
} SKYBOX_DATA;

typedef struct DRAWABLE_MESH {
	IVertexBuffer* VertexBuffer;
	IIndexBuffer* IndexBuffer;
	IGraphicsPipelineState* Pipeline;
	IShaderResourceTable* Table;
	uint64 MeshDataOffset;
	uint32 IndexCount;
} DRAWABLE_MESH;

typedef struct RENDER_LOOP_DRAW_COMMIT {
	uint64 CameraIdxOffset;
	uint64 MeshData_Offset;
	uint64 MeshData_NumUsed;
	double3 CameraPosition;
	std::vector<DRAWABLE_MESH> Meshes;
} RENDER_LOOP_DRAW_COMMIT;

class RenderLoop
{
	typedef struct RENDER_LOOP_GLOBALS {
		struct
		{
			CAMERA_CONSTANT_BUFFER_DATA Buffer[24];
			IRenderPassInfo* RenderPasses[24];
			uint64 Offset;
			uint64 Size;
		} CameraData;
		struct
		{
			MESH_RENDER_DATA* Buffer;
			uint64 Offset;
			uint64 Size;
		} MeshData;
		struct SHADER_ALIGNMENT SHADER_LIGHT_DATA
		{
			DIRECTIONAL_LIGHT_DATA	DirectionalLights[MAX_LIGHTS];
			POINT_LIGHT_DATA		PointLights[MAX_LIGHTS];
			SPOTLIGHT_DATA			SpotLights[MAX_LIGHTS];
			uint32					NumDirectionalLights;
			uint32					NumPointLights;
			uint32					NumSpotLights;
		} LightData;
		struct
		{
			const ITextureCube* Cubemap;
			float3 AmbientLight;
		} SkyboxData;
	} RENDER_LOOP_GLOBALS;

public:

	static RENDER_LOOP_GLOBALS GRenderGlobals;
	static void ResetForNextFrame();

	RenderLoop();
	~RenderLoop();

	void SetCamera(const CAMERA_DATA& CamData);
	void AddDrawable(const Mesh* pMesh, const Material& Mat, const float4x4& Transformation);
	void AddLight(const DIRECTIONAL_LIGHT_DATA& DirLight);
	void AddLight(const POINT_LIGHT_DATA& PointLight);
	void AddLight(const SPOTLIGHT_DATA& SpotLight);
	void SetSkybox(const SKYBOX_DATA& Skybox);
	void Draw();

	inline std::vector<RENDER_LOOP_DRAW_COMMIT> const& GetCommitedData() const
	{
		return m_Commits;
	}

private:

	RENDER_LOOP_DRAW_COMMIT m_Current;
	std::vector<RENDER_LOOP_DRAW_COMMIT> m_Commits;

};


