#pragma once

#include "GraphicsResources.h"
#include "PipelineState.h"
#include "ShaderResourceView.h"



// Purely internal use
typedef enum ECOMMAND_CONTEXT_TYPE {
	COMMAND_CONTEXT_TYPE_INVALID = -1,
	COMMAND_CONTEXT_TYPE_GRAPHICS = 0,
	COMMAND_CONTEXT_TYPE_COMPUTE = 1,
	COMMAND_CONTEXT_TYPE_COPY = 2,
	COMMAND_CONTEXT_TYPE_NUM_TYPES = 3
} ECOMMAND_CONTEXT_TYPE;

typedef enum EPRIMITIVE_TOPOLOGY {
	PRIMITIVE_TOPOLOGY_POINTLIST,
	PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	PRIMITIVE_TOPOLOGY_TRIANGLESTRIPS
} EPRIMITIVE_TOPOLOGY;

class IGraphicsCommandContext
{
public:
	virtual ~IGraphicsCommandContext() { }

	virtual void BeginPass(IRenderPassInfo* RenderPass) = 0;
	virtual void EndPass() = 0; // This will submit the workload for execution, no need to manually release it, it will be taken care of on a per-thread basis

	virtual void Flush() = 0; // Stall until execution is finished

	virtual void Begin() { BeginPass(nullptr); } // No graphics tasks are allowed to be done
	virtual void End() { EndPass(); }

	virtual void SetGraphicsPipelineState(const IGraphicsPipelineState* PipelineState) = 0;
	virtual void SetVertexBuffer(const IVertexBuffer* VertexBuffer) = 0;
	virtual void SetIndexBuffer(const IIndexBuffer* IndexBuffer) = 0;
	virtual void SetPrimitiveTopology(const EPRIMITIVE_TOPOLOGY Topology) = 0;

	virtual void SetGraphicsResourceTable(const IShaderResourceTable* InTable) = 0;

	virtual void CopyBuffers(IBuffer* Src, IBuffer* Dst) = 0;
	virtual void CopyBufferToTexture(IBuffer* Src, ITextureBase* Dst) = 0;
	virtual void CopyTextures(ITextureBase* Src, uint32 SrcSubresource, ITextureBase* Dst, uint32 DstSubresource) = 0;
	virtual void CopyTextures(ITextureBase* Src, int3 SrcLocation, ITextureBase* Dst, int3 DstLocation, int3 DstSize) = 0;

	virtual void Draw(uint32 VertexCount, uint32 StartVertexLocation) = 0;
	virtual void DrawIndexed(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation) = 0;
	virtual void DrawInstanced(uint32 VertexCount, uint32 InstanceCount, uint32 StartVertexLocation) = 0;
	virtual void DrawIndexedInstanced(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation, uint32 InstanceCount) = 0;

	virtual void SetComputePipelineState(const IComputePipelineState* PipelineState) = 0;
	virtual void SetComputeResourceTable(const IShaderResourceTable* InTable) = 0;

	virtual void Dispatch(uint32 ThreadX, uint32 ThreadY, uint32 ThreadZ) = 0;
};


class IComputeCommandContext
{
public:
	virtual ~IComputeCommandContext() { }

	virtual void Begin() = 0;
	virtual void End() = 0;

	virtual void StallToEnd() = 0;
	virtual void Flush() = 0;

	virtual void SetComputePipelineState(const IComputePipelineState* PipelineState) = 0;
	virtual void SetComputeResourceTable(const IShaderResourceTable* InTable) = 0;

	virtual void Dispatch(uint32 ThreadX, uint32 ThreadY, uint32 ThreadZ) = 0;
};

class IGraphicsCommand
{
public:
	virtual void Execute(IGraphicsCommandContext* Context) = 0;
};

class GraphicsCommandBuffer : public IGraphicsCommand
{
public:
	GraphicsCommandBuffer();
	GraphicsCommandBuffer(const GraphicsCommandBuffer& Other);

	void Execute(IGraphicsCommandContext* Context) final override
	{
		for (IGraphicsCommand*& Command : m_Commands)
		{
			Command->Execute(Context);
		}
	}

	void BeginPass(IRenderPassInfo* RenderPass);
	void EndPass();

	void Flush();

	void Begin() { BeginPass(nullptr); } // No graphics tasks are allowed to be done
	void End() { EndPass(); }

	void SetGraphicsPipelineState(const IGraphicsPipelineState* PipelineState);
	void SetVertexBuffer(const IVertexBuffer* VertexBuffer);
	void SetIndexBuffer(const IIndexBuffer* IndexBuffer);
	void SetPrimitiveTopology(const EPRIMITIVE_TOPOLOGY Topology);

	void SetGraphicsResourceTable(const IShaderResourceTable* InTable);

	void CopyBuffers(IBuffer* Src, IBuffer* Dst);
	void CopyBufferToTexture(IBuffer* Src, ITextureBase* Dst);
	void CopyTextures(ITextureBase* Src, uint32 SrcSubresource, ITextureBase* Dst, uint32 DstSubresource);
	void CopyTextures(ITextureBase* Src, int3 SrcLocation, ITextureBase* Dst, int3 DstLocation, int3 DstSize);

	void Draw(uint32 VertexCount, uint32 StartVertexLocation);
	void DrawIndexed(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation);
	void DrawInstanced(uint32 VertexCount, uint32 InstanceCount, uint32 StartVertexLocation);
	void DrawIndexedInstanced(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation, uint32 InstanceCount);

	void SetComputePipelineState(const IComputePipelineState* PipelineState);
	void SetComputeResourceTable(const IShaderResourceTable* InTable);

	void Dispatch(uint32 ThreadX, uint32 ThreadY, uint32 ThreadZ);

	template<typename T, class... U>
	inline void AllocateCommand(U&& ...Args)
	{
		m_Commands.push_back(new ((void*)Allocate(sizeof(T))) T(std::forward<U...>(Args...)));
	}

	byte* Allocate(uint32 NewOffset);

	byte* PtrStart = nullptr;
	byte* PtrCurrent = nullptr;
	byte* PtrEnd = nullptr;
	std::vector<IGraphicsCommand*> m_Commands;
};
