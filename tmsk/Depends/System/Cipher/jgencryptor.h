#ifndef _JG_ENCRYPTOR_H_
#define _JG_ENCRYPTOR_H_

#include "../KPlatform.h"
#include "../Misc/KStream.h"

class TFEncContext
{
public:
	enum
	{
		header_base = 0x5858e1d8,
		max_header_magic = header_base+128,
	};

public:
	TFEncContext();
	TFEncContext(DWORD magic);
	~TFEncContext();

public:
	static BOOL IsAccept(DWORD magic);

public:
	int m_varRate;		// 变化率
};

class TFEncryptor
{
public:
	TFEncryptor();
	~TFEncryptor();

public:
	static TFEncryptor& getInstance();

public:
	void encrypt(TFEncContext& ctx, void* data, int len, int seq);
	void decrypt(TFEncContext& ctx, void* data, int len, int seq);

public:
	//DWORD m_magic;
	unsigned char m_etable[256];
	unsigned char m_dtable[256];
};

class JgEncStream : public StreamInterface
{
public:
	JgEncStream();
	~JgEncStream();

public:
	BOOL setInputStream(StreamInterface* stream);
	virtual BOOL setOutputStream(StreamInterface* stream, int varRate);

public:
	int_r ReadData(void* data, size_t length);
	int_r WriteData(const void* data, size_t length);
	BOOL Seek(size_t pos);
	size_t tell() const;
	void Close();
	void flush();

public:
	BOOL m_encMode;
	StreamInterface* m_stream;
	TFEncryptor& m_encryptor;
	TFEncContext m_context;
};

class JgDecStream : public JgEncStream
{
public:
	BOOL setOutputStream(StreamInterface* stream, int varRate);
	int_r WriteData(const void* data, size_t length);
};

class JgEncInputMemeryStream : public StreamInterface
{
public:
	JgEncInputMemeryStream(const void* str, int len);
	~JgEncInputMemeryStream();

public:
	int_r ReadData(void* data, size_t length);
	int_r WriteData(const void* data, size_t length);
	BOOL  Seek(size_t pos);
	size_t tell() const;
	void  Close();
	int getString(const char*& str);
	void flush() { }

public:
	BOOL m_cstr;
	KMsgInputStream m_si;
	KMsgOutputStream m_so;
	JgEncStream m_encStream;
};

#endif