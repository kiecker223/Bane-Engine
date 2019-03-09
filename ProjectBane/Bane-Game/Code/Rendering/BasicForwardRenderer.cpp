#include "BasicForwardRenderer.h"
#include <GraphicsCore/IO/ShaderCache.h>
#include <GraphicsCore/IO/TextureCache.h>

void BasicForwardRenderer::Initialize(const Window* pWindow)
{
	UNUSED(pWindow);
	m_Device = GetApiRuntime()->GetGraphicsDevice();
	m_ImmediateGeometryPS = GetShaderCache()->LoadGraphicsPipeline("ImmediateModeGeometry.gfx");
	m_LightBuffer = m_Device->CreateConstantBuffer(GPU_BUFFER_MIN_SIZE);
	m_CameraConstants = m_Device->CreateConstantBuffer(GPU_BUFFER_MIN_SIZE);
	m_MeshDataBuffer = m_Device->CreateConstantBuffer(GPU_BUFFER_MIN_SIZE);
	m_LightBuffer = m_Device->CreateConstantBuffer(sizeof(RenderLoop::GRenderGlobals.LightData));
}

void BasicForwardRenderer::StartRenderThread()
{
	RenderThreadHandle = std::move(std::thread([this]()
	{
		m_bIsRendering = true;
		Render();
	}));
}

void BasicForwardRenderer::Render()
{
}

void BasicForwardRenderer::Present()
{
	GetSwapChain()->Present();
}

void BasicForwardRenderer::Shutdown()
{
	m_bIsRendering = false;
}

void BasicForwardRenderer::Submit(RenderLoop& pRenderLoop)
{
	//std::lock_guard<std::mutex> RenderLock(m_RenderSubmitLock);
	m_pRenderLoop = &pRenderLoop;
}

void BasicForwardRenderer::GatherSceneData()
{
	{
		MESH_RENDER_DATA* pData = m_MeshDataBuffer->MapT<MESH_RENDER_DATA>();
		auto& Buckets = m_pRenderLoop->Bucket.Values;
		for (uint32 x = 0; x < Buckets.size(); x++)
		{
			auto& CurBucket = Buckets[x];
			for (uint32 i = 0; i < CurBucket.size(); i++)
			{
				*pData = CurBucket[i].Data;
				pData++;
			}
		}
		m_MeshDataBuffer->Unmap();
	}
}
