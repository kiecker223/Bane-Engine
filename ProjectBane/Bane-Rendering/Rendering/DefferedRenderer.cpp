#include "DefferedRenderer.h"
#include "Graphics/Interfaces/ApiRuntime.h"
#include "KieckerMath.h"
#include "Graphics/IO/ShaderCache.h"
#include "Graphics/IO/TextureCache.h"


DefferedRenderer::DefferedRenderer()
{
}

DefferedRenderer::~DefferedRenderer()
{
}

void DefferedRenderer::Initialize(const Window* pWindow)
{
	m_DrawCommits.reserve(100);
	
	m_Device = GetApiRuntime()->GetGraphicsDevice();
	uint32 Height = pWindow->GetHeight();
	uint32 Width =  pWindow->GetWidth();

	m_AlbedoBuffer		= m_Device->CreateTexture2D(Width, Height, FORMAT_R8G8B8A8_UNORM, (TEXTURE_USAGE_SHADER_RESOURCE | TEXTURE_USAGE_RENDER_TARGET), nullptr);
	m_NormalBuffer		= m_Device->CreateTexture2D(Width, Height, FORMAT_R32G32B32A32_FLOAT, (TEXTURE_USAGE_SHADER_RESOURCE | TEXTURE_USAGE_RENDER_TARGET), nullptr);
	m_PositionBuffer	= m_Device->CreateTexture2D(Width, Height, FORMAT_R32G32B32A32_FLOAT, (TEXTURE_USAGE_SHADER_RESOURCE | TEXTURE_USAGE_RENDER_TARGET), nullptr);
	m_ParameterBuffer	= m_Device->CreateTexture2D(Width, Height, FORMAT_R32G32B32A32_FLOAT, (TEXTURE_USAGE_SHADER_RESOURCE | TEXTURE_USAGE_RENDER_TARGET), nullptr);

	m_MeshDataBuffer = m_Device->CreateConstantBuffer(GPU_BUFFER_MIN_SIZE);
	m_MaterialBuffer = m_Device->CreateConstantBuffer(GPU_BUFFER_MIN_SIZE);
	m_CameraBuffer	 = m_Device->CreateConstantBuffer(GPU_BUFFER_MIN_SIZE);
	m_LightBuffer	 = m_Device->CreateConstantBuffer(GPU_BUFFER_MIN_SIZE);

	const IRenderTargetView* Views[4] =
	{
		m_Device->CreateRenderTargetView(m_AlbedoBuffer),
		m_Device->CreateRenderTargetView(m_NormalBuffer),
		m_Device->CreateRenderTargetView(m_PositionBuffer),
		m_Device->CreateRenderTargetView(m_ParameterBuffer)
	};

	m_DefferedPass = m_Device->CreateRenderPass(&Views[0], 4, m_Device->GetDefaultDepthStencil(), float4(0.0f, 0.0f, 0.0f, 0.0f));

	{
		struct Vertex
		{
			struct { float x, y, z; };
			struct { float u, v; };
		};

		Vertex vertices[4] = {
			{ { -1.0f, -1.0f,  0.1f },{ 0.0f, 1.0f } },
			{ { 1.0f, -1.0f,  0.1f },{ 1.0f, 1.0f } },
			{ { 1.0f,  1.0f,  0.1f },{ 1.0f, 0.0f } },
			{ { -1.0f,  1.0f,  0.1f },{ 0.0f, 0.0f } }
		};

		uint32 indices[6] = {
			0, 3, 2,
			2, 1, 0
		};

		m_OnScreenQuad.VB = m_Device->CreateVertexBuffer(sizeof(vertices), (uint8*)vertices);
		m_OnScreenQuad.IB = m_Device->CreateIndexBuffer(sizeof(indices), (uint8*)indices);
		m_OnScreenQuad.Pipeline = GetShaderCache()->LoadGraphicsPipeline("DefferredLighting.gfx");
		m_OnScreenQuad.Table = m_Device->CreateShaderTable(m_OnScreenQuad.Pipeline);
		m_OnScreenQuad.PointSampler = m_Device->GetDefaultSamplerState();
		SAMPLER_DESC SamplerDesc = CreateDefaultSamplerDesc();
		m_OnScreenQuad.SkySampler = m_Device->CreateSamplerState(SamplerDesc);
	}
	
	m_Device->CreateShaderResourceView(m_OnScreenQuad.Table, m_AlbedoBuffer, 0);
	m_Device->CreateShaderResourceView(m_OnScreenQuad.Table, m_NormalBuffer, 1);
	m_Device->CreateShaderResourceView(m_OnScreenQuad.Table, m_PositionBuffer, 2);
	m_Device->CreateShaderResourceView(m_OnScreenQuad.Table, m_ParameterBuffer, 3);

	m_Device->CreateShaderResourceView(m_OnScreenQuad.Table, m_LightBuffer, 0);
	m_Device->CreateSamplerView(m_OnScreenQuad.Table, m_OnScreenQuad.SkySampler, 1);
}

void DefferedRenderer::Render()
{
	__debugbreak();
	Frame++;
	IGraphicsCommandContext* ctx = m_Device->GetGraphicsContext();
	GatherSceneData();
	ctx->BeginPass(m_DefferedPass);

	for (uint32 i = 0; i < m_DrawCommits.size(); i++)
	{
		auto& Commit = m_DrawCommits[i];
		for (uint32 b = 0; b < static_cast<uint32>(Commit.Meshes.size()); b++)
		{
			auto& DrawMesh = Commit.Meshes[b];
			ctx->SetGraphicsPipelineState(DrawMesh.Pipeline);
			ctx->SetGraphicsResourceTable(DrawMesh.Table);
			m_Device->CreateShaderResourceView(DrawMesh.Table, m_CameraBuffer, 0, Commit.CameraIdxOffset * sizeof(CAMERA_CONSTANT_BUFFER_DATA));
			m_Device->CreateShaderResourceView(DrawMesh.Table, m_MeshDataBuffer, 1, b * sizeof(MESH_RENDER_DATA));
			ctx->SetVertexBuffer(DrawMesh.VertexBuffer);
			ctx->SetIndexBuffer(DrawMesh.IndexBuffer);
			ctx->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			ctx->DrawIndexed(DrawMesh.IndexCount, 0, 0);
		}
	}
	bool bHasSkybox = RenderLoop::GRenderGlobals.SkyboxData.Cubemap != nullptr;
	if (bHasSkybox)
	{
// 		void* Buff = ctx->Map(m_CameraConstants);
// 		CameraConstants.Model = matTransformation(CameraPosition, Quaternion(), float3(5.f, 5.f, 5.f));
// 		memcpy(Buff, (void*)&CameraConstants, sizeof(DEFFERED_CAMERA_CONSTANTS));
// 		ctx->Unmap(m_CameraConstants);
// 		Skybox->SetCameraConstants(m_CameraConstants);
// 		Skybox->Draw(ctx);
	}
	ctx->EndPass(); // So right now its executing
	ctx->BeginPass(m_Device->GetBackBufferTargetPass());
	ctx->SetGraphicsPipelineState(m_OnScreenQuad.Pipeline);
	ctx->SetGraphicsResourceTable(m_OnScreenQuad.Table);
	ctx->SetVertexBuffer(m_OnScreenQuad.VB);
	ctx->SetIndexBuffer(m_OnScreenQuad.IB);
	ctx->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ctx->DrawIndexed(6, 0, 0);

	ctx->EndPass();
	RenderLoop::ResetForNextFrame();
	m_DrawCommits.clear();
}

void DefferedRenderer::Present()
{
	GetSwapChain()->Present();
}

void DefferedRenderer::Shutdown()
{
	delete m_OnScreenQuad.VB;
	delete m_OnScreenQuad.IB;
	delete m_OnScreenQuad.Table;
	delete m_OnScreenQuad.PointSampler;
	delete m_OnScreenQuad.SkySampler;
	delete m_MaterialBuffer;
	delete m_CameraBuffer;
	delete m_LightBuffer;
//	delete m_AlbedoBuffer;
//	delete m_NormalBuffer;
//	delete m_PositionBuffer;
//	delete m_ParameterBuffer;
//	delete m_DefferedPass;
//	delete m_CameraConstants;
	ApiRuntime::Shutdown();
}

IDeviceSwapChain* DefferedRenderer::GetSwapChain()
{
	return m_Device->GetSwapChain();
}

bool DefferedRenderer::SupportsAsyncContexts()
{
	EAPI Api = GetApiRuntime()->GetApiType();
	return (Api == API_D3D12 || Api == API_VULKAN);
}

void DefferedRenderer::Submit(const RenderLoop& InRenderLoop)
{
	for (auto& Commit : InRenderLoop.GetCommitedData())
	{
		m_DrawCommits.push_back(Commit);
	}
}

void DefferedRenderer::RenderShadows(matrix LightMatrix, IRenderPassInfo* DestRenderPass, IGraphicsCommandContext* ctx)
{
	UNUSED(LightMatrix);
	UNUSED(DestRenderPass);
	UNUSED(ctx);
}

void DefferedRenderer::GatherSceneData()
{
	{
		if (RenderLoop::GRenderGlobals.CameraData.Size > 0) 
		{
 			byte* Buff = reinterpret_cast<byte*>(m_CameraBuffer->Map());
			memcpy(Buff,
				reinterpret_cast<void*>(RenderLoop::GRenderGlobals.CameraData.Buffer),
				RenderLoop::GRenderGlobals.CameraData.Size * sizeof(CAMERA_CONSTANT_BUFFER_DATA)
			);
			m_CameraBuffer->Unmap();
		}
	}
	{
		if (RenderLoop::GRenderGlobals.MeshData.Size > 0)
		{
			byte* Buff = reinterpret_cast<byte*>(m_MeshDataBuffer->Map());
			memcpy(Buff,
				reinterpret_cast<void*>(RenderLoop::GRenderGlobals.MeshData.Buffer),
				RenderLoop::GRenderGlobals.MeshData.Size * sizeof(MESH_RENDER_DATA)
			);
			m_MeshDataBuffer->Unmap();
		}
	}
	{
		byte* Buff = reinterpret_cast<byte*>(m_LightBuffer->Map());
		memcpy(Buff,
			reinterpret_cast<void*>(&RenderLoop::GRenderGlobals.LightData),
			sizeof(RenderLoop::GRenderGlobals.LightData)
		);
		m_LightBuffer->Unmap();
	}
}

