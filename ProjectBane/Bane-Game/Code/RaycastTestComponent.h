#pragma once

#include <BaneObject/Scene/Scene.h>
#include <BaneObject/Entity/Entity.h>
#include "Platform/Input/InputSystem.h"
#include "BaneObject/CoreComponents/MeshRenderingComponent.h"

class RaycastTestComponent : public Component
{
	IMPLEMENT_COMPONENT(RaycastTestComponent)
public:

	void Tick(float Dt) override final
	{
		UNUSED(Dt);
		if (GetInput()->Keyboard.GetKeyDown(KEY_R))
		{
			RayHitInformation HitInfo;
			if (GetScene()->Raycast(GetTransform()->GetPosition(), GetTransform()->GetForward(), std::numeric_limits<double>::infinity(), HitInfo))
			{
				std::cout << "Hit nothing" << std::endl;
				Entity* NewEntity = GetScene()->CreateEntity("HitResult");
				auto MCC = NewEntity->AddComponent<MeshRenderingComponent>();
				MCC->RenderedMaterial.InitializeMaterial("MainShader.gfx");
				MCC->RenderedMesh = GetScene()->GetMeshCache().LoadMesh("Sphere");
				MCC->RenderedMaterial.SetDiffuseTexture("DefaultBlue");
				NewEntity->GetTransform()->SetPosition(HitInfo.Position);
				NewEntity->GetTransform()->Scale(0.1);
			}
		}
	}

};
