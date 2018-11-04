#pragma once
#include <initializer_list>
#include <Common.h>
#include <iostream>



template<typename TType>
class TArray
{
public:

	TArray() : m_Start(nullptr), m_Size(0), m_FullBuffSize(0)
	{
	}

	TArray(TType* pData, uint32 NumData) : m_Start(nullptr), m_Size(0), m_FullBuffSize(0)
	{
	}

	TArray(std::initializer_list<TType> InitialList) : m_Start(nullptr), m_Size(0), m_FullBuffSize(0)
	{
	}

	TArray(uint32 InitialSize) : m_Start(nullptr), m_Size(0), m_FullBuffSize(0)
	{
	}

	TArray(const TArray& Rhs) : m_Start(nullptr), m_Size(0), m_FullBuffSize(0)
	{
	}

	~TArray()
	{
	}

	inline TArray& operator = (const TArray& Rhs)
	{
	}

	inline TArray& operator = (std::initializer_list<TType> InitialList)
	{
	}

	inline TArray& operator += (const TType& Other)
	{
	}

	inline TArray& operator += (TType&& Other)
	{
	}

	inline TType& operator[](uint32 Index)
	{
	}

	inline TType& operator[](uint32 Index) const
	{
	}

	template<class ...Args>
	inline void EmplaceBack(Args&& ...InArgs)
	{
	}

	inline void Add(TType&& Value)
	{
	}

	inline void Add(const TType& Value)
	{
	}

	inline uint32 GetElementCount() const 
	{
	}

	inline uint32 GetElementsAllocatedCount() const
	{
	}

	inline TType* GetData()
	{
	}

	inline TType* GetData() const
	{
	}

	inline void Resize(uint32 NewSize, TType DefaultValue)
	{
	}

	inline void Resize(uint32 NewSize)
	{
	}

	inline TType RemoveAt(uint32 Index) 
	{
	}

	inline void Empty()
	{
	}

	inline void CheckGrow(uint32 NumNewElements)
	{
	}

	inline void ClearMemory()
	{
	}

	inline bool IsEmpty() const
	{
	}

	class TForwardIterator
	{
	public:
		using pointer = TType*;
		TForwardIterator() : Pointer(nullptr) { }
		TForwardIterator(TType* InPointer) : Pointer(InPointer) { }
		TType* Pointer;

		inline TType& operator*()
		{
			return *Pointer;
		}

		inline TType* operator++()
		{
			Pointer++;
			return Pointer;
		}

		inline bool operator == (const TForwardIterator& Rhs)
		{
			return Pointer == Rhs.Pointer;
		}

		inline bool operator != (const TForwardIterator& Rhs)
		{
			return Pointer != Rhs.Pointer;
		}

		inline TType* operator--()
		{
			Pointer--;
			return Pointer;
		}

		inline TType* operator ->()
		{
			return Pointer;
		}
	};

	class TConstForwardIterator
	{
	public:

		TConstForwardIterator(const TType* InPointer) : Pointer(InPointer) { }

		using pointer = TType*;
		const TType* Pointer;
		inline const TType& operator*() const 
		{
			return *Pointer;
		}

		inline bool operator == (const TConstForwardIterator& Rhs) const 
		{
			return Pointer == Rhs.Pointer;
		}

		inline bool operator != (const TConstForwardIterator& Rhs) const
		{
			return Pointer != Rhs.Pointer;
		}

		inline const TType* operator ->()
		{
			return Pointer;
		}

		inline TConstForwardIterator& operator++()
		{
			Pointer++;
			return *this;
		}
		
		inline TConstForwardIterator& operator--()
		{
			Pointer--;
			return *this;
		}
	};

	class TBackwardIterator
	{
	public:

		TBackwardIterator(TType* InPointer) : Pointer(InPointer) { }

		using pointer = TType* ;
		TType* Pointer;

		inline TType* operator->()
		{
			return Pointer;
		}

		inline TType& operator*()
		{
			return *Pointer;
		}
		
		inline TType* operator++()
		{
			Pointer--;
			return Pointer;
		}

		inline TType* operator--()
		{
			Pointer++;
			return Pointer;
		}
	};

	inline TConstForwardIterator begin() const 
	{
		return TConstForwardIterator(const_cast<const TType*>(m_Start));
	}

	inline TConstForwardIterator end() const 
	{
		return TConstForwardIterator(const_cast<const TType*>(GetEnd()));
	}

	inline TForwardIterator begin()
	{
		return TForwardIterator(m_Start);
	}

	inline TForwardIterator end()
	{
		return TForwardIterator(GetEnd());
	}

	inline TBackwardIterator rbegin()
	{
		return TBackwardIterator(m_Start);
	}

	inline TBackwardIterator rend()
	{
		return TBackwardIterator(GetEnd());
	}


private:

	inline void CopyToNewBuffer(TType* OldBuff, TType* NewBuff, uint32 NumOldAllocatedObjects)
	{
		for (uint32 i = 0; i < NumOldAllocatedObjects; i++)
		{
			new (&NewBuff[i]) TType(OldBuff[i]);
		}
	}

	inline void MoveElementsTo(uint32 Location)
	{
	}

	inline void Grow(uint32 NewSize)
	{
	}

	TType* m_Start;
	uint32 m_Size;
	uint32 m_FullBuffSize;
};

