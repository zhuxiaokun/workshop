#include "jgencryptor.h"

TFEncContext::TFEncContext():m_varRate(128)
{

}

TFEncContext::TFEncContext(DWORD magic)
	: m_varRate(magic-(DWORD)header_base)
{

}

TFEncContext::~TFEncContext()
{

}

BOOL TFEncContext::IsAccept(DWORD magic)
{
	return magic >= header_base && magic <= max_header_magic;
}

TFEncryptor::TFEncryptor()
{
	memset(&m_etable, 0, sizeof(m_etable));
	memset(&m_dtable, 0, sizeof(m_dtable));
	for(int i=0; i<128; i++)
	{
		m_etable[i] = 127 - i;
		m_dtable[m_etable[i]] = i;
	}
	for(int i=128; i<256; i++)
	{
		m_etable[i] = 383 - i;
		m_dtable[m_etable[i]] = i;
	}
}

TFEncryptor::~TFEncryptor()
{

}

TFEncryptor& TFEncryptor::getInstance()
{
	static TFEncryptor inst;
	return inst;
}

void TFEncryptor::encrypt(TFEncContext& ctx, void* data, int len, int seq)
{
	for(int i=0; i<len; i++)
	{
		BYTE bySeq = (BYTE)(seq % ctx.m_varRate);
		seq++;

		BYTE k = ((BYTE*)data)[i];
		if(k < 128)
		{
			k = (k+bySeq) & 0x7f;
		}
		else
		{
			k = ((k+bySeq-128) & 0x7f) + 128;
		}
		((BYTE*)data)[i] = m_etable[k];
	}

	return;
}

void TFEncryptor::decrypt(TFEncContext& ctx, void* data, int len, int seq)
{
	for(int i=0; i<len; i++)
	{
		BYTE bySeq = (BYTE)(seq % ctx.m_varRate);
		seq++;

		BYTE k = ((BYTE*)data)[i];
		BYTE c = m_dtable[k];

		if(k < 128)
		{
			c = (c-bySeq+128) & 0x7f;
		}
		else
		{
			c = ((c-bySeq+128) & 0x7f) + 128;
		}
		((BYTE*)data)[i] = c;
	}

	return;
}

JgEncStream::JgEncStream()
	: m_encMode(FALSE)
	, m_stream(NULL)
	, m_encryptor(TFEncryptor::getInstance())
{

}

JgEncStream::~JgEncStream()
{
	this->Close();
}

BOOL JgEncStream::setInputStream(StreamInterface* stream)
{
	if(m_stream != stream)
	{
		this->Close();
	}

	size_t pos = stream->tell();
	if(!stream->Seek(0))
		return FALSE;

	DWORD header = 0;
	size_t n = stream->ReadData(&header, sizeof(DWORD));
	if(n == sizeof(DWORD) && TFEncContext::IsAccept(header))
	{
		m_encMode = TRUE;
		m_context.m_varRate = header - TFEncContext::header_base;
	}

	if(m_encMode)
	{
		if(pos)
		{	// 如果文件指针不在文件头，那么不能停留在文件标记的中间	
			ASSERT(pos >= sizeof(DWORD));
			stream->Seek(pos);
		}
	}
	else
	{
		stream->Seek(pos);
	}
	
	m_stream = stream;
	return TRUE;
}

BOOL JgEncStream::setOutputStream(StreamInterface* stream, int varRate)
{
	// 最多128
	varRate &= 0x7f;
	m_context.m_varRate = varRate;

	this->Close();

	size_t pos = stream->tell();

	if(varRate)
	{
		m_encMode = TRUE;
		DWORD magic = TFEncContext::header_base + varRate;

		if(!pos)
		{	// 如果是在文件头位置，那么写入文件加密标志
			stream->WriteData(&magic, sizeof(DWORD));
		}
		else
		{	// 如果文件指针不在文件头，那么不能停留在标记中
			ASSERT(pos >= sizeof(DWORD));
		}
	}
	else
	{
		m_encMode = FALSE;
	}

	m_stream = stream;
	return TRUE;
}

int_r JgEncStream::ReadData(void* data, size_t length)
{
	ASSERT(m_stream);
	size_t n = m_stream->ReadData(data, length);
	if(m_encMode)
	{
		size_t seq = this->tell() - n;
		m_encryptor.decrypt(m_context, data, (int)n, (int)seq);
	}

	return n;
}

int_r JgEncStream::WriteData(const void* data, size_t length)
{
	ASSERT(m_stream);
	if(m_encMode)
	{
		int pos = (int)this->tell();
		m_encryptor.encrypt(m_context, (void*)data, (int)length, pos);
	}
	return m_stream->WriteData(data, length);
}

BOOL JgEncStream::Seek(size_t pos)
{
	ASSERT(m_stream);
	if(m_encMode)
	{
		pos += sizeof(DWORD);
	}
	return m_stream->Seek(pos);
}

size_t JgEncStream::tell() const
{
	ASSERT(m_stream);
	size_t pos = m_stream->tell();
	if(m_encMode)
	{
		if(pos < sizeof(DWORD))
		{
			pos = 0;
		}
		else
		{
			pos -= sizeof(DWORD);
		}
	}
	return pos;
}

void JgEncStream::Close()
{
	if(m_stream)
	{
		m_stream->Close();
		m_stream = NULL;
	}
	m_encMode = FALSE;
}

void JgEncStream::flush()
{
	if(m_stream) m_stream->flush();
}

BOOL JgDecStream::setOutputStream(StreamInterface* stream, int varRate)
{
	// 最多128
	varRate &= 0x7f;
	m_context.m_varRate = varRate;

	this->Close();

	size_t pos = stream->tell();

	if(varRate)
	{
		m_encMode = TRUE;
	}
	else
	{
		m_encMode = FALSE;
	}

	m_stream = stream;
	return TRUE;
}

int_r JgDecStream::WriteData(const void* data, size_t length)
{
	ASSERT(m_stream);
	if(m_encMode)
	{
		int pos = (int)this->tell();
		m_encryptor.decrypt(m_context, (void*)data, (int)length, pos);
	}
	return m_stream->WriteData(data, length);
}

JgEncInputMemeryStream::JgEncInputMemeryStream(const void* str, int len):m_cstr(FALSE)
{
	m_si.Attach((void*)str, len);
	m_encStream.setInputStream(&m_si);
	//m_cstr = ((char*)str) == '\0';
}

JgEncInputMemeryStream::~JgEncInputMemeryStream()
{

}

int_r JgEncInputMemeryStream::ReadData(void* data, size_t length)
{
	return m_encStream.ReadData(data, length);
}

int_r JgEncInputMemeryStream::WriteData(const void* data, size_t length)
{
	ASSERT(FALSE);
	return 0;
}

BOOL JgEncInputMemeryStream::Seek(size_t pos)
{
	return m_encStream.Seek(pos);
}

size_t JgEncInputMemeryStream::tell() const
{
	return m_encStream.tell();
}

void JgEncInputMemeryStream::Close()
{
	m_encStream.Close();
}

int JgEncInputMemeryStream::getString(const char*& str)
{
	if(!m_encStream.m_encMode)
	{
		str = m_si.m_pBuf;
		return (int)m_si.m_bufLength;
	}

	m_so.Seek(0);

	size_t srclen = m_si.m_bufLength + 1;
	if(m_so.m_bufLength < srclen)
	{
		int olen = (srclen+1023) & (~1023);
		m_so.Resize(olen);
	}

	char buf[1024];
	while(TRUE)
	{
		size_t n = m_encStream.ReadData(buf, 1024);
		if(n < 1) break;
		m_so.WriteData(buf, n);
	}

	size_t length = m_so.m_pos;

	// 不管是不是C字符串，在末尾加空结束符
	m_so.WriteByte('\0');
	
	str = m_so.m_pBuf;
	return (int)length;
}