#pragma once
#include "BaneMath.h"
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

#define GPU_BUFFER_MIN_SIZE KILOBYTE(65)

class IGraphicsDevice
{
public:

	virtual ~IGraphicsDevice() { }

	// This is here in case someone absolutely knows what they are doing and absolutely positively need a new context
	virtual IGraphicsCommandContext* CreateCommandContext(ECOMMAND_CONTEXT_TYPE Type) = 0;

	virtual IGraphicsCommandContext* GetGraphicsContext() = 0;
	virtual IComputeCommandContext* GetComputeContext() = 0;

	// Due to some of how directx 12 and opengl work, these have to be the raw code, then the compiled forms are stored in the IShader interface
	virtual IVertexShader* CreateVertexShader(const std::string& ByteCode) = 0;
	virtual IPixelShader* CreatePixelShader(const std::string& ByteCode) = 0;
	virtual IGeometryShader* CreateGeometryShader(const std::string& ByteCode) = 0;
	virtual IHullShader* CreateHullShader(const std::string& ByteCode) = 0;
	virtual IComputeShader* CreateComputeShader(const std::string& ByteCode) = 0;

	virtual bool SupportsPrecompiledShaders() const = 0;
	virtual IVertexShader* CreateVertexShaderFromBytecode(const std::vector<uint8>& ByteCode)		{ UNUSED(ByteCode); return nullptr; };
	virtual IPixelShader* CreatePixelShaderFromBytecode(const std::vector<uint8>& ByteCode)		{ UNUSED(ByteCode); return nullptr; };
	virtual IGeometryShader* CreateGeometryShaderFromBytecode(const std::vector<uint8>& ByteCode)	{ UNUSED(ByteCode); return nullptr; };
	virtual IHullShader* CreateHullShaderFromBytecode(const std::vector<uint8>& ByteCode)		{ UNUSED(ByteCode); return nullptr; };
	virtual IComputeShader*	CreateComputeShaderFromBytecode(const std::vector<uint8>& ByteCode)		{ UNUSED(ByteCode); return nullptr; };

	virtual IGraphicsPipelineState* CreatePipelineState(const GFX_PIPELINE_STATE_DESC* Desc) = 0;
	virtual IComputePipelineState* CreatePipelineState(const COMPUTE_PIPELINE_STATE_DESC* Desc) = 0;

	virtual void RecompilePipelineState(IGraphicsPipelineState* pState, const GFX_PIPELINE_STATE_DESC* NewDesc) = 0;
	virtual void RecompilePipelineState(IComputePipelineState* pState, const COMPUTE_PIPELINE_STATE_DESC* NewDesc) = 0;

	virtual IVertexBuffer* CreateVertexBuffer(uint32 ByteCount, uint8* Buffer) = 0;
	virtual IIndexBuffer* CreateIndexBuffer(uint32 ByteCount, uint8* Buffer) = 0;
	virtual IConstantBuffer* CreateConstantBuffer(uint32 ByteCount) = 0;
	virtual IBuffer* CreateStructuredBuffer(uint32 ByteCount, uint8* Buffer) = 0;
	virtual IBuffer* CreateStagingBuffer(uint32 ByteCount) = 0;
	
	template<typename T>
	inline IConstantBuffer* CreateConstBuffer()
	{
		return CreateConstantBuffer(sizeof(T));
	}
	
	virtual ITexture2D* CreateTexture2D(uint32 Width, uint32 Height, EFORMAT Format, const SAMPLER_DESC& SampleDesc, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data) = 0;
	virtual ITexture2DArray* CreateTexture2DArray(uint32 Width, uint32 Height, uint32 Count, EFORMAT Format, const SAMPLER_DESC& SampleDesc, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data) = 0;
	virtual ITexture3D* CreateTexture3D(uint32 Width, uint32 Height, uint32 Depth, EFORMAT Format, const SAMPLER_DESC& SampleDesc, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data) = 0;
	virtual ITextureCube* CreateTextureCube(uint32 CubeSize, EFORMAT Format, const SAMPLER_DESC& SampleDesc, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data) = 0;

	virtual IInputLayout* CreateInputLayout(const GFX_INPUT_LAYOUT_DESC& Desc) = 0;

	// Warn, On d3d12 and vulkan this will flush any state that was set previously on the main thread command list and the compute command list
	virtual void GenerateMips(ITextureBase* Texture) = 0;

	virtual IRenderTargetView* GetBackBuffer() = 0;
	virtual IDepthStencilView* GetDepthStencilForBackBuffer() = 0;

	virtual IRenderTargetView* CreateRenderTargetView(ITexture2D* InTexture) = 0;
	virtual IDepthStencilView* CreateDepthStencilView(ITexture2D* InTexture) = 0;
	virtual IDepthStencilState* CreateDepthStencilState(const GFX_DEPTH_STENCIL_DESC& InDesc) = 0;

	virtual IDeviceSwapChain* GetSwapChain() = 0;

	virtual EMULTISAMPLE_LEVEL GetMaximumMultisampleLevel() const = 0;
};
