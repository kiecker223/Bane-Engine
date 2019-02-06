#pragma once

#include <BaneObject.h>
#include <Platform/Input/InputSystem.h>
#include <vector>
#include "BaneObject/CoreComponents/MeshRenderingComponent.h"
#include <iostream>



class SwapParentPlanet : public Component
{
	IMPLEMENT_COMPONENT(SwapParentPlanet)
public:

	std::vector<Entity*> Planets;
	uint32 Index;

	void Start() override
	{
		Planets.push_back(GetScene()->FindEntity("Sun"));
		Planets.push_back(GetScene()->FindEntity("Luna"));
		Planets.push_back(GetScene()->FindEntity("Earth"));
		Planets.push_back(GetScene()->FindEntity("Venus"));
		Planets.push_back(GetScene()->FindEntity("Mercury"));
	}

	Entity* NextPlanet()
	{
		Index = (Index + 1) % Planets.size();
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

	void Tick(double Dt) override 
	{
		UNUSED(Dt);
		if (GetInput()->Keyboard.GetKeyUp(KEY_F))
		{
			Entity* Planet = NextPlanet();
			GetTransform()->SetPosition(Planet->GetTransform()->GetPosition());
			std::cout << "PlanetName: " << Planet->GetId().Name << std::endl;
			GetTransform()->SetRotation(0.f, 0.f, 0.f);
		}
		//if (GetInput()->Keyboard.GetKeyUp(KEY_R))
		//{
		//	GetScene()->bDrawPhysicsDebugInfo = !GetScene()->bDrawPhysicsDebugInfo;
		//}
		if (GetInput()->Keyboard.GetKeyDown(KEY_Q))
		{
			PHYSICS_RAY Ray;
			Ray.Position = GetTransform()->GetPosition();
			Ray.Direction = GetTransform()->GetForward(); 
			RayHitInformation HitInfo;
			if (GetScene()->Raycast(Ray.Position, Ray.Direction, std::numeric_limits<double>::infinity(), HitInfo))
			{
				Entity* NewEntity = GetScene()->CreateEntity("AnotherEntity");
				NewEntity->GetTransform()->SetPosition(HitInfo.Position);
				NewEntity->GetTransform()->Scale(200000.);
				Planets.push_back(NewEntity);
				auto Mrc = NewEntity->AddComponent<MeshRenderingComponent>();
				Mrc->RenderedMesh = GetScene()->GetMeshCache().LoadMesh("Sphere");
				Mrc->RenderedMaterial.InitializeMaterial("MainShader.gfx");
				Mrc->RenderedMaterial.SetTexture("DefaultBlue", 0);
				std::cout << "HitEnemy" << std::endl;
				auto SCC = NewEntity->AddComponent<SphereCollisionComponent>();
				auto OtherSCC = HitInfo.HitEntity->GetComponent<SphereCollisionComponent>();
				SCC->SetRadius(100000.0);
				SCC->SetPosition(HitInfo.Position);
				SCC->SetMass(10000.0);
				SCC->SetVelocity(OtherSCC->GetVelocity());
			}
		}
		if (GetInput()->Keyboard.GetKeyDown(KEY_Z))
		{
			PrintPlanetData();
		}
	}                                

};