#pragma once

#include <vector>
#include "Core/Data/BoundingBox.h"
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
	ITexture2D* Diffuse;
	uint32 IndexCount;
} DRAWABLE_MESH;

typedef struct RENDER_LINE_DATA {
	float3 Start;
	float3 End;
} RENDER_LINE_DATA;

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

		struct IMMEDIATE_GEOMETRY {
			IVertexBuffer* VertexBuffer;
			typedef struct IMMEDIATE_GEOMETRY_DRAW_ARGS
			{
				IMMEDIATE_GEOMETRY_DRAW_ARGS() { }
				IMMEDIATE_GEOMETRY_DRAW_ARGS(IBuffer* InUploadBuffer, uint32 InVertexCount) : UploadBuffer(InUploadBuffer), VertexCount(InVertexCount)
				{
				}
				~IMMEDIATE_GEOMETRY_DRAW_ARGS()
				{
				}
				IBuffer* UploadBuffer;
				uint32 VertexCount;
			} IMMEDIATE_GEOMETRY_DRAW_ARGS;
			std::vector<IMMEDIATE_GEOMETRY_DRAW_ARGS> DrawArgs;
			uint32 CurrentCount;
		} ImmediateGeometry;
	} RENDER_LOOP_GLOBALS;

public:

	static RENDER_LOOP_GLOBALS GRenderGlobals;
	static void ResetForNextFrame();

	RenderLoop();
	~RenderLoop();

	void SetCamera(const CAMERA_DATA& CamData);
	void AddDrawable(const Mesh* pMesh, const Material& Mat, const float4x4& Transformation);
	
	void BeginNewShape();
	void EndNewShape();
	
	void AddLine(const double3& Start, const double3& End);
	void AddBoundingBox(BoundingBox InBox);
	
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
	std::vector<RENDER_LINE_DATA> m_CurrentShape;
};


