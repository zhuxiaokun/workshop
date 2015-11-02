#pragma once
#include "RingBuffer.h"

namespace System { namespace Memory {

class KDataBuffer
{
private:
	KDataBuffer(const KDataBuffer&);
	KDataBuffer& operator = (const KDataBuffer&);

public:
	char* m_buffer;
	size_t m_capacity;
	size_t m_size;

public:
	KDataBuffer();
	~KDataBuffer();

public:
	bool attach(void* buffer, size_t capacity);
	bool attach(void* buffer, size_t capacity, size_t dataSize);
	void clear();

public:
	const void* at(size_t pos) const;
	void erase(size_t pos, size_t count);
	void erase(const void* p, size_t count);
	bool insert(size_t pos, const void* p, size_t len);
	bool push_back(const void* p, size_t len);
	bool seek(size_t pos);
	size_t size() const { return m_size; }

public:
	void* _at(size_t pos);
	void* _begin();
	void* _end();
	size_t _avail() const;
	void* _dataPtr(size_t pos);
};

class KDataBufferImpl : public KDataBuffer
{
private:
	KDataBufferImpl();
	KDataBufferImpl(const KDataBufferImpl&);
	KDataBufferImpl& operator = (const KDataBufferImpl&);

public:
	KDataBufferImpl(size_t capacity);
	~KDataBufferImpl();
};

template <size_t capacity> class KDataBufferTmpl : public KDataBuffer
{
public:
	char m_buff[capacity];
	KDataBufferTmpl()
	{
		KDataBuffer::attach(&m_buff, capacity);
	}
};

}}