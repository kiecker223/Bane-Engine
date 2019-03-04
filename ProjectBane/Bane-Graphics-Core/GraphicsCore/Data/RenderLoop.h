#pragma once

#include <vector>
#include <Core/Containers/StackQueue.h>
#include <functional>
#include "Core/Data/BoundingBox.h"
#include "Mesh.h"
#include "LightData.h"
#include "Material.h"
#include "CameraData.h"
#include "RenderPass.h"


typedef struct ALIGN_FOR_GPU_BUFFER MESH_RENDER_DATA {
	fmat4x4 Model;
	MATERIAL_PARAMETERS Parameters;
} MESH_RENDER_DATA;

typedef struct MESH_ITEM {
	Mesh* pMesh;
	Material* pMaterial;
} MESH_ITEM;

typedef struct SKYBOX_DATA {
	const ITextureCube* TexCube;
	fvec3 AmbientLight; // Possibly remove when making GI?
} SKYBOX_DATA;

typedef struct DRAWABLE_MESH {
	IVertexBuffer* VertexBuffer;
	IIndexBuffer* IndexBuffer;
	IGraphicsPipelineState* Pipeline;
	ITexture2D* Diffuse;
	uint32 IndexCount;
} DRAWABLE_MESH;

typedef struct RENDER_LINE_DATA {
	fvec3 Start;
	fvec3 End;
} RENDER_LINE_DATA;

typedef struct RENDER_LOOP_DRAW_COMMIT {
	uint64 CameraIdxOffset;
	uint64 MeshData_Offset;
	uint64 MeshData_NumUsed;
	vec3 CameraPosition;
	std::vector<DRAWABLE_MESH> Meshes;
} RENDER_LOOP_DRAW_COMMIT;


class RenderMeshMaterialBucket
{
public:

	struct BucketItem
	{
		const Mesh* pMesh;
		const ITexture2D* DiffuseTex;
		MESH_RENDER_DATA Data;
	};

	struct ShaderKey
	{
		uint64 Key;
		IGraphicsPipelineState* pShader;
	};

	void AddDrawnMesh(const Mesh* pMesh, const Material* pMat, const fmat4x4& Transformation);

	int32 KeyExists(const ShaderKey& InKey);

	std::vector<ShaderKey> Keys;
	std::vector<std::vector<BucketItem>> Values;

};

class RenderLoop
{
	typedef struct RENDER_LOOP_GLOBALS {
		struct
		{
			MESH_RENDER_DATA* Buffer;
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
			fvec3 AmbientLight;
		} SkyboxData;
	} RENDER_LOOP_GLOBALS;

public:

	RENDER_LOOP_GLOBALS GRenderGlobals;
	void ResetForNextFrame();

	RenderLoop();
	RenderLoop& operator = (const RenderLoop& Rhs) = delete;
	~RenderLoop();

	void DrawMesh(const Mesh* pMesh, const Material* pMaterial, const fmat4x4& Transformation);
	void AddCamera(const CAMERA_DATA& InData);

	TStack<std::function<void(IGraphicsDevice*, IGraphicsCommandContext*)>, 24> PostRenderCallback;

	TStack<CAMERA_DATA, 24> CameraStack;
	RenderMeshMaterialBucket Bucket;
};


