#include "Button.h"


void UIButton::Initialize()
{
	m_FontDrawInfo = GetApiRuntime()->GetGraphicsDevice()->CreateConstantBuffer(GPU_BUFFER_MIN_SIZE);
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
	BANE_CHECK(FontOverride != nullptr);

	struct ALIGN_FOR_GPU_BUFFER CBufferData
	{
		float4x4 Data;
	};

	CBufferData* pData = m_FontDrawInfo->MapT<CBufferData>();
	double2 Position = GetPosition();
	double2 Scale = GetScale();
	pData->Data = matTranslation(fromDouble3(double3(Position.x, Position.y, 0.f))) * matScale(fromDouble3(double3(Scale.x, Scale.y, 0.f)));
	pData->Data *= matScale(fromDouble3(double3(1. / 1920., 1. / 1080., 0.)));
	m_FontDrawInfo->Unmap();

	CmdList->SetGraphicsPipelineState(FontOverride->FontShader);
	CmdList->SetTexture(0, FontOverride->FontTexture);
	for (uint32 i = 0; i < Text.size(); i++)
	{
		char CurrChar = Text[i];
		CmdList->SetVertexBuffer(FontOverride->VB, FontOverride->GetVertexBufferOffset(CurrChar));
		CmdList->SetIndexBuffer(FontOverride->IB);
		CmdList->SetConstantBuffer(0, m_FontDrawInfo);
		CmdList->DrawIndexed(4, 0, 0);
	}
}

bool UIButton::PointInside(const double2& Val)
{
	return Val.x > Min.x && Val.y > Min.y && Val.x < Max.x && Val.y < Max.y;
}
