#include "BasicForwardRenderer.h"
#include <Graphics/IO/ShaderCache.h>

void BasicForwardRenderer::Initialize(const Window* pWindow)
{
	UNUSED(pWindow);
	m_Commits.CheckGrow(100);
	m_Device = GetApiRuntime()->GetGraphicsDevice();
	m_ImmediateGeometryPS = GetShaderCache()->LoadGraphicsPipeline("ImmediateModeGeometry.gfx");
	m_ImmediateGeometryTable = m_Device->CreateShaderTable(m_ImmediateGeometryPS);
	m_LightBuffer = m_Device->CreateConstantBuffer(GPU_BUFFER_MIN_SIZE);
	m_CameraConstants = m_Device->CreateConstantBuffer(GPU_BUFFER_MIN_SIZE);
	m_MeshDataBuffer = m_Device->CreateConstantBuffer(GPU_BUFFER_MIN_SIZE);
	m_LightBuffer = m_Device->CreateConstantBuffer(sizeof(RenderLoop::GRenderGlobals.LightData));
}

void BasicForwardRenderer::Render()
{
	IGraphicsCommandContext* ctx = m_Device->GetGraphicsContext();

	ctx->BeginPass(m_Device->GetBackBufferTargetPass());
	GatherSceneData();
	for (uint32 i = 0; i < m_Commits.GetElementCount(); i++)
	{
		auto& Commit = m_Commits[i];
		for (uint32 y = 0; y < Commit.Meshes.GetElementCount(); y++)
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
	{
		auto& DrawArgs = RenderLoop::GRenderGlobals.ImmediateGeometry.DrawArgs;
		if (!DrawArgs.IsEmpty())
		{
			for (uint32 i = 0; i < DrawArgs.GetElementCount(); i++)
			{
				ctx->CopyBuffers(DrawArgs[i].UploadBuffer, DrawArgs[i].VertexBuffer);
			}
			for (uint32 i = 0; i < DrawArgs.GetElementCount(); i++)
			{
				auto& Obj = DrawArgs[i];
				ctx->SetGraphicsPipelineState(m_ImmediateGeometryPS);
				m_Device->CreateShaderResourceView(m_ImmediateGeometryTable, m_CameraConstants, 0);
				ctx->SetGraphicsResourceTable(m_ImmediateGeometryTable);
				ctx->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_LINES);
				ctx->SetVertexBuffer(Obj.VertexBuffer);
				ctx->Draw(Obj.VertexCount, 0);
			}
		}
	}
	ctx->EndPass();
	RenderLoop::ResetForNextFrame();
	m_Commits.Empty();
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
		m_Commits.Add(Commit);
}

void BasicForwardRenderer::GatherSceneData()
{
	{
		if (RenderLoop::GRenderGlobals.CameraData.Size > 0)
		{
			byte* Buff = reinterpret_cast<byte*>(m_CameraConstants->Map());
			memcpy(Buff,
				reinterpret_cast<void*>(RenderLoop::GRenderGlobals.CameraData.Buffer),
				RenderLoop::GRenderGlobals.CameraData.Size * sizeof(CAMERA_CONSTANT_BUFFER_DATA)
			);
			m_CameraConstants->Unmap();
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
