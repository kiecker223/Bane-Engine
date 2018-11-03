#pragma once


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
			new (&Value) TType(std::forward<Args>(...Args));
		}
		TType Value;
		TNode* Next;
		TNode* Last;
	};

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
		TNode* NewNode = new TNode();
		NewNode->Assign(Val);
		Head->Next = NewNode;
		NewNode->Last = Head;
		Head = NewNode;
		Size++;
		return NewNode;
	}

	inline TNode* Add(const TType& Val)
	{
		return Add(std::move<T>(Val));
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


	TNode* Head;
	TNode* Tail;
	uint32 Size;
};

