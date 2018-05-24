#include "D3D12PipelineLibrary.h"
#include <d3d12.h>


ID3D12PipelineLibrary* D3D12PipelineLibrary::GInstance;


ID3D12PipelineLibrary* GetD3D12PipelineLibrary()
{
	return D3D12PipelineLibrary::GInstance;
}

void InitializeD3D12PipelineLibrary(ID3D12Device1* Device)
{
	Device->CreatePipelineLibrary(nullptr, 0, IID_PPV_ARGS(&D3D12PipelineLibrary::GInstance));
}

void DestroyD3D12PipelineLibrary()
{
	GetD3D12PipelineLibrary()->Release();
	D3D12PipelineLibrary::GInstance = nullptr;
}
