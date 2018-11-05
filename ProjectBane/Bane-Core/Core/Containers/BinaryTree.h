#pragma once


typedef enum EBINARY_SEARCH_RESULT {
	BINARY_SEARCH_RESULT_LEFT_CLOSER = 1,
	BINARY_SEARCH_RESULT_RIGHT_CLOSER = 2,
	BINARY_SEARCH_RESULT_FOUND = 3
} EBINARY_SEARCH_RESULT;

template<typename TType>
class TBinaryTree
{
public:

	struct TNode
	{
		TNode() : Parent(nullptr), NextL(nullptr), NextR(nullptr) { }
		TType Value;
		template<class ...Args>
		inline void Assign(Args&& ...InArgs)
		{
			new (&Value) TType(std::forward<Args...>(InArgs...));
		}
		TNode* Parent;
		TNode* NextL;
		TNode* NextR;
	};

	TBinaryTree()
	{
		Initialize();
	}

	TBinaryTree(const TBinaryTree& Rhs)
	{
		if (Tail)
		{
			RecursiveDelete();
		}
		Copy(Rhs);
	}

	~TBinaryTree()
	{
		if (Tail)
		{
			RecursiveDelete();
		}
	}

	inline TBinaryTree& operator = (const TBinaryTree& Rhs)
	{
		if (Tail)
		{
			RecursiveDelete();
		}
		Copy(Rhs);
		return *this;
	}

	inline TNode* CreateNode(TNode* Parent, TType Value)
	{
		TNode* Result = new TNode();
		Result->Parent = Parent;
		Result->Assign(Value);
		return Result;
	}

	inline TNode* CreateNode(TNode* Parent)
	{
		TNode* Result = new TNode();
		Result->Parent = Parent;
		return Result;
	}

	inline void RecursiveDelete()
	{
		RecursiveDeleteImpl(Tail);
		Tail = nullptr;
		Head = nullptr;
	}

	inline void Initialize()
	{
		Head = new TNode();
		Tail = Head;
	}

	inline void Copy(const TBinaryTree& Rhs) 
	{
		if (!Tail)
		{
			Tail = new TNode();
			Tail->Assign(Rhs.Tail->Value);
		}
		Head = Tail;
		RecursiveCopyImpl(Tail, Rhs.Tail);
	}

	template<class TFNComparer>
	inline TNode* Find(TFNComparer CompareFn) 
	{
		TNode* ClosestResult = Tail;
		EBINARY_SEARCH_RESULT Result = CompareFn(ClosestResult);
		while (Result != BINARY_SEARCH_RESULT_FOUND && ClosestResult)
		{
			if (Result == BINARY_SEARCH_RESULT_LEFT_CLOSER)
			{
				ClosestResult = ClosestResult->NextL;
			}
			if (Result == BINARY_SEARCH_RESULT_RIGHT_CLOSER)
			{
				ClosestResult = ClosestResult->NextR;
			}
			if (Result == BINARY_SEARCH_RESULT_FOUND)
			{
				return ClosestResult;
			}
			Result = CompareFn(ClosestResult);
		}
		return nullptr
	}

private:

	inline void RecursiveCopyImpl(TNode* MyNode, TNode* OtherNode)
	{
		MyNode->Assign(OtherNode->Value);
		if (OtherNode->NextL)
		{
			MyNode->NextL = CreateNode(MyNode);
			RecursiveCopyImpl(MyNode->NextL, OtherNode->NextL);
		}
		if (OtherNode->NextR)
		{
			MyNode->NextR = CreateNode(MyNode);
			RecursiveCopyImpl(MyNode->NextR, OtherNode->NextR);
		}
	}

	inline void RecursiveDeleteImpl(TNode* pNode)
	{
		TNode* Pointer = pNode;
		if (pNode->NextL)
		{
			RecursiveDeleteImpl(pNode->NextL);
		}
		if (pNode->NextR)
		{
			RecursiveDeleteImpl(pNode->NextR);
		}
		delete Pointer;
	}

public:

	TNode* Head;
	TNode* Tail;

};