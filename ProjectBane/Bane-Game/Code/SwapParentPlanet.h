#pragma once

#include <BaneObject.h>
#include <Platform/Input/InputSystem.h>
#include <vector>
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
		Planets.push_back(GetScene()->FindEntity("Earth"));
		Planets.push_back(GetScene()->FindEntity("Venus"));
		Planets.push_back(GetScene()->FindEntity("Mercury"));
	}

	Entity* NextPlanet()
	{
		Index = (Index + 1) % Planets.size();
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