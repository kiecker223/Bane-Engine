#pragma once

#include <KieckerMath.h>
#include <GraphicsCore/Interfaces/ApiRuntime.h>
#include "Font.h"
#include "Platform/Input/InputKeyboardDevice.h"
#include "Platform/Input/InputMouseDevice.h"

class IUIElement;

class UIContext
{
public:

	UIContext() {}

	void Initialize();

	void BeginFrame(IGraphicsCommandContext* InCommandList, IComputeCommandContext* InComputeList);
	void EndFrame();

	void DestroyItem(IUIElement* ElementToDestroy);

	Font* CurrentFont;

	IVertexBuffer* QuadVB;
	IIndexBuffer* QuadIB;
	InputKeyboardDevice* KeyboardState;
	InputMouseDevice* MouseState;
	IGraphicsCommandContext* CommandList;
	IComputeCommandContext* ComputeContext;
};
