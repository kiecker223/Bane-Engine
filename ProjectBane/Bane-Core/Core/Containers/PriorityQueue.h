#pragma once
#include "Tree.h"
#include "Common/Types.h"
#include "Logical.h"



template<typename T, class TComparer = CompareLess<T>>
class TPriorityQueue
{
public:
	using TType = T;
	using TNode = TBinaryTree<TType>::TNode;


	class TForwardIterator
	{
	public:
		inline bool operator == (const TForwardIterator& Rhs)
		{
			return Value == Rhs.Value;
		}

		inline bool operator != (const TForwardIterator& Rhs)
		{
			return !(*this == Rhs);
		}

		inline TForwardIterator& operator ++ ()
		{
			Value = Value->Next;
			return *this;
		}

		inline TForwardIterator& operator --()
		{
			Value = Value->Last;
			return *this;
		}

		TNode* Value;
	};

	inline TType Pop()
	{

	}


	struct Heap
	{
		struct HeapNode
		{
			TType Value;
			HeapNode* ChildNodes[2];
		};
		std::vector<HeapNode> Values;

	};

	void Insert(const TType& Value)
	{
		if (SortingTree.Base)
		{
			SortingTree.Base = new TNode();
			SortingTree.Base->Assign(Value);
		}



		Size++;
	}

	inline TForwardIterator begin()
	{
		return { First };
	}

	inline TForwardIterator end()
	{
		return { Last };
	}

	TBinaryTree<TType> SortingTree;
	uint32 Size;
};