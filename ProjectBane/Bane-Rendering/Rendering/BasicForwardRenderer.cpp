#include "BasicForwardRenderer.h"
#include <Graphics/IO/ShaderCache.h>
#include <Graphics/IO/TextureCache.h>

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
	//std::lock_guard<std::mutex> RenderLock(m_RenderSubmitLock);
	IGraphicsCommandContext* ctx = m_Device->GetGraphicsContext();
	ctx->BeginPass(m_Device->GetBackBufferTargetPass());
	GatherSceneData();
	
	for (uint32 d = 0; d < m_pRenderLoop->CameraStack.GetNumElements(); d++)
	{
		uint32 Index = 0;
		CAMERA_DATA Data = m_pRenderLoop->CameraStack.Pop();
		{
			CAMERA_CBUFFER_DATA* pData = m_CameraConstants->MapT<CAMERA_CBUFFER_DATA>();
			pData[d] = GetCBufferCameraDataFromAcquireData(Data);
			auto TestData = pData[d];
			UNUSED(TestData);
			m_CameraConstants->Unmap();
		}
		for (uint32 x = 0; x < m_pRenderLoop->Bucket.Values.size(); x++)
		{
			ctx->SetConstantBuffer(0, m_CameraConstants, sizeof(CAMERA_DATA) * d);
			ctx->SetGraphicsPipelineState(m_pRenderLoop->Bucket.Keys[x].pShader);
			for (uint32 i = 0; i < m_pRenderLoop->Bucket.Values[x].size(); i++)
			{
				auto& Item = m_pRenderLoop->Bucket.Values[x][i];
				ctx->SetVertexBuffer(Item.pMesh->GetVertexBuffer());
				ctx->SetIndexBuffer(Item.pMesh->GetIndexBuffer());
				ctx->SetConstantBuffer(1, m_MeshDataBuffer, sizeof(MESH_RENDER_DATA) * Index);
				ctx->SetTexture(0, Item.DiffuseTex);
				ctx->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				ctx->DrawIndexed(Item.pMesh->GetIndexCount(), 0, 0);
				Index++;
			}
		}
	}

	for (uint32 i = 0; i < m_pRenderLoop->PostRenderCallback.GetNumElements(); i++)
	{
		auto& Callback = m_pRenderLoop->PostRenderCallback.Pop();
		Callback(m_Device, ctx);
	}

	ctx->EndPass();
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
