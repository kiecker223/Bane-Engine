#pragma once
#include <vector>
#include <functional>
#include <KieckerMath.h>
#include "UIElement.h"


class UIButton : public IUIElement
{
public:

	std::vector<std::function<void()>> OnButtonClicked;

	void Initialize() override final;
	void Update(UIContext& Context) override final;
	void Render(UIContext& Context) override final;

	double2 Min, Max;

private:

	bool PointInside(const double2& Val);

};
