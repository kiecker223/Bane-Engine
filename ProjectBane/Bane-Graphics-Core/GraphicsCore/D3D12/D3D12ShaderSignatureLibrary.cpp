#include "D3D12ShaderSignatureLibrary.h"
#include "D3D12PipelineState.h"
#include "BaneMath.h"
#include "Platform/System/Logging/Logger.h"


D3D12ShaderSignatureLibrary* D3D12ShaderSignatureLibrary::GD3D12ShaderSignatureLibraryInstance = nullptr;


typedef struct D3D12_ROOT_SIGNATURE_INFO {
	D3D12_ROOT_SIGNATURE_DESC Desc;
	D3D12ShaderItemData Data;
} D3D12_ROOT_SIGNATURE_INFO;

D3D12_ROOT_SIGNATURE_INFO CreateRootSignatureDesc(const D3D12ShaderItemData& InData)
{
	D3D12_ROOT_SIGNATURE_INFO Result = { { } };
	D3D12_ROOT_SIGNATURE_DESC& ResultDesc = Result.Desc;
	Result.Data = InData;

	uint32 NumParameters = InData.NumCBVs + (InData.NumSRVs >= 1 ? 1 : 0) + (InData.NumUAVs >= 1 ? 1 : 0) + (InData.NumSMPs >= 1 ? 1 : 0); // Only two because we can throw them into descriptor tables
	D3D12_ROOT_PARAMETER* pRootParams = new D3D12_ROOT_PARAMETER[NumParameters];
	ZeroMemory(pRootParams, sizeof(D3D12_ROOT_PARAMETER) * NumParameters);

	uint32 i = 0;
	uint32 NumProcessed;
	for (; i < InData.NumCBVs; i++)
	{
		CD3DX12_ROOT_PARAMETER::InitAsConstantBufferView(pRootParams[i], i);
		NumProcessed = i;
	}
	if (InData.NumSRVs > 0)
	{
		D3D12_DESCRIPTOR_RANGE* Range = new D3D12_DESCRIPTOR_RANGE();
		CD3DX12_DESCRIPTOR_RANGE::Init(*Range, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, InData.NumSRVs, 0);
		pRootParams[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		pRootParams[i].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		pRootParams[i].DescriptorTable.pDescriptorRanges = Range;
		pRootParams[i].DescriptorTable.NumDescriptorRanges = 1;
		i++;
	}
	if (InData.NumSMPs > 0)
	{
		D3D12_DESCRIPTOR_RANGE* Range = new D3D12_DESCRIPTOR_RANGE();
		CD3DX12_DESCRIPTOR_RANGE::Init(*Range, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, InData.NumSMPs, 0);
		pRootParams[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		pRootParams[i].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		pRootParams[i].DescriptorTable.pDescriptorRanges = Range;
		pRootParams[i].DescriptorTable.NumDescriptorRanges = 1;
		i++;
	}
	if (InData.NumUAVs > 0)
	{
		D3D12_DESCRIPTOR_RANGE* Range = new D3D12_DESCRIPTOR_RANGE();
		CD3DX12_DESCRIPTOR_RANGE::Init(*Range, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, InData.NumUAVs, 0);
		pRootParams[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		pRootParams[i].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		pRootParams[i].DescriptorTable.pDescriptorRanges = Range;
		pRootParams[i].DescriptorTable.NumDescriptorRanges = 1;
		i++;
	}
	ResultDesc.pParameters = pRootParams;
	ResultDesc.NumParameters = NumParameters;
	ResultDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	return Result;
}

void DisposeOfRootSignatureDescriptor(D3D12_ROOT_SIGNATURE_DESC& Desc)
{
	for (uint32 i = 0; i < Desc.NumParameters; i++)
	{
		if (Desc.pParameters[i].ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
		{
			delete Desc.pParameters[i].DescriptorTable.pDescriptorRanges;
		}
	}
	delete[] Desc.pParameters;
}

D3D12ShaderSignature CreateRootSignature(ID3D12Device1* Device, D3D12_ROOT_SIGNATURE_INFO InDesc)
{
	D3D12ShaderSignature Result;
	ID3DBlob* ByteCode = nullptr;
	ID3DBlob* ErrorMessage = nullptr;

	HRESULT SerializeResult = D3D12SerializeRootSignature(&InDesc.Desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &ByteCode, &ErrorMessage);

	if (FAILED(SerializeResult))
	{
		BaneLog() << (const char*)ErrorMessage->GetBufferPointer() << END_LINE;
		ErrorMessage->Release();
	}

	Result.ShaderData = InDesc.Data;
	D3D12ERRORCHECK(Device->CreateRootSignature(1, ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), IID_PPV_ARGS(&Result.RootSignature)));
	DisposeOfRootSignatureDescriptor(InDesc.Desc);
	ByteCode->Release();
	return Result;
}

void D3D12ShaderSignatureLibrary::Initialize(ID3D12Device1* Device)
{
	m_TieredSignatures[(uint32)D3D12_SHADER_SIGNATURE_TIER_1CBV_1SRV_1SAMPLER]			= CreateRootSignature(Device, CreateRootSignatureDesc(D3D12ShaderItemData( 1, 1, 0, 1 )));
	m_TieredSignatures[(uint32)D3D12_SHADER_SIGNATURE_TIER_2CBV_2SRV_2SAMPLER]			= CreateRootSignature(Device, CreateRootSignatureDesc(D3D12ShaderItemData( 2, 2, 0, 2 )));
	m_TieredSignatures[(uint32)D3D12_SHADER_SIGNATURE_TIER_4CBV_4SRV_4SAMPLER]			= CreateRootSignature(Device, CreateRootSignatureDesc(D3D12ShaderItemData( 4, 4, 0, 4 )));
	m_TieredSignatures[(uint32)D3D12_SHADER_SIGNATURE_TIER_8CBV_8SRV_8SAMPLER]			= CreateRootSignature(Device, CreateRootSignatureDesc(D3D12ShaderItemData( 8, 8, 0, 8 )));
	m_TieredSignatures[(uint32)D3D12_SHADER_SIGNATURE_TIER_16CBV_16SRV_16SAMPLER]			= CreateRootSignature(Device, CreateRootSignatureDesc(D3D12ShaderItemData( 16, 16, 0, 16 )));
// 	m_TieredSignatures[(uint32)D3D12_SHADER_SIGNATURE_TIER_2UAV]							= CreateRootSignature(Device, CreateRootSignatureDesc(D3D12ShaderItemData( 0, 0, 2, 0 )));
// 	m_TieredSignatures[(uint32)D3D12_SHADER_SIGNATURE_TIER_4UAV]							= CreateRootSignature(Device, CreateRootSignatureDesc(D3D12ShaderItemData( 0, 0, 4, 0 )));
//	m_TieredSignatures[(uint32)D3D12_SHADER_SIGNATURE_TIER_2CBV_2UAV]						= CreateRootSignature(Device, CreateRootSignatureDesc(D3D12ShaderItemData( 2, 0, 2, 0 )));
//	m_TieredSignatures[(uint32)D3D12_SHADER_SIGNATURE_TIER_4CBV_4UAV]						= CreateRootSignature(Device, CreateRootSignatureDesc(D3D12ShaderItemData( 4, 0, 4, 0 )));
	m_TieredSignatures[(uint32)D3D12_SHADER_SIGNATURE_TIER_1CBV_1SRV_1UAV_1SAMPLER]		= CreateRootSignature(Device, CreateRootSignatureDesc(D3D12ShaderItemData( 1, 1, 1, 1 )));
	m_TieredSignatures[(uint32)D3D12_SHADER_SIGNATURE_TIER_2CBV_2SRV_2UAV_2SAMPLER]		= CreateRootSignature(Device, CreateRootSignatureDesc(D3D12ShaderItemData( 2, 2, 2, 2 )));
	m_TieredSignatures[(uint32)D3D12_SHADER_SIGNATURE_TIER_4CBV_4SRV_4UAV_4SAMPLER]		= CreateRootSignature(Device, CreateRootSignatureDesc(D3D12ShaderItemData( 4, 4, 4, 4 )));
	m_TieredSignatures[(uint32)D3D12_SHADER_SIGNATURE_TIER_8CBV_8SRV_8UAV_8SAMPLER]		= CreateRootSignature(Device, CreateRootSignatureDesc(D3D12ShaderItemData( 8, 8, 8, 8 )));
	m_TieredSignatures[(uint32)D3D12_SHADER_SIGNATURE_TIER_16CBV_16SRV_16UAV_16SAMPLER]	= CreateRootSignature(Device, CreateRootSignatureDesc(D3D12ShaderItemData( 16, 16, 16, 16 )));
}

void D3D12ShaderItemData::Quantize()
{
	bool bSkipUAVs = false;
	if (NumUAVs == 0)
	{
		bSkipUAVs = true;
	}

	if (NumCBVs == 0 && NumSRVs == 0 && NumUAVs == 0)
	{
		BaneLog() << "Invalid shader parameters here!!";
		return;
	}

	if ((NumCBVs == 1 || NumCBVs == 0) && (NumSRVs == 1 || NumSRVs == 0) && (NumUAVs == 1 || NumUAVs == 0))
	{
		return;
	}

	NumCBVs = NextPowerOfTwo(NumCBVs);
	NumSRVs = NextPowerOfTwo(NumSRVs);

	if (!bSkipUAVs)
	{
		NumUAVs = NextPowerOfTwo(NumUAVs);
	}

	uint32 LargestParameter = GetLargestParameterSize();
	NumCBVs = LargestParameter;
	NumSRVs = LargestParameter;

	if (!bSkipUAVs)
	{
		NumUAVs = LargestParameter;
	}
}

void D3D12ShaderSignatureLibrary::Shutdown()
{
	for (int i = 0; i < D3D12_SHADER_SIGNATURE_NUM_TIERS; i++)
	{
		m_TieredSignatures[i].Delete();
	}
} 

D3D12ShaderSignature D3D12ShaderSignatureLibrary::GetSignature(const PIPELINE_STATE_RESOURCE_COUNTS& ParameterList)
{
	ED3D12_SHADER_SIGNATURE_TIER Tier = GetTier(ParameterList);
	return m_TieredSignatures[(uint32)Tier];
}

D3D12ShaderSignature D3D12ShaderSignatureLibrary::DetermineBestRootSignature(const PIPELINE_STATE_RESOURCE_COUNTS& Counts)
{
	D3D12ShaderItemData ParameterList(Counts);
	ED3D12_SHADER_SIGNATURE_TIER Tier = GetTier(ParameterList);
	return m_TieredSignatures[(uint32)Tier];
}

ED3D12_SHADER_SIGNATURE_TIER D3D12ShaderSignatureLibrary::GetTier(const D3D12ShaderItemData& InData)
{
	ED3D12_SHADER_SIGNATURE_TIER Tier;
	D3D12ShaderItemData Data = InData;
	if (Data.NumCBVs <= 1 && Data.NumSRVs <= 1 && Data.NumUAVs == 0)
	{
		Tier = D3D12_SHADER_SIGNATURE_TIER_1CBV_1SRV_1SAMPLER;
	}
	else if (Data.NumCBVs <= 1 && Data.NumSRVs <= 1 && Data.NumUAVs == 1)
	{
		return D3D12_SHADER_SIGNATURE_TIER_1CBV_1SRV_1UAV_1SAMPLER;
	}

	bool bHasUAVs = Data.NumUAVs != 0;

	Data.NumCBVs = NextPowerOfTwo(Data.NumCBVs);
	Data.NumSRVs = NextPowerOfTwo(Data.NumSRVs);
	Data.NumUAVs = NextPowerOfTwo(Data.NumUAVs);

	uint32 NumOfLargestParam = Data.GetLargestParameterSize();

	Tier = D3D12_SHADER_SIGNATURE_TIER_1CBV_1SRV_1SAMPLER;
	switch (NumOfLargestParam)
	{
		case 0:
		{
			Tier = D3D12_SHADER_SIGNATURE_TIER_1CBV_1SRV_1SAMPLER;
		} break;
		case 2:
		{
			Tier = (bHasUAVs ? D3D12_SHADER_SIGNATURE_TIER_2CBV_2SRV_2UAV_2SAMPLER : D3D12_SHADER_SIGNATURE_TIER_2CBV_2SRV_2SAMPLER);
		} break;
		case 4:
		{
			Tier = (bHasUAVs ? D3D12_SHADER_SIGNATURE_TIER_4CBV_4SRV_4UAV_4SAMPLER : D3D12_SHADER_SIGNATURE_TIER_4CBV_4SRV_4SAMPLER);
		} break;
		case 8:
		{
			Tier = (bHasUAVs ? D3D12_SHADER_SIGNATURE_TIER_8CBV_8SRV_8UAV_8SAMPLER : D3D12_SHADER_SIGNATURE_TIER_8CBV_8SRV_8SAMPLER);
		} break;
		case 16:
		{
			Tier = (bHasUAVs ? D3D12_SHADER_SIGNATURE_TIER_16CBV_16SRV_16UAV_16SAMPLER : D3D12_SHADER_SIGNATURE_TIER_16CBV_16SRV_16SAMPLER);
		} break;
	}
	return Tier;
}
