#include "Entity.h"
//#include "Engine/Application/Application.h"
#include "../Scene/Scene.h"




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
	//SceneRenderer* Renderer = GetApplicationInstance()->GetSceneRenderer();
	//for (auto Comp : m_Allocator.GetAllocatedObjects())
	//{
	//	if (Comp->IsRenderComponent())
	//	{
	//		RenderComponent* rc = (RenderComponent*)Comp;
	//		rc->SubmitFeature(Renderer);
	//	}
	//}
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
	matrix Result = GetTransform()->GetMatrix();

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

Entity* GetEntityById(const EntityIdentifier& Id)
{
	return GetCurrentScene()->FindEntity(Id.HashedName);
}
