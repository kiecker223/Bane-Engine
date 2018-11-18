#pragma once

#include <BaneObject.h>
#include <Platform/Input/InputSystem.h>
#include <Core/Containers/Array.h>
#include "BaneObject/CoreComponents/MeshRenderingComponent.h"
#include <iostream>



class SwapParentPlanet : public Component
{
	IMPLEMENT_COMPONENT(SwapParentPlanet)
public:

	TArray<Entity*> Planets;
	uint32 Index;

	void Start() override
	{
		Planets.Add(GetScene()->FindEntity("Sun"));
		Planets.Add(GetScene()->FindEntity("Luna"));
		Planets.Add(GetScene()->FindEntity("Earth"));
		Planets.Add(GetScene()->FindEntity("Venus"));
		Planets.Add(GetScene()->FindEntity("Mercury"));
	}

	Entity* NextPlanet()
	{
		Index = (Index + 1) % Planets.GetCount();
		return Planets[Index];
	}

	void PrintPlanetData()
	{
		for (auto* Planet : Planets)
		{
			TComponentHandle<SphereCollisionComponent> SCC = Planet->GetComponent<SphereCollisionComponent>();
			if (SCC)
			{
				std::cout << "Planet handle for planet: " << Planet->GetId().Name << " " << SCC->GetBodyId() << std::endl;
			}
		}
	}

	void Tick(float Dt) override 
	{
		UNUSED(Dt);
		if (GetInput()->Keyboard.GetKeyUp(KEY_F))
		{
			Entity* Planet = NextPlanet();
			GetTransform()->SetPosition(Planet->GetTransform()->GetPosition());
			std::cout << "PlanetName: " << Planet->GetId().Name << std::endl;
		}
		if (GetInput()->Keyboard.GetKeyUp(KEY_R))
		{
			GetScene()->bDrawPhysicsDebugInfo = !GetScene()->bDrawPhysicsDebugInfo;
		}
		if (GetInput()->Keyboard.GetKeyDown(KEY_Q))
		{
			PHYSICS_RAY Ray;
			Ray.Position = GetTransform()->GetPosition();
			Ray.Direction = GetTransform()->GetForward(); 
			RAY_HIT_INFO HitInfo;
			if (GetScene()->GetPhysicsWorld().CastRay(Ray, HitInfo))
			{
				Entity* NewEntity = GetScene()->CreateEntity("AnotherEntity");
				//NewEntity->GetPhysicsProperties().bCanTick = false;
				NewEntity->GetTransform()->SetPosition(HitInfo.Position);
				NewEntity->GetTransform()->Scale(200000.);
				Planets.Add(NewEntity);
				auto Mrc = NewEntity->AddComponent<MeshRenderingComponent>();
				Mrc->RenderedMesh = GetScene()->GetMeshCache().LoadMesh("Sphere");
				Mrc->RenderedMaterial.InitializeMaterial("MainShader.gfx");
				Mrc->RenderedMaterial.SetTexture("DefaultBlue", 0);
				std::cout << "HitEnemy" << std::endl;
			}
		}
		if (GetInput()->Keyboard.GetKeyDown(KEY_Z))
		{
			PrintPlanetData();
		}
	}

};