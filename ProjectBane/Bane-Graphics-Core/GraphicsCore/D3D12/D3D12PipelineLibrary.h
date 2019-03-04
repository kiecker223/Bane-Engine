#pragma once

#include <d3d12.h>

class D3D12PipelineLibrary
{
public:

	static ID3D12PipelineLibrary* GInstance;
};

ID3D12PipelineLibrary* GetD3D12PipelineLibrary();
void InitializeD3D12PipelineLibrary(ID3D12Device1* Device);
void DestroyD3D12PipelineLibrary();

