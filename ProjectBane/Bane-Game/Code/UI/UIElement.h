#pragma once

#include "UIContext.h"

class IUIElement
{
public:
	static IUIElement* Root;

	virtual void Initialize() = 0;
	virtual void Update(UIContext& Context) = 0;
	virtual void Render(UIContext& Context) = 0;

	// Enable this when you want to override the font with something else
	Font* FontOverride = nullptr;

	IUIElement* Parent;
	std::vector<IUIElement*> Children;
};

