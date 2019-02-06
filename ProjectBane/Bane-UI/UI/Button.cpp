#include "Button.h"


void UIButton::Initialize()
{

}

void UIButton::Update(UIContext& Context)
{
	if (Context.MouseState.GetButtonUp(0))
	{
		double2 MousePos = Context.MouseState.GetMousePosition();

		if (PointInside(MousePos))
		{
			if (!OnButtonClicked.empty())
			{
				for (auto& Action : OnButtonClicked)
				{
					Action();
				}
			}
		}
	}
}

void UIButton::Render(UIContext& Context)
{
	auto* CmdList = Context.CommandList;
	
}

bool UIButton::PointInside(const double2& Val)
{
	return Val.x > Min.x && Val.y > Min.y && Val.x < Max.x && Val.y < Max.y;
}
