#ifndef _K_STREAM_H_
#define _K_STREAM_H_

#include "../KType.h"
#include <stdio.h>
#include "../KPlatform.h"
#include "./StreamInterface.h"

class KMemoryStream : public StreamInterface
{
public:
	KMemoryStream();
	virtual ~KMemoryStream() {}

public:
	void bind(void* p, size_t len);
	void clear() { m_pos = 0; }

public:
	virtual int_r ReadData(void* value, size_t length);
	virtual int_r WriteData(const void* value, size_t length);
	virtual BOOL CheckBuffer(size_t wantedBytes);

public:
	BOOL Insert(size_t pos, const void* pData, size_t len);
	BOOL Remove(size_t pos, size_t len);

public:
	BOOL Seek(size_t pos);
	size_t tell() const { return m_pos; }
	void Close();
	void flush();

public:
	const void* data() const { return m_pBuf; }
	size_t size() const { return m_pos; }

protected:
	inline BOOL _chk(size_t len){ return m_pos + len <= m_bufLength; }
	inline char* _rp()			{ return m_pBuf + m_pos; }
	inline char* _wp()			{ return m_pBuf + m_pos; }

public:
	inline BOOL ReadChar(char& value)
	{
		if(!_chk(sizeof(value))) return FALSE;
		value = *(char*)_rp(); m_pos += sizeof(value);
		return TRUE;
	}
	inline BOOL ReadByte(BYTE& value)
	{
		if(!_chk(sizeof(value))) return FALSE;
		value = *(BYTE*)_rp(); m_pos += sizeof(value);
		return TRUE;
	}
	inline BOOL ReadShort(short& value)
	{
		if(!_chk(sizeof(value))) return FALSE;
		value = *(short*)_rp(); m_pos += sizeof(value);
		return TRUE;
	}
	inline BOOL ReadWord(WORD& value)
	{
		if(!_chk(sizeof(value))) return FALSE;
		value = *(WORD*)_rp(); m_pos += sizeof(value);
		return TRUE;
	}
	inline BOOL ReadInt(int& value)
	{
		if(!_chk(sizeof(value))) return FALSE;
		value = *(int*)_rp(); m_pos += sizeof(value);
		return TRUE;
	}
	inline BOOL ReadDword(DWORD& value)
	{
		if(!_chk(sizeof(value))) return FALSE;
		value = *(DWORD*)_rp(); m_pos += sizeof(value);
		return TRUE;
	}
	inline BOOL ReadInt64(INT64& value)
	{
		if(!_chk(sizeof(value))) return FALSE;
		value = *(INT64*)_rp(); m_pos += sizeof(value);
		return TRUE;
	}
	inline BOOL ReadUint64(UINT64& value)
	{
		if(!_chk(sizeof(value))) return FALSE;
		value = *(UINT64*)_rp(); m_pos += sizeof(value);
		return TRUE;
	}
	inline BOOL ReadFloat(float& value)
	{
		if(!_chk(sizeof(value))) return FALSE;
		value = *(float*)_rp(); m_pos += sizeof(value);
		return TRUE;
	}
	inline BOOL ReadDouble(double& value)
	{
		if(!_chk(sizeof(value))) return FALSE;
		value = *(double*)_rp(); m_pos += sizeof(value);
		return TRUE;
	}
	
	BOOL ReadByteArray(void* value, size_t length);
	BOOL ReadString(char* value, size_t bufLength);
	BOOL ReadString8(char* value, size_t bufLength);
	BOOL ReadString32(char* value, size_t bufLength);

	template<typename T> BOOL Read(char& value)  { return this->ReadChar(value);  }
	template<typename T> BOOL Read(BYTE& value)  { return this->ReadByte(value);  }
	template<typename T> BOOL Read(short& value) { return this->ReadShort(value); }
	template<typename T> BOOL Read(int& value)   { return this->ReadInt(value);   }
	template<typename T> BOOL Read(DWORD& value) { return this->ReadDword(value); }
	template<typename T> BOOL Read(INT64& value) { return this->ReadInt64(value); }
	template<typename T> BOOL Read(UINT64& value){ return this->ReadUint64(value);}
	template<typename T> BOOL Read(float& value) { return this->ReadFloat(value); }
	template<typename T> BOOL Read(double& value){ return this->ReadDouble(value);}

	template<typename T> BOOL Read(T& value)
	{
		return ReadByteArray(&value, sizeof(T));
	}

public:
	inline BOOL WriteChar(char value)
	{
		if(!this->CheckBuffer(sizeof(value))) return FALSE;
		*(char*)_wp() = value; m_pos += sizeof(value);
		return TRUE;
	}
	inline BOOL WriteByte(BYTE value)
	{
		if(!this->CheckBuffer(sizeof(value))) return FALSE;
		*(BYTE*)_wp() = value; m_pos += sizeof(value);
		return TRUE;
	}
	inline BOOL WriteShort(short value)
	{
		if(!this->CheckBuffer(sizeof(value))) return FALSE;
		*(short*)_wp() = value; m_pos += sizeof(value);
		return TRUE;
	}
	inline BOOL WriteWord(WORD value)
	{
		if(!this->CheckBuffer(sizeof(value))) return FALSE;
		*(WORD*)_wp() = value; m_pos += sizeof(value);
		return TRUE;
	}
	inline BOOL WriteInt(int value)
	{
		if(!this->CheckBuffer(sizeof(value))) return FALSE;
		*(int*)_wp() = value; m_pos += sizeof(value);
		return TRUE;
	}
	inline BOOL WriteDword(DWORD value)
	{
		if(!this->CheckBuffer(sizeof(value))) return FALSE;
		*(DWORD*)_wp() = value; m_pos += sizeof(value);
		return TRUE;
	}
	inline BOOL WriteFloat(float value)
	{
		if(!this->CheckBuffer(sizeof(value))) return FALSE;
		*(float*)_wp() = value; m_pos += sizeof(value);
		return TRUE;
	}
	inline BOOL WriteDouble(double value)
	{
		if(!this->CheckBuffer(sizeof(value))) return FALSE;
		*(double*)_wp() = value; m_pos += sizeof(value);
		return TRUE;
	}
	inline BOOL WriteInt64(INT64 value)
	{
		if(!this->CheckBuffer(sizeof(value))) return FALSE;
		*(INT64*)_wp() = value; m_pos += sizeof(value);
		return TRUE;
	}
	inline BOOL WriteUint64(UINT64 value)
	{
		if(!this->CheckBuffer(sizeof(value))) return FALSE;
		*(UINT64*)_wp() = value; m_pos += sizeof(value);
		return TRUE;
	}

	BOOL WriteByteArray(const void* value, size_t length);
	BOOL WriteString(const char* value, size_t length=0);
	BOOL WriteString8(const char* value, size_t length=0);
	BOOL WriteString32(const char* value, size_t length=0);

	template<typename T> BOOL Write(char value)  { return this->WriteChar(value);  }
	template<typename T> BOOL Write(BYTE value)  { return this->WriteByte(value);  }
	template<typename T> BOOL Write(short value) { return this->WriteShort(value); }
	template<typename T> BOOL Write(int value)   { return this->WriteInt(value);   }
	template<typename T> BOOL Write(DWORD value) { return this->WriteDword(value); }
	template<typename T> BOOL Write(INT64 value) { return this->WriteInt64(value); }
	template<typename T> BOOL Write(UINT64 value){ return this->WriteUint64(value);}
	template<typename T> BOOL Write(float value) { return this->WriteFloat(value); }
	template<typename T> BOOL Write(double value){ return this->WriteDouble(value);}

	template<typename T> BOOL Write(const T& value)
	{
		return WriteByteArray(&value, sizeof(T));
	}
	// 增加个简单类型的 >> by Allen, 20121107
	template<typename T> KMemoryStream& operator>>(T& o)
	{
		Read(o);
		return *this;
	}
	template<typename T> KMemoryStream& operator<<(const T& o)
	{
		Write(o);
		return *this;
	}

public:
	size_t m_pos;
	char* m_pBuf;
	size_t m_bufLength;
};

class KMsgInputStream : public KMemoryStream
{
public:
	KMsgInputStream() {}
	KMsgInputStream(const void* buf, size_t length)
	{
		m_pBuf = (char*)buf;
		m_bufLength = length;
		m_pos = 0;
	}
	void Attach(const void* buf, size_t length)
	{
		m_pBuf = (char*)buf;
		m_bufLength = length;
		m_pos = 0;
	}
	const void* GetCurrentHead() const
	{
		return &m_pBuf[m_pos];
	}
	size_t GetDataSizeLeft() const
	{
		if (m_pos >= m_bufLength) return 0;
		return m_bufLength - m_pos;
	}
};

class KMsgOutputStream : public KMemoryStream
{
public:
	KMsgOutputStream();
	~KMsgOutputStream();
	void Close();
	BOOL CheckBuffer(size_t wantedBytes);
	BOOL Resize(size_t length);

protected:
	BOOL SetLength(size_t length);
};

class KInputFileStream : public StreamInterface
{
public:
	KInputFileStream();
	KInputFileStream(FILE* f, bool own=false);
	KInputFileStream(const char* filename);
	virtual ~KInputFileStream();

public:
	bool attach(FILE* filep) { ASSERT(filep); m_pfile = filep; return true; }
	void detach() { m_pfile = NULL; }
	
public:
	void Close();
	BOOL Open(const char* filename);
	BOOL Open(const char* filename, const char* mode);
	BOOL Good() const;
	int_r ReadData(void* value, size_t length);
	int_r WriteData(const void* value, size_t length);
	BOOL Seek(size_t pos);
	size_t tell() const;
	void flush();

public:
	bool m_own;
	FILE* m_pfile;
};

class KOutputFileStream : public StreamInterface
{
public:
	KOutputFileStream();
	KOutputFileStream(FILE* f, bool own=false);
	KOutputFileStream(const char* filename, const char* mode);
	virtual ~KOutputFileStream();

public:
	bool attach(FILE* filep) { ASSERT(filep); m_pfile = filep; return true; }
	void detach() { m_pfile = NULL; }

public:
	void Close();
	BOOL Open(const char* filename, const char* mode);
	BOOL OpenForce(const char* filename, const char* mode);
	BOOL Good() const;
	int_r ReadData(void* value, size_t length);
	int_r WriteData(const void* value, size_t length);
	BOOL Seek(size_t pos);
	size_t tell() const;
	void flush();

public:
	bool m_own;
	FILE* m_pfile;
};

class KDataOutputStream : public KMemoryStream
{
public:
	KDataOutputStream(void* data, size_t len)
	{
		m_pBuf = (char*)data;
		m_pos = 0;
		m_bufLength = len;
	}
};

template <size_t capacity> class KBufferOutputStream : public KMemoryStream
{
public:
	char m_buf[capacity];
	KBufferOutputStream()
	{
		m_pos = 0;
		m_pBuf = (char*)&m_buf;
		m_bufLength = capacity;
	}
};

#endif