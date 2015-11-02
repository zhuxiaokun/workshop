/********************************************************************
created:	2010/5/31
filename: 	System\Collections\KBitset.h
file path:	System\Collections
file base:	DynComplexArray
file ext:	h
author:		Allen Wang
copyright:	AuroraGame

purpose:	一个bitset容器，用以存储bit并且查询。使用对象空间存储整个map，
			没有提供iterator，从原来KTreasureCaseManager.h分离出来
*********************************************************************/
#pragma once
#include "../KType.h"
#include "../KMacro.h"
#include "../Misc/KStream.h"
namespace System { namespace Collections {

/*
 * bitset类，一个1维的bitmap。
 */
template <DWORD LENGTH>
class KBitset
{
public:
	typedef KBitset<LENGTH>  My_type;
	const static DWORD BYTE_LENGTH = (LENGTH + 7) >> 3;
	const static DWORD ARRAY_LENGTH = (BYTE_LENGTH + 3) >> 2;
	
public:
	KBitset()
	{
		Reset();
	}

	KBitset(const My_type &Src)
	{
		memcpy(m_bitmap, Src.m_bitmap, BYTE_LENGTH);
	}

	/**
	 * 所有bit清零
	 */
	inline void Reset()
	{
		memset(m_bitmap, 0x0, BYTE_LENGTH);
	}

	/**
	 * 所有bit置1
	 */
	inline void SetAll()
	{
		memset(m_bitmap, 0xff, BYTE_LENGTH);
	}

	/**
	 * 设某一位为1
	 */
	inline void Set(DWORD pos)
	{
		ASSERT_RETURN_VOID(pos < LENGTH);
		m_bitmap[pos >> 5] |= ( 1 << (pos & 0x1f) );
	}

	/**
	 * 设某一位为0
	 */
	inline void Clear(DWORD pos)
	{
		ASSERT_RETURN_VOID(pos < LENGTH);
		m_bitmap[pos >> 5] &= ~( 1 << (pos & 0x1f) );
	}

	inline bool IsClearAll()
	{
		bool bRet = true;
		for(int i = 0;i<ARRAY_LENGTH;i++)
		{
			if(m_bitmap[i] != 0)
			{
				bRet = false;
				break;
			}
		}
		return bRet;
	}


	/**
	 * 检查某位，为1返回true，否则false
	 */
	inline bool Check(DWORD pos) const
	{
		ASSERT_RETURN(pos < LENGTH, 0);
		return (m_bitmap[pos >> 5] & ( 1 << (pos & 0x1f) ) ) != 0;
	}

	/**
	 * 同check
	 */
	inline bool operator[](const DWORD pos) const
	{
		return Check(pos);
	}

	/**
	* 重载位&运算符,add by Augustine 6/18/2012
	*/

	inline My_type operator & (const My_type &Other)
	{
		My_type Temp;
		for(int i = 0;i<ARRAY_LENGTH;i++)
		{
			Temp.m_bitmap[i] = (Other.m_bitmap[i]&m_bitmap[i]);
		}
		return Temp;
	}

	/**
	* 重载位|运算符,add by Augustine 6/18/2012
	*/
	inline My_type operator | (const My_type &Other)
	{
		My_type Temp;
		for(int i = 0;i<ARRAY_LENGTH;i++)
		{
			Temp.m_bitmap[i] = (Other.m_bitmap[i]|m_bitmap[i]);
		}
		return Temp;
	}

	/**
	* 重载位^运算符,add by Augustine 6/18/2012
	*/
	inline My_type operator ^ (const My_type &Other)
	{
		My_type Temp;
		for(int i = 0;i<ARRAY_LENGTH;i++)
		{
			Temp.m_bitmap[i] = (Other.m_bitmap[i]^m_bitmap[i]);
		}
		return Temp;
	}

	/**
	* 重载位~运算符,add by Augustine 6/18/2012
	*/
	inline My_type operator ~ ()
	{
		My_type Temp;
		for(int i = 0;i<ARRAY_LENGTH;i++)
		{
			Temp.m_bitmap[i] = ~m_bitmap[i];
		}
		return Temp;
	}

	/**
	* 重载位&=运算符,add by Augustine 6/18/2012
	*/
	inline My_type& operator &= (const My_type &Other)
	{
		for(int i = 0;i<ARRAY_LENGTH;i++)
		{
			m_bitmap[i] = (Other.m_bitmap[i]&m_bitmap[i]);
		}
		return *this;
	}

	/**
	* 重载位|=运算符,add by Augustine 6/18/2012
	*/
	inline My_type& operator |= (const My_type &Other)
	{
		for(int i = 0;i<ARRAY_LENGTH;i++)
		{
			m_bitmap[i] = (Other.m_bitmap[i]|m_bitmap[i]);
		}
		return *this;
	}
	/**
	* 重载位^=运算符,add by Augustine 6/18/2012
	*/
	inline My_type& operator ^= (const My_type &Other)
	{
		for(int i = 0;i<ARRAY_LENGTH;i++)
		{
			m_bitmap[i] = (Other.m_bitmap[i]^m_bitmap[i]);
		}
		return *this;
	}

	/**
	* 重载 ==运算符,add by Augustine 6/18/2012
	*/
	inline bool operator == (const My_type &Other)
	{
		bool bRet = true;
		for(int i = 0;i<ARRAY_LENGTH;i++)
		{
			if(m_bitmap[i] != Other.m_bitmap[i])
			{
				bRet = false;
				break;
			}
		}
		return bRet;
	}

	/**
	* 重载 !运算符,add by Augustine 6/18/2012
	*/
	inline bool operator !()
	{
		return IsClearAll();
	}
	
	inline My_type& operator = (const My_type &Other)
	{
		for(int i = 0;i<ARRAY_LENGTH;i++)
		{
			m_bitmap[i] = Other.m_bitmap[i];
		}
		return *this;
	}

	inline void SetStorageBuf(DWORD BufData,int iIndex)
	{
		ASSERT_RETURN_VOID(iIndex < ARRAY_LENGTH);
		m_bitmap[iIndex] = BufData;
	}

	inline DWORD GetStorageBuf(int iIndex) const
	{
		ASSERT_RETURN(iIndex < ARRAY_LENGTH,0);
		return m_bitmap[iIndex];
	}

	/**
	 * 输出自己到SERILIZER
	 */
	template<typename SERILIZER>
	inline void Output(SERILIZER& cp) const
	{
		for (DWORD i = 0; i < ARRAY_LENGTH; i++)
		{
			cp << m_bitmap[i];
		}
	}
	/**
	 * 从SERILIZER输入自己
	 */
	template<typename SERILIZER>
	inline void Input(SERILIZER& cp)
	{
		for (DWORD i = 0; i < ARRAY_LENGTH; i++)
		{
			cp >> m_bitmap[i];
		}
	}

	/**
	 * 配套System下，流相关存储，有返回值的
	 */
	inline bool Serilize( StreamInterface& os )
	{
		return os.WriteData(m_bitmap, sizeof(m_bitmap)) == sizeof(m_bitmap);
	}

	inline bool Unserilize( StreamInterface& is )
	{
		return is.ReadData(m_bitmap, sizeof(m_bitmap)) == sizeof(m_bitmap);
	}

protected:
	DWORD m_bitmap[ARRAY_LENGTH];
};


};	};