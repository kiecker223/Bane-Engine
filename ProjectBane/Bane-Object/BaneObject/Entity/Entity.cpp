#include "Entity.h"
#include "Graphics/Data/RenderLoop.h"
#include "../CoreComponents/RenderComponent.h"
#include "../Scene/SceneManager.h"




TComponentHandle<Component> Entity::GetComponentByHash(uint64 Hash)
{
	int IndexToUse = -1;
	for (uint32 i = 0; i < m_Components.size(); i++)
	{
		if (m_Components[i] == Hash)
		{
			IndexToUse = i;
			break;
		}
	}

	if (IndexToUse >= 0)
		return TComponentHandle<Component>(&m_Allocator.GetAllocatedObjects(), IndexToUse);
	return nullptr;
}

void Entity::RemoveComponent(uint64 ComponentHash)
{
	Component* Position = GetComponentByHash(ComponentHash).operator->();
	m_Allocator.RemovePointer(Position);
}

void Entity::SubmitRenderingComponents()
{
}

void Entity::UpdateRenderObjects(RenderLoop& RL)
{
	for (auto& Comp : m_Allocator.GetAllocatedObjects())
	{
		if (Comp->IsRenderComponent())
		{
			RenderComponent* RC = static_cast<RenderComponent*>(Comp);
			RC->GraphicsUpdate(RL);
		}
	}
}

void Entity::AddChild(EntityIdentifier Child)
{
	m_Children.push_back(Child);
}

void Entity::RemoveChild(uint32 ChildIndex)
{
	m_Children.erase(m_Children.begin() + ChildIndex);
}

float4x4 Entity::GetMatrixAffectedByParents() const
{
	const Entity* Parent = this;
	float4x4 Result = GetTransform()->GetMatrix();

	while (Parent)
	{
		Result *= Parent->GetTransform()->GetMatrix();
		Parent = Parent->GetParent();
	}
	return Result;
}

Entity* Entity::GetParent() const
{
	if (m_Parent.IsValid())
	{
		return GetEntityById(m_Parent);
	}
	return nullptr;
}

Entity* Entity::GetChild(uint32 Idx)
{
	if (Idx > GetChildCount())
	{
		return nullptr;
	}
	return GetEntityById(m_Children[Idx]);
}

Entity* GetEntityById(const EntityIdentifier& Id)
{
	return GetSceneManager()->CurrentScene->FindEntity(Id.HashedName);
}
