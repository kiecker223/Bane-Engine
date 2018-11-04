#pragma once
#include <Core/Containers/Array.h>
#include "PhysicsBody.h"

class PhysicsUpdateBuffer
{
public:
	TArray<PhysicsBody> Bodies;
};
