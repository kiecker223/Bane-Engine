#pragma once
#include <vector>
#include <functional>
#include <KieckerMath.h>
#include <string>
#include "UIElement.h"


class UIButton : public IUIElement
{
public:

	std::vector<std::function<void()>> OnButtonClicked;

	void Initialize() override final;
	void Update(UIContext& Context) override final;
	void Render(UIContext& Context) override final;

	vec2 Min, Max;

	inline vec2 GetPosition() const { return Max - (Max - Min / 2.f); }
	inline double GetSizeX() const { return Max.x - Min.x; }
	inline double GetSizeY() const { return Max.y - Min.y; }
	inline vec2 GetScale() const { return vec2(GetSizeX(), GetSizeY()); }

	std::string Text;

private:

	IConstantBuffer* m_FontDrawInfo;

	bool PointInside(const vec2& Val);

};
