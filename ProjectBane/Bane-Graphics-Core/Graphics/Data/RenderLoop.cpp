#include "RenderLoop.h"
#include "../Interfaces/ApiRuntime.h"


RenderLoop::RENDER_LOOP_GLOBALS RenderLoop::GRenderGlobals = { };

RenderLoop::RenderLoop()
{
	if (GRenderGlobals.MeshData.Buffer == nullptr)
	{
		GRenderGlobals.MeshData.Buffer = reinterpret_cast<MESH_RENDER_DATA*>(new byte[GPU_BUFFER_MIN_SIZE]);
	}
	if (GRenderGlobals.ImmediateGeometry.VertexBuffer == nullptr)
	{
		GRenderGlobals.ImmediateGeometry.VertexBuffer = GetApiRuntime()->GetGraphicsDevice()->CreateVertexBuffer(KILOBYTE(1524), nullptr);
		GRenderGlobals.ImmediateGeometry.VertexBuffer->SetDebugName("Immediate mode Vertex Buffer");
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
	m_Current.Meshes.push_back({ 
		pMesh->GetVertexBuffer(),
		pMesh->GetIndexBuffer(),
		Mat.GetShaderConfiguration(),
		Mat.GetTable(),
		pMesh->GetIndexCount() 
	});
	m_Current.MeshData_NumUsed++;
	GRenderGlobals.MeshData.Size++;
}

void RenderLoop::BeginNewShape()
{
	m_CurrentShape.clear();
}

void RenderLoop::EndNewShape()
{
	if (GRenderGlobals.ImmediateGeometry.DrawArgs.size() > GRenderGlobals.ImmediateGeometry.CurrentCount &&
		GRenderGlobals.ImmediateGeometry.DrawArgs[
		GRenderGlobals.ImmediateGeometry.CurrentCount
			].UploadBuffer->GetSizeInBytes() ==
			m_CurrentShape.size() * sizeof(RENDER_LINE_DATA)
		)
	{
		IBuffer* UploadBuff = GRenderGlobals.ImmediateGeometry.DrawArgs[GRenderGlobals.ImmediateGeometry.CurrentCount].UploadBuffer;
		void* Ptr = UploadBuff->Map();
		memcpy(Ptr, m_CurrentShape.data(), m_CurrentShape.size() * sizeof(RENDER_LINE_DATA));
		UploadBuff->Unmap();
	}
	else
	{
		IBuffer* UploadBuff = GetApiRuntime()->GetGraphicsDevice()->CreateStagingBuffer(static_cast<uint32>(m_CurrentShape.size()) * sizeof(RENDER_LINE_DATA));
		void* Ptr = UploadBuff->Map();
		memcpy(Ptr, m_CurrentShape.data(), m_CurrentShape.size() * sizeof(RENDER_LINE_DATA));
		UploadBuff->Unmap();
		GRenderGlobals.ImmediateGeometry.DrawArgs.push_back(
			RENDER_LOOP_GLOBALS::IMMEDIATE_GEOMETRY::IMMEDIATE_GEOMETRY_DRAW_ARGS(UploadBuff, static_cast<uint32>(m_CurrentShape.size() * 2))
		);
	}
	if (GRenderGlobals.ImmediateGeometry.CurrentCount * sizeof(RENDER_LINE_DATA) > GPU_BUFFER_MIN_SIZE && false)
	{
		auto* OldBuff = GRenderGlobals.ImmediateGeometry.VertexBuffer;
		auto* NewBuff = GetApiRuntime()->GetGraphicsDevice()->CreateVertexBuffer(OldBuff->GetSizeInBytes() * 2, nullptr);
		GetApiRuntime()->GetGraphicsDevice()->GetGraphicsContext()->CopyBuffers(OldBuff, NewBuff);
		GRenderGlobals.ImmediateGeometry.VertexBuffer = NewBuff;
		delete OldBuff;
	}
	GRenderGlobals.ImmediateGeometry.CurrentCount++;
}

void RenderLoop::AddLine(const double3& Start, const double3& End)
{
	m_CurrentShape.push_back({ fromDouble3(Start), fromDouble3(End) });
}

void RenderLoop::AddBoundingBox(BoundingBox InBox)
{
	BeginNewShape();
	InBox.Max -= m_Current.CameraPosition;
	InBox.Min -= m_Current.CameraPosition;
	
	//Top face
	AddLine(InBox.Max, double3(InBox.Max.xy, InBox.Min.z));
	AddLine(double3(InBox.Max.xy, InBox.Min.z), double3(InBox.Min.x, InBox.Max.y, InBox.Min.z));
	AddLine(double3(InBox.Min.x, InBox.Max.y, InBox.Min.z), double3(InBox.Min.x, InBox.Max.y, InBox.Max.z));
	AddLine(double3(InBox.Min.x, InBox.Max.y, InBox.Max.z), InBox.Max);

	// Sides
	AddLine(double3(InBox.Max.x, InBox.Min.y, InBox.Min.z), double3(InBox.Max.x, InBox.Max.y, InBox.Min.z));
	AddLine(double3(InBox.Min.x, InBox.Min.y, InBox.Min.z), double3(InBox.Min.x, InBox.Max.y, InBox.Min.z));
	AddLine(double3(InBox.Min.x, InBox.Min.y, InBox.Max.z), double3(InBox.Min.x, InBox.Max.y, InBox.Max.z));
	AddLine(double3(InBox.Max.x, InBox.Min.y, InBox.Max.z), double3(InBox.Max.x, InBox.Max.y, InBox.Max.z));

	// Bottom face
	AddLine(double3(InBox.Max.x, InBox.Min.y, InBox.Max.z), double3(InBox.Max.x, InBox.Min.y, InBox.Min.z));
	AddLine(double3(InBox.Max.x, InBox.Min.y, InBox.Min.z), double3(InBox.Min.x, InBox.Min.y, InBox.Min.z));
	AddLine(double3(InBox.Min.x, InBox.Min.y, InBox.Min.z), double3(InBox.Min.x, InBox.Min.y, InBox.Max.z));
	AddLine(double3(InBox.Min.x, InBox.Min.y, InBox.Max.z), double3(InBox.Max.x, InBox.Min.y, InBox.Max.z));
	EndNewShape();
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
	m_Commits.push_back(m_Current);
	m_Current.CameraIdxOffset = GRenderGlobals.CameraData.Size;
	m_Current.MeshData_NumUsed = 0;
	m_Current.MeshData_Offset = Prev.MeshData_NumUsed;
	m_Current.Meshes.clear();
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
	GRenderGlobals.ImmediateGeometry.CurrentCount = 0;
}

