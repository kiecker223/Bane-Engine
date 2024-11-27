#include "D3D12GraphicsDevice.h"
#include "D3D12Translator.h"
#include "../IO/ShaderCache.h"
#include "D3D12PipelineLibrary.h"
#include "D3D12PipelineState.h"
#include "D3D12GraphicsResource.h"
#include <Platform/System/Logging/Logger.h>
#include <thread>
#include <d3dcompiler.h>


uint32 D3D12GraphicsDevice::GCurrentFrameIndex = 0;


static D3D12_COMMAND_LIST_TYPE FromContextType(ECOMMAND_CONTEXT_TYPE ContextType)
{
	switch (ContextType)
	{
	case COMMAND_CONTEXT_TYPE_GRAPHICS:
		return D3D12_COMMAND_LIST_TYPE_DIRECT;
	case COMMAND_CONTEXT_TYPE_COMPUTE:
		return D3D12_COMMAND_LIST_TYPE_COMPUTE;
	case COMMAND_CONTEXT_TYPE_COPY:
		return D3D12_COMMAND_LIST_TYPE_COPY;
	}
	return (D3D12_COMMAND_LIST_TYPE)-1;
}

D3D12GraphicsDevice::D3D12GraphicsDevice(D3D12SwapChain* SwapChain, const Window* RenderingWindow, ID3D12Device1* Device, ID3D12CommandQueue* MainQueue) :
	m_Device(Device),
	m_SwapChain(SwapChain),
	m_GenerateMipsPipeline2D(nullptr)
{
	m_CommandQueues[0].Initialize(MainQueue, Device, "D3D12GraphicsDevice::m_DirectQueue", COMMAND_CONTEXT_TYPE_GRAPHICS);
	m_CommandQueues[0].SetParentDevice(this);
	
	m_CommandQueues[1].Initialize({ D3D12_COMMAND_LIST_TYPE_COMPUTE, 100, D3D12_COMMAND_QUEUE_FLAG_NONE, 1 }, Device, "D3D12GraphicsDevice::m_ComputeQueue", COMMAND_CONTEXT_TYPE_COMPUTE);
	m_CommandQueues[1].SetParentDevice(this);
	
	m_CommandQueues[2].Initialize({ D3D12_COMMAND_LIST_TYPE_COPY, 100, D3D12_COMMAND_QUEUE_FLAG_NONE, 1 }, Device, "D3D12GraphicsDevice::m_UploadList", COMMAND_CONTEXT_TYPE_COPY);
	m_CommandQueues[2].SetParentDevice(this);
	
	m_SwapChain->Device = this;
	uint32 AvailableThreadCount = std::thread::hardware_concurrency();
	BANE_CHECK(AvailableThreadCount != 0);
	m_AvailableContexts.resize(AvailableThreadCount);

	m_SrvAllocator.Initialize(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024 * 2, true);
	m_RtvAllocator.Initialize(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 256, false);
	m_DsvAllocator.Initialize(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 48, false);
	m_SmpAllocator.Initialize(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1024 * 2, true);
	
	m_ViewPort = { 0 };
	m_ViewPort.TopLeftX = 0.f;
	m_ViewPort.TopLeftY = 0.f;
	m_ViewPort.Width = (float)RenderingWindow->GetWidth();
	m_ViewPort.Height = (float)RenderingWindow->GetHeight();
	m_ViewPort.MaxDepth = 1.f;

	m_Rect = { 0 };
	m_Rect.left = 0;
	m_Rect.top = 0;
	m_Rect.right = RenderingWindow->GetWidth();
	m_Rect.bottom = RenderingWindow->GetHeight();

	{
		ID3D12Resource* BackBuffers[3] = { };
		for (uint32 i = 0; i < 3; i++)
		{
			m_SwapChain->SwapChain->GetBuffer(i, IID_PPV_ARGS(&BackBuffers[i]));
		}

		D3D12TextureBase* Bases = new D3D12TextureBase[3];

		for (uint32 i = 0; i < 3; i++)
		{
			Bases[i].Resource.SetParentDevice(this);
			Bases[i].Resource.SetResource(BackBuffers[i]);
			Bases[i].SetDebugName((std::string("BackBuffer: ") + std::to_string(i)).c_str());
			Bases[i].Width = RenderingWindow->GetWidth();
			Bases[i].Height = RenderingWindow->GetHeight();
			Bases[i].Depth = 1;
			Bases[i].Usage = TEXTURE_USAGE_RENDER_TARGET;
			Bases[i].Format = FORMAT_R8G8B8A8_UNORM;
			Bases[i].CurrentState = D3D12_RESOURCE_STATE_COMMON;
			Bases[i].PromotedState = D3D12_RESOURCE_STATE_COMMON;
			Bases[i].PendingState = D3D12_RESOURCE_STATE_INVALID_STATE;
		}

		m_BackBuffer = CreateRenderTargetView(Bases);

		ITexture2D* DepthTexture = CreateTexture2D(RenderingWindow->GetWidth(), RenderingWindow->GetHeight(), FORMAT_UNKNOWN, CreateDefaultSamplerDesc(), TEXTURE_USAGE_DEPTH_STENCIL, nullptr);
		m_DepthStencil = CreateDepthStencilView(DepthTexture);
	}
	
	for (uint32 i = 0; i < COMMAND_CONTEXT_TYPE_NUM_TYPES; i++)
	{	
		uint32 NumCommandListsAllowed;
		switch (i)
		{
			case COMMAND_CONTEXT_TYPE_GRAPHICS: NumCommandListsAllowed = min(AvailableThreadCount - 1, 6); break;
			case COMMAND_CONTEXT_TYPE_COMPUTE: NumCommandListsAllowed = 3; break;
			case COMMAND_CONTEXT_TYPE_COPY: NumCommandListsAllowed = 5; break;
		}
		for (uint32 b = 0; b < NumCommandListsAllowed; b++)
		{
			ID3D12CommandAllocator* CommandAllocator;
			ID3D12GraphicsCommandList* CommandList;
			D3D12_COMMAND_LIST_TYPE ListType = FromContextType((ECOMMAND_CONTEXT_TYPE)i);

			D3D12ERRORCHECK(
				Device->CreateCommandAllocator(ListType, IID_PPV_ARGS(&CommandAllocator))
			);

			D3D12ERRORCHECK(
				Device->CreateCommandList(1, ListType, CommandAllocator, nullptr, IID_PPV_ARGS(&CommandList))
			);
			CommandList->SetName((std::wstring(L"Command Pool Command List: ") + std::to_wstring(b)).c_str());

			m_AvailableCLs[i].Push(new D3D12CommandList(CommandAllocator, CommandList, this));
		}
	}

	for (uint32 i = 0; i < 2; i++)
	{
		m_AvailableContexts[i] = new D3D12GraphicsCommandContext(this, COMMAND_CONTEXT_TYPE_GRAPHICS, m_Rect, m_ViewPort);
	}
	m_ComputeContext = new D3D12ComputeCommandContext(this);
	m_UploadList = new D3D12GraphicsCommandContext(this, COMMAND_CONTEXT_TYPE_COPY, m_Rect, m_ViewPort);
	m_CommandQueues[0].SetParentDevice(this);
	m_CommandQueues[1].SetParentDevice(this);
	m_CommandQueues[2].SetParentDevice(this);
	
	for (uint32 i = 0; i < 3; i++)
	{
		ID3D12Heap* pHeap = nullptr;
		D3D12_HEAP_DESC HeapDesc = { };
		/*
		UINT64 SizeInBytes;
		D3D12_HEAP_PROPERTIES Properties;
		UINT64 Alignment;
		D3D12_HEAP_FLAGS Flags;
		*/
		HeapDesc.SizeInBytes = MEGABYTE(16);
		/*
		D3D12_HEAP_TYPE Type;
		D3D12_CPU_PAGE_PROPERTY CPUPageProperty;
		D3D12_MEMORY_POOL MemoryPoolPreference;
		UINT CreationNodeMask;
		UINT VisibleNodeMask;
		*/
		HeapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		HeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;

// 		D3D12ERRORCHECK(
// 			Device->CreateHeap(
// 				&HeapDesc,
// 				IID_PPV_ARGS(&pHeap)
// 			)
// 		);
	}

	InitializeD3D12Translator();
	InitializeD3D12ShaderSignatureLibrary(m_Device);
}

D3D12GraphicsDevice::~D3D12GraphicsDevice()
{
	m_Device->Release();
	DestroyD3D12ShaderSignatureLibrary();
	delete m_SwapChain;
}

IGraphicsCommandContext* D3D12GraphicsDevice::CreateCommandContext(ECOMMAND_CONTEXT_TYPE Type)
{
	return new D3D12GraphicsCommandContext(this, Type, m_Rect, m_ViewPort);
}

IGraphicsCommandContext* D3D12GraphicsDevice::GetGraphicsContext()
{
	return m_AvailableContexts[0];
}

IComputeCommandContext* D3D12GraphicsDevice::GetComputeContext()
{
	return m_ComputeContext;
}

static std::vector<uint8> CompileFromByteCode(const std::string& InByteCode, ESHADER_STAGE ShaderStage, uint32& OutNumConstantBuffers, uint32& OutNumSamplers, uint32& OutNumShaderResourceViews, uint32& OutNumUnorderedAccessViews)
{
	std::string EntryPoint = "";
	std::string Target = "";
	switch (ShaderStage)
	{
	case SHADER_STAGE_VERTEX:
		EntryPoint = "VSMain";
		Target = "vs_5_0";
		break;
	case SHADER_STAGE_PIXEL:
		EntryPoint = "PSMain";
		Target = "ps_5_0";
		break;
	case SHADER_STAGE_HULL:
		EntryPoint = "HSMain";
		Target = "hs_5_0";
		break;
	case SHADER_STAGE_GEOMETRY:
		EntryPoint = "GSMain";
		Target = "gs_5_0";
		break;
	case SHADER_STAGE_COMPUTE:
		EntryPoint = "CSMain";
		Target = "cs_5_0";
		break;
	}

	uint32 CompileFlags = 0;
#ifdef _DEBUG
	CompileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL0 | D3DCOMPILE_DEBUG;
#endif 

	ID3DBlob* CompiledCode = nullptr;
	ID3DBlob* ErrorMessage = nullptr;

	if (FAILED(
		D3DCompile(
		(void*)InByteCode.data(),
			InByteCode.size(),
			nullptr, nullptr, nullptr,
			EntryPoint.c_str(),
			Target.c_str(),
			CompileFlags, 0,
			&CompiledCode,
			&ErrorMessage)))
	{
		BaneLog() << "Failed to compile shader, just gonna silently fail now "
			<< (const char*)ErrorMessage->GetBufferPointer() << END_LINE;
		ErrorMessage->Release();
		return std::vector<uint8>();
	}

	// HACK
	// Basically leave the resources in to compile for all platforms, but only apply the resources to the vertex shader
	// Cheap trick to fix my bad mistake, Possibly replace if I fix my error
	if (ShaderStage == SHADER_STAGE_VERTEX || ShaderStage == SHADER_STAGE_COMPUTE)
	{
		std::string ByteCodeAsString = InByteCode;
		{
			uint32 NumConstBuffers = 0;

			for (size_t i = 0; i < ByteCodeAsString.length(); i++)
			{
				i = ByteCodeAsString.find("register(b", i);
				if (i == std::string::npos)
					break;
				NumConstBuffers++;
			}
			OutNumConstantBuffers = NumConstBuffers;
		}
		{
			uint32 NumSamplers = 0;
			for (size_t i = 0; i < ByteCodeAsString.length(); i++)
			{
				i = ByteCodeAsString.find("register(s", i);
				if (i == std::string::npos)
					break;
				NumSamplers++;
			}
			OutNumSamplers = NumSamplers;
		}
		{
			uint32 NumShaderResources = 0;
			for (size_t i = 0; i < ByteCodeAsString.length(); i++)
			{
				i = ByteCodeAsString.find("register(t", i);
				if (i == std::string::npos)
					break;
				NumShaderResources++;
			}
			OutNumShaderResourceViews = NumShaderResources;
		}
		{
			uint32 NumUnorderedAccessViews = 0;
			for (size_t i = 0; i < ByteCodeAsString.length(); i++)
			{
				i = ByteCodeAsString.find("register(u", i);
				if (i == std::string::npos)
					break;
				NumUnorderedAccessViews++;
			}
			OutNumUnorderedAccessViews = NumUnorderedAccessViews;
		}
	}
	else
	{
		OutNumConstantBuffers = 0;
		OutNumSamplers = 0;
		OutNumShaderResourceViews = 0;
		OutNumUnorderedAccessViews = 0;
	}

	std::vector<uint8> RetCode(static_cast<uint32>(CompiledCode->GetBufferSize()));
	memcpy(RetCode.data(), CompiledCode->GetBufferPointer(), static_cast<uint32>(RetCode.size()));
	CompiledCode->Release();
	return RetCode;
}

IVertexShader* D3D12GraphicsDevice::CreateVertexShader(const std::string& ByteCode)
{
	uint32 NumConstantBuffers;
	uint32 NumSamplers;
	uint32 NumShaderResources;
	uint32 NumUnorderedAccessViews;
	std::vector<uint8> CompiledCode = CompileFromByteCode(ByteCode, SHADER_STAGE_VERTEX, NumConstantBuffers, NumSamplers, NumShaderResources, NumUnorderedAccessViews);
#ifdef _DEBUG
	if (CompiledCode.size() == 0)
	{
		return nullptr;
	}
#endif
	return new D3D12VertexShader(CompiledCode);
}

IPixelShader* D3D12GraphicsDevice::CreatePixelShader(const std::string& ByteCode)
{
	uint32 NumConstantBuffers;
	uint32 NumSamplers;
	uint32 NumShaderResources;
	uint32 NumUnorderedAccessViews;
	std::vector<uint8> CompiledCode = CompileFromByteCode(ByteCode, SHADER_STAGE_PIXEL, NumConstantBuffers, NumSamplers, NumShaderResources, NumUnorderedAccessViews);

#ifdef _DEBUG
	if (CompiledCode.size() == 0)
	{
		return nullptr;
	}
#endif

	return new D3D12PixelShader(CompiledCode);
}

IGeometryShader* D3D12GraphicsDevice::CreateGeometryShader(const std::string& ByteCode)
{
	uint32 NumConstantBuffers;
	uint32 NumSamplers;
	uint32 NumShaderResources;
	uint32 NumUnorderedAccessViews;
	std::vector<uint8> CompiledCode = CompileFromByteCode(ByteCode, SHADER_STAGE_GEOMETRY, NumConstantBuffers, NumSamplers, NumShaderResources, NumUnorderedAccessViews);
#ifdef _DEBUG
	if (CompiledCode.size() == 0)
	{
		return nullptr;
	}
#endif
	return new D3D12GeometryShader(CompiledCode);
}

IHullShader* D3D12GraphicsDevice::CreateHullShader(const std::string& ByteCode)
{
	uint32 NumConstantBuffers;
	uint32 NumSamplers;
	uint32 NumShaderResources;
	uint32 NumUnorderedAccessViews;
	std::vector<uint8> CompiledCode = CompileFromByteCode(ByteCode, SHADER_STAGE_HULL, NumConstantBuffers, NumSamplers, NumShaderResources, NumUnorderedAccessViews);
#ifdef _DEBUG
	if (CompiledCode.size() == 0)
	{
		return nullptr;
	}
#endif
	return new D3D12HullShader(CompiledCode);
}

IComputeShader* D3D12GraphicsDevice::CreateComputeShader(const std::string& ByteCode)
{
	uint32 NumConstantBuffers;
	uint32 NumSamplers;
	uint32 NumShaderResources;
	uint32 NumUnorderedAccessViews;
	std::vector<uint8> CompiledCode = CompileFromByteCode(ByteCode, SHADER_STAGE_COMPUTE, NumConstantBuffers, NumSamplers, NumShaderResources, NumUnorderedAccessViews);
#ifdef _DEBUG
	if (CompiledCode.size() == 0)
	{
		return nullptr;
	}
#endif
	return new D3D12ComputeShader(CompiledCode);
}

IVertexShader* D3D12GraphicsDevice::CreateVertexShaderFromBytecode(const std::vector<uint8>& InByteCode)
{
	return new D3D12VertexShader(InByteCode);
}

IPixelShader* D3D12GraphicsDevice::CreatePixelShaderFromBytecode(const std::vector<uint8>& InByteCode)
{
	return new D3D12PixelShader(InByteCode);
}

IGeometryShader* D3D12GraphicsDevice::CreateGeometryShaderFromBytecode(const std::vector<uint8>& InByteCode)
{
	return new D3D12GeometryShader(InByteCode);
}

IHullShader* D3D12GraphicsDevice::CreateHullShaderFromBytecode(const std::vector<uint8>& InByteCode)
{
	return new D3D12HullShader(InByteCode);
}

IComputeShader* D3D12GraphicsDevice::CreateComputeShaderFromBytecode(const std::vector<uint8>& InByteCode)
{
	return new D3D12ComputeShader(InByteCode);
}

IGraphicsPipelineState* D3D12GraphicsDevice::CreatePipelineState(const GFX_PIPELINE_STATE_DESC* Desc)
{
	D3D12ShaderSignature UsedSignature;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC GFXDesc = D3D12_TranslateGraphicsPipelineStateDesc(Desc, &UsedSignature);
	return new D3D12GraphicsPipelineState(m_Device, GFXDesc, *Desc, UsedSignature);
}

IComputePipelineState* D3D12GraphicsDevice::CreatePipelineState(const COMPUTE_PIPELINE_STATE_DESC* Desc)
{
	D3D12ComputeShader* Shader = (D3D12ComputeShader*)Desc->CS;
	D3D12ShaderSignature UsedSignature = GetD3D12ShaderSignatureLibrary()->DetermineBestRootSignature(Desc->Counts);
	D3D12_COMPUTE_PIPELINE_STATE_DESC CreationDesc = { };
	CreationDesc.pRootSignature = UsedSignature.RootSignature;
	CreationDesc.NodeMask = 0;
	CreationDesc.CS = Shader->GetShaderByteCode();
	return new D3D12ComputePipelineState(m_Device, CreationDesc, *Desc, UsedSignature);
}

void D3D12GraphicsDevice::RecompilePipelineState(IGraphicsPipelineState* pState, const GFX_PIPELINE_STATE_DESC* NewDesc)
{
	D3D12GraphicsPipelineState* PipelineState = (D3D12GraphicsPipelineState*)pState;
	D3D12ShaderSignature UsedSignature;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC GFXDesc = D3D12_TranslateGraphicsPipelineStateDesc(NewDesc, &UsedSignature);
	PipelineState->Reset(m_Device, GFXDesc, *NewDesc, UsedSignature);
}

void D3D12GraphicsDevice::RecompilePipelineState(IComputePipelineState* pState, const COMPUTE_PIPELINE_STATE_DESC* NewDesc)
{
	D3D12ComputePipelineState* PipelineState = (D3D12ComputePipelineState*)pState;
	D3D12ComputeShader* Shader = (D3D12ComputeShader*)NewDesc->CS;
	D3D12ShaderSignature UsedSignature = GetD3D12ShaderSignatureLibrary()->DetermineBestRootSignature(NewDesc->Counts);
	D3D12_COMPUTE_PIPELINE_STATE_DESC CreationDesc = { };
	CreationDesc.pRootSignature = UsedSignature.RootSignature;
	CreationDesc.NodeMask = 0;
	CreationDesc.CS = Shader->GetShaderByteCode();
	PipelineState->Reset(m_Device, CreationDesc, *NewDesc, UsedSignature);
}

IVertexBuffer* D3D12GraphicsDevice::CreateVertexBuffer(uint32 ByteCount, uint8* Buffer)
{
	if (!m_UploadList->HasBegun())
	{
		m_UploadList->Begin();
	}
	
	D3D12Buffer* Result = new D3D12Buffer(this, ByteCount, BUFFER_USAGE_GPU);
	if (Buffer)
	{
		Result->UploadDataToGPU(m_UploadList, Buffer);
	}
	return Result;
}

IIndexBuffer* D3D12GraphicsDevice::CreateIndexBuffer(uint32 ByteCount, uint8* Buffer)
{
	if (!m_UploadList->HasBegun())
	{
		m_UploadList->Begin();
	}

	D3D12Buffer* Result = new D3D12Buffer(this, ByteCount, BUFFER_USAGE_GPU);
	Result->UploadDataToGPU(m_UploadList, Buffer);
	return Result;
}

IConstantBuffer* D3D12GraphicsDevice::CreateConstantBuffer(uint32 ByteCount)
{
	return new D3D12Buffer(this, ByteCount, BUFFER_USAGE_CPU);
}

IBuffer* D3D12GraphicsDevice::CreateStructuredBuffer(uint32 ByteCount, uint8* Buffer)
{
	if (!m_UploadList->HasBegun())
	{
		m_UploadList->Begin();
	}
	D3D12Buffer* Result = new D3D12Buffer(this, ByteCount, BUFFER_USAGE_GPU);
	Result->UploadDataToGPU(m_UploadList, Buffer);
	return Result;
}

IBuffer* D3D12GraphicsDevice::CreateStagingBuffer(uint32 ByteCount)
{
	return new D3D12Buffer(this, ByteCount, BUFFER_USAGE_UPLOAD);
}

ITexture2D* D3D12GraphicsDevice::CreateTexture2D(uint32 Width, uint32 Height, EFORMAT Format, const SAMPLER_DESC& InSamplerDesc, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data)
{
	D3D12TextureBase* Result; 
	D3D12_SAMPLER_DESC D3DSamplerDesc = D3D12_TranslateSamplerDesc(InSamplerDesc);
	bool bDoNoUpload = false; // Never start off with initial contents, only allow texture to texture copies or writes
	if ((Usage & TEXTURE_USAGE_RENDER_TARGET) == TEXTURE_USAGE_RENDER_TARGET)
	{
		Result = new D3D12TextureBase[3]{
			D3D12TextureBase(this, Width, Height, 1U, 1U, Format, InSamplerDesc, D3DSamplerDesc, Usage),
			D3D12TextureBase(this, Width, Height, 1U, 1U, Format, InSamplerDesc, D3DSamplerDesc, Usage),
			D3D12TextureBase(this, Width, Height, 1U, 1U, Format, InSamplerDesc, D3DSamplerDesc, Usage)
		};

		for (uint32 i = 0; i < 3; i++)
		{
			Result[i].Resource.SRVDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			Result[i].Resource.UAVDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		}

		bDoNoUpload = true;
	}
	else if ((Usage & TEXTURE_USAGE_DEPTH_STENCIL) == TEXTURE_USAGE_DEPTH_STENCIL) // We do an explicit check because there is a specific format to be enforced here
	{
		Result = new D3D12TextureBase[3]{
			D3D12TextureBase(this, Width, Height, 1U, 1U, FORMAT_D24_UNORM_S8_UINT, InSamplerDesc, D3DSamplerDesc, Usage),
			D3D12TextureBase(this, Width, Height, 1U, 1U, FORMAT_D24_UNORM_S8_UINT, InSamplerDesc, D3DSamplerDesc, Usage),
			D3D12TextureBase(this, Width, Height, 1U, 1U, FORMAT_D24_UNORM_S8_UINT, InSamplerDesc, D3DSamplerDesc, Usage),
		};

		for (uint32 i = 0; i < 3; i++)
		{
			Result[i].Resource.SRVDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			Result[i].Resource.UAVDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		}

		bDoNoUpload = true;
	}
	else
	{
		Result = new D3D12TextureBase(this, Width, Height, 1U, 1U, Format, InSamplerDesc, D3DSamplerDesc, Usage);
	}
	if (Data && !bDoNoUpload)
	{
		if (!m_UploadList->HasBegun())
		{
			m_UploadList->Begin();
		}

		Result->UploadToGPU(m_UploadList, Data->Pointer, Data->Width, Data->Height, 1U, Data->Step);
	}
	Result->Resource.SRVDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	Result->Resource.UAVDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	return Result;
}

D3D12TextureBase* D3D12GraphicsDevice::CreateTemporaryRenderTexture(uint32 Width, uint32 Height, EFORMAT Format, const SAMPLER_DESC& InSamplerDesc, ETEXTURE_USAGE Usage)
{
	D3D12TextureBase* Result;
	D3D12_SAMPLER_DESC D3DSamplerDesc = D3D12_TranslateSamplerDesc(InSamplerDesc);
	BANE_CHECK(Usage & TEXTURE_USAGE_RENDER_TARGET); // It has to be a render target
	Result = new D3D12TextureBase(this, Width, Height, 1U, 1U, Format, InSamplerDesc, D3DSamplerDesc, Usage, GetTemporaryTextureHeap());
	Result->Resource.SRVDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	Result->Resource.UAVDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	return Result;
}

D3D12RenderTargetView* D3D12GraphicsDevice::CreateTemporaryRenderTargetView(D3D12TextureBase* Texture)
{
	D3D12RenderTargetView* Result = new D3D12RenderTargetView();
#if DEBUG
	if (!Texture->IsRenderTarget())
	{
		BaneLog() << "The texture: " << Texture->GetDebugName() << " is not a valid resource for making a render target view";
		return nullptr;
	}
#endif
	D3D12DescriptorAllocation Allocation = m_RtvAllocator.AllocateDescriptor();
	m_Device->CreateRenderTargetView(Texture->Resource.D3DResource, nullptr, Allocation.CpuHandle);
	
	// Just stuff it in all 3 frames, it will be deleted
	for (uint32 i = 0; i < 3; i++)
	{
		Result->FrameData[i] = { Texture, Allocation.CpuHandle, Allocation.GpuHandle };
	}
	return Result;
}

D3D12DepthStencilView* D3D12GraphicsDevice::CreateTemporaryDepthStencilView(D3D12TextureBase* Texture)
{
	D3D12DepthStencilView* Result = new D3D12DepthStencilView();
#if DEBUG
	if (!Texture->IsDepthStencil())
	{
		BaneLog() << "The texture: " << Texture->GetDebugName() << " is not a valid resource for making a depth stencil view";
		return nullptr;
	}
#endif
	D3D12DescriptorAllocation Allocation = m_RtvAllocator.AllocateDescriptor();
	m_Device->CreateDepthStencilView(Texture->Resource.D3DResource, nullptr, Allocation.CpuHandle);

	// Just stuff it in all 3 frames, it will be deleted
	for (uint32 i = 0; i < 3; i++)
	{
		Result->FrameData[i] = { Texture, Allocation.CpuHandle, Allocation.GpuHandle };
	}
	return Result;
}

ITexture2DArray* D3D12GraphicsDevice::CreateTexture2DArray(uint32 Width, uint32 Height, uint32 Count, EFORMAT Format, const SAMPLER_DESC& InSampleDesc, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data)
{
	D3D12_SAMPLER_DESC D3DSampleDesc = D3D12_TranslateSamplerDesc(InSampleDesc);
	D3D12TextureBase* Result = new D3D12TextureBase(this, Width, Height, 1, Count, Format, InSampleDesc, D3DSampleDesc, Usage);

	if (Data)
	{
		Result->UploadToGPU(m_UploadList, Data->Pointer, Data->Width, Data->Height, Data->Depth, Data->Step);
	}

	Result->Resource.SRVDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
	Result->Resource.UAVDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;

	return Result;
}

ITexture3D* D3D12GraphicsDevice::CreateTexture3D(uint32 Width, uint32 Height, uint32 Depth, EFORMAT Format, const SAMPLER_DESC& InSampleDesc, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data)
{
	D3D12TextureBase* Result = nullptr;
	
	D3D12_SAMPLER_DESC D3DSampleDesc = D3D12_TranslateSamplerDesc(InSampleDesc);
	Result = new D3D12TextureBase(this, Width, Height, Depth, 1, Format, InSampleDesc, D3DSampleDesc, Usage);

	if (Data)
	{
		Result->UploadToGPU(m_UploadList, Data->Pointer, Data->Width, Data->Height, Data->Depth, Data->Step);
	}
	Result->Resource.SRVDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
	Result->Resource.UAVDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
	
	return Result;
}

ITextureCube* D3D12GraphicsDevice::CreateTextureCube(uint32 CubeSize, EFORMAT Format, const SAMPLER_DESC& InSampleDesc, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data)
{
	D3D12TextureBase* TextureArray = (D3D12TextureBase*)CreateTexture2DArray(CubeSize, CubeSize, 6, Format, InSampleDesc, Usage, Data);
	TextureArray->Resource.SRVDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	TextureArray->Resource.UAVDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
	return TextureArray;
}

void D3D12GraphicsDevice::GenerateMips(ITextureBase* InTexture)
{
	D3D12TextureBase* Texture = (D3D12TextureBase*)InTexture;
	if (Texture->MipCount <= 1)
	{
		BaneLog() << "[ERROR] D3D12 : The specified texture: " << (InTexture->GetDebugName().size() == 0 ? InTexture->GetDebugName() : "UNAMED TEXTURE") 
					<< " is not suitable for mipmap generation" << END_LINE;
		return;
	}

	D3D12GraphicsCommandContext* DirectContext = m_AvailableContexts[0];
	bool bDirectContextHasBegun = DirectContext->HasBegun();
	D3D12RenderPassInfo* PreviousRenderPass = nullptr;
	if (!bDirectContextHasBegun)
	{
		DirectContext->Begin();
	}
	else
	{
		DirectContext->Flush();
		DirectContext->Begin();
	}
	EnsureAllUploadsOccured();

	ID3D12GraphicsCommandList* DirectCL = DirectContext->CurrentCommandBuffer->CommandList->GetGraphicsCommandList();
	ID3D12Resource* Res = Texture->Resource.D3DResource;

	if (m_GenerateMipsPipeline2D == nullptr)
	{
		m_GenerateMipsPipeline2D = (D3D12ComputePipelineState*)GetShaderCache()->LoadComputePipeline("GenerateMips.cmpt");
	}

	ID3D12Resource* CopyRes = nullptr;


	if (Texture->Depth == 1)
	{
		auto SrvAllocator = m_SrvAllocator.AllocateMultiple(Texture->MipCount * 2);
		auto SmpAllocator = m_SmpAllocator.AllocateMultiple(1);
		
		{
			D3D12_HEAP_PROPERTIES HeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			D3D12_RESOURCE_DESC ResourceDesc = Res->GetDesc();
			ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			D3D12ERRORCHECK(
				m_Device->CreateCommittedResource(
					&HeapProps,
					D3D12_HEAP_FLAG_NONE,
					&ResourceDesc,
					D3D12_RESOURCE_STATE_COPY_DEST,
					nullptr,
					IID_PPV_ARGS(&CopyRes)
				);
			);
		}
		{
			D3D12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(Res, Texture->PromotedState, D3D12_RESOURCE_STATE_COPY_SOURCE);
			DirectCL->ResourceBarrier(1, &Barrier);
		}
		DirectCL->CopyResource(CopyRes, Res);
		{
			D3D12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(CopyRes, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
			DirectCL->ResourceBarrier(1, &Barrier);
		}
		ID3D12DescriptorHeap* ppHeaps[2] = { m_SrvAllocator.DescriptorHeap, m_SmpAllocator.DescriptorHeap };
		DirectCL->SetDescriptorHeaps(2, ppHeaps);
		D3D12Buffer* ConstBuff = (D3D12Buffer*)CreateConstantBuffer(Texture->ArrayCount * (Texture->MipCount - 1) * 256);
		for (uint32 b = 0; b < Texture->ArrayCount; b++)
		{
			for (uint32 i = 0; i < Texture->MipCount - 1; i++)
			{
				uint32 DstWidth = max(Texture->Width >> (i + 1), 1);
				uint32 DstHeight = max(Texture->Height >> (i + 1), 1);

				D3D12DescriptorAllocation UavAllocation = m_SrvAllocator.AllocateDescriptor();
				D3D12DescriptorAllocation SrvAllocation = m_SrvAllocator.AllocateDescriptor();
				D3D12DescriptorAllocation SmpAllocation = m_SmpAllocator.AllocateDescriptor();
				{
					D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc = { };
					UavDesc.Format = D3D_TranslateFormat(Texture->Format);
					UavDesc.ViewDimension = Texture->Resource.UAVDimension;
					if (Texture->ArrayCount == 1)
					{
						UavDesc.Texture2D.MipSlice = i + 1;
						UavDesc.Texture2D.PlaneSlice = 0;
					}
					else
					{
						UavDesc.Texture2DArray.MipSlice = i + 1;
						if (b == 0)
						{
							UavDesc.Texture2DArray.FirstArraySlice = b;
							UavDesc.Texture2DArray.ArraySize = Texture->ArrayCount;
						}
						else
						{
							UavDesc.Texture2DArray.FirstArraySlice = b;
							UavDesc.Texture2DArray.ArraySize = 1;
						}
					}


					m_Device->CreateUnorderedAccessView(CopyRes, nullptr, &UavDesc, UavAllocation.CpuHandle);
				}
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = { };
					SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					SrvDesc.Format = D3D_TranslateFormat(Texture->Format);
					SrvDesc.ViewDimension = Texture->Resource.SRVDimension;
					if (SrvDesc.ViewDimension == D3D12_SRV_DIMENSION_TEXTURECUBE)
					{
						SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
					}
					if (Texture->ArrayCount == 1)
					{
						SrvDesc.Texture2D.MipLevels = 1;
						SrvDesc.Texture2D.MostDetailedMip = 0;
					}
					else
					{
						if (i == 0)
						{
							SrvDesc.Texture2DArray.MostDetailedMip = 0;
							SrvDesc.Texture2DArray.MipLevels = Texture->MipCount;
						}
						else
						{
							SrvDesc.Texture2DArray.MostDetailedMip = i;
							SrvDesc.Texture2DArray.MipLevels = 1;
						}
						if (b == 0)
						{
							SrvDesc.Texture2DArray.ArraySize = Texture->ArrayCount;
							SrvDesc.Texture2DArray.FirstArraySlice = b;
						}
						else
						{
							SrvDesc.Texture2DArray.ArraySize = 1;
							SrvDesc.Texture2DArray.FirstArraySlice = b;
						}
					}
					m_Device->CreateShaderResourceView(CopyRes, &SrvDesc, SrvAllocation.CpuHandle);
				}
				uint64 Offset = i * 256;
				{
					fvec2* pParams = (fvec2*)(reinterpret_cast<uint64>(ConstBuff->MappedPointer) + Offset);
					pParams->x = (1.0f / DstWidth);
					pParams->y = (1.0f / DstHeight);
					UNUSED(pParams);
				}
				DirectCL->SetPipelineState(m_GenerateMipsPipeline2D->PipelineState);
				DirectCL->SetComputeRootSignature(m_GenerateMipsPipeline2D->ShaderSignature.RootSignature);

				DirectCL->SetComputeRootConstantBufferView(0, ConstBuff->Resource.Location + Offset);
				DirectCL->SetComputeRootDescriptorTable(1, SrvAllocation.GpuHandle);
				DirectCL->SetComputeRootDescriptorTable(2, SmpAllocation.GpuHandle);
				DirectCL->SetComputeRootDescriptorTable(3, UavAllocation.GpuHandle);
				{
					D3D12_RESOURCE_BARRIER Barrier =
					{
						CD3DX12_RESOURCE_BARRIER::Transition(CopyRes, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, i + 1)
					};
					DirectCL->ResourceBarrier(1, &Barrier);
				}
				DirectCL->Dispatch(
					max(static_cast<uint32>(std::ceil(static_cast<float>(DstWidth) / 8.f)), 1),
					max(static_cast<uint32>(std::ceil(static_cast<float>(DstHeight) / 8.f)), 1),
					1);
				{
					D3D12_RESOURCE_BARRIER Barriers[2] =
					{
						CD3DX12_RESOURCE_BARRIER::UAV(CopyRes),
						CD3DX12_RESOURCE_BARRIER::Transition(CopyRes, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, i + 1)
					};
					DirectCL->ResourceBarrier(2, Barriers);
				}
			}
		}
		D3D12_RESOURCE_STATES PreviousTextureState = Texture->PromotedState;
		{
			D3D12_RESOURCE_BARRIER Barriers[2] =
			{
				CD3DX12_RESOURCE_BARRIER::Transition(CopyRes, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_SOURCE),
				CD3DX12_RESOURCE_BARRIER::Transition(Res, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST)
			};
			DirectCL->ResourceBarrier(2, Barriers);
			DirectCL->CopyResource(Res, CopyRes);
		}
		if (PreviousTextureState != D3D12_RESOURCE_STATE_COPY_DEST)
		{
			D3D12_RESOURCE_BARRIER Barriers[] =
			{
				CD3DX12_RESOURCE_BARRIER::Transition(Res, D3D12_RESOURCE_STATE_COPY_DEST, PreviousTextureState)
			};
			DirectCL->ResourceBarrier(1, Barriers);
		}

		DirectContext->Flush();
		delete ConstBuff;
	}
	if (bDirectContextHasBegun)
	{
		DirectContext->Begin();
	}

	while (CopyRes->Release() > 0) { }
}

IInputLayout* D3D12GraphicsDevice::CreateInputLayout(const GFX_INPUT_LAYOUT_DESC& Desc)
{
	D3D12_INPUT_LAYOUT_DESC CreationDesc = D3D12_TranslateInputLayout(Desc);
	return new D3D12InputLayout(Desc, CreationDesc);
}

IRenderTargetView* D3D12GraphicsDevice::GetBackBuffer()
{
	return m_BackBuffer;
}

IDepthStencilView* D3D12GraphicsDevice::GetDepthStencilForBackBuffer()
{
	return m_DepthStencil;
}

// 
// void D3D12GraphicsDevice::CreateConstantBufferView(IShaderResourceTable* InDestTable, IBuffer* InBuffer, uint32 InSlot, uint64 InOffset)
// {
// 	D3D12ShaderResourceTable* DestTable = (D3D12ShaderResourceTable*)InDestTable;
// 	D3D12Buffer* Buffer = (D3D12Buffer*)InBuffer;
// 	DestTable->ConstantBuffers[InSlot] = { Buffer, InOffset };
// }
// 
// void D3D12GraphicsDevice::CreateShaderResourceView(IShaderResourceTable* InDestTable, IBuffer* InBuffer, uint32 InSlot, uint32 StructureByteStride, uint32 NumElements, uint64 InOffset)
// {
// 	D3D12ShaderResourceTable* DestTable = (D3D12ShaderResourceTable*)InDestTable;
// 	D3D12Buffer* Buffer = (D3D12Buffer*)InBuffer;
// 	D3D12DescriptorAllocation SlotAlloc = DestTable->BaseSRVAllocation;
// 
// 	D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = { };
// 	SRVDesc.Buffer.StructureByteStride = static_cast<uint64>(StructureByteStride);
// 	SRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
// 	SRVDesc.Buffer.NumElements = NumElements;
// 	SRVDesc.Buffer.FirstElement = InOffset;
// 	SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
// 	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
// 
// 	m_Device->CreateShaderResourceView(Buffer->Resource.D3DResource, &SRVDesc, SlotAlloc.OffsetFromStart(InSlot).CpuHandle);
// 
// 	DestTable->ShaderResources[InSlot] = Buffer;
// }
// 
// void D3D12GraphicsDevice::CreateUnorderedAccessView(IShaderResourceTable* InDestTable, IBuffer* InBuffer, uint32 InSlot, uint32 InSubresource)
// {
// 	UNUSED(InDestTable);
// 	UNUSED(InBuffer);
// 	UNUSED(InSlot);
// 	UNUSED(InSubresource);
// 	// Why the actual fuck did I write this function
// 	__debugbreak();
// //#ifdef _DEBUG
// //	if (InSubresource != 0)
// //	{
// //		BaneLog() << "[WARN] D3D12 : Subresources not supported on buffer type views" << END_LINE;
// //	}
// //#endif
// //	D3D12ShaderResourceTable* DestTable = (D3D12ShaderResourceTable*)InDestTable;
// //	D3D12Buffer* Buffer = (D3D12Buffer*)InBuffer;
// //	DestTable->ConstantBuffers[InSlot] = Buffer;
// }
// 
// void D3D12GraphicsDevice::CreateShaderResourceView(IShaderResourceTable* InDestTable, ITextureBase* InTexture, uint32 Slot, uint32 InSubresource)
// {
// 	D3D12ShaderResourceTable* DestTable = (D3D12ShaderResourceTable*)InDestTable;
// 	D3D12TextureBase* Texture = (D3D12TextureBase*)InTexture;
// 	DestTable->ShaderResources[Slot] = Texture;
// 	
// 	D3D12DescriptorAllocation SlotAlloc = DestTable->BaseSRVAllocation.OffsetFromStart(Slot);
// 	D3D12DescriptorAllocation SampleAlloc = DestTable->BaseSMPAllocation.OffsetFromStart(Slot);
// 
// 	D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = { };
// 	SrvDesc.Format = D3D_TranslateFormat(Texture->Format);
// 	SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
// 	SrvDesc.ViewDimension = Texture->Resource.SRVDimension;
// 	if (InSubresource == 0)
// 	{
// 		SrvDesc.Texture2D.MipLevels = Texture->MipCount;
// 	}
// 	else
// 	{
// 		if (SrvDesc.ViewDimension == D3D12_SRV_DIMENSION_TEXTURE2D)
// 		{
// 			SrvDesc.Texture2D.MipLevels = 1;
// 			SrvDesc.Texture2D.MostDetailedMip = InSubresource;
// 		}
// 		if (SrvDesc.ViewDimension == D3D12_SRV_DIMENSION_TEXTURE2DARRAY)
// 		{
// 			SrvDesc.Texture2DArray.MipLevels = 1;
// 			SrvDesc.Texture2DArray.MostDetailedMip = InSubresource;
// 		}
// 	}
// 
// 	if (SrvDesc.ViewDimension == D3D12_SRV_DIMENSION_TEXTURE2DARRAY)
// 	{
// 		SrvDesc.Texture2DArray.ArraySize = Texture->ArrayCount;
// 	}
// 	if (SrvDesc.ViewDimension == D3D12_SRV_DIMENSION_TEXTURECUBE)
// 	{
// 		SrvDesc.TextureCube.MipLevels = Texture->MipCount;
// 		SrvDesc.TextureCube.MostDetailedMip = 0;
// 	}
// 
// 	m_Device->CreateShaderResourceView(Texture->Resource.D3DResource, &SrvDesc, SlotAlloc.CpuHandle);
// 	m_Device->CreateSampler(&Texture->D3DSampleDesc, SampleAlloc.CpuHandle);
// }
// 
// void D3D12GraphicsDevice::CreateUnorderedAccessView(IShaderResourceTable* InDestTable, ITextureBase* InTexture, uint32 Slot, uint32 InSubresource)
// {
// 	D3D12ShaderResourceTable* DestTable = (D3D12ShaderResourceTable*)InDestTable;
// 	D3D12TextureBase* Texture = (D3D12TextureBase*)InTexture;
// 	DestTable->UnorderedAccessResources[Slot] = Texture;
// 
// 	D3D12DescriptorAllocation SlotAlloc = DestTable->BaseUAVAllocation.OffsetFromStart(Slot);
// 
// 	D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc = { };
// 	UavDesc.Format = D3D_TranslateFormat(Texture->Format);
// 	UavDesc.ViewDimension = Texture->Resource.UAVDimension;
// 	if (InSubresource == 0)
// 	{
// 		UavDesc.Texture2D.MipSlice = 0;
// 		UavDesc.Texture2D.PlaneSlice = 0;
// 	}
// 	else
// 	{
// 		if (InSubresource <= Texture->MipCount)
// 		{
// 			UavDesc.Texture2D.MipSlice = InSubresource;
// 		}
// 		else
// 		{
// 			__debugbreak();
// 		}
// 	}
// 
// 	m_Device->CreateUnorderedAccessView(Texture->Resource.D3DResource, nullptr, &UavDesc, SlotAlloc.CpuHandle);
// }

D3D12ShaderItemData GetShaderRequirements(IGraphicsPipelineState* pState)
{
	GFX_PIPELINE_STATE_DESC PipelineDesc = pState->Desc;
	auto& Counts = PipelineDesc.Counts;
	D3D12ShaderItemData ParameterList(
		static_cast<uint32>(Counts.NumConstantBuffers), 
		static_cast<uint32>(Counts.NumShaderResourceViews), 
		static_cast<uint32>(Counts.NumUnorderedAccessViews), 
		static_cast<uint32>(Counts.NumSamplers)
	);
	return ParameterList;
}

D3D12ShaderItemData GetShaderRequirements(IComputePipelineState* pState)
{
	COMPUTE_PIPELINE_STATE_DESC Desc = pState->Desc;
	auto& Counts = Desc.Counts;
	D3D12ShaderItemData ParameterList;
	ParameterList.NumCBVs = Counts.NumConstantBuffers;
	ParameterList.NumSRVs = Counts.NumShaderResourceViews;
	ParameterList.NumUAVs = Counts.NumUnorderedAccessViews;
	ParameterList.NumSMPs = Counts.NumSamplers;
	return ParameterList;
}

IRenderTargetView* D3D12GraphicsDevice::CreateRenderTargetView(ITexture2D* InTexture)
{
	D3D12RenderTargetView* Result = new D3D12RenderTargetView();
#if DEBUG
	if (!InTexture->IsRenderTarget())
	{
		BaneLog() << "The texture: " << InTexture->GetDebugName() << " is not a valid resource for making a render target view";
		return nullptr;
	}
#endif
	D3D12TextureBase* Textures = (D3D12TextureBase*)InTexture;
	D3D12DescriptorAllocation Allocations[3] = { };
	for (uint32 i = 0; i < 3; i++)
	{
		Allocations[i] = m_RtvAllocator.AllocateDescriptor();
		m_Device->CreateRenderTargetView(Textures[i].Resource.D3DResource, nullptr, Allocations[i].CpuHandle);
		Result->FrameData[i] = { &Textures[i], Allocations[i].CpuHandle, Allocations[i].GpuHandle };
	}
	return Result;
}

IDepthStencilView* D3D12GraphicsDevice::CreateDepthStencilView(ITexture2D* InTexture)
{
	D3D12DepthStencilView* Result = new D3D12DepthStencilView();
#if DEBUG
	if (!InTexture->IsDepthStencil())
	{
		BaneLog() << "The texture: " << InTexture->GetDebugName() << " is not a valid resource for making a depth stencil view";
		return nullptr;
	}
#endif
	D3D12TextureBase* Textures = (D3D12TextureBase*)InTexture;
	D3D12DescriptorAllocation Allocations[3] = { };
	for (uint32 i = 0; i < 3; i++)
	{
		Allocations[i] = m_DsvAllocator.AllocateDescriptor();
		m_Device->CreateDepthStencilView(Textures[i].Resource.D3DResource, nullptr, Allocations[i].CpuHandle);
		Result->FrameData[i] = { &Textures[i], Allocations[i].CpuHandle, Allocations[i].GpuHandle };
	}
	return Result;
}

IDepthStencilState* D3D12GraphicsDevice::CreateDepthStencilState(const GFX_DEPTH_STENCIL_DESC& InDesc)
{
	return new D3D12DepthStencilState(InDesc);
}

IDeviceSwapChain* D3D12GraphicsDevice::GetSwapChain()
{
	return m_SwapChain;
}


EMULTISAMPLE_LEVEL D3D12GraphicsDevice::GetMaximumMultisampleLevel() const
{
	return m_MaxMultisampleLevel;
}

D3D12CommandList* D3D12GraphicsDevice::GetCommandList(ECOMMAND_CONTEXT_TYPE ContextType)
{
	return m_AvailableCLs[ContextType].Pop();
}

void D3D12GraphicsDevice::UpdateCurrentFrameInfo()
{
	GCurrentFrameIndex = m_SwapChain->SwapChain->GetCurrentBackBufferIndex();
	//m_SrvAllocator.Reset();
	//m_SmpAllocator.Reset();
	reinterpret_cast<D3D12GraphicsCommandContext*>(GetGraphicsContext())->Begin();
	m_RTHeaps[GCurrentFrameIndex].ResetHeap();
}

void D3D12GraphicsDevice::EnsureAllUploadsOccured()
{
	if (m_UploadList->CurrentCommandBuffer)
	{
		if (m_UploadList->CurrentCommandBuffer->HasDoneWork())
		{
			D3D12GraphicsCommandBuffer* CmdBuff = m_UploadList->CurrentCommandBuffer;
			D3D12CommandList* CommandList = CmdBuff->CommandList;
			CmdBuff->CloseCommandBuffer();
			D3D12CommandQueue& CmdQueue = GetCopyQueue();
			CmdQueue.ExecuteImmediate(CommandList, true);
			CmdBuff->FetchNewCommandList();
		}
	}
}
