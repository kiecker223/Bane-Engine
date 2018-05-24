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
	virtual void CopyTextures(ITextureBase* Src, uint SrcSubresource, ITextureBase* Dst, uint DstSubresource) = 0;
	virtual void CopyTextures(ITextureBase* Src, XMINT3 SrcLocation, ITextureBase* Dst, XMINT3 DstLocation, XMINT3 DstSize) = 0;

	virtual void* Map(IGPUResource* ResourceToMap, uint Subresource = 0) = 0;
	virtual void Unmap(IGPUResource* ResourceToMap, uint Subresource = 0) = 0;

	virtual void Draw(uint VertexCount, uint StartVertexLocation) = 0;
	virtual void DrawIndexed(uint IndexCount, uint StartIndexLocation, int BaseVertexLocation) = 0;
	virtual void DrawInstanced(uint VertexCount, uint InstanceCount, uint StartVertexLocation) = 0;
	virtual void DrawIndexedInstanced(uint IndexCount, uint StartIndexLocation, int BaseVertexLocation, uint InstanceCount) = 0;
};

class IComputeCommandContext
{
public:
	virtual ~IComputeCommandContext() { }

	virtual void Begin() = 0;
	virtual void End() = 0;

	virtual void StallToEnd() = 0;
	virtual void Flush() = 0;
	virtual void* Map(IGPUResource* ResourceToMap, uint Subresource = 0) = 0;
	virtual void Unmap(IGPUResource* ResourceToUnmap, uint Subresource = 0) = 0;

	virtual void SetComputePipelineState(const IComputePipelineState* PipelineState) = 0;
	virtual void SetComputeResourceTable(const IShaderResourceTable* InTable) = 0;

	virtual void Dispatch(uint ThreadX, uint ThreadY, uint ThreadZ) = 0;
};
