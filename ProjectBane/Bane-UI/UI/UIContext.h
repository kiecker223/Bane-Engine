#pragma once

#include <KieckerMath.h>
#include <Graphics/Interfaces/ApiRuntime.h>
#include "Font.h"
#include "Platform/Input/InputKeyboardDevice.h"
#include "Platform/Input/InputMouseDevice.h"

class IUIElement;

class UIContext
{
public:

	void Initialize();

	void BeginFrame(IGraphicsCommandContext* InCommandList, IComputeCommandContext* InComputeList);
	void EndFrame();

	void DestroyItem(IUIElement* ElementToDestroy);

	Font* CurrentFont;

	void DrawText(Font* FontToDraw, const std::string& TextToDraw);

	IVertexBuffer* QuadVB;
	IIndexBuffer* QuadIB;
	InputKeyboardDevice& KeyboardState;
	InputMouseDevice& MouseState;
	IGraphicsCommandContext* CommandList;
	IComputeCommandContext* ComputeContext;
};
