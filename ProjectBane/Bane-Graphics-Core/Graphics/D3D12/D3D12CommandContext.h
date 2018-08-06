#pragma once

#include "../Interfaces/GraphicsCommandList.h"
#include "Core/Containers/StackQueue.h"
#include "D3D12GraphicsResource.h"

class D3D12ResourceLocation;
class D3D12GraphicsDevice;
class D3D12ShaderResourceTable;
class D3D12RenderPassInfo;

class D3D12CommandList
{
public:

	ID3D12CommandAllocator* CommandAllocator;
	ID3D12CommandList* CommandList;

	// TODO: Replace std::vector with my DynamicArray implementation, to prevent getting killed by the fact it resets its pointer when its cleared

	struct UploadResource
	{
		D3D12GPUResource* Resource;
		void Destroy()
		{
			delete Resource;
		}
	};

	struct DedicatedResource
	{
	public:
		D3D12GPUResource* Resource;
		void Uncommit()
		{
			Resource->Uncommit();
		}
	};

	std::vector<UploadResource> UploadResourcesToDestroy;
	std::vector<DedicatedResource> CommitedResources;
	bool bNeedsWaitForComputeQueue = false;
	bool bNeedsWaitForGraphicsQueue = false;

	D3D12CommandList(ID3D12CommandAllocator* InCommandAllocator, ID3D12CommandList* InCommandList) :
		CommandAllocator(InCommandAllocator),
		CommandList(InCommandList)
	{
	}

	inline void EnqueueUploadResourceToDestroy(D3D12GPUResource* Resource)
	{
		UploadResource Res;
		Res.Resource = Resource;
		UploadResourcesToDestroy.push_back(Res);
	}

	// See if this is even needed, because I imagine this incurs a significant perf cost
	inline void CommitResourceToExecution(D3D12GPUResource* Resource)
	{
		DedicatedResource Res;
		Res.Resource = Resource;
		CommitedResources.push_back(Res);
	}

	inline D3D12CommandList& operator = (const D3D12CommandList& Rhs)
	{
		CommandAllocator = Rhs.CommandAllocator;
		CommandList = Rhs.CommandList;
		return *this;
	}

	void FlushDestructionQueue();

	inline void FlushCommitQueue()
	{
		if (CommitedResources.empty())
		{
			return;
		}
		for (uint32 i = 0; i < CommitedResources.size(); i++)
		{
			CommitedResources[i].Uncommit();
		}
		CommitedResources.clear();
	}

	inline void Reset()
	{
		CommandAllocator->Reset();
		GetGraphicsCommandList()->Reset(CommandAllocator, nullptr);
	}

	inline void Close()
	{
		GetGraphicsCommandList()->Close();
	}

	inline ID3D12GraphicsCommandList* GetGraphicsCommandList()
	{
		return (ID3D12GraphicsCommandList*)CommandList;
	}

};

class D3D12GraphicsCommandContext : public IGraphicsCommandContext
{
public:

	D3D12GraphicsCommandContext(D3D12GraphicsDevice* InDevice, ECOMMAND_CONTEXT_TYPE InContextType, D3D12_RECT InRect, D3D12_VIEWPORT InViewport) :
		CommandList(nullptr),
		ParentDevice(InDevice),
		ContextType(InContextType),
		CurrentRenderPass(nullptr),
		ScissorRect(InRect),
		Viewport(InViewport)
	{ 
	}

	inline bool HasBegun() { return CommandList != nullptr; }
	virtual void BeginPass(IRenderPassInfo* InRenderPass) override final;
	virtual void EndPass() override final; // This will submit our handle and grab a new one

	virtual void Flush() override final;

	virtual void SetGraphicsPipelineState(const IGraphicsPipelineState* InPipelineState) final override;
	virtual void SetVertexBuffer(const IBuffer* InVertexBuffer) final override;
	virtual void SetIndexBuffer(const IBuffer* InIndexBuffer) final override;
	virtual void SetPrimitiveTopology(const EPRIMITIVE_TOPOLOGY InTopology) final override;

	virtual void SetGraphicsResourceTable(const IShaderResourceTable* InTable) final override;

	virtual void* Map(IGPUResource* InResource, uint32 Subresource = 0) final override;
	virtual void Unmap(IGPUResource* InResource, uint32 Subresource = 0) final override;
	
	virtual void Draw(uint32 VertexCount, uint32 StartVertexLocation) final override;
	virtual void DrawIndexed(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation) final override;
	virtual void DrawInstanced(uint32 VertexCount, uint32 InstanceCount, uint32 StartVertexLocation) final override;
	virtual void DrawIndexedInstanced(uint32 IndexCount, uint32 StartIndexLocation, int BaseVertexLocation, uint32 InstanceCount) final override;

	virtual void CopyBuffers(IBuffer* Src, IBuffer* Dst) final override;
	virtual void CopyBufferToTexture(IBuffer* Src, ITextureBase* Dst) final override;
	virtual void CopyTextures(ITextureBase* InSrc, uint32 SrcSubresource, ITexture2D* InDst, uint32 DstSubresource) final override;
	virtual void CopyTextures(ITextureBase* Src, int3 SrcLocation, ITextureBase* Dst, int3 DstLocation, int3 DstSize) final override;

	void FlushResourceTransitions();

	void CommitResources();

	bool bHasCheckedCurrentTable = false;
	ECOMMAND_CONTEXT_TYPE ContextType;
	D3D12CommandList* CommandList;
	D3D12GraphicsPipelineState* PipelineState;
	D3D12RenderPassInfo* CurrentRenderPass;
	ID3D12GraphicsCommandList* D3DCL;
	D3D12ShaderResourceTable* CurrentTable;
	ID3D12RootSignature* RootSignature;
	D3D12GraphicsDevice* ParentDevice;
	StackQueue<D3D12_RESOURCE_BARRIER, 32> PendingTransitions;
	StackQueue<D3D12GPUResource*, 32> TransitionedResources;
	D3D12_RECT ScissorRect;
	D3D12_VIEWPORT Viewport;
};

class D3D12ComputeCommandContext : public IComputeCommandContext
{
public:

	D3D12ComputeCommandContext(D3D12GraphicsDevice* Device) :
		CommandList(nullptr),
		ParentDevice(Device)
	{
	}

	inline bool HasBegun() const { return CommandList != nullptr; }

	virtual void Begin() final override;
	virtual void End() final override;

	virtual void StallToEnd() final override;
	virtual void Flush() final override { StallToEnd(); }

	virtual void* Map(IGPUResource* ResourceToMap, uint32 Subresource = 0) final override;
	virtual void Unmap(IGPUResource* ResourceToUnmap, uint32 Subresource = 0) final override;

	virtual void SetComputePipelineState(const IComputePipelineState* InState) final override;
	virtual void SetComputeResourceTable(const IShaderResourceTable* InTable) final override;

	virtual void Dispatch(uint32 ThreadX, uint32 ThreadY, uint32 ThreadZ) final override;

	void CommitResources();

	void FlushResourceTransitions();

	bool bHasCheckedCurrentTable = false;
	ID3D12GraphicsCommandList* D3DCL;
	D3D12CommandList* CommandList;
	D3D12ComputePipelineState* PipelineState;
	D3D12ShaderResourceTable* CurrentTable;
	ID3D12RootSignature* RootSignature;
	D3D12GraphicsDevice* ParentDevice;
	StackQueue<D3D12_RESOURCE_BARRIER, 32> PendingTransitions;
	StackQueue<D3D12GPUResource*, 32> TransitionedResources;
};
