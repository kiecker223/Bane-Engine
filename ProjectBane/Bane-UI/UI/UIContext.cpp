#include "UIContext.h"
#include <Graphics/Interfaces/ApiRuntime.h>


void UIContext::Initialize()
{
	struct QUAD_VERTICES
	{
		float3 Position;
		float2 UV;
	};

	QUAD_VERTICES Vertices[4] = {
		{ float3( -1.0f, -1.0f,  0.1f ),float2( 0.0f, 1.0f ) },
		{ float3( 1.0f, -1.0f,  0.1f  ),float2( 1.0f, 1.0f ) },
		{ float3( 1.0f,  1.0f,  0.1f  ),float2( 1.0f, 0.0f ) },
		{ float3( -1.0f,  1.0f,  0.1f ),float2( 0.0f, 0.0f ) }
	};

	uint32 indices[6] = {
		0, 3, 2,
		2, 1, 0
	};

	auto* Device = GetApiRuntime()->GetGraphicsDevice();

	QuadVB = Device->CreateVertexBuffer(sizeof(Vertices), reinterpret_cast<uint8*>(Vertices));
	QuadIB = Device->CreateIndexBuffer(sizeof(indices), reinterpret_cast<uint8*>(indices));
}

void UIContext::BeginFrame(IGraphicsCommandContext* InCommandList, IComputeCommandContext* InComputeList)
{
	CommandList = InCommandList;
	ComputeContext = InComputeList;


}

void UIContext::EndFrame()
{

}

void UIContext::DestroyItem(IUIElement* ElementToDestroy)
{

}
