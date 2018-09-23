#include "BasicForwardRenderer.h"

void BasicForwardRenderer::Initialize(const Window* pWindow)
{
	UNUSED(pWindow);
	m_Commits.reserve(100);
	m_Device = GetApiRuntime()->GetGraphicsDevice();

	m_LightBuffer = m_Device->CreateConstantBuffer(GPU_BUFFER_MIN_SIZE);
	m_CameraConstants = m_Device->CreateConstantBuffer(GPU_BUFFER_MIN_SIZE);
	m_MeshDataBuffer = m_Device->CreateConstantBuffer(GPU_BUFFER_MIN_SIZE);
	m_LightBuffer = m_Device->CreateConstantBuffer(sizeof(RenderLoop::GRenderGlobals.LightData));
}

void BasicForwardRenderer::Render()
{
	IGraphicsCommandContext* ctx = m_Device->GetGraphicsContext();

	ctx->BeginPass(m_Device->GetBackBufferTargetPass());
	GatherSceneData(ctx);
	for (uint32 i = 0; i < m_Commits.size(); i++)
	{
		auto& Commit = m_Commits[i];
		for (uint32 y = 0; y < Commit.Meshes.size(); y++)
		{
			auto& DrawMesh = Commit.Meshes[y];
			ctx->SetGraphicsPipelineState(DrawMesh.Pipeline);			
			m_Device->CreateShaderResourceView(DrawMesh.Table, m_CameraConstants, 0, Commit.CameraIdxOffset * sizeof(CAMERA_CONSTANT_BUFFER_DATA));
			m_Device->CreateShaderResourceView(DrawMesh.Table, m_MeshDataBuffer, 1, y * sizeof(MESH_RENDER_DATA));
			m_Device->CreateShaderResourceView(DrawMesh.Table, m_LightBuffer, 2);
			ctx->SetGraphicsResourceTable(DrawMesh.Table);
			ctx->SetVertexBuffer(DrawMesh.VertexBuffer);
			ctx->SetIndexBuffer(DrawMesh.IndexBuffer);
			ctx->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			ctx->DrawIndexed(DrawMesh.IndexCount, 0, 0);
		}
	}

	ctx->EndPass();
	RenderLoop::ResetForNextFrame();
	m_Commits.clear();
}

void BasicForwardRenderer::Present()
{
	GetSwapChain()->Present();
}

void BasicForwardRenderer::Shutdown()
{
	ApiRuntime::Shutdown();
}

void BasicForwardRenderer::Submit(const RenderLoop& pRenderLoop)
{
	for (auto& Commit : pRenderLoop.GetCommitedData())
		m_Commits.push_back(Commit);
}

void BasicForwardRenderer::GatherSceneData(IGraphicsCommandContext* ctx)
{
	{
		if (RenderLoop::GRenderGlobals.CameraData.Size > 0)
		{
			byte* Buff = reinterpret_cast<byte*>(ctx->Map(m_CameraConstants));
			memcpy(Buff,
				reinterpret_cast<void*>(RenderLoop::GRenderGlobals.CameraData.Buffer),
				RenderLoop::GRenderGlobals.CameraData.Size * sizeof(CAMERA_CONSTANT_BUFFER_DATA)
			);
			ctx->Unmap(m_CameraConstants);
		}
	}
	{
		if (RenderLoop::GRenderGlobals.MeshData.Size > 0)
		{
			byte* Buff = reinterpret_cast<byte*>(ctx->Map(m_MeshDataBuffer));
			memcpy(Buff,
				reinterpret_cast<void*>(RenderLoop::GRenderGlobals.MeshData.Buffer),
				RenderLoop::GRenderGlobals.MeshData.Size * sizeof(MESH_RENDER_DATA)
			);
			ctx->Unmap(m_MeshDataBuffer);
		}
	}
	{
		byte* Buff = reinterpret_cast<byte*>(ctx->Map(m_LightBuffer));
		memcpy(Buff,
			reinterpret_cast<void*>(&RenderLoop::GRenderGlobals.LightData),
			sizeof(RenderLoop::GRenderGlobals.LightData)
		);
		ctx->Unmap(m_LightBuffer);
	}
}
