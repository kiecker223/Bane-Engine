#include "D3D12GraphicsDevice.h"
#include "Systems/Logging/Logger.h"
#include "D3D12Translator.h"
#include "../IO/ShaderCache.h"
#include "D3D12PipelineLibrary.h"
#include "D3D12PipelineState.h"
#include "D3D12GraphicsResource.h"
#include <thread>
#include <d3dcompiler.h>


uint D3D12GraphicsDevice::GCurrentFrameIndex = 0;


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

D3D12GraphicsDevice::D3D12GraphicsDevice(D3D12SwapChain* SwapChain, Window* RenderingWindow, ID3D12Device1* Device, ID3D12CommandQueue* MainQueue) :
	m_CommandQueues{
		D3D12CommandQueue(MainQueue, Device, "D3D12GraphicsDevice::m_DirectQueue", COMMAND_CONTEXT_TYPE_GRAPHICS),
		D3D12CommandQueue({ D3D12_COMMAND_LIST_TYPE_COMPUTE, 100, D3D12_COMMAND_QUEUE_FLAG_NONE, 1 }, Device, "D3D12GraphicsDevice::m_ComputeQueue", COMMAND_CONTEXT_TYPE_COMPUTE),
		D3D12CommandQueue({ D3D12_COMMAND_LIST_TYPE_COPY, 100, D3D12_COMMAND_QUEUE_FLAG_NONE, 1 }, Device, "D3D12GraphicsDevice::m_UploadList", COMMAND_CONTEXT_TYPE_COPY)
	},
	m_Device(Device),
	m_SwapChain(SwapChain),
	m_GenerateMipsPipeline2D(nullptr),
	m_GenerateMipsTable2D(nullptr)
{
	m_SwapChain->Device = this;
	uint AvailableThreadCount = std::thread::hardware_concurrency();
	assert(AvailableThreadCount != 0);
	m_AvailableContexts.reserve(AvailableThreadCount);
	m_AvailableContexts.resize(AvailableThreadCount);

	m_SrvAllocator.Initialize(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 256, true);
	m_RtvAllocator.Initialize(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 48);
	m_DsvAllocator.Initialize(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 9);
	m_SmpAllocator.Initialize(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 20, true);
	

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

	m_DefaultSamplerState = CreateSamplerState(CreateDefaultSamplerDesc());

	{
		ID3D12Resource* BackBuffers[3] = { };
		for (uint i = 0; i < 3; i++)
		{
			m_SwapChain->SwapChain->GetBuffer(i, IID_PPV_ARGS(&BackBuffers[i]));
		}

		D3D12TextureBase* Bases = new D3D12TextureBase[3];

		for (uint i = 0; i < 3; i++)
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
		}

		m_BackBuffer = CreateRenderTargetView(Bases);

		ITexture2D* DepthTexture = CreateTexture2D(RenderingWindow->GetWidth(), RenderingWindow->GetHeight(), FORMAT_UNKNOWN, TEXTURE_USAGE_DEPTH_STENCIL, nullptr);
		IDepthStencilView* DepthStencil = CreateDepthStencilView(DepthTexture);

		m_BasicRenderPass = (D3D12RenderPassInfo*)IRuntimeGraphicsDevice::CreateRenderPass(m_BackBuffer, DepthStencil, XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f));
	}
	
	for (uint i = 0; i < COMMAND_CONTEXT_TYPE_NUM_TYPES; i++)
	{	
		for (uint b = 0; b < 4; b++)
		{
			ID3D12CommandAllocator* CommandAllocator;
			ID3D12GraphicsCommandList* CommandList;
			D3D12_COMMAND_LIST_TYPE ListType = FromContextType((ECOMMAND_CONTEXT_TYPE)i);

			D3D12ERRORCHECK(
				Device->CreateCommandAllocator(ListType, IID_PPV_ARGS(&CommandAllocator));
			);

			D3D12ERRORCHECK(
				Device->CreateCommandList(1, ListType, CommandAllocator, nullptr, IID_PPV_ARGS(&CommandList))
			);

			m_AvailableCLs[i].Push(new D3D12CommandList(CommandAllocator, CommandList));
		}
	}

	for (uint i = 0; i < AvailableThreadCount; i++)
	{
		m_AvailableContexts[i] = new D3D12GraphicsCommandContext(this, COMMAND_CONTEXT_TYPE_GRAPHICS, m_Rect, m_ViewPort);
	}
	m_ComputeContext = new D3D12ComputeCommandContext(this, COMMAND_CONTEXT_TYPE_COMPUTE);
	m_UploadList = new D3D12GraphicsCommandContext(this, COMMAND_CONTEXT_TYPE_COPY, m_Rect, m_ViewPort);
	m_CommandQueues[0].SetParentDevice(this);
	m_CommandQueues[1].SetParentDevice(this);
	m_CommandQueues[2].SetParentDevice(this);
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

static std::vector<uint8> CompileFromByteCode(const std::string& InByteCode, ESHADER_STAGE ShaderStage, uint& OutNumConstantBuffers, uint& OutNumSamplers, uint& OutNumShaderResourceViews, uint& OutNumUnorderedAccessViews)
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

	uint CompileFlags = 0;
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
			uint NumConstBuffers = 0;

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
			uint NumSamplers = 0;
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
			uint NumShaderResources = 0;
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
			uint NumUnorderedAccessViews = 0;
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

	std::vector<uint8> RetCode(CompiledCode->GetBufferSize());
	memcpy(RetCode.data(), CompiledCode->GetBufferPointer(), RetCode.size());
	CompiledCode->Release();
	return RetCode;
}

IVertexShader* D3D12GraphicsDevice::CreateVertexShader(const std::string& ByteCode)
{
	uint NumConstantBuffers;
	uint NumSamplers;
	uint NumShaderResources;
	uint NumUnorderedAccessViews;
	std::vector<uint8> CompiledCode = CompileFromByteCode(ByteCode, SHADER_STAGE_VERTEX, NumConstantBuffers, NumSamplers, NumShaderResources, NumUnorderedAccessViews);
#ifdef _DEBUG
	if (CompiledCode.size() == 0)
	{
		return nullptr;
	}
#endif
	return new D3D12VertexShader(ByteCode, CompiledCode, NumConstantBuffers, NumShaderResources, NumSamplers, NumUnorderedAccessViews);
}

IPixelShader* D3D12GraphicsDevice::CreatePixelShader(const std::string& ByteCode)
{
	uint NumConstantBuffers;
	uint NumSamplers;
	uint NumShaderResources;
	uint NumUnorderedAccessViews;
	std::vector<uint8> CompiledCode = CompileFromByteCode(ByteCode, SHADER_STAGE_PIXEL, NumConstantBuffers, NumSamplers, NumShaderResources, NumUnorderedAccessViews);

#ifdef _DEBUG
	if (CompiledCode.size() == 0)
	{
		return nullptr;
	}
#endif

	return new D3D12PixelShader(ByteCode, CompiledCode, NumConstantBuffers, NumShaderResources, NumSamplers, NumUnorderedAccessViews);
}

IGeometryShader* D3D12GraphicsDevice::CreateGeometryShader(const std::string& ByteCode)
{
	uint NumConstantBuffers;
	uint NumSamplers;
	uint NumShaderResources;
	uint NumUnorderedAccessViews;
	std::vector<uint8> CompiledCode = CompileFromByteCode(ByteCode, SHADER_STAGE_GEOMETRY, NumConstantBuffers, NumSamplers, NumShaderResources, NumUnorderedAccessViews);
#ifdef _DEBUG
	if (CompiledCode.size() == 0)
	{
		return nullptr;
	}
#endif
	return new D3D12GeometryShader(ByteCode, CompiledCode, NumConstantBuffers, NumShaderResources, NumSamplers, NumUnorderedAccessViews);
}

IHullShader* D3D12GraphicsDevice::CreateHullShader(const std::string& ByteCode)
{
	uint NumConstantBuffers;
	uint NumSamplers;
	uint NumShaderResources;
	uint NumUnorderedAccessViews;
	std::vector<uint8> CompiledCode = CompileFromByteCode(ByteCode, SHADER_STAGE_HULL, NumConstantBuffers, NumSamplers, NumShaderResources, NumUnorderedAccessViews);
#ifdef _DEBUG
	if (CompiledCode.size() == 0)
	{
		return nullptr;
	}
#endif
	return new D3D12HullShader(ByteCode, CompiledCode, NumConstantBuffers, NumShaderResources, NumSamplers, NumUnorderedAccessViews);
}

IComputeShader* D3D12GraphicsDevice::CreateComputeShader(const std::string& ByteCode)
{
	uint NumConstantBuffers;
	uint NumSamplers;
	uint NumShaderResources;
	uint NumUnorderedAccessViews;
	std::vector<uint8> CompiledCode = CompileFromByteCode(ByteCode, SHADER_STAGE_COMPUTE, NumConstantBuffers, NumSamplers, NumShaderResources, NumUnorderedAccessViews);
#ifdef _DEBUG
	if (CompiledCode.size() == 0)
	{
		return nullptr;
	}
#endif
	return new D3D12ComputeShader(ByteCode, CompiledCode, NumConstantBuffers, NumShaderResources, NumSamplers, NumUnorderedAccessViews);
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
	D3D12ShaderSignature UsedSignature = GetD3D12ShaderSignatureLibrary()->DetermineBestComputeSignature(Shader);
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
	D3D12ShaderSignature UsedSignature = GetD3D12ShaderSignatureLibrary()->DetermineBestComputeSignature(Shader);
	D3D12_COMPUTE_PIPELINE_STATE_DESC CreationDesc = { };
	CreationDesc.pRootSignature = UsedSignature.RootSignature;
	CreationDesc.NodeMask = 0;
	CreationDesc.CS = Shader->GetShaderByteCode();
	PipelineState->Reset(m_Device, CreationDesc, *NewDesc, UsedSignature);
}

IVertexBuffer* D3D12GraphicsDevice::CreateVertexBuffer(uint ByteCount, uint8* Buffer)
{
	if (!m_UploadList->HasBegun())
	{
		m_UploadList->Begin();
	}

	D3D12Buffer* Result = new D3D12Buffer(this, ByteCount, BUFFER_USAGE_GPU);
	Result->UploadDataToGPU(m_UploadList, Buffer);
	return Result;
}

IIndexBuffer* D3D12GraphicsDevice::CreateIndexBuffer(uint ByteCount, uint8* Buffer)
{
	if (!m_UploadList->HasBegun())
	{
		m_UploadList->Begin();
	}

	D3D12Buffer* Result = new D3D12Buffer(this, ByteCount, BUFFER_USAGE_GPU);
	Result->UploadDataToGPU(m_UploadList, Buffer);
	return Result;
}

IConstantBuffer* D3D12GraphicsDevice::CreateConstantBuffer(uint ByteCount)
{
	return new D3D12Buffer(this, ByteCount, BUFFER_USAGE_CPU);
}

IBuffer* D3D12GraphicsDevice::CreateStagingBuffer(uint ByteCount)
{
	return new D3D12Buffer(this, ByteCount, BUFFER_USAGE_UPLOAD);
}

ITexture2D* D3D12GraphicsDevice::CreateTexture2D(uint Width, uint Height, EFORMAT Format, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data)
{
	D3D12TextureBase* Result; 
	bool bDoNoUpload = false; // Never start off with initial contents, only allow texture to texture copies or writes
	if ((Usage & TEXTURE_USAGE_RENDER_TARGET) == TEXTURE_USAGE_RENDER_TARGET)
	{
		Result = new D3D12TextureBase[3]{
			D3D12TextureBase(this, Width, Height, 1U, 1U, Format, Usage),
			D3D12TextureBase(this, Width, Height, 1U, 1U, Format, Usage),
			D3D12TextureBase(this, Width, Height, 1U, 1U, Format, Usage)
		};

		for (uint i = 0; i < 3; i++)
		{
			Result[i].Resource.SRVDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			Result[i].Resource.UAVDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		}

		bDoNoUpload = true;
	}
	else if ((Usage & TEXTURE_USAGE_DEPTH_STENCIL) == TEXTURE_USAGE_DEPTH_STENCIL) // We do an explicit check because there is a specific format to be enforced here
	{
		Result = new D3D12TextureBase[3]{
			D3D12TextureBase(this, Width, Height, 1U, 1U, FORMAT_D24_UNORM_S8_UINT, Usage),
			D3D12TextureBase(this, Width, Height, 1U, 1U, FORMAT_D24_UNORM_S8_UINT, Usage),
			D3D12TextureBase(this, Width, Height, 1U, 1U, FORMAT_D24_UNORM_S8_UINT, Usage),
		};

		for (uint i = 0; i < 3; i++)
		{
			Result[i].Resource.SRVDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			Result[i].Resource.UAVDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		}

		bDoNoUpload = true;
	}
	else
	{
		Result = new D3D12TextureBase(this, Width, Height, 1U, 1U, Format, Usage);
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

ITexture2DArray* D3D12GraphicsDevice::CreateTexture2DArray(uint Width, uint Height, uint Count, EFORMAT Format, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data)
{
	D3D12TextureBase* Result = new D3D12TextureBase(this, Width, Height, 1, Count, Format, Usage);

	if (Data)
	{
		Result->UploadToGPU(m_UploadList, Data->Pointer, Data->Width, Data->Height, Data->Depth, Data->Step);
	}

	Result->Resource.SRVDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
	Result->Resource.UAVDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;

	return Result;
}

ITexture3D* D3D12GraphicsDevice::CreateTexture3D(uint Width, uint Height, uint Depth, EFORMAT Format, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data)
{
	D3D12TextureBase* Result = nullptr;

	Result = new D3D12TextureBase(this, Width, Height, Depth, 1, Format, Usage);

	if (Data)
	{
		Result->UploadToGPU(m_UploadList, Data->Pointer, Data->Width, Data->Height, Data->Depth, Data->Step);
	}
	Result->Resource.SRVDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
	Result->Resource.UAVDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
	
	return Result;
}

ITextureCube* D3D12GraphicsDevice::CreateTextureCube(uint CubeSize, EFORMAT Format, ETEXTURE_USAGE Usage, const SUBRESOURCE_DATA* Data)
{
	D3D12TextureBase* TextureArray = (D3D12TextureBase*)CreateTexture2DArray(CubeSize, CubeSize, 6, Format, Usage, Data);
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
	if (!bDirectContextHasBegun)
	{
		DirectContext->Begin();
	}
	else
	{
		DirectContext->EndPass();
		DirectContext->Flush();
		DirectContext->Begin();
	}
	EnsureAllUploadsOccured();

	ID3D12GraphicsCommandList* DirectCL = DirectContext->CommandList->GetGraphicsCommandList();
	ID3D12Resource* Res = Texture->Resource.D3DResource;

	if (m_GenerateMipsPipeline2D == nullptr)
	{
		m_GenerateMipsPipeline2D = (D3D12ComputePipelineState*)GetShaderCache()->LoadComputePipeline("Shaders/GenerateMips.hlsl");
		m_GenerateMipsTable2D = (D3D12ShaderResourceTable*)IRuntimeGraphicsDevice::CreateShaderTable((IComputePipelineState*)m_GenerateMipsPipeline2D);
	}

	ID3D12Resource* CopyRes = nullptr;

	if (Texture->Depth == 1)
	{
		{
			D3D12_HEAP_PROPERTIES HeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			D3D12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(D3D_TranslateFormat(Texture->Format), Texture->Width, Texture->Height, Texture->ArrayCount, Texture->MipCount);
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
		DirectCL->CopyResource(CopyRes, Res);
		{
			D3D12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(CopyRes, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
			DirectCL->ResourceBarrier(1, &Barrier);
		}
		DirectContext->End();
		DirectContext->Flush();
		DirectContext->Begin();
		D3D12Buffer* ConstBuff = (D3D12Buffer*)CreateConstBuffer<XMFLOAT4>();
		for (uint b = 0; b < Texture->ArrayCount; b++)
		{
			for (uint i = 0; i < Texture->MipCount - 1; i++)
			{
				ID3D12DescriptorHeap* ppHeaps[2] = { m_SrvAllocator.GetDescriptorHeap(), m_SmpAllocator.GetDescriptorHeap() };
				DirectCL->SetDescriptorHeaps(2, ppHeaps);
				uint DstWidth = max(Texture->Width >> (i + 1), 1);
				uint DstHeight = max(Texture->Height >> (i + 1), 1);

				D3D12DescriptorAllocation UavAllocation = m_GenerateMipsTable2D->BaseUAVAllocation;
				D3D12DescriptorAllocation SrvAllocation = m_GenerateMipsTable2D->BaseSRVAllocation;
				D3D12DescriptorAllocation SmpAllocation = m_GenerateMipsTable2D->BaseSMPAllocation;
				{
					D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc = { };
					UavDesc.Format = D3D_TranslateFormat(Texture->Format);
					UavDesc.ViewDimension = Texture->Resource.UAVDimension;
					if (Texture->ArrayCount == 1)
					{
						UavDesc.Texture2D.MipSlice = i + 1;
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
						SrvDesc.Texture2D.MostDetailedMip = i;
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
				{
					XMFLOAT2* pParams = (XMFLOAT2*)ConstBuff->MappedPointer;
					pParams->x = (1.0f / DstWidth);
					pParams->y = (1.0f / DstHeight);
				}
				DirectCL->SetPipelineState(m_GenerateMipsPipeline2D->PipelineState);
				DirectCL->SetComputeRootSignature(m_GenerateMipsTable2D->AssociatedSignature.RootSignature);

				DirectCL->SetComputeRootConstantBufferView(0, ConstBuff->Resource.Location);
				DirectCL->SetComputeRootDescriptorTable(1, m_GenerateMipsTable2D->BaseSRVAllocation.GpuHandle);
				DirectCL->SetComputeRootDescriptorTable(2, m_GenerateMipsTable2D->BaseSMPAllocation.GpuHandle);
				DirectCL->SetComputeRootDescriptorTable(3, m_GenerateMipsTable2D->BaseUAVAllocation.GpuHandle);
				{
					D3D12_RESOURCE_BARRIER Barrier =
					{
						CD3DX12_RESOURCE_BARRIER::Transition(CopyRes, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, i + 1)
					};
					DirectCL->ResourceBarrier(1, &Barrier);
				}
				DirectCL->Dispatch(max(DstWidth / 8, 1), max(DstHeight / 8, 1), 1);
				{
					D3D12_RESOURCE_BARRIER Barriers[2] =
					{
						CD3DX12_RESOURCE_BARRIER::UAV(CopyRes),
						CD3DX12_RESOURCE_BARRIER::Transition(CopyRes, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, i + 1)
					};
					DirectCL->ResourceBarrier(2, Barriers);
				}
				DirectContext->End();
				DirectContext->Flush();
				DirectContext->Begin();
				DirectCL = DirectContext->CommandList->GetGraphicsCommandList();
			}
		}
		D3D12_RESOURCE_STATES PreviousTextureState = Texture->CurrentState;
		{
			D3D12_RESOURCE_BARRIER Barriers[2] =
			{
				CD3DX12_RESOURCE_BARRIER::Transition(CopyRes, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_SOURCE),
				CD3DX12_RESOURCE_BARRIER::Transition(Res, Texture->CurrentState, D3D12_RESOURCE_STATE_COPY_DEST)
			};
			DirectCL->ResourceBarrier(2, Barriers);
			DirectCL->CopyResource(Res, CopyRes);
		}
		{
			D3D12_RESOURCE_BARRIER Barriers[] =
			{
				CD3DX12_RESOURCE_BARRIER::Transition(Res, D3D12_RESOURCE_STATE_COPY_DEST, PreviousTextureState)
			};
			DirectCL->ResourceBarrier(1, Barriers);
		}

		DirectContext->End();
		DirectContext->Flush();
		delete ConstBuff;
	}
	if (bDirectContextHasBegun)
	{
		DirectContext->Begin();
	}

	while (CopyRes->Release() > 0) { }
}


ISamplerState* D3D12GraphicsDevice::CreateSamplerState(const SAMPLER_DESC& Desc)
{
	D3D12_SAMPLER_DESC CreationDesc = D3D12_TranslateSamplerDesc(Desc);
	return new D3D12SamplerState(CreationDesc, Desc);
}

ISamplerState* D3D12GraphicsDevice::GetDefaultSamplerState()
{
	return m_DefaultSamplerState;
}

IInputLayout* D3D12GraphicsDevice::CreateInputLayout(const GFX_INPUT_LAYOUT_DESC& Desc)
{
	D3D12_INPUT_LAYOUT_DESC CreationDesc = D3D12_TranslateInputLayout(Desc);
	return new D3D12InputLayout(Desc, CreationDesc);
}

IRenderPassInfo* D3D12GraphicsDevice::CreateRenderPass(const IRenderTargetView** RenderTargets, uint NumRenderTargets, const IDepthStencilView* DepthStencil, const XMFLOAT4& ClearColor)
{
	// Consider making this store the command list too? That would make alot of the task submission easier, but potentially less performant
	D3D12RenderPassInfo* RenderPassInfo = new D3D12RenderPassInfo(RenderTargets, NumRenderTargets, DepthStencil, ClearColor);
	return RenderPassInfo;
}

IRenderPassInfo* D3D12GraphicsDevice::GetBackBufferTargetPass()
{
	return m_BasicRenderPass;
}

IRenderTargetView* D3D12GraphicsDevice::GetBackBuffer()
{
	return m_BackBuffer;
}

void D3D12GraphicsDevice::CreateShaderResourceView(IShaderResourceTable* InDestTable, IBuffer* InBuffer, uint InSlot, uint InSubresource)
{
#ifdef _DEBUG
	if (InSubresource != 0)
	{
		BaneLog() << "[WARN] D3D12 : Subresources not supported on buffer type views" << END_LINE;
	}
#endif
	D3D12ShaderResourceTable* DestTable = (D3D12ShaderResourceTable*)InDestTable;
	D3D12Buffer* Buffer = (D3D12Buffer*)InBuffer;
	DestTable->ConstantBuffers[InSlot] = Buffer;
}

void D3D12GraphicsDevice::CreateUnorderedAccessView(IShaderResourceTable* InDestTable, IBuffer* InBuffer, uint InSlot, uint InSubresource)
{
#ifdef _DEBUG
	if (InSubresource != 0)
	{
		BaneLog() << "[WARN] D3D12 : Subresources not supported on buffer type views" << END_LINE;
	}
#endif
	D3D12ShaderResourceTable* DestTable = (D3D12ShaderResourceTable*)InDestTable;
	D3D12Buffer* Buffer = (D3D12Buffer*)InBuffer;
	DestTable->ConstantBuffers[InSlot] = Buffer;
}

void D3D12GraphicsDevice::CreateShaderResourceView(IShaderResourceTable* InDestTable, ITextureBase* InTexture, uint Slot, uint InSubresource)
{
	D3D12ShaderResourceTable* DestTable = (D3D12ShaderResourceTable*)InDestTable;
	D3D12TextureBase* Texture = (D3D12TextureBase*)InTexture;
	DestTable->ShaderResources[Slot] = Texture;
	
	D3D12DescriptorAllocation SlotAlloc = DestTable->BaseSRVAllocation.OffsetFromStart(Slot);

	D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = { };
	SrvDesc.Format = D3D_TranslateFormat(Texture->Format);
	SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SrvDesc.ViewDimension = Texture->Resource.SRVDimension;
	if (InSubresource == 0)
	{
		SrvDesc.Texture2D.MipLevels = Texture->MipCount;
	}
	else
	{
		if (SrvDesc.ViewDimension == D3D12_SRV_DIMENSION_TEXTURE2D)
		{
			SrvDesc.Texture2D.MipLevels = 1;
			SrvDesc.Texture2D.MostDetailedMip = InSubresource;
		}
		if (SrvDesc.ViewDimension == D3D12_SRV_DIMENSION_TEXTURE2DARRAY)
		{
			SrvDesc.Texture2DArray.MipLevels = 1;
			SrvDesc.Texture2DArray.MostDetailedMip = InSubresource;
		}
	}

	if (SrvDesc.ViewDimension == D3D12_SRV_DIMENSION_TEXTURE2DARRAY)
	{
		SrvDesc.Texture2DArray.ArraySize = Texture->ArrayCount;
	}
	if (SrvDesc.ViewDimension == D3D12_SRV_DIMENSION_TEXTURECUBE)
	{
		SrvDesc.TextureCube.MipLevels = Texture->MipCount;
		SrvDesc.TextureCube.MostDetailedMip = 0;
	}

	m_Device->CreateShaderResourceView(Texture->Resource.D3DResource, &SrvDesc, SlotAlloc.CpuHandle);
}

void D3D12GraphicsDevice::CreateUnorderedAccessView(IShaderResourceTable* InDestTable, ITextureBase* InTexture, uint Slot, uint InSubresource)
{
	D3D12ShaderResourceTable* DestTable = (D3D12ShaderResourceTable*)InDestTable;
	D3D12TextureBase* Texture = (D3D12TextureBase*)InTexture;
	DestTable->UnorderedAccessResources[Slot] = Texture;

	D3D12DescriptorAllocation SlotAlloc = DestTable->BaseUAVAllocation.OffsetFromStart(Slot);

	D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc = { };
	UavDesc.Format = D3D_TranslateFormat(Texture->Format);
	UavDesc.ViewDimension = Texture->Resource.UAVDimension;
	if (InSubresource == 0)
	{
		UavDesc.Texture2D.MipSlice = 0;
		UavDesc.Texture2D.PlaneSlice = 0;
	}
	else
	{
		if (InSubresource <= Texture->MipCount)
		{
			UavDesc.Texture2D.MipSlice = InSubresource;
		}
		else
		{
			__debugbreak();
		}
	}

	m_Device->CreateUnorderedAccessView(Texture->Resource.D3DResource, nullptr, &UavDesc, SlotAlloc.CpuHandle);
}

void D3D12GraphicsDevice::CreateSamplerView(IShaderResourceTable* InDestTable, ISamplerState* InSamplerState, uint Slot)
{
	D3D12ShaderResourceTable* DestTable = (D3D12ShaderResourceTable*)InDestTable;
	D3D12SamplerState* SamplerState = (D3D12SamplerState*)InSamplerState;

	DestTable->Samplers[Slot] = SamplerState;

	D3D12DescriptorAllocation SlotAlloc = DestTable->BaseSMPAllocation.OffsetFromStart(Slot);

	m_Device->CreateSampler(&SamplerState->CreationDesc, SlotAlloc.CpuHandle);
}

D3D12ShaderItemData GetShaderRequirements(IVertexShader* VS, IPixelShader* PS, IHullShader* HS, IGeometryShader* GS)
{
	D3D12VertexShader* VertexShader = (D3D12VertexShader*)VS;
	D3D12PixelShader* PixelShader = (D3D12PixelShader*)PS;
	D3D12ShaderItemData ParameterList;
	if (VertexShader)
	{
		ParameterList.NumCBVs += VertexShader->NumConstantBuffers;
		ParameterList.NumSRVs += VertexShader->NumShaderResourceViews;
		ParameterList.NumUAVs += VertexShader->NumUnorderedAccessViews;
		ParameterList.NumSMPs += VertexShader->NumSamplers;
	}
	if (PixelShader)
	{
		ParameterList.NumCBVs += PixelShader->NumConstantBuffers;
		ParameterList.NumSRVs += PixelShader->NumShaderResourceViews;
		ParameterList.NumUAVs += PixelShader->NumUnorderedAccessViews;
		ParameterList.NumSMPs += PixelShader->NumSamplers;
	}
	if (HS)
	{
		D3D12HullShader* HullShader = (D3D12HullShader*)HS;
		ParameterList.NumCBVs += HullShader->NumConstantBuffers;
		ParameterList.NumSRVs += HullShader->NumShaderResourceViews;
		ParameterList.NumUAVs += HullShader->NumUnorderedAccessViews;
		ParameterList.NumSMPs += HullShader->NumSamplers;
	}
	if (GS)
	{
		D3D12GeometryShader* GeometryShader = (D3D12GeometryShader*)GS;
		ParameterList.NumCBVs += GeometryShader->NumConstantBuffers;
		ParameterList.NumSRVs += GeometryShader->NumShaderResourceViews;
		ParameterList.NumUAVs += GeometryShader->NumUnorderedAccessViews;
		ParameterList.NumSMPs += GeometryShader->NumSamplers;
	}
	return ParameterList;
}

D3D12ShaderItemData GetShaderRequirements(IComputeShader* CS)
{
	D3D12ComputeShader* ComputeShader = (D3D12ComputeShader*)CS;
	D3D12ShaderItemData ParameterList;
	ParameterList.NumCBVs = ComputeShader->NumConstantBuffers;
	ParameterList.NumSRVs = ComputeShader->NumShaderResourceViews;
	ParameterList.NumUAVs = ComputeShader->NumUnorderedAccessViews;
	ParameterList.NumSMPs = ComputeShader->NumSamplers;
	return ParameterList;
}

IShaderResourceTable* D3D12GraphicsDevice::CreateShaderTable(IVertexShader* VS, IPixelShader* PS, IHullShader* HS, IGeometryShader* GS)
{
	D3D12ShaderItemData Reqs = GetShaderRequirements(VS, PS, HS, GS);
	D3D12DescriptorAllocation BaseSrvAllocation = m_SrvAllocator.AllocateMultiple(Reqs.NumSRVs); // Just allocate this many so we aren't overusing descriptors
	D3D12DescriptorAllocation BaseSmpAllocation = m_SmpAllocator.AllocateMultiple(Reqs.NumSMPs);
	D3D12DescriptorAllocation BaseUavAllocation = { }; // We can just leave an empty one here as, if there aren't any UAVs we won't be expected to use any
	if (Reqs.NumUAVs > 0)
	{
		BaseUavAllocation = m_SrvAllocator.AllocateMultiple(Reqs.NumUAVs);
	}
	D3D12ShaderResourceTable* Result = new D3D12ShaderResourceTable(Reqs.NumCBVs, Reqs.NumSRVs, Reqs.NumSMPs, Reqs.NumUAVs, BaseSrvAllocation, BaseSmpAllocation, BaseUavAllocation);
	return Result;
}

IShaderResourceTable* D3D12GraphicsDevice::CreateShaderTable(IComputeShader* CS)
{
	D3D12ShaderItemData Reqs = GetShaderRequirements(CS);
	D3D12DescriptorAllocation BaseSrvAllocation = m_SrvAllocator.AllocateMultiple(Reqs.NumSRVs); // Just allocate this many so we aren't overusing descriptors
	D3D12DescriptorAllocation BaseSmpAllocation = m_SmpAllocator.AllocateMultiple(Reqs.NumSMPs);
	D3D12DescriptorAllocation BaseUavAllocation = { }; // We can just leave an empty one here as, if there aren't any UAVs we won't be expected to use any
	if (Reqs.NumUAVs > 0)
	{
		BaseUavAllocation = m_SrvAllocator.AllocateMultiple(Reqs.NumUAVs);
	}
	D3D12ShaderResourceTable* Result = new D3D12ShaderResourceTable(Reqs.NumCBVs, Reqs.NumSRVs, Reqs.NumSMPs, Reqs.NumUAVs, BaseSrvAllocation, BaseSmpAllocation, BaseUavAllocation);
	return Result;
}

IRenderTargetView* D3D12GraphicsDevice::CreateRenderTargetView(ITexture2D* InTexture)
{
	D3D12RenderTargetView* Result = new D3D12RenderTargetView();
	if (!InTexture->IsRenderTarget())
	{
		BaneLog() << "The texture: " << InTexture->GetDebugName() << " is not a valid resource for making a render target view";
		return nullptr;
	}
	D3D12TextureBase* Textures = (D3D12TextureBase*)InTexture;
	D3D12DescriptorAllocation Allocations[3] = { };
	for (uint i = 0; i < 3; i++)
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
	if (!InTexture->IsDepthStencil())
	{
		BaneLog() << "The texture: " << InTexture->GetDebugName() << " is not a valid resource for making a depth stencil view";
		return nullptr;
	}
	D3D12TextureBase* Textures = (D3D12TextureBase*)InTexture;
	D3D12DescriptorAllocation Allocations[3] = { };
	for (uint i = 0; i < 3; i++)
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
}

void D3D12GraphicsDevice::EnsureAllUploadsOccured()
{
	if (m_UploadList->HasBegun()) // If there is work for it to do
	{
		D3D12CommandList* CL = m_UploadList->CommandList;
		m_UploadList->End(); // End it, Begin will be called again when someone wants to upload
		GetCopyQueue().StallForFinish();
		m_UploadList->Begin();
	}
}
