#pragma once

#include "Array.h"

template<typename TType>
class TNaryTree
{
public:

	TNaryTree()
	{
		Head = new TNode();
		Tail = Head;
	}

	~TNaryTree()
	{
		if (Head && Tail)
		{
			RecursiveDelete();
		}
	}

	struct TNode
	{
		TType Value;
		template<class ...Args>
		inline void Assign(Args&& ...InArgs)
		{
			new (&Value) TType(std::forward<Args>(InArgs...));
		}
		TNode* Parent;
		TArray<TNode*> Children;
	};

	inline void RecursiveDelete()
	{
		RecursiveDeleteImpl(Tail);
		Head = nullptr;
		Tail = nullptr;
	}

	template<typename... Args>
	inline TNode* EmplaceItem(TNode* Parent, Args&& ...InArgs)
	{
		TNode* Result = new TNode();
		Result->Assign(InArgs);
		Result->Parent = Parent;
	}

	inline TNode* AddItem(TType&& Value, TNode* Parent)
	{
		BANE_CHECK(Parent != nullptr);
		return EmplaceItem(Parent, Value);
	}

	inline TNode* AddItem(const TType& Value, TNode* Parent)
	{
		BANE_CHECK(Parent != nullptr);
		return EmplaceItem(Parent, std::move(Value));
	}

private:

	inline void RecursiveDeleteImpl(TNode* Node)
	{
		TNode* NodePtr = Node;

		if (!Node->Children.IsEmpty())
		{
			for (uint32 i = 0; i < Node->Children.GetCount(); i++)
			{
				RecursiveDeleteImpl(Node->Children[i]);
			}
		}

		delete NodePtr;
	}

public:

	TNode* Head;
	TNode* Tail;
};
