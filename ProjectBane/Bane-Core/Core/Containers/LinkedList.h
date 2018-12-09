#pragma once
#include "Common/Types.h"

template<typename TType>
class TLinkedList
{
public:
	struct TNode
	{
		TNode() : Next(nullptr), Last(nullptr) { }
		template<class ...Args>
		void Assign(Args&& ...InArgs)
		{
			new (&Value) TType(std::forward<Args...>(InArgs...));
		}
		TType Value;
		TNode* Next;
		TNode* Last;
	};

	class TForwardIterator
	{
	public:

		using pointer = TNode*;

		inline bool operator ==(const TForwardIterator& Rhs)
		{
			return Node == Rhs.Node;
		}

		inline bool operator != (const TForwardIterator& Rhs)
		{
			return !(*this == Rhs);
		}

		inline TForwardIterator& operator ++()
		{
			Node = Node->Next;
		}

		inline TForwardIterator& operator --()
		{
			Node = Node->Last;
		}

		inline TNode* operator*()
		{
			return Node;
		}

		TNode* Node;
	};

	class TBackwardIterator
	{
	public:

		using pointer = TNode*;

		inline bool operator == (const TBackwardIterator& Rhs)
		{
			return Node == Rhs.Node;
		}

		inline bool operator != (const TBackwardIterator& Rhs)
		{
			return !(*this == Rhs);
		}

		inline TBackwardIterator& operator ++()
		{
			Node = Node->Last;
		}

		inline TBackwardIterator& operator --()
		{
			Node = Node->Next;
		}

		inline TNode* operator*()
		{
			return Node;
		}

		TNode* Node;
	};

	inline TForwardIterator begin()
	{
		return { Tail };
	}

	inline TForwardIterator end()
	{
		return { Head };
	}

	TLinkedList()
	{
		Construct();
	}

	~TLinkedList()
	{
		RecursiveDelete();
	}

	inline void Construct()
	{
		Size = 0;
		Head = new TNode();
		Tail = Head;
	}

	inline TNode* Add(TType&& Val)
	{
		return AddImpl(std::forward<TType>(Val));
	}

	inline TNode* Add(const TType& Val)
	{
		return AddImpl(Val);
	}

	inline void RecursiveDelete()
	{
		TNode* Node = Tail;
		while (Node)
		{
			TNode* Next = Node->Next;
			delete Node;
			Node = Next;
		}
		Tail = nullptr;
		Head = nullptr;
	}

private:

	TNode* AddImpl(const TType& Val)
	{
		TNode* NewNode = new TNode();
		Head->Assign(Val);
		Head->Next = NewNode;
		NewNode->Last = Head;
		TNode* OldHead = Head;
		Head = NewNode;
		Size++;
		return OldHead;
	}


public:

	TNode* Head;
	TNode* Tail;
	uint32 Size;
};

