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
		CheckGrow(2);
	}

	TArray(TType* pData, uint32 NumData) : m_Start(nullptr), m_Size(0), m_FullBuffSize(0)
	{
		CopyFullBuffer(pData, NumData);
		m_Size = NumData;
	}

	TArray(std::initializer_list<TType> InitialList) : m_Start(nullptr), m_Size(0), m_FullBuffSize(0)
	{
		CopyFullBuffer(const_cast<TType*>(InitialList.begin()), static_cast<uint32>(InitialList.size()));
		m_Size = static_cast<uint32>(InitialList.size());
	}

	TArray(uint32 InitialSize) : m_Start(nullptr), m_Size(0), m_FullBuffSize(0)
	{
		Resize(InitialSize);
	}

	TArray(const TArray& Rhs) : m_Start(nullptr), m_Size(0), m_FullBuffSize(0)
	{ 
		if (Rhs.GetElementCount() > 0)
		{
			CopyFullBuffer(Rhs.GetData(), Rhs.GetElementCount());
			m_Size = Rhs.GetElementCount();
		}
	}

	~TArray()
	{
		ClearMemory();
	}

	inline TArray& operator = (const TArray& Rhs)
	{
		if (Rhs.GetElementCount() > 0)
		{
			CopyFullBuffer(Rhs.GetData(), Rhs.GetElementCount());
			m_Size = Rhs.GetElementCount();
		}
		return *this;
	}

	inline TArray& operator = (std::initializer_list<TType> InitialList)
	{
		CopyFullBuffer(const_cast<TType*>(InitialList.begin()), static_cast<uint32>(InitialList.size()));
		m_Size = static_cast<uint32>(InitialList.size());
		return *this;
	}

	inline TArray& operator += (const TType& Other)
	{
		Add(Other);
		return *this;
	}

	inline TArray& operator += (TType&& Other)
	{
		Add(Other);
		return *this;
	}

	inline TType& operator[](uint32 Index)
	{
		return m_Start[Index];
	}

	inline const TType& operator[](uint32 Index) const
	{
		return m_Start[Index];
	}

	template<class ...Args>
	inline void EmplaceBack(Args&& ...InArgs)
	{
		CheckGrow(m_Size + 1);
		new (&m_Start[m_Size]) TType(std::forward<Args...>(InArgs...));
		m_Size++;
	}

	inline void Add(TType&& Value)
	{
		EmplaceBack(Value);
	}

	inline void Add(const TType& Value)
	{
		EmplaceBack(std::move(Value));
	}

	inline uint32 GetElementCount() const 
	{
		return m_Size;
	}

	inline uint32 GetElementsAllocatedCount() const
	{
		return m_FullBuffSize;
	}

	inline TType* GetData()
	{
		return m_Start;
	}

	inline TType* GetData() const
	{
		return m_Start;
	}

	inline void Resize(uint32 NewSize, TType DefaultValue)
	{
		if (NewSize > 0)
		{
			CheckGrow(NewSize);
			for (uint32 i = 0; i < NewSize; i++)
			{
				new (&m_Start[i]) TType(DefaultValue);
			}
			m_Size = NewSize;
		}
	}

	inline void Resize(uint32 NewSize)
	{
		if (NewSize > 0)
		{
			CheckGrow(NewSize);
			for (uint32 i = 0; i < NewSize; i++)
			{
				new (&m_Start[i]) TType();
			}
			m_Size = NewSize;
		}
	}

	inline TType RemoveAt(uint32 Index) 
	{
		TType Result(m_Start[Index]);
		MoveElementsTo(Index);
		return Result;
	}

	inline void Empty()
	{
		for (uint32 i = 0; i < m_Size; i++)
		{
			m_Start[i].~TType();
		}
		m_Size = 0;
	}

	inline void CheckGrow(uint32 NumNewElements)
	{
		if (m_Start == nullptr || NumNewElements > m_FullBuffSize)
		{
			Grow(NextPowerOfTwo(NumNewElements));
		}
	}

	inline void ClearMemory()
	{
		if (m_FullBuffSize)
		{
			delete[] m_Start;
			m_Start = nullptr;
			m_Size = 0;
			m_FullBuffSize = 0;
		}
	}

	inline bool IsEmpty() const
	{
		return m_Size == 0;
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

	inline void CopyFullBuffer(TType* OldBuffer, uint32 OldBufferSize)
	{
		CheckGrow(OldBufferSize);
		CopyToNewBuffer(OldBuffer, m_Start, OldBufferSize);
	}

	inline void CopyToNewBuffer(TType* OldBuff, TType* NewBuff, uint32 NumOldAllocatedObjects)
	{
		if (NumOldAllocatedObjects > 0)
		{
			for (uint32 i = 0; i < NumOldAllocatedObjects; i++)
			{
				new (&NewBuff[i]) TType(OldBuff[i]);
			}
		}
	}

	inline void MoveElementsTo(uint32 Location)
	{
		for (uint32 i = Location; i < m_Size - 2; i++)
		{
			new (&m_Start[i]) TType(m_Start[i + 1]);
		}
	}

	inline void Grow(uint32 NewSize)
	{
		TType* OldPointer = m_Start;
		m_Start = new TType[NewSize];
		if (m_Size > 0)
		{
			CopyToNewBuffer(OldPointer, m_Start, m_Size);
		}
		if (OldPointer)
		{
			delete[] OldPointer;
		}
		m_FullBuffSize = NewSize;
	}

	inline TType* GetEnd()
	{
		return reinterpret_cast<TType*>(reinterpret_cast<ptrdiff_t>(m_Start) + (m_Size * sizeof(TType)));
	}

	inline TType* GetEnd() const
	{
		return reinterpret_cast<TType*>(reinterpret_cast<ptrdiff_t>(m_Start) + (m_Size * sizeof(TType)));
	}

	TType* m_Start;
	uint32 m_Size;
	uint32 m_FullBuffSize;
};

