#pragma once
#include <System/Misc/KStream.h>
#include <System/Collections/DynArray.h>
#include "CppTable.h"

#pragma warning(disable:4275)

class KLuaTableWraper;
class LUAWARP_API KParamPackage : protected KMemoryStream
{
public:
	enum EnumDataType
	{
		enum_int,
		enum_float,
		enum_int64,
		enum_string,
		enum_binary,
		enum_count,
	};
	class IVisitor
	{
	public:
		virtual ~IVisitor() { }
		virtual bool onparam(const char* name, EnumDataType type, const void* data, size_t len) = 0;
	};

public:
	KParamPackage() { }
	virtual ~KParamPackage()  { }

protected:
	int read(void* buffer, size_t len);
	int write(const void* data, size_t len);

public:
	void bind(void* data, size_t len)
	{
		return KMemoryStream::bind(data, len);
	}
	size_t size() const
	{
		return KMemoryStream::size();
	}

public:
	bool append(const char* name, int val);
	bool append(const char* name, double val);
	bool append(const char* name, INT64 val);
	bool append(const char* name, const char* val);
	bool append(const char* name, const void* val, int len);
	bool foreach(IVisitor& v);

public:
	bool dump(KLuaTableWraper& tbl);
	bool remove(KLuaTableWraper& tbl);
};

#pragma warning(default:4275)

template <size_t capacity> class KParamPackageWriter : public KParamPackage
{
public:
	char m_buffer[capacity];
	KParamPackageWriter()
	{
		this->bind(m_buffer, sizeof(m_buffer));
	}
};

class LUAWARP_API KParamPackageReader : public KParamPackage
{
public:
	KParamPackageReader(void* data, size_t len)
	{
		this->bind(data, len);
	}
	template <size_t capacity> KParamPackageReader(KParamPackageWriter<capacity>& w)
	{
		this->bind((void*)w.data(), w.size());
	}
};

template<size_t n> class KRemoteScript
{
public:
	char m_buffer[n];
	KDataOutputStream m_s;
	short* m_pParamSize;
	KParamPackage m_paramPackage;

public:
	// constructor for writer
	KRemoteScript(const char* script):m_s(m_buffer, sizeof(m_buffer))
	{
		m_s.WriteString(script, strlen(script));
		m_pParamSize = (short*)&m_buffer[m_s.m_pos];
		m_s.WriteShort(0);
		m_paramPackage.bind(m_s.m_pBuf+m_s.size(), sizeof(m_buffer)-m_s.size());
	}

public: // for writer
	bool appendParam(const char* name, int val)
	{
		if(!m_paramPackage.append(name, val)) return false;
		*m_pParamSize = (short)m_paramPackage.size();
		return true;
	}
	bool appendParam(const char* name, double val)
	{
		if(!m_paramPackage.append(name, val)) return false;
		*m_pParamSize = (short)m_paramPackage.size();
		return true;
	}
	bool appendParam(const char* name, INT64 val)
	{
		if(!m_paramPackage.append(name, val)) return false;
		*m_pParamSize = (short)m_paramPackage.size();
		return true;
	}
	bool appendParam(const char* name, const char* val)
	{
		if(!m_paramPackage.append(name, val)) return false;
		*m_pParamSize = (short)m_paramPackage.size();
		return true;
	}
	bool appendParam(const char* name, const void* val, int len)
	{
		if(!m_paramPackage.append(name, val, len)) return false;
		*m_pParamSize = (short)m_paramPackage.size();
		return true;
	}
	const void* data() const
	{
		return &m_buffer;
	}
	size_t size() const
	{
		return m_s.size() + m_paramPackage.size();
	}
};

class LUAWARP_API KRemoteScriptReader
{
public:
	char* m_data;
	size_t m_size;
	KRemoteScriptReader(const void* data, size_t len);
	bool execute();
};

class KScriptContextPool
{
public:
	typedef System::Collections::DynArray<KLuaTableWraper*> ContextArray;

public:
	KScriptContextPool();
	~KScriptContextPool();

public:
	KLuaTableWraper* allocContext();
	void freeContext(KLuaTableWraper* tbl);

public:
	ContextArray m_used;
	ContextArray m_avail;
};