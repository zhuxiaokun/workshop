#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include "../KPlatform.h"

template <size_t capacity> class KPieceBuffer
{
public:
	size_t m_rp;
	size_t m_wp;
	char m_buffer[capacity];

	KPieceBuffer():m_rp(0),m_wp(0)
	{
	}
	size_t Capacity() const
	{
		return capacity;
	}
	int_r ReadAvail() const
	{
		return (int_r)(m_wp - m_rp);
	}
	int_r WriteAvail() const
	{
		return (int_r)(capacity - m_wp);
	}
	size_t Read(void* pData, size_t want)
	{
		int_r ravail = this->ReadAvail();
		if(ravail < 1) return 0;
		if(ravail < (int)want) want = ravail;
		memcpy(pData, &m_buffer[m_rp], want);
		m_rp += want;
		return want;
	}
	size_t Read(size_t& rp, void* pData, size_t want)
	{
		int_r ravail = m_wp - rp;
		if(ravail < 1) return 0;
		if(ravail < (int)want) want = ravail;
		memcpy(pData, &m_buffer[rp], want);
		rp += want;
		return want;
	}
	size_t Write(const void* pData, size_t len)
	{
		int_r wavail = this->WriteAvail();
		if(wavail < 1) return 0;
		if(wavail < (int_r)len) len = wavail;
		memcpy(&m_buffer[m_wp], pData, len);
		m_wp += len;
		return len;
	}
	const char* GetBuffer() const
	{
		return &m_buffer[0];
	}
	const char* GetReadBuffer() const
	{
		return &m_buffer[m_rp];
	}
	char* GetWriteBuffer()
	{
		return &m_buffer[m_wp];
	}
	void Compact()
	{
		if(m_rp)
		{
			if(m_wp > m_rp)
			{
				memmove(&m_buffer[0], &m_buffer[m_rp], m_wp-m_rp);
				m_wp = m_wp - m_rp;
				m_rp = 0;
			}
			else
			{
				m_rp = m_wp = 0;
			}
		}
	}
	void Reset()
	{
		m_rp = m_wp = 0;
	}
};

template <class ValueType> class RingBuffer
{
public:
	RingBuffer()
	{
		m_bufLength = 0;
		m_dataBegPos = 0;
		m_dataLength = 0;
		m_pBuf = NULL;
	}
	RingBuffer(size_t capacity)
	{
		m_bufLength = capacity;
		m_dataBegPos = 0;
		m_dataLength = 0;
		m_pBuf = new ValueType[m_bufLength];
		if(!m_pBuf) throw 1;
	}
	~RingBuffer()
	{
		if(m_pBuf)
		{
			delete [] m_pBuf;
			m_pBuf = NULL;
		}
	}
public:
	BOOL SetCapacity(int capacity)
	{
		if(m_bufLength > 0) return FALSE;
		m_bufLength = capacity;
		m_pBuf = new ValueType[m_bufLength];
		return (m_pBuf != NULL);
	}
	int_r ReadAvailable() const
	{
		return (int_r)m_dataLength;
	}
	int_r WriteAvailable() const
	{
		return (int_r)(m_bufLength - m_dataLength);
	}
	BOOL Read(size_t length, ValueType* pOut)
	{
		if(this->ReadAvailable() < (int_r)length)
			return FALSE;
		if(m_dataBegPos + length <= m_bufLength)
		{
			size_t nBytes = length * sizeof(ValueType);
			memcpy(pOut, m_pBuf+m_dataBegPos, nBytes);
			m_dataBegPos += length;
			m_dataLength -= length;
			if(m_dataBegPos == m_bufLength)
				m_dataBegPos = 0;
		}
		else
		{//分两段读
			size_t num = m_bufLength - m_dataBegPos;
			this->Read(num, pOut);
			return this->Read(length-num, pOut+num);
		}
		return TRUE;
	}
	BOOL Write(const ValueType* pData, size_t length)
	{
		if(this->WriteAvailable() < (int_r)length)
			return FALSE;
		size_t begPos = m_dataBegPos + m_dataLength;
		if(begPos >= m_bufLength) begPos -= m_bufLength;
		if(begPos+length <= m_bufLength)
		{
			size_t nBytes = length * sizeof(ValueType);
			memcpy(m_pBuf+begPos, pData, nBytes);
			m_dataLength += length;
		}
		else
		{
			size_t num = m_bufLength - begPos;
			this->Write(pData, num);
			return this->Write(pData+num,length-num);
		}
		return TRUE;
	}
	void Reset()
	{
		m_dataBegPos = 0;
		m_dataLength = 0;
	}

public:
	ValueType* m_pBuf;
	size_t m_bufLength;
	size_t m_dataBegPos;
	size_t m_dataLength;
};

//
// 与KPieceBuffer类似，只是可以附着到外部提供的内存上面
// pos_type maybe : int, DWORD, short, WORD
//
template <typename pos_type> class KDockPieceBuffer
{
public:
	pos_type* m_rp;
	pos_type* m_wp;
	pos_type m_r;
	pos_type m_w;
	int m_capacity;
	char* m_p;

public:
	KDockPieceBuffer():m_rp(&m_r),m_wp(&m_w),m_r(0),m_w(0),m_capacity(0),m_p(NULL)
	{

	}
	~KDockPieceBuffer()
	{

	}

public:
	void Attach(void* p, int capacity)
	{
		m_p = (char*)p;
		m_capacity = capacity;
		m_r = m_w = 0;
		m_rp = &m_r; m_wp = &m_w;
	}
	void Attach(void* p, int capacity, int r, int w)
	{
		m_p = (char*)p;
		m_capacity = capacity;
		m_r = r; m_w = w;
		m_rp = &m_r; m_wp = &m_w;
	}
	void Attach(void* p, int capacity, pos_type* rp, pos_type* wp)
	{
		m_p = (char*)p;
		m_capacity = capacity;
		m_rp = rp; m_wp = wp;
	}

public:
	int_r ReadAvail() const
	{
		return *m_wp - *m_rp;
	}
	int_r WriteAvail() const
	{
		return m_capacity - *m_wp;
	}
	void* WriteBuffer()
	{
		return m_p + *m_wp;
	}
	const void* ReadBuffer()
	{
		return m_p + *m_rp;
	}
	size_t Read(void* p, int want)
	{
		int_r ravail = this->ReadAvail();
		if(ravail < 1) return 0;
		if(ravail < want) want = ravail;
		memcpy(p, m_p + *m_rp, want);
		*m_rp += want;
		return want;
	}
	size_t Peek(void* p, int want)
	{
		size_t r = *m_rp;
		size_t n = this->Read(p, want);
		*m_rp = r;
		return n;
	}
	size_t Write(const void* p, size_t len)
	{
		int_r wavail = this->WriteAvail();
		if(wavail < 1) return 0;
		if(wavail < (int_r)len) len = wavail;
		memcpy(m_p + *m_wp, p, len);
		*m_wp += len;
		return len;
	}
	void Reset()
	{
		m_r = m_w = 0;
		m_rp = &m_r; m_wp = &m_w;
	}
	void Compact()
	{
		size_t r = *m_rp;
		size_t w = *m_wp;
		if(r == w)
		{
			*m_rp = *m_wp = 0;
			return;
		}

		void* src = m_p + r;
		void* dst = m_p;
		size_t len = w - r;
		
		if(len > 0)
		{
			memmove(dst, src, len);
		}

		*m_rp = 0;
		*m_wp = len;

		return;
	}
};

template <typename pos_type> class KDockRingBuffer
{
public:
	pos_type* m_rp;
	pos_type* m_lp;
	pos_type m_r;
	pos_type m_len;
	size_t m_capacity;
	char* m_p;

public:
	KDockRingBuffer():m_rp(&m_r),m_lp(&m_len),m_r(0),m_len(0),m_capacity(0),m_p(NULL)
	{

	}
	~KDockRingBuffer()
	{

	}

public:
	void Reset()
	{
		m_rp = &m_r;
		m_lp = &m_len;
		m_r = 0;
		m_len = 0;
		m_capacity = 0;
		m_p = NULL;
	}
	void Clear()
	{
		*m_rp = *m_lp = 0;
	}
	void Attach(void* p, size_t capacity)
	{
		m_p = (char*)p;
		m_capacity = capacity;
		m_r = 0; m_len = 0;
		m_rp = &m_r; m_lp = &m_len;
	}
	void Attach(void* p, size_t capacity, size_t r, size_t len)
	{
		m_p = (char*)p;
		m_capacity = capacity;
		m_r = r; m_len = len;
		m_rp = &m_r; m_lp = &m_len;
	}
	void Attach(void* p, size_t capacity, pos_type* rp, pos_type* lp)
	{
		m_p = (char*)p;
		m_capacity = capacity;
		m_rp = rp; m_lp = lp;
	}

public:
	int_r ReadAvail() const
	{
		return (int_r)*m_lp;
	}
	int_r WriteAvail() const
	{
		return (int_r)(m_capacity - *m_lp);
	}
	size_t Read(void* p, size_t want)
	{
		int_r avail = this->ReadAvail();
		if((int_r)want > avail) want = avail;

		if(*m_rp + want <= m_capacity)
		{
			memcpy(p, m_p + *m_rp, want);
			
			*m_rp += want;
			*m_lp -= want;

			if(*m_rp == m_capacity)
			{
				*m_rp = 0;
			}
			
			return want;
		}
		else
		{
			//分两段读
			size_t num = m_capacity - *m_rp;
			size_t nRet = this->Read(p, num);
			ASSERT(nRet == num);
			return nRet + this->Read((char*)p+num, want-num);
		}
	}
	size_t Peek(void* p, size_t want)
	{
		size_t r = (size_t)*m_rp;
		size_t len = (size_t)*m_lp;

		size_t n = this->Read(p, want);
		
		*m_rp = r;
		*m_lp = len;
		
		return n;
	}
	size_t Write(const void* p, size_t len)
	{
		int_r avail = this->WriteAvail();
		if((int_r)len > avail) return 0;
		
		size_t w = *m_rp + *m_lp;
		if(w >= m_capacity) w -= m_capacity;

		if(w + len <= m_capacity)
		{
			memcpy(m_p+w, p, len);
			*m_lp += len;
			return len;
		}
		else
		{
			size_t num = m_capacity - w;
			size_t nRet = this->Write(p, num);
			ASSERT(nRet == num);
			return nRet + this->Write((char*)p+num,len-num);
		}
	}
};

template <typename T,typename pos_type=size_t> class KRingBuffer
	: private KDockRingBuffer<pos_type>
{
public:
	typedef KDockRingBuffer<pos_type> base_type;

public:
	KRingBuffer()
	{

	}
	~KRingBuffer()
	{

	}

public:
	void attach(T* buffer, size_t capacity)
	{
		base_type::Attach(buffer, capacity*sizeof(T));
	}
	void attach(T* buffer, size_t capacity, size_t size)
	{
		base_type::Attach(buffer, capacity*sizeof(T), 0, sizeof(T)*size);
	}
	void attach(T* buffer, size_t capacity, pos_type* rp, pos_type* lp)
	{
		base_type::Attach(buffer, capacity*sizeof(T), *rp*sizeof(T), *lp*sizeof(T));
	}
	void attach(void* buffer, size_t capacity, pos_type* rp, pos_type* lp)
	{
		base_type::Attach(buffer, capacity, rp, lp);
	}

public:
	size_t capacity() const
	{
		return base_type::m_capacity/sizeof(T);
	}

	BOOL empty() const
	{
		return base_type::ReadAvail() < 1;
	}

	BOOL full() const
	{
		return base_type::WriteAvail() < 1;
	}

public:
	size_t size() const
	{
		return base_type::ReadAvail() / sizeof(T);
	}

	T operator [] (int_r idx)
	{
		int_r n = base_type::ReadAvail() / sizeof(T);
		ASSERT(idx >= 0 && idx < n);

		size_t len = idx * sizeof(T);
		size_t offset = *(this->m_rp) + len;
		if(offset >= this->m_capacity)
			offset -= this->m_capacity;

		void * vp = this->m_p + offset;

		T ret;
		memcpy(&ret, vp, sizeof(T));

		//		int r = (int)*m_rp;
		//		int len = (int)*m_lp;
		//
		//		for(int i=0; i<idx; i++) this->read();
		//		T ret = this->read();
		//
		//		*m_rp = r;
		//		*m_lp = len;

		return ret;
	}

public:
	T read()
	{
		T ret;
		size_t n = base_type::Read(&ret, sizeof(T));
		ASSERT(n == sizeof(T));
		return ret;
	}
	KRingBuffer& write(const T& val)
	{
		if(this->full()) this->read();
		size_t n = base_type::Write(&val, sizeof(T));
		ASSERT(n == sizeof(T));
		return *this;
	}
};

template <typename T,size_t Capacity> class KRingBufferTmpl
	: public KRingBuffer<T>
{
public:
	T m_buffer[Capacity];
	KRingBufferTmpl()
	{
		attach( m_buffer, Capacity );
	}
	
};

#endif