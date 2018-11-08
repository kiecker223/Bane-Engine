#pragma once

#include <BaneObject.h>
#include <Platform/Input/InputSystem.h>
#include <Core/Containers/Array.h>
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


	void Tick(float Dt) override 
	{
		UNUSED(Dt);
		if (GetInput()->Keyboard.GetKeyUp(KEY_F))
		{
			Entity* Planet = NextPlanet();
			GetTransform()->SetPosition(Planet->GetTransform()->GetPosition());
			std::cout << "PlanetName: " << Planet->GetId().Name << std::endl;
		}
	}

};