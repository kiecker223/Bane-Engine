#pragma once
#include "Common/Types.h"


template<typename T, uint32 TNumChildrenPerNode>
class TTree
{
public:

	static const uint32 NumChildrenPerNode = TNumChildrenPerNode;
	using TType = T;
	using TSelfType = TTree<TType, TNumChildrenPerNode>;

	class TNode
	{
	public:
		TNode() : Parent(nullptr)
		{
			for (uint32 i = 0; i < NumChildrenPerNode; i++)
			{
				Children[i] = nullptr;
			}
		}
		TNode(TType& ValRef)
		{
			for (uint32 i = 0; i < NumChildrenPerNode; i++)
			{
				Children[i] = nullptr;
			}
			Assign(ValRef);
		}
		TType Value;
		template<class ...Args>
		inline void Assign(Args&& ...InArgs)
		{
			new (&Value) TType(std::forward<Args...>(InArgs...));
		}
		TNode* Parent;
		TNode* Children[NumChildrenPerNode];
	};


	TTree() : Base(nullptr)
	{
	}

	~TTree()
	{
		RecursivelyDelete();
	}

	inline TNode* AddNode(TNode* Parent, uint32 ChildIdx, const TType& Val)
	{
		TNode* NewNode = new TNode(Val);
		NewNode->Parent = Parent;
		Parent->Children[ChildIdx] = NewNode;
		return NewNode;
	}

	inline void RecursivelyDeleteNode(TNode* Node)
	{
		DeleteNodeImpl(Node);
	}

	inline void RecursivelyDelete()
	{
		RecursivelyDeleteNode(Base);
		Base = nullptr;
	}

	inline TSelfType& operator = (const TSelfType& Rhs)
	{
		if (Rhs.Base)
		{
			if (!Base)
			{
				Base = new TNode();
			}
			DeepCopyImpl(Base, Rhs.Base);
		}
		return *this;
	}

private:

	inline void DeepCopyImpl(TNode* MyStart, TNode* OtherStart)
	{
		MyStart->Assign(OtherStart->Value);
		for (uint32 i = 0; i < NumChildrenPerNode; i++)
		{
			if (OtherStart->Children[i])
			{
				if (!MyStart->Children[i])
				{
					MyStart->Children[i] = new TNode();
				}
				DeepCopyImpl(MyStart->Children[i], OtherStart->Children[i]);
			}
		}
	}

	inline void DeleteNodeImpl(TNode* BaseNode)
	{
		if (BaseNode->Children[0]) // We assume that there is at least one child
		{
			for (uint32 i = 0; i < NumChildrenPerNode; i++)
			{
				if (BaseNode->Children[i])
				{
					DeleteNodeImpl(BaseNode->Children[i]);
				}
			}
		}
		delete BaseNode;
	}

public:

	TNode* Base;
};

template<typename T>
using TBinaryTree = TTree<T, 2>;

