#pragma once

#include <d3dcommon.h>
#include "../Interfaces/PipelineState.h"


void InitializeD3DCommonTranslator();

D3D_PRIMITIVE_TOPOLOGY D3D_TranslatePrimitiveTopology(EPOLYGON_TYPE PolygonType);

DXGI_FORMAT D3D_TranslateFormat(EFORMAT Format);

DXGI_SAMPLE_DESC D3D_TranslateMultisampleLevel(EMULTISAMPLE_LEVEL MultisampleLevel);
