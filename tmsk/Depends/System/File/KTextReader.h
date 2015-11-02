#pragma once
#include "../KType.h"
#include "../KMacro.h"
#include "../KPlatform.h"
#include "../Misc/KStream.h"
#include "../Misc/StrUtil.h"
#include "../Memory/KBuffer64k.h"

namespace System { namespace File {

// 读取文本内容，实现每次读取一行的功能
class KTextReader
{
friend class KIniFileReader;
friend class KColumnFileReader;
public:
	enum
	{
		m_bufSize = 10240+2048,
		m_maxLineSize=4096
	};

public:
	KTextReader();
	virtual ~KTextReader();
	bool good() const { return m_si != NULL; }
	void Clear();

public:
	BOOL SetInputStream(StreamInterface& si);

public:
	void Reset();
	BOOL eof() const;
	int ReadLine(char*& ptr, const char* delimiters);
	int ReadLine(char* buf, int len);
	int ReadLine(char* buf, int len, const char* delimiters);

protected:
	void CompactBuffer();
	BOOL ReadBuffer();
	int _transDelimiters_be(const char* seps, wchar_16* delimiters, int len);
	int _transDelimiters_le(const char* seps, wchar_16* delimiters, int len);
	int _readLine_mb(char*& ptr, const char* delimiters);
	int _readLine_utf16be(wchar_16*& ptr, const char* delimiters);
	int _readLine_utf16le(wchar_16*& ptr, const char* delimiters);

public:
	int  m_rp;
	int  m_wp;
	char m_buffer[m_bufSize];
	int m_orgStreamOffset;
	BOOL m_all_in_memory; BOOL m_eof;
	StreamInterface* m_si;
	EnumEncoding m_encode;
};

// 存在于内存中的文本内容读取
class KTextMeassgeReader : public KTextReader
{
friend class KIniFileReader;
friend class KColumnFileReader;
protected:
	KTextMeassgeReader();
	BOOL Attach(const char* msg, size_t len);

public:
	KTextMeassgeReader(const char* msg, size_t len);
	~KTextMeassgeReader();

public:
	KMsgInputStream m_siObj;
};

// 存在于文本文件中的文本内容读取
class KTextFileReader : public KTextReader
{
public:
	KTextFileReader();
	KTextFileReader(const char* filename);
	~KTextFileReader();
	bool open(const char* fileName, const char* mode);

public:
	KInputFileStream m_siObj;
};

//
// INI 格式的内容读取
// 实现两个功能: 
// 1. 取 secion.key 的内容
// 2. 取一个 section 中的整个内容
//
class KIniReader
{
	enum {m_maxLineSize = KTextReader::m_maxLineSize};

public:
	KIniReader();
	virtual ~KIniReader();
	void Clear();

public:
	BOOL SetInputStream(StreamInterface& si);

public:
	int  GetSections(char* secNames[], int count, KBuffer64k* pBuffer);
	BOOL GetInt(const char* sec, const char* keyName, int& val);
	BOOL GetInt(const char* sec, const char* keyName, int defVal, int& val);
	BOOL GetFloat(const char* sec, const char* keyName, float& val);
	BOOL GetFloat(const char* sec, const char* keyName, float defVal, float& val);
	BOOL GetUint(const char* sec, const char* keyName, DWORD& val);
	BOOL GetUint(const char* sec, const char* keyName, DWORD defVal, DWORD& val);
	BOOL GetInt64(const char* sec, const char* keyName, INT64& val);
	BOOL GetInt64(const char* sec, const char* keyName, INT64 defVal, INT64& val);
	BOOL GetUint64(const char* sec, const char* keyName, UINT64& val);
	BOOL GetUint64(const char* sec, const char* keyName, UINT64 defVal, UINT64& val);
	BOOL GetString(const char* sec, const char* keyName, char* val, int len);
	BOOL GetString(const char* sec, const char* keyName, const char* defVal, char* val, int len);

	// 每行字符之间用'\0'隔开
	BOOL GetSection(const char* sec, char* val, size_t len, size_t& readed);

	// sep : 每行之间用什么字符隔开
	BOOL GetSection(const char* sec, char* val, size_t len, char sep, size_t& readed);

	// 得到Section下面所有的Key
	int GetSectionKeys(const char* sec, ::System::Collections::KString<64>* keys, int count);

protected:
	int ReadLine(char* buf, int len);
	const char* GetSectionName(char* pline, int len);
	BOOL GetKeyValuePair(char* pline, int len, char*& pkey, char*& pval);

public:
	KTextReader m_textReader;
};

// 存在于内存中的 INI 格式信息的读取
class KIniMsgReader : public KIniReader
{
public:
	KIniMsgReader(const char* msg, int len);
	~KIniMsgReader();

public:
	KMsgInputStream m_si;
};

// 存在于文件中的 INI 格式信息的读取
class KIniFileReader : public KIniReader
{
public:
	KIniFileReader(const char* filename);
	~KIniFileReader();

public:
	KInputFileStream m_siFile;
};

// 读取以固定分隔符分开的文本内容
class KColumnReader
{
public:
	enum { m_maxLineSize = KTextReader::m_maxLineSize };

public:
	KColumnReader();
	virtual ~KColumnReader();
	void Clear();

public:
	BOOL SetInputStream(StreamInterface& si);

public:
	int ReadLine(char*& pline);
	int ReadLine(char**& pCols, char delimiter);
	int ReadLine(char**& pCols, const char* delimiters);
	
public:
	KStringList m_sl;
	char m_line[m_maxLineSize];
	KTextReader m_textReader;
};

// 读取内存中的以固定分隔符分开的文本内容
class KColumnMsgReader : public KColumnReader
{
public:
	KColumnMsgReader(const char* msg, int len);
	~KColumnMsgReader();

public:
	KMsgInputStream m_si;
};

// 读取文件中的以固定分隔符分开的文本内容
class KColumnFileReader : public KColumnReader
{
public:
	KColumnFileReader(const char* filename);
	~KColumnFileReader();

public:
	KInputFileStream m_siFile;
};

};};