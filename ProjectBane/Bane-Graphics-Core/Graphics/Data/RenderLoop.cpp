#include "RenderLoop.h"
#include "../Interfaces/ApiRuntime.h"


RenderLoop::RENDER_LOOP_GLOBALS RenderLoop::GRenderGlobals = { };

RenderLoop::RenderLoop()
{
	if (GRenderGlobals.MeshData.Buffer == nullptr)
	{
		GRenderGlobals.MeshData.Buffer = reinterpret_cast<MESH_RENDER_DATA*>(new byte[GPU_BUFFER_MIN_SIZE]);
	}
	m_Current.CameraIdxOffset = 0;
}

RenderLoop::~RenderLoop()
{

}

void RenderLoop::SetCamera(const CAMERA_DATA& CamData)
{
	uint64& Size = GRenderGlobals.CameraData.Size;
	GRenderGlobals.CameraData.Buffer[m_Current.CameraIdxOffset + Size] = { CamData.View, CamData.Projection, fromDouble3(CamData.Position), CamData.ZResolution, CamData.FarPlane };
	m_Current.CameraIdxOffset = GRenderGlobals.CameraData.Offset * sizeof(CAMERA_CONSTANT_BUFFER_DATA);
	m_Current.CameraPosition = CamData.Position;
	GRenderGlobals.CameraData.RenderPasses[Size] = CamData.CurrentRenderPass;
	Size++;
}

void RenderLoop::AddDrawable(const Mesh* pMesh, const Material& Mat, const float4x4& Transformation)
{
	GRenderGlobals.MeshData.Buffer[GRenderGlobals.MeshData.Size].Parameters = Mat.GetMaterialParameters();
	GRenderGlobals.MeshData.Buffer[GRenderGlobals.MeshData.Size].Model = Transformation;
	m_Current.Meshes.Add({ 
		pMesh->GetVertexBuffer(),
		pMesh->GetIndexBuffer(),
		Mat.GetShaderConfiguration(),
		Mat.GetTable(),
		pMesh->GetIndexCount() 
	});
	m_Current.MeshData_NumUsed++;
	GRenderGlobals.MeshData.Size++;
}

void RenderLoop::AddLight(const DIRECTIONAL_LIGHT_DATA& DirLight)
{
	GRenderGlobals.LightData.DirectionalLights[GRenderGlobals.LightData.NumDirectionalLights] = DirLight;
	GRenderGlobals.LightData.NumDirectionalLights++;
}

void RenderLoop::AddLight(const POINT_LIGHT_DATA& PointLight)
{
	GRenderGlobals.LightData.PointLights[GRenderGlobals.LightData.NumPointLights] = PointLight;
	GRenderGlobals.LightData.NumPointLights++;
}

void RenderLoop::AddLight(const SPOTLIGHT_DATA& SpotLight)
{
	GRenderGlobals.LightData.SpotLights[GRenderGlobals.LightData.NumSpotLights] = SpotLight;
	GRenderGlobals.LightData.NumSpotLights++;
}

void RenderLoop::SetSkybox(const SKYBOX_DATA& Skybox)
{
	GRenderGlobals.SkyboxData.Cubemap = Skybox.TexCube;
	GRenderGlobals.SkyboxData.AmbientLight = Skybox.AmbientLight;
}

void RenderLoop::Draw()
{
	RENDER_LOOP_DRAW_COMMIT Prev = m_Current;
	m_Commits.Add(m_Current);
	m_Current.CameraIdxOffset = GRenderGlobals.CameraData.Size;
	m_Current.MeshData_NumUsed = 0;
	m_Current.MeshData_Offset = Prev.MeshData_NumUsed;
	m_Current.Meshes.Empty();
}

void RenderLoop::ResetForNextFrame()
{
	GRenderGlobals.CameraData.Size = 0;
	GRenderGlobals.CameraData.Offset = 0;
	GRenderGlobals.MeshData.Offset = 0;
	GRenderGlobals.MeshData.Size = 0;
	GRenderGlobals.LightData.NumDirectionalLights = 0;
	GRenderGlobals.LightData.NumPointLights = 0;
	GRenderGlobals.LightData.NumSpotLights = 0;
	GRenderGlobals.SkyboxData.AmbientLight = float3(0.f, 0.f, 0.f);
	GRenderGlobals.SkyboxData.Cubemap = nullptr;
}

