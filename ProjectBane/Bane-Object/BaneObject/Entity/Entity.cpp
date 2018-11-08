#include "Entity.h"
#include "Graphics/Data/RenderLoop.h"
#include "../CoreComponents/RenderComponent.h"
#include "../Scene/SceneManager.h"




TComponentHandle<Component> Entity::GetComponentByHash(uint64 Hash)
{
	int IndexToUse = -1;
	for (uint32 i = 0; i < m_Components.GetCount(); i++)
	{
		if (m_Components[i] == Hash)
		{
			IndexToUse = i;
			break;
		}
	}

	if (IndexToUse >= 0)
		return TComponentHandle<Component>(&m_Allocator.PtrBegin, static_cast<ptrdiff_t>(m_Components[IndexToUse]));
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
	for (Component* pCompOffset : m_Allocator.GetAllocatedObjects())
	{
		const auto CompOffset = reinterpret_cast<ptrdiff_t>(pCompOffset);
		Component* Comp = reinterpret_cast<Component*>(CompOffset + reinterpret_cast<ptrdiff_t>(m_Allocator.PtrBegin));
		if (Comp->IsRenderComponent())
		{
			RenderComponent* RC = static_cast<RenderComponent*>(Comp);
			RC->GraphicsUpdate(RL);
		}
	}
}

void Entity::AddChild(Entity* Child)
{
	m_Children.Add(Child);
}

void Entity::RemoveChild(uint32 ChildIndex)
{
	m_Children.RemoveAt(ChildIndex);
}

float4x4 Entity::GetMatrixAffectedByParents() const
{
	const Entity* Parent = this;
	double3 CamPos = m_SceneOwner->GetCameraEntity()->GetTransform()->GetPosition();
	float4x4 Result = GetTransform()->GetMatrix(CamPos);
	while (true)
	{
		Parent = Parent->GetParent();
		if (!Parent)
		{
			break;
		}
		Result *= Parent->GetTransform()->GetRawMatrix();
	}
	return Result;
}

Entity* GetEntityById(const EntityIdentifier& Id)
{
	return GetSceneManager()->CurrentScene->FindEntity(Id.HashedName);
}
