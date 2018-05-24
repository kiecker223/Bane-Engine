#include "D3DCommonTranslator.h"


static std::vector<D3D_PRIMITIVE_TOPOLOGY> D3DPrimitiveTopologies;


void InitializeD3DCommonTranslator()
{
	{
		D3D_PRIMITIVE_TOPOLOGY PrimitiveTopology[3] = {
			D3D_PRIMITIVE_TOPOLOGY_LINELIST,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
		};

		D3DPrimitiveTopologies.resize(3);
		memcpy((void*)D3DPrimitiveTopologies.data(), PrimitiveTopology, sizeof(PrimitiveTopology));
	}
}

D3D_PRIMITIVE_TOPOLOGY D3D_TranslatePrimitiveTopology(EPOLYGON_TYPE InType)
{
	return D3DPrimitiveTopologies[(uint)InType];
}

DXGI_FORMAT D3D_TranslateFormat(EFORMAT Format)
{
	return (DXGI_FORMAT)Format;
}

DXGI_SAMPLE_DESC D3D_TranslateMultisampleLevel(EMULTISAMPLE_LEVEL MultisampleLevel)
{
	DXGI_SAMPLE_DESC RetDesc = { };
#if 0
	switch (MultisampleLevel)
	{
	case MULTISAMPLE_LEVEL_0:
		break;
	case MULTISAMPLE_LEVEL_4X:
		RetDesc.Count = 3;
		RetDesc.Quality = 4;
		break;
	case MULTISAMPLE_LEVEL_8X:
		RetDesc.Count = 7;
		RetDesc.Quality = 8;
		break;
	case MULTISAMPLE_LEVEL_16X:
		RetDesc.Count = 15;
		RetDesc.Quality = 16;
		break;
	}
#endif
	RetDesc.Count = 1;
	RetDesc.Quality = 0;
	return RetDesc;
}
