#include "KTableRecordData.h"
#include "KTableCacheReader.h"
using namespace System::Memory;

KTableRecordData::KTableRecordData()
	: m_tbname(NULL)
	, m_bufferImpl(NULL)
	, m_dataBuffer(NULL)
{

}

KTableRecordData::~KTableRecordData()
{
	if(m_bufferImpl)
	{
		delete m_bufferImpl;
		m_bufferImpl = NULL;
	}
}

void KTableRecordData::attach(const void* pData, size_t capacity, size_t len)
{
	this->clear();
	m_tmpBuffer.attach((void*)pData, capacity);
	m_dataBuffer = &m_tmpBuffer;
	m_dataBuffer->m_size = len;
}

size_t KTableRecordData::calcCapacity(const KSQLTableSchema& schema)
{
	size_t capacity = 0;
	KTableRecordData::Field col;
	int_r n = schema.GetFieldCount();
	for(int_r i=0; i<n; i++)
	{
		const KSQLTableField* field = schema.GetField((int)i);
		col.fieldIndex = (short)i;
		col.length = field->m_cdt == KSQLTypes::sql_c_timestamp ? sizeof(MYSQL_TIME_K) : field->m_length;
		capacity += col.size();
	}
	return _align_size_(capacity+1024);
}

BOOL KTableRecordData::copyKey(const KSQLTableSchema& schema, KTableRecordData& o)
{
	this->clear();
	this->setVersion(o.version());

	Field* field = o.begin();

	int_r pkNum = schema.GetPrimaryKeyFieldCount();
	while(pkNum > 0)
	{
		if(!field) return FALSE;
		this->append(field->fieldIndex, field->value, field->length);
		pkNum -= 1; field = o.next(field);
	}
	return TRUE;
}

int KTableRecordData::getFieldCount()
{
	int n = 0;
	Field* field = this->begin();
	while(field)
	{
		n++;
		Field* p = this->next(field); if(!p) break;
		field = p;
	}
	return n;
}

bool KTableRecordData::resize(size_t capacity)
{
	if(!m_bufferImpl)
	{
		m_bufferImpl = new ::System::Memory::KDataBufferImpl(capacity);
	}
	else
	{
		if(m_bufferImpl->m_capacity < capacity)
		{
			delete m_bufferImpl;
			m_bufferImpl = new ::System::Memory::KDataBufferImpl(capacity);
			ASSERT(m_bufferImpl);
		}
	}

	m_dataBuffer = m_bufferImpl;
	m_dataBuffer->clear();
	
	return true;
}

size_t KTableRecordData::capacity() const
{
	return m_dataBuffer->m_capacity;
}

size_t KTableRecordData::size() const
{
	return m_dataBuffer->m_size + m_tailw.size();
}

const void* KTableRecordData::dataPtr() const
{
	return ((KDataBuffer*)m_dataBuffer)->_begin();
}

void KTableRecordData::clear()
{
	if(m_dataBuffer) m_dataBuffer->clear();
	m_tailr.clear();
	m_tailw.clear();
}

UINT64 KTableRecordData::version()
{
	ASSERT(m_dataBuffer);
	return *(UINT64*)m_dataBuffer->at(0);
}

KTableRecordData::Field* KTableRecordData::begin()
{
	ASSERT(m_dataBuffer);
	Field* field = (Field*)m_dataBuffer->_dataPtr(sizeof(UINT64));
	if(!field || field->fieldIndex & 0x8000) return NULL;
	ASSERT(m_dataBuffer->_end() >= (char*)field + field->size());
	return field;
}

KTableRecordData::Field* KTableRecordData::next(KTableRecordData::Field* field)
{
	ASSERT(m_dataBuffer);
	int_r pos = (int_r)((char*)field - (char*)m_dataBuffer->_begin());
	ASSERT(pos >= sizeof(UINT64));

	pos += (int_r)field->size();
	field = (Field*)m_dataBuffer->_dataPtr(pos);
	if(!field || field->fieldIndex & 0x8000) return NULL;

	ASSERT(m_dataBuffer->_end() >= (char*)field + field->size());
	return field;
}

size_t KTableRecordData::tail(Field* field, void** ppData)
{
	*ppData = NULL;
	int_r pos = (int_r)((char*)field - (char*)m_dataBuffer->_begin());
	pos += (int_r)field->size();

	WORD* p = (WORD*)m_dataBuffer->_dataPtr(pos);
	if(!p) return 0; ASSERT(*p & 0x8000); p += 1;

	*ppData = p;
	//return m_dataBuffer->m_size - ((char*)p - (char*)m_dataBuffer->_begin());
	return this->size() - ((char*)p - (char*)m_dataBuffer->_begin());
}

KMemoryStream& KTableRecordData::tail_r(Field* field)
{
	void* pData;
	size_t len = this->tail(field, &pData);
	m_tailr.bind(pData, len);
	return m_tailr;
}

KMemoryStream& KTableRecordData::tail_r()
{
	Field* field = this->begin();
	while(field)
	{
		Field* p = this->next(field);
		if(!p) break; field = p;
	}
	
	if(field)
		return this->tail_r(field);
	
	WORD* peof = (WORD*)m_dataBuffer->_dataPtr(sizeof(UINT64));
	ASSERT(*peof & 0x8000);

	void* pData = peof + 1; //m_dataBuffer->_end();
	size_t len = (size_t)((char*)m_dataBuffer->_end() - (char*)pData); // m_dataBuffer->m_capacity - m_dataBuffer->m_size;
	m_tailr.bind(pData, len);
	return m_tailr;
}

void KTableRecordData::setVersion(UINT64 ver)
{
	ASSERT(m_dataBuffer);
	m_dataBuffer->clear();
	ASSERT(m_dataBuffer->push_back(&ver, sizeof(ver)));
}

void KTableRecordData::updateVersion(UINT64 ver)
{
	ASSERT(m_dataBuffer);
	if(!m_dataBuffer->size())
	{
		this->setVersion(ver);
	}
	else
	{
		*(INT64*)m_dataBuffer->_at(0) = ver;
	}
}

void KTableRecordData::append(int_r fieldIndex, const void* data, size_t len)
{
	ASSERT(m_dataBuffer);
	int_r want = sizeof(Field) + len;
	ASSERT(m_dataBuffer->_avail() >= (size_t)want);
	Field* field = (Field*)m_dataBuffer->_end();
	field->fieldIndex = (short)fieldIndex;
	field->length = (WORD)len;
	if(len) memcpy(&field->value, data, len);
	m_dataBuffer->m_size += field->size();
}

void KTableRecordData::append(KTableCacheReader* tableCache, HANDLE h, int_r fieldIndex)
{
	void* pData;
	size_t len = tableCache->_getFieldData(h, fieldIndex, &pData);
	ASSERT(len);
	this->append(fieldIndex, pData, len);
}

void KTableRecordData::eof()
{
	WORD w = 0xffff;
	m_dataBuffer->push_back(&w, sizeof(w));
}

KMemoryStream& KTableRecordData::tail_w()
{
	void* pData = m_dataBuffer->_end();
	size_t len = m_dataBuffer->m_capacity - m_dataBuffer->m_size;
	m_tailw.bind(pData, len);
	return m_tailw;
}

BOOL KTableRecordData::pushData(const void* pData, size_t len)
{
	return m_dataBuffer->push_back(pData, len) ? TRUE : FALSE;
}