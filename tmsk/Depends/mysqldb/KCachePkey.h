#pragma once

#include <System/KType.h>
#include <System/KMacro.h>
#include "KCacheObject.h"
#include "KTableRecordData.h"

/**
 * 作为 Cache Table 的主键的数据类型需要满足几个条件
 *
 * 1. public inner type (1) key_type (2) key_cmp
 * 2. BOOL SetPrimaryKey(KCacheObject& cache, const key_type& val)
 * 3. BOOL GetPrimaryKey(KCacheObject& cache, key_type& val) const
 * 4. BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache)
 * 5. KString<512> ToString(const key_type& val) const
 * 6. BOOL SetAutoIncreaseKey(KCacheObject& cache, UINT64 key)
 * 7. BOOL Read(KTableRecordData& recData, key_type& key);
 * 8. BOOL Write(const key_type& key, KTableRecordData& recData);
 *
 * 这里假定表的主键一定是从第一个字段开始并且字段之间是连续的
 *
 *
 * 缺省主键数据类型使用规则
 *
 * 单个数值型(非浮点)主键
 * 1. 长度小于等于4字节时使用DWORD为主键数据类型
 * 2. 长度大于4字节时使用UINT64为主键数据类型
 * 3. 字符串主键当长度小于64时，使用KString<64>为主键数据类型
 *
 * 两个字段联合主键
 * 两个字段为非浮点数值类型时，使用KPair<first,second>为主键数据类型
 * first和second数据类型的选择同上
 *
 * 非以上情况或者不愿使用上面的规则，需要自己扩展
 *
 */

class KSQLPkey_U32
{
public:
	typedef DWORD key_type;
	class key_cmp
	{
	public:
		int operator () (const key_type& a, const key_type& b) const
		{
			if(a < b) return -1;
			else if(a == b) return 0;
			else return 1;
		}
	};

public:
	BOOL SetPrimaryKey(KCacheObject& cache, const key_type& val);
	BOOL GetPrimaryKey(KCacheObject& cache, key_type& val) const;
	BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache);
	System::Collections::KString<512> ToString(const key_type& val) const;
	BOOL SetAutoIncreaseKey(KCacheObject& cache, UINT64 key);
	BOOL Read(KTableRecordData& recData, key_type& key);
	BOOL Write(const key_type& key, KTableRecordData& recData)
	{
		recData.append(0, &key, sizeof(key));
		return TRUE;
	}
};

class KSQLPkey_U64
{
public:
	typedef UINT64 key_type;
	class key_cmp
	{
	public:
		int operator () (const key_type& a, const key_type& b) const
		{
			if(a < b) return -1;
			else if(a == b) return 0;
			else return 1;
		}
	};

public:
	BOOL SetPrimaryKey(KCacheObject& cache, const key_type& val);
	BOOL GetPrimaryKey(KCacheObject& cache, key_type& val) const;
	BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache);
	System::Collections::KString<512> ToString(const key_type& val) const;
	BOOL SetAutoIncreaseKey(KCacheObject& cache, UINT64 key);
	BOOL Read(KTableRecordData& recData, key_type& key);
	BOOL Write(const key_type& key, KTableRecordData& recData)
	{
		recData.append(0, &key, sizeof(key));
		return TRUE;
	}
};

template<int size> class KSQLPkey_String
{
public:
	typedef System::Collections::KString<size+1> key_type;
	class key_cmp
	{
	public:
		int operator () (const key_type& a, const key_type& b) const
		{
			return strcmp(a.c_str(), b.c_str());
		}
	};

public:
	BOOL SetPrimaryKey(KCacheObject& cache, const key_type& val)
	{
		const char* s = val.c_str();
		return cache.SetString(0, s, (int)val.size());
	}
	BOOL GetPrimaryKey(KCacheObject& cache, key_type& val) const
	{
		int readed = 0;
		char buf[size+1];
		
		if(!cache.GetString(0, buf, size, readed))
			return FALSE;

		val.assign(buf, readed);
		return TRUE;
	}
	BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache)
	{
		key_type val;
		if(!this->GetPrimaryKey(cache, val))
			return FALSE;
		return cmd.SetString(paramBaseIdx, val.c_str(), (int)val.size());
	}
	System::Collections::KString<512> ToString(const key_type& val) const
	{
		return System::Collections::KString<512>(val.c_str(), val.size());
	}
	BOOL SetAutoIncreaseKey(KCacheObject& cache, UINT64 key)
	{
		ASSERT(FALSE); return FALSE;
	}
	BOOL Read(KTableRecordData& recData, key_type& key)
	{
		KTableRecordData::Field* field = recData.begin();
		if(!field) return FALSE;
		ASSERT(field->length <= size);
		key.assign((char*)&field->value, field->length);
		return TRUE;
	}
	BOOL Write(const key_type& key, KTableRecordData& recData)
	{
		recData.append(0, key.c_str(), key.size());
		return TRUE;
	}
};

class KSQLPkey_U32_U32
{
public:
	typedef KPair<DWORD,DWORD> key_type;
	class key_cmp
	{
	public:
		int operator () (const key_type& a, const key_type& b) const
		{
			if(a.first < b.first)
			{
				return -1;
			}
			else if(a.first > b.first)
			{
				return 1;
			}
			else
			{
				if(a.second < b.second)
					return -1;
				else if(a.second == b.second)
					return 0;
				else
					return 1;
			}
		}
	};

public:
	BOOL SetPrimaryKey(KCacheObject& cache, const key_type& val)
	{
		return cache.SetInteger(0, val.first) 
			&& cache.SetInteger(1, val.second);
	}
	BOOL GetPrimaryKey(KCacheObject& cache, key_type& val) const
	{
		int ret1, ret2;
		if(!cache.GetInteger(0, ret1))
			return FALSE;

		if(!cache.GetInteger(1, ret2))
			return FALSE;

		val.first = (DWORD)ret1;
		val.second = (DWORD)ret2;

		return TRUE;
	}
	BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache)
	{
		key_type val;

		if(!this->GetPrimaryKey(cache, val))
			return FALSE;

		return cmd.SetInteger(paramBaseIdx, val.first)
			&& cmd.SetInteger(paramBaseIdx+1, val.second);
	}
	System::Collections::KString<512> ToString(const key_type& val) const
	{
		char buf[128];
		sprintf_k(buf, sizeof(buf), "%u-%u", val.first, val.second);
		return buf;
	}
	BOOL SetAutoIncreaseKey(KCacheObject& cache, UINT64 key)
	{
		ASSERT(FALSE); return FALSE;
	}
	BOOL Read(KTableRecordData& recData, key_type& key)
	{
		KTableRecordData::Field* field = recData.begin();
		if(!field) return FALSE;
		ASSERT(field->length == sizeof(DWORD));
		key.first = *(DWORD*)&field->value;

		field = recData.next(field);
		if(!field) return FALSE;
		ASSERT(field->length == sizeof(DWORD));
		key.second = *(DWORD*)&field->value;
		return TRUE;
	}
	BOOL Write(const key_type& key, KTableRecordData& recData)
	{
		recData.append(0, &key.first, sizeof(key.first));
		recData.append(1, &key.second, sizeof(key.second));
		return TRUE;
	}
};

class KSQLPkey_U32_U64
{
public:
	typedef KPair<DWORD,UINT64> key_type;
	class key_cmp
	{
	public:
		int operator () (const key_type& a, const key_type& b) const
		{
			if(a.first < b.first)
			{
				return -1;
			}
			else if(a.first > b.first)
			{
				return 1;
			}
			else
			{
				if(a.second < b.second)
					return -1;
				else if(a.second == b.second)
					return 0;
				else
				return 1;
			}
		}
	};

public:
	BOOL SetPrimaryKey(KCacheObject& cache, const key_type& val)
	{
		return cache.SetInteger(0, val.first) 
			&& cache.SetInt64(1, val.second);
	}
	BOOL GetPrimaryKey(KCacheObject& cache, key_type& val) const
	{
		int ret1;
		INT64 ret2;
		if(!cache.GetInteger(0, ret1))
			return FALSE;

		if(!cache.GetInt64(1, ret2))
			return FALSE;

		val.first = (DWORD)ret1;
		val.second = (UINT64)ret2;

		return TRUE;
	}
	BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache)
	{
		key_type val;

		if(!this->GetPrimaryKey(cache, val))
			return FALSE;

		return cmd.SetInteger(paramBaseIdx, val.first)
			&& cmd.SetInt64(paramBaseIdx+1, val.second);
	}
	System::Collections::KString<512> ToString(const key_type& val) const
	{
		char buf[128];
		sprintf_k(buf, sizeof(buf), "%u-%llu", val.first, val.second);
		return buf;
	}
	BOOL SetAutoIncreaseKey(KCacheObject& cache, UINT64 key)
	{
		ASSERT(FALSE); return FALSE;
	}
	BOOL Read(KTableRecordData& recData, key_type& key)
	{
		KTableRecordData::Field* field = recData.begin();
		if(!field) return FALSE;
		ASSERT(field->length == sizeof(DWORD));
		key.first = *(DWORD*)&field->value;

		field = recData.next(field);
		if(!field) return FALSE;
		ASSERT(field->length == sizeof(UINT64));
		key.second = *(UINT64*)&field->value;
		return TRUE;
	}
	BOOL Write(const key_type& key, KTableRecordData& recData)
	{
		recData.append(0, &key.first, sizeof(key.first));
		recData.append(1, &key.second, sizeof(key.second));
		return TRUE;
	}
};

class KSQLPkey_U32_S64
{
public:
	typedef KPair< DWORD,System::Collections::KString<64> > key_type;
	class key_cmp
	{
	public:
		int operator () (const key_type& a, const key_type& b) const
		{
			if(a.first < b.first)
			{
				return -1;
			}
			else if(a.first > b.first)
			{
				return 1;
			}
			else
			{
				return strcmp(a.second.c_str(), b.second.c_str());
			}
		}
	};

public:
	BOOL SetPrimaryKey(KCacheObject& cache, const key_type& val)
	{
		return cache.SetInteger(0, val.first) 
			&& cache.SetString(1, val.second.c_str(), (int)val.second.size());
	}
	BOOL GetPrimaryKey(KCacheObject& cache, key_type& val) const
	{
		int ret1;
		char ret2[64];
		int readed = 0;

		if(!cache.GetInteger(0, ret1))
			return FALSE;

		if(!cache.GetString(1, ret2, 64, readed))
			return FALSE;

		val.first = (DWORD)ret1;
		val.second.assign(ret2, readed);

		return TRUE;
	}
	BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache)
	{
		key_type val;

		if(!this->GetPrimaryKey(cache, val))
			return FALSE;

		return cmd.SetInteger(paramBaseIdx, val.first)
			&& cmd.SetString(paramBaseIdx+1, val.second.c_str(), (int)val.second.size());
	}
	System::Collections::KString<512> ToString(const key_type& val) const
	{
		char buf[128];
		sprintf_k(buf, sizeof(buf), "%u-%s", val.first, val.second.c_str());
		return buf;
	}
	BOOL SetAutoIncreaseKey(KCacheObject& cache, UINT64 key)
	{
		ASSERT(FALSE); return FALSE;
	}
	BOOL Read(KTableRecordData& recData, key_type& key)
	{
		KTableRecordData::Field* field = recData.begin();
		if(!field) return FALSE;
		ASSERT(field->length == sizeof(DWORD));
		key.first = *(DWORD*)&field->value;

		field = recData.next(field);
		if(!field) return FALSE;
		ASSERT(field->length < 64);
		key.second.assign((char*)field->value, field->length);
		return TRUE;
	}
	BOOL Write(const key_type& key, KTableRecordData& recData)
	{
		recData.append(0, &key.first, sizeof(key.first));
		recData.append(1, key.second.c_str(), key.second.size());
		return TRUE;
	}
};

class KSQLPkey_U64_U32
{
public:
	typedef KPair<UINT64,DWORD> key_type;
	class key_cmp
	{
	public:
		int operator () (const key_type& a, const key_type& b) const
		{
			if(a.first < b.first)
			{
				return -1;
			}
			else if(a.first > b.first)
			{
				return 1;
			}
			else
			{
				if(a.second < b.second)
					return -1;
				else if(a.second == b.second)
					return 0;
				else
				return 1;
			}
		}
	};

public:
	BOOL SetPrimaryKey(KCacheObject& cache, const key_type& val)
	{
		return cache.SetInt64(0, val.first) 
			&& cache.SetInteger(1, val.second);
	}
	BOOL GetPrimaryKey(KCacheObject& cache, key_type& val) const
	{
		INT64 ret1;
		int ret2;

		if(!cache.GetInt64(0, ret1))
			return FALSE;

		if(!cache.GetInteger(1, ret2))
			return FALSE;

		val.first = (UINT64)ret1;
		val.second = (DWORD)ret2;

		return TRUE;
	}
	BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache)
	{
		key_type val;

		if(!this->GetPrimaryKey(cache, val))
			return FALSE;

		return cmd.SetInt64(paramBaseIdx, val.first)
			&& cmd.SetInteger(paramBaseIdx+1, val.second);
	}
	System::Collections::KString<512> ToString(const key_type& val) const
	{
		char buf[128];
		sprintf_k(buf, sizeof(buf), "%llu-%u", val.first, val.second);
		return buf;
	}
	BOOL SetAutoIncreaseKey(KCacheObject& cache, UINT64 key)
	{
		ASSERT(FALSE); return FALSE;
	}
	BOOL Read(KTableRecordData& recData, key_type& key)
	{
		KTableRecordData::Field* field = recData.begin();
		if(!field) return FALSE;
		ASSERT(field->length == sizeof(UINT64));
		key.first = *(UINT64*)&field->value;

		field = recData.next(field);
		if(!field) return FALSE;
		ASSERT(field->length == sizeof(DWORD));
		key.second = *(DWORD*)&field->value;
		return TRUE;
	}
	BOOL Write(const key_type& key, KTableRecordData& recData)
	{
		recData.append(0, &key.first, sizeof(key.first));
		recData.append(1, &key.second, sizeof(key.second));
		return TRUE;
	}
};

class KSQLPkey_U64_U64
{
public:
	typedef KPair<UINT64,UINT64> key_type;
	class key_cmp
	{
	public:
		int operator () (const key_type& a, const key_type& b) const
		{
			if(a.first < b.first)
			{
				return -1;
			}
			else if(a.first > b.first)
			{
				return 1;
			}
			else
			{
				if(a.second < b.second)
					return -1;
				else if(a.second == b.second)
					return 0;
				else
				return 1;
			}
		}
	};

public:
	BOOL SetPrimaryKey(KCacheObject& cache, const key_type& val)
	{
		return cache.SetInt64(0, val.first) 
			&& cache.SetInt64(1, val.second);
	}
	BOOL GetPrimaryKey(KCacheObject& cache, key_type& val) const
	{
		INT64 ret1;
		INT64 ret2;

		if(!cache.GetInt64(0, ret1))
			return FALSE;

		if(!cache.GetInt64(1, ret2))
			return FALSE;

		val.first = (UINT64)ret1;
		val.second = (UINT64)ret2;

		return TRUE;
	}
	BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache)
	{
		key_type val;

		if(!this->GetPrimaryKey(cache, val))
			return FALSE;

		return cmd.SetInt64(paramBaseIdx, val.first)
			&& cmd.SetInt64(paramBaseIdx+1, val.second);
	}
	System::Collections::KString<512> ToString(const key_type& val) const
	{
		char buf[128];
		sprintf_k(buf, sizeof(buf), "%llu-%llu", val.first, val.second);
		return buf;
	}
	BOOL SetAutoIncreaseKey(KCacheObject& cache, UINT64 key)
	{
		ASSERT(FALSE); return FALSE;
	}
	BOOL Read(KTableRecordData& recData, key_type& key)
	{
		KTableRecordData::Field* field = recData.begin();
		if(!field) return FALSE;
		ASSERT(field->length == sizeof(UINT64));
		key.first = *(UINT64*)&field->value;

		field = recData.next(field);
		if(!field) return FALSE;
		ASSERT(field->length == sizeof(UINT64));
		key.second = *(UINT64*)&field->value;
		return TRUE;
	}
	BOOL Write(const key_type& key, KTableRecordData& recData)
	{
		recData.append(0, &key.first, sizeof(key.first));
		recData.append(1, &key.second, sizeof(key.second));
		return TRUE;
	}
};

class KSQLPkey_U64_S64
{
public:
	typedef KPair< UINT64,System::Collections::KString<64> > key_type;
	class key_cmp
	{
	public:
		int operator () (const key_type& a, const key_type& b) const
		{
			if(a.first < b.first)
			{
				return -1;
			}
			else if(a.first > b.first)
			{
				return 1;
			}
			else
			{
				return strcmp(a.second.c_str(), b.second.c_str());
			}
		}
	};

public:
	BOOL SetPrimaryKey(KCacheObject& cache, const key_type& val)
	{
		return cache.SetInt64(0, val.first) 
			&& cache.SetString(1, val.second.c_str(), (int)val.second.size());
	}
	BOOL GetPrimaryKey(KCacheObject& cache, key_type& val) const
	{
		INT64 ret1;
		char ret2[64];
		int readed = 0;

		if(!cache.GetInt64(0, ret1))
			return FALSE;

		if(!cache.GetString(1, ret2, 64, readed))
			return FALSE;

		val.first = (UINT64)ret1;
		val.second.assign(ret2, readed);

		return TRUE;
	}
	BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache)
	{
		key_type val;

		if(!this->GetPrimaryKey(cache, val))
			return FALSE;

		return cmd.SetInt64(paramBaseIdx, val.first)
			&& cmd.SetString(paramBaseIdx+1, val.second.c_str(), (int)val.second.size());
	}
	System::Collections::KString<512> ToString(const key_type& val) const
	{
		char buf[128];
		sprintf_k(buf, sizeof(buf), "%llu-%s", val.first, val.second.c_str());
		return buf;
	}
	BOOL SetAutoIncreaseKey(KCacheObject& cache, UINT64 key)
	{
		ASSERT(FALSE); return FALSE;
	}
	BOOL Read(KTableRecordData& recData, key_type& key)
	{
		KTableRecordData::Field* field = recData.begin();
		if(!field) return FALSE;
		ASSERT(field->length == sizeof(UINT64));
		key.first = *(UINT64*)&field->value;

		field = recData.next(field);
		if(!field) return FALSE;
		ASSERT(field->length < 64);
		key.second.assign((char*)&field->value, field->length);
		return TRUE;
	}
	BOOL Write(const key_type& key, KTableRecordData& recData)
	{
		recData.append(0, &key.first, sizeof(key.first));
		recData.append(1, key.second.c_str(), key.second.size());
		return TRUE;
	}
};

class KSQLPkey_S64_U32
{
public:
	typedef KPair<System::Collections::KString<64>,DWORD> key_type;
	class key_cmp
	{
	public:
		int operator () (const key_type& a, const key_type& b) const
		{
			int n = strcmp(a.first.c_str(), b.first.c_str());
			if(n) return n;

			if(a.second < b.second)
				return -1;
			else if(a.second == b.second)
				return 0;
			else
				return 1;
		}
	};

public:
	BOOL SetPrimaryKey(KCacheObject& cache, const key_type& val)
	{
		return cache.SetString(0, val.first.c_str(), (int)val.first.size())
			&& cache.SetInteger(1, val.second);
	}
	BOOL GetPrimaryKey(KCacheObject& cache, key_type& val) const
	{
		char ret1[64];
		int ret2;

		int readed = 0;

		if(!cache.GetString(0, ret1, 64, readed))
			return FALSE;

		if(!cache.GetInteger(1, ret2))
			return FALSE;

		val.first.assign(ret1, readed);
		val.second = (DWORD)ret2;

		return TRUE;
	}
	BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache)
	{
		key_type val;

		if(!this->GetPrimaryKey(cache, val))
			return FALSE;

		return cmd.SetString(paramBaseIdx, val.first.c_str(), (int)val.first.size())
			&& cmd.SetInteger(paramBaseIdx+1, val.second);
	}
	System::Collections::KString<512> ToString(const key_type& val) const
	{
		char buf[128];
		sprintf_k(buf, sizeof(buf), "%s-%u", val.first.c_str(), val.second);
		return buf;
	}
	BOOL SetAutoIncreaseKey(KCacheObject& cache, UINT64 key)
	{
		ASSERT(FALSE); return FALSE;
	}
	BOOL Read(KTableRecordData& recData, key_type& key)
	{
		KTableRecordData::Field* field = recData.begin();
		if(!field) return FALSE;
		ASSERT(field->length < 64);
		key.first.assign((char*)&field->value, field->length);

		field = recData.next(field);
		if(!field) return FALSE;
		ASSERT(field->length == sizeof(DWORD));
		key.second = *(DWORD*)&field->value;
		return TRUE;
	}
	BOOL Write(const key_type& key, KTableRecordData& recData)
	{
		recData.append(0, key.first.c_str(), key.first.size());
		recData.append(1, &key.second, sizeof(key.second));
		return TRUE;
	}
};

class KSQLPkey_S64_U64
{
public:
	typedef KPair<System::Collections::KString<64>,UINT64> key_type;
	class key_cmp
	{
	public:
		int operator () (const key_type& a, const key_type& b) const
		{
			int n = strcmp(a.first.c_str(), b.first.c_str());
			if(n) return n;

			if(a.second < b.second)
				return -1;
			else if(a.second == b.second)
				return 0;
			else
				return 1;
		}
	};

public:
	BOOL SetPrimaryKey(KCacheObject& cache, const key_type& val)
	{
		return cache.SetString(0, val.first.c_str(), (int)val.first.size())
			&& cache.SetInt64(1, val.second);
	}
	BOOL GetPrimaryKey(KCacheObject& cache, key_type& val) const
	{
		char ret1[64];
		INT64 ret2;

		int readed = 0;

		if(!cache.GetString(0, ret1, 64, readed))
			return FALSE;

		if(!cache.GetInt64(1, ret2))
			return FALSE;

		val.first.assign(ret1, readed);
		val.second = (UINT64)ret2;

		return TRUE;
	}
	BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache)
	{
		key_type val;

		if(!this->GetPrimaryKey(cache, val))
			return FALSE;

		return cmd.SetString(paramBaseIdx, val.first.c_str(), (int)val.first.size())
			&& cmd.SetInt64(paramBaseIdx+1, val.second);
	}
	System::Collections::KString<512> ToString(const key_type& val) const
	{
		char buf[128];
		sprintf_k(buf, sizeof(buf), "%s-%llu", val.first.c_str(), val.second);
		return buf;
	}
	BOOL SetAutoIncreaseKey(KCacheObject& cache, UINT64 key)
	{
		ASSERT(FALSE); return FALSE;
	}
	BOOL Read(KTableRecordData& recData, key_type& key)
	{
		KTableRecordData::Field* field = recData.begin();
		if(!field) return FALSE;
		ASSERT(field->length < 64);
		key.first.assign((char*)&field->value, field->length);

		field = recData.next(field);
		if(!field) return FALSE;
		ASSERT(field->length == sizeof(UINT64));
		key.second = *(UINT64*)&field->value;
		return TRUE;
	}
	BOOL Write(const key_type& key, KTableRecordData& recData)
	{
		recData.append(0, key.first.c_str(), key.first.size());
		recData.append(1, &key.second, sizeof(key.second));
		return TRUE;
	}
};

class KSQLPkey_S64_S64
{
public:
	typedef KPair< System::Collections::KString<64>,System::Collections::KString<64> > key_type;
	class key_cmp
	{
	public:
		int operator () (const key_type& a, const key_type& b) const
		{
			int n = strcmp(a.first.c_str(), b.first.c_str());
			if(n) return n;
			return strcmp(a.second.c_str(), b.second.c_str());
		}
	};

public:
	BOOL SetPrimaryKey(KCacheObject& cache, const key_type& val)
	{
		return cache.SetString(0, val.first.c_str(), (int)val.first.size())
			&& cache.SetString(1, val.second.c_str(), (int)val.second.size());
	}
	BOOL GetPrimaryKey(KCacheObject& cache, key_type& val) const
	{
		char ret1[64];
		char ret2[64];

		int readed1 = 0, readed2 = 0;

		if(!cache.GetString(0, ret1, 64, readed1))
			return FALSE;

		if(!cache.GetString(1, ret2, 64, readed2))
			return FALSE;

		val.first.assign(ret1, readed1);
		val.second.assign(ret2, readed2);

		return TRUE;
	}
	BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache)
	{
		key_type val;

		if(!this->GetPrimaryKey(cache, val))
			return FALSE;

		return cmd.SetString(paramBaseIdx, val.first.c_str(), (int)val.first.size())
			&& cmd.SetString(paramBaseIdx+1, val.second.c_str(), (int)val.second.size());
	}
	System::Collections::KString<512> ToString(const key_type& val) const
	{
		char buf[128];
		sprintf_k(buf, sizeof(buf), "%s-%s", val.first.c_str(), val.second.c_str());
		return buf;
	}
	BOOL SetAutoIncreaseKey(KCacheObject& cache, UINT64 key)
	{
		ASSERT(FALSE); return FALSE;
	}
	BOOL Read(KTableRecordData& recData, key_type& key)
	{
		KTableRecordData::Field* field = recData.begin();
		if(!field) return FALSE;
		ASSERT(field->length < 64);
		key.first.assign((char*)&field->value, field->length);

		field = recData.next(field);
		if(!field) return FALSE;
		ASSERT(field->length < 64);
		key.second.assign((char*)&field->value, field->length);
		return TRUE;
	}
	BOOL Write(const key_type& key, KTableRecordData& recData)
	{
		recData.append(0, key.first.c_str(), key.first.size());
		recData.append(1, key.second.c_str(), key.second.size());
		return TRUE;
	}
};

class KSQLPkey_U32_U32_U32
{
public:
	typedef KTriple<DWORD,DWORD,DWORD> key_type;
	class key_cmp
	{
	public:
		int operator () (const key_type& a, const key_type& b) const
		{
			return memcmp(&a, &b, 3*sizeof(DWORD));
		}
	};

public:
	BOOL SetPrimaryKey(KCacheObject& cache, const key_type& val)
	{
		return cache.SetInteger(0, val.first) 
			&& cache.SetInteger(1, val.second)
			&& cache.SetInteger(2, val.third);
	}
	BOOL GetPrimaryKey(KCacheObject& cache, key_type& val) const
	{
		int ret1, ret2, ret3;
		if(!cache.GetInteger(0, ret1) ||
			!cache.GetInteger(1, ret2) ||
			!cache.GetInteger(2, ret3))
			return FALSE;
		val.first = (DWORD)ret1;
		val.second = (DWORD)ret2;
		val.third = (DWORD)ret3;
		return TRUE;
	}
	BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache)
	{
		key_type val;

		if(!this->GetPrimaryKey(cache, val))
			return FALSE;

		return cmd.SetInteger(paramBaseIdx, val.first)
			&& cmd.SetInteger(paramBaseIdx+1, val.second)
			&& cmd.SetInteger(paramBaseIdx+2, val.third);
	}
	System::Collections::KString<512> ToString(const key_type& val) const
	{
		char buf[128];
		sprintf_k(buf, sizeof(buf), "%u-%u-%u", val.first, val.second, val.third);
		return buf;
	}
	BOOL SetAutoIncreaseKey(KCacheObject& cache, UINT64 key)
	{
		ASSERT(FALSE); return FALSE;
	}
	BOOL Read(KTableRecordData& recData, key_type& key)
	{
		KTableRecordData::Field* field = recData.begin();
		if(!field) return FALSE;
		ASSERT(field->length == sizeof(DWORD));
		key.first = *(DWORD*)&field->value;

		field = recData.next(field);
		if(!field) return FALSE;
		ASSERT(field->length == sizeof(DWORD));
		key.second = *(DWORD*)&field->value;

		field = recData.next(field);
		if(!field) return FALSE;
		ASSERT(field->length == sizeof(DWORD));
		key.third = *(DWORD*)&field->value;

		return TRUE;
	}
	BOOL Write(const key_type& key, KTableRecordData& recData)
	{
		recData.append(0, &key.first, sizeof(key.first));
		recData.append(1, &key.second, sizeof(key.second));
		recData.append(2, &key.third, sizeof(key.third));
		return TRUE;
	}
};