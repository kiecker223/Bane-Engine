#include "UIContext.h"
#include <GraphicsCore/Interfaces/ApiRuntime.h>


void UIContext::Initialize()
{
	struct QUAD_VERTICES
	{
		fvec3 Position;
		fvec2 UV;
	};

	QUAD_VERTICES Vertices[4] = {
		{ fvec3( -1.0f, -1.0f,  0.1f ),fvec2( 0.0f, 1.0f ) },
		{ fvec3( 1.0f, -1.0f,  0.1f  ),fvec2( 1.0f, 1.0f ) },
		{ fvec3( 1.0f,  1.0f,  0.1f  ),fvec2( 1.0f, 0.0f ) },
		{ fvec3( -1.0f,  1.0f,  0.1f ),fvec2( 0.0f, 0.0f ) }
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
