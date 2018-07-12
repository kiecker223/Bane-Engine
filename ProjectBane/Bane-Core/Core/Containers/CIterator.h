#pragma once


template<typename T>
struct CIterator
{
	CIterator() { }
	CIterator(T* ptr) : m_Data(ptr) { }

	inline CIterator& operator++()
	{
		m_Data++;
		return *this;
	}

	inline CIterator& operator++() const
	{
		m_Data++;
		return *this;
	}

	inline CIterator& operator--()
	{
		m_Data--;
		return *this;
	}

	inline CIterator& operator--() const 
	{
		m_Data--;
		return *this;
	}

	inline bool operator == (const CIterator<T>& other) const
	{
		return m_Data == other.GetPointer();
	}

	inline bool operator != (const CIterator<T>& other) const
	{
		return !((*this) == other);
	}

	inline T& operator* () { return *m_Data; }
	inline T& operator* () const { return *m_Data; }

	inline T* operator->() { return m_Data; }
	inline T* operator->() const { return m_Data; }

	inline T** operator&() { return &m_Data; }
	inline T** operator&() const { return &m_Data; }

	inline T& Get() { return *m_Data; }
	inline T& Get() const { return *m_Data; }

	inline T*& GetPointer() { return m_Data; }
	inline T*& GetPointer() const { return m_Data; }

	inline T**& GetAddressOfPointer() { return &m_Data; }
	inline T**& GetAddressOfPointer() const { return &m_Data; }

	inline CIterator& operator = (const CIterator& right)
	{
		m_Data = right.GetPointer();
		return *this;
	}

	inline CIterator& operator = (T* right)
	{
		m_Data = right;
		return *this;
	}

private:

	T* m_Data;
};

template<typename T>
class CReverseIterator
{
	CReverseIterator() { }
	CReverseIterator(T* ptr) : m_Data(ptr) { }

	inline CReverseIterator& operator++()
	{
		m_Data--;
		return *this;
	}

	inline CReverseIterator& operator++() const
	{
		m_Data--;
		return *this;
	}

	inline CReverseIterator& operator--()
	{
		m_Data++;
		return *this;
	}

	inline CReverseIterator& operator--() const
	{
		m_Data++;
		return *this;
	}

	inline bool operator == (const CReverseIterator<T>& other) const
	{
		return m_Data == other.GetPointer();
	}

	inline bool operator != (const CReverseIterator<T>& other) const
	{
		return !((*this) == other);
	}

	inline T& Get() { return *m_Data; }
	inline T& Get() const { return *m_Data; }

	inline T& operator* () { return *m_Data; }
	inline T& operator* () const { return *m_Data; }

	inline T*& operator->() { return m_Data; }
	inline T*& operator->() const { return m_Data; }

	inline T*& GetPointer() { return m_Data; }
	inline T*& GetPointer() const { return m_Data; }

	inline T**& GetAddressOfPointer() { return &m_Data; }
	inline T**& GetAddressOfPointer() const { return &m_Data; }

	inline CReverseIterator& operator = (const CIterator& right)
	{
		m_Data = right.GetPointer();
		return *this;
	}

	inline CReverseIterator& operator = (T* right)
	{
		m_Data = right;
		return *this;
	}

private:

	T* m_Data;
};

