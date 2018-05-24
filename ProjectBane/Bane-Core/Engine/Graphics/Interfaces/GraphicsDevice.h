#pragma once
#include "Core/ExpanseMath.h"
#include "PipelineState.h"
#include "GraphicsResources.h"
#include "GraphicsCommandList.h"
#include "SwapChainInterface.h"
#include "ShaderResourceView.h"


enum EGENERATE_MIPS_MODE
{
	NO_MIPS,
	GENERATE_MIPS
};



class IRuntimeGraphicsDevice
{
public:

	virtual ~IRuntimeGraphicsDevice() { }

	// This is here in case someone absolutely knows what they are doing and absolutely positively need a new context
	virtual IGraphicsCommandContext* CreateCommandContext(ECOMMAND_CONTEXT_TYPE Type) = 0;

	// For opengl, d3d11, and d3d9 this will just return a normal command list essentially
	// However for d3d12 and vulkan it will genuinely get an available command list
	virtual IGraphicsCommandContext* GetGraphicsContext() = 0;
	virtual IComputeCommandContext* GetComputeContext() = 0;

	// Due to some of how directx 12 and opengl work, these have to be the raw code, then the compiled forms are stored in the IShader interface
	virtual IVertexShader* CreateVertexShader(const std::string& ByteCode) = 0;
	virtual IPixelShader* CreatePixelShader(const std::string& ByteCode) = 0;
	virtual IGeometryShader* CreateGeometryShader(const std::string& ByteCode) = 0;
	virtual IHullShader* CreateHullShader(const std::string& ByteCode) = 0;
	virtual IComputeShader* CreateComputeShader(const std::string& ByteCode) = 0;

	virtual IGraphicsPipelineState* CreatePipelineState(const GFX_PIPELINE_STATE_DESC* Desc) = 0;
	virtual IComputePipelineState* CreatePipelineState(const COMPUTE_PIPELINE_STATE_DESC* Desc) = 0;

	virtual void RecompilePipelineState(IGraphicsPipelineState* pState, const GFX_PIPELINE_STATE_DESC* NewDesc) = 0;
	virtual void RecompilePipelineState(IComputePipelineState* pState, const COMPUTE_PIPELINE_STATE_DESC* NewDesc) = 0;

	virtual IVertexBuffer* CreateVertexBuffer(uint ByteCount, uint8* Buffer) = 0;
	virtual IIndexBuffer* CreateIndexBuffer(uint ByteCount, uint8* Buffer) = 0;
	virtual IConstantBuffer* CreateConstantBuffer(uint ByteCount) = 0;
	virtual IBuffer* CreateStagingBuffer(uint ByteCount) = 0;
	
	template<typename T>
	ForceInline IConstantBuffer* CreateConstBuffer()
	{
		return CreateConstantBuffer(sizeof(T));
	}

	ForceInline const IDepthStencilView* GetDefaultDepthStencil()
	{
		return GetBackBufferTargetPass()->GetDepthStencil();
	}
	
	virtual ITexture2D* CreateTexture2D(uint Width, uint Height, EFORMAT Format, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data) = 0;
	virtual ITexture2DArray* CreateTexture2DArray(uint Width, uint Height, uint Count, EFORMAT Format, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data) = 0;
	virtual ITexture3D* CreateTexture3D(uint Width, uint Height, uint Depth, EFORMAT Format, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data) = 0;
	virtual ITextureCube* CreateTextureCube(uint CubeSize, EFORMAT Format, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data) = 0;

	// @Warn! This is an extremely slow function in directx 12!!
	virtual ISamplerState* CreateSamplerState(const SAMPLER_DESC& Desc) = 0;
	virtual ISamplerState* GetDefaultSamplerState() = 0;
	
	virtual IInputLayout* CreateInputLayout(const GFX_INPUT_LAYOUT_DESC& Desc) = 0;

	virtual IRenderPassInfo* CreateRenderPass(const IRenderTargetView** RenderTargets, uint NumRenderTargets, const IDepthStencilView* DepthStencil, const XMFLOAT4& ClearColor) = 0;
	
	inline IRenderPassInfo* CreateRenderPass(const IRenderTargetView* RenderTarget, const IDepthStencilView* DepthStencil, const XMFLOAT4& ClearColor)
	{
		return CreateRenderPass(&RenderTarget, 1U, DepthStencil, ClearColor);
	}

	// Warn, On d3d12 and vulkan this will flush any state that was set previously on the main thread command list and the compute command list
	virtual void GenerateMips(ITextureBase* Texture) = 0;

	virtual IRenderPassInfo* GetBackBufferTargetPass() = 0; // This just returns the screen target
	virtual IRenderTargetView* GetBackBuffer() = 0;

	virtual void CreateShaderResourceView(IShaderResourceTable* DestTable, IBuffer* Buffer, uint Slot, uint Subresource = 0) = 0;
	virtual void CreateUnorderedAccessView(IShaderResourceTable* DestTable, IBuffer* Buffer, uint Slot, uint Subresource = 0) = 0;
	virtual void CreateShaderResourceView(IShaderResourceTable* DestTable, ITextureBase* Texture, uint Slot, uint Subresource = 0) = 0;
	virtual void CreateUnorderedAccessView(IShaderResourceTable* DestTable, ITextureBase* Texture, uint Slot, uint Subresource = 0) = 0;
	virtual void CreateSamplerView(IShaderResourceTable* DestTable, ISamplerState* SamplerState, uint Slot) = 0;
	virtual IShaderResourceTable* CreateShaderTable(IVertexShader* VS, IPixelShader* PS, IHullShader* HS = nullptr, IGeometryShader* GS = nullptr) = 0;

	ForceInline IShaderResourceTable* CreateShaderTable(IGraphicsPipelineState* Pipeline)
	{
		GFX_PIPELINE_STATE_DESC Desc = { };
		Pipeline->GetDesc(&Desc);
		return CreateShaderTable(Desc.VS, Desc.PS, Desc.HS, Desc.GS);
	}

	ForceInline IShaderResourceTable* CreateShaderTable(IComputePipelineState* Pipeline)
	{
		COMPUTE_PIPELINE_STATE_DESC Desc = { };
		Pipeline->GetDesc(&Desc);
		return CreateShaderTable(Desc.CS);
	}

	virtual IShaderResourceTable* CreateShaderTable(IComputeShader* CS) = 0;

	virtual IRenderTargetView* CreateRenderTargetView(ITexture2D* InTexture) = 0;
	virtual IDepthStencilView* CreateDepthStencilView(ITexture2D* InTexture) = 0;
	virtual IDepthStencilState* CreateDepthStencilState(const GFX_DEPTH_STENCIL_DESC& InDesc) = 0;

	virtual IDeviceSwapChain* GetSwapChain() = 0;

	virtual EMULTISAMPLE_LEVEL GetMaximumMultisampleLevel() const = 0;
};
