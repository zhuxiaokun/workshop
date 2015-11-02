#ifndef __CMD5_H__
#define __CMD5_H__
#include "md5.h"
#include <string>
#include "StrUtil.h"
#include <algorithm>

/*
使用说明：
CMD5 md5;
md5.Update("calvin");
md5.Final();
string a = md5.Format();
*/

// using namespace std;

class CMD5 
{
public:
	CMD5()
	{
		MD5Init(&m_Context);
	}

public:
	void Digest(unsigned char* str, unsigned int len)
	{
		_Digest(str, len);
	}

	void Digest(char* str, unsigned int len)
	{
		_Digest((unsigned char*)str, len);
	}

	void Digest(char* str)
	{
		_Digest((unsigned char*)str, (unsigned int)strlen(str));
	}

	void Update(unsigned char* buf, unsigned int len)
	{
		_Update(buf, len);
	}

	void Update(char* buf, unsigned int len)
	{
		_Update((unsigned char*)buf, len);
	}

	void Update(char* str)
	{
		_Update((unsigned char*)str, (unsigned int)strlen(str));
	}

	void Final()
	{
		_Final();
	}

	char* Format(char str[33])
	{
		return MD5Format(str, m_Val);
	}

	std::string Format()
	{
		char str[33];
		Format(str);
		return str;
	}

	//取内部缓冲区
	const unsigned char* Get()
	{
		return m_Val;
	}

	//写内部缓冲区
	void Set(unsigned char val[16])
	{
		memcpy(m_Val, val, 16);
		MD5Init(&m_Context);	//恢复初始状态
	}

public:
	bool operator< (const CMD5& rhs) const
	{
		return (memcmp(m_Val, rhs.m_Val, 16) < 0);
	}

	operator size_t() const
	{
		return *(size_t*)m_Val;
	}

	CMD5& operator=(const CMD5& rhs)
	{
		memcpy(this->m_Val, rhs.m_Val, 16);
		MD5Init(&m_Context);	//恢复初始状态

		return *this;
	}

protected:
	void _Digest(unsigned char* buf, unsigned int len)
	{
		MD5Init(&m_Context);
		MD5Update(&m_Context, buf, len);
		MD5Final(m_Val, &m_Context);
		MD5Init(&m_Context);	//恢复初始状态
	}

	void _Update(unsigned char* buf, unsigned int len)
	{
		MD5Update(&m_Context, buf, len);
	}

	void _Final()
	{
		MD5Final(m_Val, &m_Context);
		MD5Init(&m_Context);	//恢复初始状态
	}

private:
	unsigned char m_Val[16];
	MD5_CTX m_Context;
};

#endif