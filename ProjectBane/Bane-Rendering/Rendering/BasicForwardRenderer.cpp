#include "BasicForwardRenderer.h"
#include <Graphics/IO/ShaderCache.h>
#include <Graphics/IO/TextureCache.h>

void BasicForwardRenderer::Initialize(const Window* pWindow)
{
	UNUSED(pWindow);
	m_Commits.reserve(100);
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
	if (!m_Commits.empty())
	{
		for (uint32 i = 0; i < m_Commits.size(); i++)
		{
			auto& Commit = m_Commits[i];
			for (uint32 y = 0; y < Commit.Meshes.size(); y++)
			{
				auto& DrawMesh = Commit.Meshes[y];
				ctx->SetGraphicsPipelineState(DrawMesh.Pipeline);
				ctx->SetConstantBuffer(0, m_CameraConstants, Commit.CameraIdxOffset * sizeof(CAMERA_CONSTANT_BUFFER_DATA));
				ctx->SetConstantBuffer(1, m_MeshDataBuffer, y * sizeof(MESH_RENDER_DATA));
				ctx->SetConstantBuffer(2, m_LightBuffer);
				ctx->SetTexture(0, DrawMesh.Diffuse);
				ctx->SetVertexBuffer(DrawMesh.VertexBuffer);
				ctx->SetIndexBuffer(DrawMesh.IndexBuffer);
				ctx->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				ctx->DrawIndexed(DrawMesh.IndexCount, 0, 0);
			}
		}
		{
			auto& IG = RenderLoop::GRenderGlobals.ImmediateGeometry;
			auto& DrawArgs = IG.DrawArgs;
			if (IG.CurrentCount)
			{
				uint64 ByteOffsetForVertexBuffer = 0;
				for (uint32 i = 0; i < DrawArgs.size(); i++)
				{
					uint64 SizeInBytes = static_cast<uint64>(DrawArgs[i].UploadBuffer->GetSizeInBytes());
					ctx->CopyBufferLocations(DrawArgs[i].UploadBuffer, 0, IG.VertexBuffer, ByteOffsetForVertexBuffer, SizeInBytes);
					ByteOffsetForVertexBuffer += SizeInBytes;
				}
				ByteOffsetForVertexBuffer = 0;
				for (uint32 i = 0; i < IG.CurrentCount; i++)
				{
					auto& Obj = DrawArgs[i];
					ctx->SetGraphicsPipelineState(m_ImmediateGeometryPS);
					ctx->SetConstantBuffer(0, m_CameraConstants);
					ctx->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_LINES);
					ctx->SetVertexBuffer(IG.VertexBuffer, ByteOffsetForVertexBuffer);
					ctx->Draw(Obj.VertexCount, 0);
					ByteOffsetForVertexBuffer += Obj.UploadBuffer->GetSizeInBytes();
				}
			}
		}
	}
	ctx->EndPass();
	RenderLoop::ResetForNextFrame();
}

void BasicForwardRenderer::Present()
{
	GetSwapChain()->Present();
}

void BasicForwardRenderer::Shutdown()
{
	m_bIsRendering = false;
}

void BasicForwardRenderer::Submit(const RenderLoop& pRenderLoop)
{
	//std::lock_guard<std::mutex> RenderLock(m_RenderSubmitLock);
	m_Commits.clear();
	for (auto& Commit : pRenderLoop.GetCommitedData())
		m_Commits.push_back(Commit);
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
