#include "KStream.h"
#include <stdlib.h>
#include "../File/KFile.h"
#include <string.h>

KMemoryStream::KMemoryStream():m_pBuf(NULL),m_bufLength(0),m_pos(0)
{
}

int_r KMemoryStream::WriteData(const void* value, size_t length)
{
	if(this->WriteByteArray(value,length)) return length;
	return -1;
}

BOOL KMemoryStream::CheckBuffer(size_t wantedBytes)
{
	return (m_pos + wantedBytes <= m_bufLength);
}

BOOL KMemoryStream::Insert(size_t pos, const void* pData, size_t len)
{
	// must insert in existing data or end of existing data
	if(m_pos < pos) return FALSE;

	// check if there has enough space to hold data with length of len
	if(!this->CheckBuffer(len)) return FALSE;
	
	size_t n = m_pos - pos;
	char* src = m_pBuf + pos;
	char* dst = m_pBuf + pos + len;

	if(n > 0)
	{	
		memmove(dst, src, n);
	}
	
	memcpy(src, pData, len);
	m_pos += len;

	return TRUE;
}

BOOL KMemoryStream::Remove(size_t pos, size_t len)
{
	if(m_pos < pos+len) return FALSE;
	
	char* dst = m_pBuf + pos;
	char* src = dst + len;
	size_t n = m_pos - (pos + len);

	if(n > 0)
	{
		memmove(dst, src, n);
	}

	m_pos -= len;
	return TRUE;
}

BOOL KMemoryStream::Seek(size_t pos)
{
	if(pos > m_bufLength) return FALSE;
	m_pos = pos;
	return TRUE;
}

void KMemoryStream::Close()
{
	return;
}

void KMemoryStream::flush()
{
	return;
}

//BOOL KMemoryStream::ReadInt(int& value)
//{
//	size_t wantedBytes = sizeof(value);
//	if(!this->CheckBuffer(wantedBytes)) return FALSE;
//	memcpy(&value, m_pBuf+m_pos, wantedBytes);
//	m_pos += wantedBytes;
//	return TRUE;
//}

//BOOL KMemoryStream::ReadShort(short& value)
//{
//	size_t wantedBytes = sizeof(value);
//	if(!this->CheckBuffer(wantedBytes)) return FALSE;
//	memcpy(&value, m_pBuf+m_pos, wantedBytes);
//	m_pos += wantedBytes;
//	return TRUE;
//}
//
//BOOL KMemoryStream::ReadByte(BYTE& value)
//{
//	size_t wantedBytes = sizeof(value);
//	if(!this->CheckBuffer(wantedBytes)) return FALSE;
//	memcpy(&value, m_pBuf+m_pos, wantedBytes);
//	m_pos += wantedBytes;
//	return TRUE;
//}
//
//BOOL KMemoryStream::ReadWord(WORD& value)
//{
//	size_t wantedBytes = sizeof(value);
//	if(!this->CheckBuffer(wantedBytes)) return FALSE;
//	memcpy(&value, m_pBuf+m_pos, wantedBytes);
//	m_pos += wantedBytes;
//	return TRUE;
//}
//
//BOOL KMemoryStream::ReadDword(DWORD& value)
//{
//	size_t wantedBytes = sizeof(value);
//	if(!this->CheckBuffer(wantedBytes)) return FALSE;
//	memcpy(&value, m_pBuf+m_pos, wantedBytes);
//	m_pos += wantedBytes;
//	return TRUE;
//}
//
//BOOL KMemoryStream::ReadFloat(float& value)
//{
//	size_t wantedBytes = sizeof(value);
//	if(!this->CheckBuffer(wantedBytes)) return FALSE;
//	memcpy(&value, m_pBuf+m_pos, wantedBytes);
//	m_pos += wantedBytes;
//	return TRUE;
//}
//
//BOOL KMemoryStream::ReadDouble(double& value)
//{
//	size_t wantedBytes = sizeof(value);
//	if(!this->CheckBuffer(wantedBytes)) return FALSE;
//	memcpy(&value, m_pBuf+m_pos, wantedBytes);
//	m_pos += wantedBytes;
//	return TRUE;
//}
//
//BOOL KMemoryStream::ReadInt64(INT64& value)
//{
//	size_t wantedBytes = sizeof(value);
//	if(!this->CheckBuffer(wantedBytes)) return FALSE;
//	memcpy(&value, m_pBuf+m_pos, wantedBytes);
//	m_pos += wantedBytes;
//	return TRUE;
//}
//
//BOOL KMemoryStream::ReadUint64(UINT64& value)
//{
//	size_t wantedBytes = sizeof(value);
//	if(!this->CheckBuffer(wantedBytes)) return FALSE;
//	memcpy(&value, m_pBuf+m_pos, wantedBytes);
//	m_pos += wantedBytes;
//	return TRUE;
//}

BOOL KMemoryStream::ReadByteArray(void* value, size_t length)
{
	size_t wantedBytes = length;
	if(!this->CheckBuffer(wantedBytes)) return FALSE;
	memcpy(value, m_pBuf+m_pos, wantedBytes);
	m_pos += wantedBytes;
	return TRUE;
}

void KMemoryStream::bind(void* p, size_t len)
{
	m_pos = 0;
	m_pBuf = (char*)p;
	m_bufLength = len;
}

int_r KMemoryStream::ReadData(void* value, size_t length)
{
	size_t availLen = m_bufLength - m_pos;
	if(length > availLen) length = availLen;
	if(this->ReadByteArray(value, length))
		return length;
	else
		return -1;
}

BOOL KMemoryStream::ReadString(char* value, size_t bufLength)
{
	WORD length;
	if(!this->ReadWord(length))
		return FALSE;
	if(bufLength < (size_t)(length+1))
		return FALSE;
	
	if(length == 0)
	{
		value[0] = '\0';
		return TRUE;
	}

	if(!this->ReadByteArray((BYTE*)value, length))
		return FALSE;
	
	value[length] = '\0';
	return TRUE;
}

BOOL KMemoryStream::ReadString8(char* value, size_t bufLength)
{
	BYTE length;
	if(!this->ReadByte(length))
		return FALSE;
	if(bufLength < (size_t)(length+1))
		return FALSE;

	if(length == 0)
	{
		value[0] = '\0';
		return TRUE;
	}

	if(!this->ReadByteArray((BYTE*)value, length))
		return FALSE;

	value[length] = '\0';
	return TRUE;
}

BOOL KMemoryStream::ReadString32(char* value, size_t bufLength)
{
	DWORD length;
	if(!this->ReadDword(length))
		return FALSE;
	if(bufLength < (size_t)(length+1))
		return FALSE;

	if(length == 0)
	{
		value[0] = '\0';
		return TRUE;
	}

	if(!this->ReadByteArray((BYTE*)value, length))
		return FALSE;

	value[length] = '\0';
	return TRUE;
}

//BOOL KMemoryStream::WriteInt(int value)
//{
//	size_t wantedBytes = sizeof(value);
//	if(!this->CheckBuffer(wantedBytes)) return FALSE;
//	memcpy(m_pBuf+m_pos, &value, wantedBytes);
//	m_pos += wantedBytes;
//	return TRUE;
//}
//
//BOOL KMemoryStream::WriteShort(short value)
//{
//	size_t wantedBytes = sizeof(value);
//	if(!this->CheckBuffer(wantedBytes)) return FALSE;
//	memcpy(m_pBuf+m_pos, &value, wantedBytes);
//	m_pos += wantedBytes;
//	return TRUE;
//}
//
//BOOL KMemoryStream::WriteByte(BYTE value)
//{
//	size_t wantedBytes = sizeof(value);
//	if(!this->CheckBuffer(wantedBytes)) return FALSE;
//	memcpy(m_pBuf+m_pos, &value, wantedBytes);
//	m_pos += wantedBytes;
//	return TRUE;
//}
//
//BOOL KMemoryStream::WriteWord(WORD value)
//{
//	size_t wantedBytes = sizeof(value);
//	if(!this->CheckBuffer(wantedBytes)) return FALSE;
//	memcpy(m_pBuf+m_pos, &value, wantedBytes);
//	m_pos += wantedBytes;
//	return TRUE;
//}
//
//BOOL KMemoryStream::WriteDword(DWORD value)
//{
//	size_t wantedBytes = sizeof(value);
//	if(!this->CheckBuffer(wantedBytes)) return FALSE;
//	memcpy(m_pBuf+m_pos, &value, wantedBytes);
//	m_pos += wantedBytes;
//	return TRUE;
//}
//
//BOOL KMemoryStream::WriteFloat(float value)
//{
//	size_t wantedBytes = sizeof(value);
//	if(!this->CheckBuffer(wantedBytes)) return FALSE;
//	memcpy(m_pBuf+m_pos, &value, wantedBytes);
//	m_pos += wantedBytes;
//	return TRUE;
//}
//BOOL KMemoryStream::WriteDouble(double value)
//{
//	size_t wantedBytes = sizeof(value);
//	if(!this->CheckBuffer(wantedBytes)) return FALSE;
//	memcpy(m_pBuf+m_pos, &value, wantedBytes);
//	m_pos += wantedBytes;
//	return TRUE;
//}
//
//BOOL KMemoryStream::WriteInt64(INT64 value)
//{
//	size_t wantedBytes = sizeof(value);
//	if(!this->CheckBuffer(wantedBytes)) return FALSE;
//	memcpy(m_pBuf+m_pos, &value, wantedBytes);
//	m_pos += wantedBytes;
//	return TRUE;
//}
//
//BOOL KMemoryStream::WriteUint64(UINT64 value)
//{
//	size_t wantedBytes = sizeof(value);
//	if(!this->CheckBuffer(wantedBytes)) return FALSE;
//	memcpy(m_pBuf+m_pos, &value, wantedBytes);
//	m_pos += wantedBytes;
//	return TRUE;
//}

BOOL KMemoryStream::WriteByteArray(const void* value, size_t length)
{
	size_t wantedBytes = length;
	if(!this->CheckBuffer(wantedBytes)) return FALSE;
	memcpy(m_pBuf+m_pos, value, wantedBytes);
	m_pos += wantedBytes;
	return TRUE;
}

BOOL KMemoryStream::WriteString(const char* value, size_t length)
{
	if(!length) length = strlen(value);
	if(!this->WriteWord((WORD)length))
		return FALSE;

	if(!length)
		return TRUE;

	if(!this->WriteByteArray((BYTE*)value, length))
		return FALSE;
	return TRUE;
}

BOOL KMemoryStream::WriteString8(const char* value, size_t length)
{
	if(!length) length = strlen(value);
	if(length > 0xff) return FALSE;

	if(!this->WriteByte((BYTE)length))
		return FALSE;

	if(!length)
		return TRUE;

	if(!this->WriteByteArray((BYTE*)value, length))
		return FALSE;
	return TRUE;
}

BOOL KMemoryStream::WriteString32(const char* value, size_t length)
{
	if(!length) length = strlen(value);
	if(!this->WriteDword((DWORD)length))
		return FALSE;

	if(!length)
		return TRUE;

	if(!this->WriteByteArray((BYTE*)value, length))
		return FALSE;
	return TRUE;
}

KMsgOutputStream::KMsgOutputStream()
{
}

KMsgOutputStream::~KMsgOutputStream()
{
	this->Close();
}

void KMsgOutputStream::Close()
{
	if(m_pBuf) free_k(m_pBuf);
	m_pBuf = NULL;
	m_bufLength = 0;
	m_pos = 0;
}

BOOL KMsgOutputStream::CheckBuffer(size_t wantedBytes)
{
	size_t length = m_pos + wantedBytes;
	if(length > m_bufLength)
	{
		length = (length + 1023) & (~1023); //长度和1024对齐
		return this->SetLength(length);
	}
	return TRUE;
}

BOOL KMsgOutputStream::Resize(size_t length)
{
	m_pos = 0;
	return this->SetLength(length);
}

BOOL KMsgOutputStream::SetLength(size_t length)
{
	if(!m_pBuf)
	{
		m_pBuf = (char*)malloc_k(length);
		if(!m_pBuf) return FALSE;
		m_bufLength = length;
		m_pos = 0;
		return TRUE;
	}
	else
	{
		if(m_bufLength < length)
		{
			void* p = realloc_k(m_pBuf, length);
			if(!p) return FALSE;
			m_pBuf = (char*)p;
			m_bufLength = length;
			return TRUE;
		}
		return TRUE;
	}
}

/// KInputFileStream 

KInputFileStream::KInputFileStream():m_own(false),m_pfile(NULL)
{

}

KInputFileStream::KInputFileStream(FILE* f, bool own):m_pfile(f),m_own(own)
{

}

KInputFileStream::KInputFileStream(const char* filename):m_own(true)
{
	m_pfile = JG_F::KFileUtil::OpenFile(filename, "r+b");
}

KInputFileStream::~KInputFileStream()
{
	if(m_own && m_pfile)
	{
		fclose(m_pfile);
	}
	m_pfile = NULL;
}

void KInputFileStream::Close()
{
	if(m_own && m_pfile)
	{
		fclose(m_pfile);
	}
	m_own = false;
	m_pfile = NULL;
}

BOOL KInputFileStream::Open(const char* filename)
{
	this->Close();
	m_own = true;
	m_pfile = JG_F::KFileUtil::OpenFile(filename, "rb");
	return m_pfile != NULL;
}

BOOL KInputFileStream::Open(const char* filename, const char* mode)
{
	this->Close();
	m_own = true;
	m_pfile = JG_F::KFileUtil::OpenFile(filename, mode);
	return m_pfile != NULL;
}

BOOL KInputFileStream::Good() const
{
	return m_pfile != NULL;
}

int_r KInputFileStream::ReadData(void* value, size_t length)
{
	if(!m_pfile) return -1;
	return (int)fread(value, 1, length, m_pfile);
}

int_r KInputFileStream::WriteData(const void* value, size_t length)
{
	return -1;
}

BOOL KInputFileStream::Seek(size_t pos)
{
	if(!m_pfile) return FALSE;
	int nRet = fseek(m_pfile, (long)pos, SEEK_SET);
	return (nRet == 0);
}

size_t KInputFileStream::tell() const
{
	if(!m_pfile) return 0;
	return ftell(m_pfile);
}

void KInputFileStream::flush()
{
	return;
}

/// KOutputFileStream

KOutputFileStream::KOutputFileStream():m_own(false),m_pfile(NULL)
{

}

KOutputFileStream::KOutputFileStream(FILE* f, bool own):m_own(own),m_pfile(f)
{

}

KOutputFileStream::KOutputFileStream(const char* filename, const char* mode):m_own(true),m_pfile(NULL)
{
	this->Open(filename, mode);
}

KOutputFileStream::~KOutputFileStream()
{
	if(m_own && m_pfile)
	{
		fclose(m_pfile);
	}
	m_pfile = NULL;
}

void KOutputFileStream::Close()
{
	if(m_own && m_pfile)
	{
		fclose(m_pfile);
	}
	m_own = false;
	m_pfile = NULL;
}

BOOL KOutputFileStream::Open(const char* filename, const char* mode)
{
	this->Close();
	m_own = true;
	m_pfile = ::System::File::KFileUtil::OpenFile(filename, mode);
	return m_pfile != NULL;
}

BOOL KOutputFileStream::OpenForce(const char* filename, const char* mode)
{
	this->Close();
	m_own = true;
	m_pfile = ::System::File::KFileUtil::OpenFileForce(filename, mode);
	return m_pfile != NULL;
}

BOOL KOutputFileStream::Good() const
{
	return m_pfile != NULL;
}

int_r KOutputFileStream::ReadData(void* value, size_t length)
{
	if(!m_pfile) return -1;
	return (int)fread(value, 1, length, m_pfile);
}

int_r KOutputFileStream::WriteData(const void* value, size_t length)
{
	if(!m_pfile) return -1;
	return (int)fwrite(value, 1, length, m_pfile);
}

BOOL KOutputFileStream::Seek(size_t pos)
{
	if(!m_pfile) return FALSE;
	int nRet = fseek(m_pfile, (long)pos, SEEK_SET);
	return (nRet == 0);
}

size_t KOutputFileStream::tell() const
{
	if(!m_pfile) return 0;
	return ftell(m_pfile);
}

void KOutputFileStream::flush()
{
	if(m_pfile) fflush(m_pfile);
}