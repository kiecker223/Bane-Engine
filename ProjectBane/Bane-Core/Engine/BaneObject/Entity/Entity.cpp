#include "Entity.h"
#include "EntityManager.h"
#include "Application/Application.h"
#include "../CoreComponents/RenderComponent.h"


EntityIdentifier EntityIdentifier::FindIdentifierFromHash(uint64 InHash)
{
	return EntityIdentifier();
}

Component* Entity::GetComponentByHash(uint64 Hash)
{
	int IndexToUse = -1;
	for (uint i = 0; i < m_Components.size(); i++)
	{
		if (m_Components[i] == Hash)
		{
			IndexToUse = i;
			break;
		}
	}

	if (IndexToUse >= 0)
		return m_Allocator.GetAllocatedObjects()[IndexToUse];
	return nullptr;
}

void Entity::RemoveComponent(uint64 ComponentHash)
{
	Component* Position = GetComponentByHash(ComponentHash);
	m_Allocator.RemovePointer(Position);
}

void Entity::SubmitRenderingComponents()
{
	for (auto Comp : m_Allocator.GetAllocatedObjects())
	{
		if (Comp->IsRenderComponent())
		{
			RenderComponent* rc = (RenderComponent*)Comp;
			rc->SubmitFeature(GetApplicationInstance()->GetSceneRenderer());
		}
	}
}

void Entity::AddChild(EntityIdentifier Child)
{
	m_Children.push_back(Child);
}

void Entity::RemoveChild(uint ChildIndex)
{
	m_Children.erase(m_Children.begin() + ChildIndex);
}

matrix Entity::GetMatrixAffectedByParents() const
{
	const Entity* Parent = this;
	matrix Result = Parent->GetTransform()->GetMatrix();

	while (Parent = Parent->GetParent())
	{
		Result *= Parent->GetTransform()->GetMatrix();
	}
	return Result;
}

Entity* Entity::GetParent() const
{
	if (m_Parent.IsValid())
	{
		//return GetEntityFromId(m_Parent);
	}
	return nullptr;
}
