#include "KParamPackage.h"
#include "KLuaTableHolder.h"

int KParamPackage::read(void* buffer, size_t len)
{
	return (int)KMemoryStream::ReadData(buffer, len);
}

int KParamPackage::write(const void* data, size_t len)
{
	return (int)KMemoryStream::WriteData(data, len);
}

bool KParamPackage::append(const char* name, int val)
{
	if(!this->WriteString8(name)) return false;
	if(!this->WriteByte(enum_int)) return false;
	if(!this->WriteInt(val)) return false;
	return true;
}

bool KParamPackage::append(const char* name, double val)
{
	if(!this->WriteString8(name)) return false;
	if(!this->WriteByte(enum_float)) return false;
	if(!this->WriteByteArray(&val, sizeof(val))) return false;
	return true;
}

bool KParamPackage::append(const char* name, INT64 val)
{
	if(!this->WriteString8(name)) return false;
	if(!this->WriteByte(enum_int64)) return false;
	if(!this->WriteInt64(val)) return false;
	return true;
}

bool KParamPackage::append(const char* name, const char* val)
{
	if(!this->WriteString8(name)) return false;
	if(!this->WriteByte(enum_string)) return false;
	short len = (short)strlen(val) + 1;
	if(!this->WriteShort(len)) return false;
	if(!this->WriteByteArray(val, len)) return false;
	return true;
}

bool KParamPackage::append(const char* name, const void* val, int len)
{
	if(!this->WriteString8(name)) return false;
	if(!this->WriteByte(enum_binary)) return false;
	if(!this->WriteInt(len)) return false;
	if(!this->WriteByteArray(val, len)) return false;
	return true;
}

bool KParamPackage::foreach(IVisitor& v)
{
	this->Seek(0);

	BYTE type;
	char name[128];

	while(this->ReadString8(name, sizeof(name)))
	{
		if(!this->ReadByte(type))
			return false;
		switch((EnumDataType)type)
		{
		case enum_int:
			{
				int val;
				if(!this->ReadInt(val))
					return false;
				if(!v.onparam(name, (EnumDataType)type, &val, sizeof(val)))
					return false;
			} break;
		case enum_float:
			{
				double val;
				if(!this->ReadByteArray(&val, sizeof(val)))
					return false;
				if(!v.onparam(name, (EnumDataType)type, &val, sizeof(val)))
					return false;
			} break;
		case enum_int64:
			{
				INT64 val;
				if(!this->ReadInt64(val))
					return false;
				if(!v.onparam(name, (EnumDataType)type, &val, sizeof(val)))
					return false;
			} break;
		case enum_string:
			{
				short len;
				if(!this->ReadShort(len))
					return false;
				if(!this->CheckBuffer(len))
					return false;
				size_t pos = this->tell();
				this->Seek(pos+len);
				void* val = (char*)this->data() + pos;
				if(!v.onparam(name, (EnumDataType)type, val, len))
					return false;
			} break;
		case enum_binary:
			{
				int len;
				if(!this->ReadInt(len))
					return false;
				if(!this->CheckBuffer(len))
					return false;
				size_t pos = this->tell();
				this->Seek(pos+len);
				void* val = (char*)this->data() + pos;
				if(!v.onparam(name, (EnumDataType)type, val, len))
					return false;
			} break;
		default:
			return false;
		}
	}
	
	return true;
}

class KVisitor_setParam : public KParamPackage::IVisitor
{
public:
	KLuaTableWraper& m_tbl;
	typedef KParamPackage::EnumDataType EnumDataType;
	KVisitor_setParam(KLuaTableWraper& tbl):m_tbl(tbl)
	{
	}
	bool onparam(const char* name, EnumDataType type, const void* data, size_t len)
	{
		switch(type)
		{
		case KParamPackage::enum_int:
			m_tbl.setTableField(name, *(int*)data);
			break;
		case KParamPackage::enum_float:
			m_tbl.setTableField(name, *(double*)data);
			break;
		case KParamPackage::enum_int64:
			m_tbl.setTableField(name, *(INT64*)data);
			break;
		case KParamPackage::enum_string:
			m_tbl.setTableField(name, (char*)data);
			break;
		case KParamPackage::enum_binary:
			m_tbl.setTableField(name, data);
			break;
		}
		return true;
	}
};

bool KParamPackage::dump(KLuaTableWraper& tbl)
{
	KVisitor_setParam v(tbl);
	return this->foreach(v);
}

class KVisitor_removeParam : public KParamPackage::IVisitor
{
public:
	KLuaTableWraper& m_tbl;
	typedef KParamPackage::EnumDataType EnumDataType;
	KVisitor_removeParam(KLuaTableWraper& tbl):m_tbl(tbl)
	{
	}
	bool onparam(const char* name, EnumDataType type, const void* data, size_t len)
	{
		m_tbl.setnil(name);
		return true;
	}
};

bool KParamPackage::remove(KLuaTableWraper& tbl)
{
	KVisitor_removeParam v(tbl);
	return this->foreach(v);
}

////////////////////////////////////////////////////////
// KRemoteScriptReader
//
KRemoteScriptReader::KRemoteScriptReader(const void* data, size_t len):m_data((char*)data),m_size(len)
{
}

bool KRemoteScriptReader::execute()
{
	short script_len = *(short*)m_data;
	const char* script = m_data + sizeof(short);
	short param_len = (short)(m_size - sizeof(short) - script_len - sizeof(short));
	void* params = m_data + sizeof(short) + script_len + sizeof(short);
	KParamPackageReader r(params, param_len);
	return LuaWraper.doStringWithParam(r, script, script_len);
}

//////////////////////////////////////////////////////
// KScriptContextPool
//
KScriptContextPool::KScriptContextPool()
{

}

KScriptContextPool::~KScriptContextPool()
{
	size_t n = m_used.size();
	for(size_t i=0; i<n; i++) delete m_used[(int)i];
	m_used.clear();
	n = m_avail.size();
	for(size_t i=0; i<n; i++) delete m_avail[(int)i];
	m_avail.clear();
}

KLuaTableWraper* KScriptContextPool::allocContext()
{
	KLuaTableWraper* tbl = NULL;
	lua_State* L = LuaWraper.m_l;

	if(m_avail.empty())
	{
		KLuaStackRecover sr(L);
		lua_newtable(L);
		tbl = new KLuaTableWraper(-1);
		tbl->setMetatable("_G");
	}
	else
	{
		tbl = m_avail.pop_back();
	}
	
	m_used.push_back(tbl);
	return tbl;
}

void KScriptContextPool::freeContext(KLuaTableWraper* tbl)
{
	int n = m_used.size();
	for(int i=0; i<n; i++)
	{
		if(m_used[i] == tbl)
		{
			m_used.erase(i);
			m_avail.push_back(tbl);
			return;
		}
	}
	ASSERT(FALSE);
}