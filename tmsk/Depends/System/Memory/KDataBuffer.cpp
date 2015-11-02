#include "KDataBuffer.h"

namespace System { namespace Memory {

/// KDataBuffer

KDataBuffer::KDataBuffer():m_buffer(NULL),m_capacity(0),m_size(0)
{

}

KDataBuffer::~KDataBuffer()
{

}

bool KDataBuffer::attach(void* buffer, size_t capacity)
{
	m_buffer = (char*)buffer;
	m_capacity = capacity;
	m_size = 0;
	return true;
}

bool KDataBuffer::attach(void* buffer, size_t capacity, size_t dataSize)
{
	m_buffer = (char*)buffer;
	m_capacity = capacity;
	m_size = dataSize;
	return true;
}

void KDataBuffer::clear()
{
	m_size = 0;
}

const void* KDataBuffer::at(size_t pos) const
{
	ASSERT(pos >= 0 && pos < m_size);
	return m_buffer+pos;
}

void KDataBuffer::erase(size_t pos, size_t count)
{
	ASSERT(count >= 0);
	ASSERT(pos >= 0 && pos+count <= m_size);
	void* src = m_buffer + pos + count;
	void* dst = m_buffer + pos;
	size_t len = m_size - (pos+count);
	if(len > 0) memmove(dst, src, len);
	m_size -= count;
}

void KDataBuffer::erase(const void* p, size_t count)
{
	size_t pos = (size_t)((char*)p - m_buffer);
	this->erase(pos, count);
}

bool KDataBuffer::insert(size_t pos, const void* p, size_t len)
{
	ASSERT(pos <= m_size);
	if(m_size+len > m_capacity)
		return false;

	void* src = m_buffer + pos;
	void* dst = m_buffer + pos + len;
	
	size_t l = m_size - pos;
	if(l > 0) memmove(dst, src, l);
	
	memcpy(src, p, len);
	m_size += len;
	
	return true;
}

bool KDataBuffer::push_back(const void* p, size_t len)
{
	return this->insert(m_size, p, len);
}

bool KDataBuffer::seek(size_t pos)
{
	ASSERT(pos >= 0 && pos <= m_capacity);
	m_size = pos;
	return true;
}

void* KDataBuffer::_at(size_t pos)
{
	ASSERT(pos >= 0 && pos < m_capacity);
	return m_buffer+pos;
}

void* KDataBuffer::_begin()
{
	return m_buffer;
}

void* KDataBuffer::_end()
{
	return m_buffer+m_size;
}

size_t KDataBuffer::_avail() const
{
	return m_capacity - m_size;
}

void* KDataBuffer::_dataPtr(size_t pos)
{
	if(pos >= m_size) return NULL;
	return m_buffer+pos;
}

//// KDataBufferImpl

KDataBufferImpl::KDataBufferImpl(size_t capacity)
{
	char* buffer = (char*)malloc_k(capacity); ASSERT(buffer);
	KDataBuffer::attach(buffer, capacity);
}

KDataBufferImpl::~KDataBufferImpl()
{
	if(m_buffer) free_k(m_buffer);
}

}}