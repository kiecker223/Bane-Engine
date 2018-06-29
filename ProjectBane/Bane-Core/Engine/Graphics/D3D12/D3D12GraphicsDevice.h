#pragma once

#include "../Interfaces/GraphicsDevice.h"
#include "D3D12SwapChain.h"
#include "D3D12CommandQueue.h"
#include "D3D12CommandContext.h"
#include "D3D12ShaderResourceViews.h"
#include "D3D12DescriptorAllocator.h"


class D3D12GraphicsDevice : public IRuntimeGraphicsDevice
{
	friend class D3D12CommandQueue;
	friend class D3D12GraphicsCommandContext;
	friend class D3D12ComputeCommandContext;

public:
	D3D12GraphicsDevice(D3D12SwapChain* SwapChain, Window* RenderingWindow, ID3D12Device1* Device, ID3D12CommandQueue* MainQueue);
	~D3D12GraphicsDevice();

	virtual IGraphicsCommandContext* CreateCommandContext(ECOMMAND_CONTEXT_TYPE Type) final override;
	virtual IGraphicsCommandContext* GetGraphicsContext() final override;

	virtual IComputeCommandContext* GetComputeContext() final override;

	virtual IVertexShader* CreateVertexShader(const std::string& ByteCode) final override;
	virtual IPixelShader* CreatePixelShader(const std::string& ByteCode) final override;
	virtual IGeometryShader* CreateGeometryShader(const std::string& ByteCode) final override;
	virtual IHullShader* CreateHullShader(const std::string& ByteCode) final override;
	virtual IComputeShader* CreateComputeShader(const std::string& ByteCode) final override;

	virtual IGraphicsPipelineState* CreatePipelineState(const GFX_PIPELINE_STATE_DESC* Desc) final override;
	virtual IComputePipelineState* CreatePipelineState(const COMPUTE_PIPELINE_STATE_DESC* Desc) final override;

	virtual void RecompilePipelineState(IGraphicsPipelineState* pState, const GFX_PIPELINE_STATE_DESC* NewDesc) final override;
	virtual void RecompilePipelineState(IComputePipelineState* pState, const COMPUTE_PIPELINE_STATE_DESC* NewDesc) final override;

	virtual IVertexBuffer* CreateVertexBuffer(uint ByteCount, uint8* Buffer) final override;
	virtual IIndexBuffer* CreateIndexBuffer(uint ByteCount, uint8* Buffer) final override;
	virtual IConstantBuffer* CreateConstantBuffer(uint ByteCount) final override;
	virtual IBuffer* CreateStagingBuffer(uint ByteCount) final override;

	virtual ITexture2D* CreateTexture2D(uint Width, uint Height, EFORMAT Format, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data) final override;
	virtual ITexture2DArray* CreateTexture2DArray(uint Width, uint Height, uint Count, EFORMAT Format, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data) final override;
	virtual ITexture3D* CreateTexture3D(uint Width, uint Height, uint Depth, EFORMAT Format, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data) final override;
	virtual ITextureCube* CreateTextureCube(uint CubeSize, EFORMAT Format, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data) final override;

	virtual void GenerateMips(ITextureBase* InTexture) final override;

	virtual ISamplerState* CreateSamplerState(const SAMPLER_DESC& Desc) final override;
	virtual ISamplerState* GetDefaultSamplerState() final override;
	virtual IInputLayout* CreateInputLayout(const GFX_INPUT_LAYOUT_DESC& Desc) final override;

	virtual IRenderPassInfo* CreateRenderPass(const IRenderTargetView** RenderTargets, uint NumRenderTargets, const IDepthStencilView* DepthStencil, const float4& ClearColor) final override;
	virtual IRenderPassInfo* GetBackBufferTargetPass() final override;
	virtual IRenderTargetView* GetBackBuffer() final override;

	virtual void CreateShaderResourceView(IShaderResourceTable* InDestTable, IBuffer* Buffer, uint Slot, uint Subresource = 0) final override;
	virtual void CreateUnorderedAccessView(IShaderResourceTable* InDestTable, IBuffer* Buffer, uint Slot, uint Subresource = 0) final override;
	virtual void CreateShaderResourceView(IShaderResourceTable* InDestTable, ITextureBase* Texture, uint Slot, uint Subresource = 0) final override;
	virtual void CreateUnorderedAccessView(IShaderResourceTable* InDestTable, ITextureBase* Texture, uint Slot, uint Subresource = 0) final override;
	virtual void CreateSamplerView(IShaderResourceTable* DestTable, ISamplerState* SamplerState, uint Slot) final override;
	virtual IShaderResourceTable* CreateShaderTable(IVertexShader* VS, IPixelShader* PS, IHullShader* HS = nullptr, IGeometryShader* GS = nullptr) final override;
	virtual IShaderResourceTable* CreateShaderTable(IComputeShader* CS) final override;

	virtual IRenderTargetView* CreateRenderTargetView(ITexture2D* InTexture) final override;
	virtual IDepthStencilView* CreateDepthStencilView(ITexture2D* InTexture) final override;
	virtual IDepthStencilState* CreateDepthStencilState(const GFX_DEPTH_STENCIL_DESC& InDesc) final override;

	virtual IDeviceSwapChain* GetSwapChain() final override;

	virtual EMULTISAMPLE_LEVEL GetMaximumMultisampleLevel() const final override;

	ForceInline ID3D12Device1* GetDevice() { return m_Device; }
	
	ForceInline D3D12GraphicsCommandContext* GrabUploadContextForThread() { return m_UploadList; }

	// This queue controls all of the drawing tasks
	ForceInline D3D12CommandQueue& GetMainCommandQueue() { return GetCommandQueue(COMMAND_CONTEXT_TYPE_GRAPHICS); }

	// This queue controls all of the async compute tasks
	ForceInline D3D12CommandQueue& GetComputeQueue() { return GetCommandQueue(COMMAND_CONTEXT_TYPE_COMPUTE); }

	// This queue controls all the upload operations
	ForceInline D3D12CommandQueue& GetCopyQueue() { return GetCommandQueue(COMMAND_CONTEXT_TYPE_COPY); }


	D3D12CommandList* GetCommandList(ECOMMAND_CONTEXT_TYPE ContextType);

	void UpdateCurrentFrameInfo();

	void EnsureAllUploadsOccured();

	static uint GetCurrentFrameIndex()
	{
		return GCurrentFrameIndex;
	}

	ForceInline D3D12CommandQueue& GetCommandQueue(ECOMMAND_CONTEXT_TYPE ContextType)
	{
		return m_CommandQueues[ContextType];
	}

private:

	static uint GCurrentFrameIndex;

	StackQueue<D3D12CommandList*, 16> m_AvailableCLs[COMMAND_CONTEXT_TYPE_NUM_TYPES];
	std::vector<D3D12GraphicsCommandContext*> m_AvailableContexts;
	D3D12ComputeCommandContext* m_ComputeContext;
	D3D12ComputePipelineState* m_GenerateMipsPipeline2D;
	D3D12ShaderResourceTable* m_GenerateMipsTable2D;
	D3D12GraphicsCommandContext* m_UploadList;
	D3D12CommandQueue m_CommandQueues[COMMAND_CONTEXT_TYPE_NUM_TYPES];
	D3D12DescriptorAllocator m_SrvAllocator;
	D3D12DescriptorAllocator m_DsvAllocator;
	D3D12DescriptorAllocator m_RtvAllocator;
	D3D12DescriptorAllocator m_SmpAllocator;
	IRenderTargetView* m_BackBuffer;
	D3D12_RECT m_Rect;
	D3D12_VIEWPORT m_ViewPort;
	EMULTISAMPLE_LEVEL m_MaxMultisampleLevel;
	ISamplerState* m_DefaultSamplerState;
	D3D12SwapChain* m_SwapChain;
	D3D12RenderPassInfo* m_BasicRenderPass;
	ID3D12Device1* m_Device;
};


