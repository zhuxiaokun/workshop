#ifndef _K_BITSET_H_
#define _K_BITSET_H_

#include "../KType.h"
#include "../KPlatform.h"

class KBitset
{
public:
	KBitset(int bitCount);
	KBitset(void* pData, int bitCount);
	~KBitset();

public:
	static int GetByteCount(int bitCount);

public:
	int  GetBitCount() const;
	void SetAll();
	void ClearAll();
	BOOL SetBit(int bitno);
	BOOL ClearBit(int bitno);
	BOOL GetBit(int bitno) const;

protected:
	int m_bitCount;
	int m_byteCount;
	DWORD* m_pData;
	DWORD m_bits;    // 当需要的BIT数量小于32时，使用这个DWORD来存储
	BOOL m_memOwner;
};

template <size_t capacity> class KBitsetTmpl : public ::KBitset
{
public:
	enum { CAPACITY = (capacity+31)&(~31), LENGTH=CAPACITY/sizeof(char) };
	char m_bitData[LENGTH];

public:
	KBitsetTmpl() : KBitset(&m_bitData, capacity)
	{
		memset(&m_bitData, 0, sizeof(m_bitData));
	}
	KBitsetTmpl(const KBitsetTmpl& o) : KBitset(&m_bitData, capacity)
	{
		memcpy(&m_bitData, &o.m_bitData, sizeof(m_bitData));
	}
	~KBitsetTmpl()
	{

	}
	KBitsetTmpl& operator = (const KBitsetTmpl& o)
	{
		if(this == &o) return *this;
		memcpy(&m_bitData, &o.m_bitData, sizeof(m_bitData));
		return *this;
	}
	bool init(const void* pData, int bit_count)
	{
		int len = (bit_count + 7) >> 3;
		ASSERT(len >= 0 && LENGTH >= len);
		memcpy(&m_bitData, pData, len);
		if(LENGTH > len)
			memset(&m_bitData[len], 0, LENGTH-len);

		int to_bit = (bit_count + 7) & (~7);
		for(int i=bit_count; i<to_bit; i++)
			this->ClearBit(i);

		return true;
	}
};

#endif
