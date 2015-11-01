#pragma once
#include <System/KPlatform.h>
#include "System/Collections/KMapByVector.h"
#include "System/Collections/KString.h"
#include "System/Misc/KStream.h"

class KDwordSession
{
public:
	KDwordSession(){clear();}
	VOID clear(){dwSessionID = 0; nVlaue = 0;}
	DWORD dwSessionID;
	INT nVlaue;
};
DECLARE_SIMPLE_TYPE(KDwordSession)

class KCommonSession
{
public:
	typedef System::Collections::KMapByVector<int, ::System::Collections::KString<32> > StringSessionMap;
	typedef System::Collections::KMapByVector<int, float> FloatSessionMap;
	typedef System::Collections::KMapByVector<int, int> IntSessionMap;
	typedef System::Collections::KMapByVector<int, INT64> Int64SessionMap;

	class KDwordSessionLess
	{
	public:
		BOOL operator () (const KDwordSession& a, const KDwordSession& b) const
		{
			return a.dwSessionID < a.dwSessionID;
		}
	};
	typedef System::Collections::DynSortedArray<KDwordSession, KDwordSessionLess> DworldArray;
	typedef System::Collections::KMapByVector<DWORD,DworldArray> TypeSessionMap;

public:
	KCommonSession(void);
	~KCommonSession(void);

	const char* GetString(int n) const;
	void SetString(int n, const char* str);

	float GetFloat(int n) const;
	void SetFloat(int n, float f);

	int GetInt(int n) const;
	void SetInt(int n, int val);

	INT GetIntByDworlds(DWORD dwType, DWORD dwSessionID) const;
	void SetIntByDworlds(DWORD dwType, DWORD dwSessionID, INT nValue);

	INT64 GetInt64(int n) const;
	void SetInt64(int n, INT64 val);

	void Clear();

	BOOL Serialize(KMsgOutputStream &oStream);
	BOOL UnSerialize(KMsgInputStream &oStream);

private:
	IntSessionMap m_intSessionMap;
	Int64SessionMap m_int64SessionMap;
	FloatSessionMap m_floatSessionMap;
	StringSessionMap m_stringSessionMap;
	TypeSessionMap m_typeSessionMap;
};
