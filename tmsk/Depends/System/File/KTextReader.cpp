#include "./KTextReader.h"
#include "../Misc/StrUtil.h"
#include "../Misc/StrUtil.h"
#include <locale.h>
#include <wchar.h>
using namespace ::System::Collections;

namespace System { namespace File {

KTextReader::KTextReader()
	: m_rp(0)
	, m_wp(0)
	, m_orgStreamOffset(0)
	, m_all_in_memory(FALSE)
	, m_eof(FALSE)
	, m_si(NULL)
	, m_encode(encode_ansii)
{

}

KTextReader::~KTextReader()
{
	
}

void KTextReader::Clear()
{
	m_orgStreamOffset = 0;
	m_all_in_memory = FALSE;
	m_eof = FALSE;
	m_si = NULL;
	m_rp = m_wp = 0;
	m_encode = encode_ansii;
}

BOOL KTextReader::SetInputStream(StreamInterface& si)
{
	m_si = &si;
	size_t pos = m_si->tell();

	DWORD flag = 0;
	int rbytes = (int)m_si->ReadData(&flag, 3);
	if(rbytes > 2)
	{
		if((flag & 0xffff) == utf16_be_signature)
		{
			m_encode = encode_utf16_be;
			m_si->Seek(pos+2);
		}
		else if((flag & 0xffff) == utf16_le_signature)
		{
			m_encode = encode_utf16_le;
			m_si->Seek(pos+2);
		}
		else if(flag == utf8_signature)
		{
			m_encode = encode_utf8;
		}
		else
		{
			m_si->Seek(pos);
		}
	}
	else
	{
		m_si->Seek(pos);
	}

	m_orgStreamOffset = (int)m_si->tell();
	this->ReadBuffer();
	if(m_eof) m_all_in_memory = TRUE;

	return TRUE;
}

void KTextReader::Reset()
{
	if(m_all_in_memory)
	{
		m_rp = 0;
	}
	else
	{
		m_eof = FALSE;
		m_si->Seek(m_orgStreamOffset);
		m_rp = m_wp = 0;
	}
}

BOOL KTextReader::eof() const
{
	return m_eof && (m_rp >= m_wp);
}

int KTextReader::ReadLine(char*& ptr, const char* delimiters)
{
	ptr = NULL;
	if(!this->ReadBuffer()) return -1;
	if(this->eof()) return -1;

	switch(m_encode)
	{
	case encode_ansii:
	case encode_utf8:
		{
			return this->_readLine_mb(ptr, delimiters);
		}
	case encode_utf16_be:
		{
			wchar_16* wptr = (wchar_16*)ptr;
			int n = this->_readLine_utf16be(wptr, delimiters);
			ptr = (char*)wptr;
			return n*2;
		}
	case encode_utf16_le:
		{
			wchar_16* wptr = (wchar_16*)ptr;
			int n = this->_readLine_utf16le(wptr, delimiters);
			ptr = (char*)wptr;
			return n*2;
		}
	default:
		{
			ASSERT(FALSE);
		}
	}
	ASSERT(FALSE); return 0;
}

int KTextReader::ReadLine(char* buf, int len)
{
	return this->ReadLine(buf, len, "\r\n");
}

int KTextReader::ReadLine(char* buf, int len, const char* delimiters)
{
	char* ptr;
	int readed = this->ReadLine(ptr, delimiters);
	if(readed == -1) return -1;
	if(!readed) return 0;

	int n;
	if(m_encode == g_encoding)
	{
		n = kmin(readed, len-1);
		memcpy(buf, ptr, n);
	}
	else
	{
		if(g_encoding == encode_utf8)
		{	// target is utf8
			switch(m_encode)
			{
			case encode_ansii:
				n = local_to_utf8(ptr, readed, buf, len-1);
				break;
			case encode_utf16_be:
				n = utf16be_to_utf8(ptr, readed, buf, len-1);
				break;
			case encode_utf16_le:
				n = utf16le_to_utf8(ptr, readed, buf, len-1);
				break;
			default:
				ASSERT(FALSE);
			}
			if(n < 0) return -1; //ASSERT(n >= 0);
		}
		else
		{	// target is local
			switch(m_encode)
			{
			case encode_utf8:
				n = utf8_to_local(ptr, readed, buf, len-1);
				break;
			case encode_utf16_be:
				n = utf16be_to_local(ptr, readed, buf, len-1);
				break;
			case encode_utf16_le:
				n = utf16le_to_local(ptr, readed, buf, len-1);
				break;
			default:
				ASSERT(FALSE);
			}
			if(n < 0) return -1; //ASSERT(n >= 0);
		}
	}
	
	buf[n] = '\0';
	return n;
}

void KTextReader::CompactBuffer()
{
	if(m_wp == m_rp)
	{
		m_rp = m_wp = 0;
	}
	else
	{
		int len = m_wp - m_rp;
		if(len > 0)
		{
			char* pDst = &m_buffer[0];
			char* pSrc = &m_buffer[m_rp];
			memmove(pDst, pSrc, len);
			m_rp = 0;
			m_wp = len;
		}
	}
}

BOOL KTextReader::ReadBuffer()
{
	if(m_eof) return TRUE;
	if(m_wp-m_rp >= m_maxLineSize) return TRUE;

	this->CompactBuffer();

	int_r len = m_bufSize - m_wp;
	if(len < 1) return TRUE;

	int_r readed = m_si->ReadData(&m_buffer[m_wp], len);
	if(readed < len) m_eof = TRUE;
	if(readed < 0) return FALSE;
	
	m_wp += (int)readed;
	return TRUE;
}

int KTextReader::_transDelimiters_be(const char* seps, wchar_16* delimiters, int len)
{
	int sepn = 0, srclen = (int)strlen(seps);
	while(seps[0] && srclen > 0 && len > 0)
	{
		unsigned int uchar;
		int n = local_to_unicode(seps, srclen, &uchar);
		if(n < 1) n = 1;
		seps += n; srclen -= n;
		uchar = ((uchar&0xff) << 8) + (uchar>>8);
		delimiters[sepn++] = (wchar_16)uchar; len--;
	}
	if(len > 0) delimiters[sepn] = 0;
	return sepn;
}

int KTextReader::_transDelimiters_le(const char* seps, wchar_16* delimiters, int len)
{
	int sepn = 0, srclen = (int)strlen(seps);
	while(seps[0] && srclen > 0 && len > 0)
	{
		unsigned int uchar;
		int n = local_to_unicode(seps, srclen, &uchar);
		if(n < 1) n = 1;
		seps += n; srclen -= n;
		delimiters[sepn++] = (wchar_16)uchar; len--;
	}
	if(len > 0) delimiters[sepn] = 0;
	return sepn;
}


int KTextReader::_readLine_mb(char*& ptr, const char* delimiters)
{
	int i = 0;
	ptr = &m_buffer[m_rp];
	while(m_rp < m_wp)
	{
		char chr = m_buffer[m_rp];
		if(strchr(delimiters, chr))
		{
			m_rp += 1;
			if(chr != '\n')
			{
				while(m_rp < m_wp && strchr(delimiters, m_buffer[m_rp]))
				{
					char ch = m_buffer[m_rp];
					m_rp += 1;
					if(ch == '\n') break;
				}
			}
			break;
		}
		i += 1; m_rp += 1;
	}
	return i;
}

int KTextReader::_readLine_utf16be(wchar_16*& ptr, const char* seps)
{
	wchar_16 NEW_LINE = '\n';
	NEW_LINE = (NEW_LINE>>8) + ((NEW_LINE&0xff)<<8);

	wchar_16 delimiters[256];
	wchar_16* buffer = (wchar_16*)&m_buffer[m_rp];
	this->_transDelimiters_be(seps, delimiters, 256);
	
	int i = 0, rp = 0;
	ptr = buffer;
	while(m_rp < m_wp)
	{
		wchar_16 chr = buffer[rp];
		rp++; m_rp += sizeof(wchar_16);
		if(wcschr_k(delimiters, chr))
		{	
			if(chr == NEW_LINE) break;
			while(m_rp < m_wp)
			{
				chr = buffer[rp];
				if(!wcschr_k(delimiters, chr)) break;
				rp++; m_rp += sizeof(wchar_16);
				if(chr == NEW_LINE) break;
			}
			break;
		}
		i += 1;
	}
	return i;
}

int KTextReader::_readLine_utf16le(wchar_16*& ptr, const char* seps)
{
	wchar_16 NEW_LINE = '\n';
	wchar_16 delimiters[256];
	wchar_16* buffer = (wchar_16*)&m_buffer[m_rp];
	this->_transDelimiters_le(seps, delimiters, 256);

	int i = 0, rp = 0;
	ptr = buffer;
	while(m_rp < m_wp)
	{
		wchar_16 chr = buffer[rp];
		rp++; m_rp += sizeof(wchar_16);
		if(wcschr_k(delimiters, chr))
		{	
			if(chr == NEW_LINE) break;
			while(m_rp < m_wp)
			{
				chr = buffer[rp];
				if(!wcschr_k(delimiters, chr)) break;
				rp++; m_rp += sizeof(wchar_16);
				if(chr == NEW_LINE) break;
			}
			break;
		}
		i += 1;
	}
	return i;
}

// KTextMeassgeReader
KTextMeassgeReader::KTextMeassgeReader()
{
	this->SetInputStream(m_siObj);
}

BOOL KTextMeassgeReader::Attach(const char* msg, size_t len)
{
	m_siObj.Attach((void*)msg, (int)len);
	this->SetInputStream(m_siObj);
	this->Reset();
	return TRUE;
}

KTextMeassgeReader::KTextMeassgeReader(const char* msg, size_t len)
{
	m_siObj.Attach((void*)msg, (int)len);
	this->SetInputStream(m_siObj);
}

KTextMeassgeReader::~KTextMeassgeReader()
{

}

KTextFileReader::KTextFileReader()
{

}

KTextFileReader::KTextFileReader(const char* filename)
{
	if(!m_siObj.Open(filename))
		m_eof = TRUE;
	this->SetInputStream(m_siObj);
}

KTextFileReader::~KTextFileReader()
{

}

bool KTextFileReader::open(const char* fileName, const char* mode)
{
	this->Clear();
	if(!m_siObj.Open(fileName, mode))
		return false;
	this->SetInputStream(m_siObj);
	return true;
}

KIniReader::KIniReader()
{

}

KIniReader::~KIniReader()
{

}

void KIniReader::Clear()
{
	m_textReader.Clear();
}

BOOL KIniReader::SetInputStream(StreamInterface& si)
{
	return m_textReader.SetInputStream(si);
}

int KIniReader::GetSections(char* secNames[], int count, KBuffer64k* pBuffer)
{
	int num = 0;
	char buf[m_maxLineSize];

	m_textReader.Reset();

	while(TRUE)
	{
		int readed = this->ReadLine(buf, m_maxLineSize);

		if(readed == -1) return num;
		if(readed == 0) continue;
		ASSERT(readed < m_maxLineSize);

		const char* psec = this->GetSectionName(buf, readed);
		if(psec)
		{
			secNames[num++] = pBuffer->GetWriteBuffer();
			pBuffer->Write(psec, (int)strlen(psec)+1);
			if(num >= count) break;
		}
	}

	return num;
}

BOOL KIniReader::GetInt(const char* sec, const char* keyName, int& val)
{
	m_textReader.Reset();

	char* pkey;
	char* pval;
	char buf[m_maxLineSize];

	BOOL underSec = FALSE;
	if(!sec) underSec = TRUE;

	while(TRUE)
	{
		int readed = this->ReadLine(buf, m_maxLineSize);
		
		if(readed == -1) return FALSE;
		if(readed == 0) continue;
		ASSERT(readed < m_maxLineSize);

		const char* psec = this->GetSectionName(buf, readed);
		
		if(underSec)
		{
			if(psec) return FALSE;
			if(!this->GetKeyValuePair(buf, readed, pkey, pval)) continue;

			if(!pkey[0]) continue;
			if(_stricmp(keyName, pkey)) continue;

			if(!pval[0]) return FALSE;
			if(!isnumber3(pval)) return FALSE;

			val = str2int(pval);
			return TRUE;
		}
		else
		{
			if(psec && _stricmp(psec, sec) == 0)
			{
				underSec = TRUE;
			}
		}
	}

	return FALSE;
}

BOOL KIniReader::GetInt(const char* sec, const char* keyName, int defVal, int& val)
{
	if(this->GetInt(sec, keyName, val)) return TRUE;
	val = defVal;
	return TRUE;
}

BOOL KIniReader::GetFloat(const char* sec, const char* keyName, float& val)
{
	char str[64];
	if(!this->GetString(sec, keyName, str, 64)) return FALSE;
	if(!isfloat(str)) return FALSE;
	val = (float)atof(str);
	return TRUE;
}

BOOL KIniReader::GetFloat(const char* sec, const char* keyName, float defVal, float& val)
{
	if(this->GetFloat(sec, keyName, val)) return TRUE;
	val = defVal;
	return TRUE;
}

BOOL KIniReader::GetUint(const char* sec, const char* keyName, DWORD& val)
{
	int nVal;
	if(!this->GetInt(sec, keyName, nVal)) return FALSE;

	val = (DWORD)nVal;
	return TRUE;
}

BOOL KIniReader::GetUint(const char* sec, const char* keyName, DWORD defVal, DWORD& val)
{
	if(this->GetUint(sec, keyName, val)) return TRUE;
	val = defVal;
	return TRUE;
}

BOOL KIniReader::GetInt64(const char* sec, const char* keyName, INT64& val)
{
	m_textReader.Reset();

	char* pkey;
	char* pval;
	char buf[m_maxLineSize];

	BOOL underSec = FALSE;
	if(!sec) underSec = TRUE;

	while(TRUE)
	{
		int readed = this->ReadLine(buf, m_maxLineSize);

		if(readed == -1) return FALSE;
		if(readed == 0) continue;
		ASSERT(readed < m_maxLineSize);

		const char* psec = this->GetSectionName(buf, readed);

		if(underSec)
		{
			if(psec) return FALSE;
			if(!this->GetKeyValuePair(buf, readed, pkey, pval)) continue;

			if(!pkey[0]) continue;
			if(_stricmp(keyName, pkey)) continue;

			if(!pval[0]) return FALSE;
			if(!isnumber3(pval)) return FALSE;

			val = str2i64(pval);
			return TRUE;
		}
		else
		{
			if(psec && _stricmp(psec, sec) == 0)
			{
				underSec = TRUE;
			}
		}
	}

	return FALSE;
}

BOOL KIniReader::GetInt64(const char* sec, const char* keyName, INT64 defVal, INT64& val)
{
	if(this->GetInt64(sec, keyName, val)) return TRUE;
	val = defVal;
	return TRUE;
}

BOOL KIniReader::GetUint64(const char* sec, const char* keyName, UINT64& val)
{
	INT64 nVal;
	if(!this->GetInt64(sec, keyName, nVal)) return FALSE;
	val = (UINT64)nVal;
	return TRUE;
}

BOOL KIniReader::GetUint64(const char* sec, const char* keyName, UINT64 defVal, UINT64& val)
{
	if(this->GetUint64(sec, keyName, val)) return TRUE;
	val = defVal;
	return TRUE;
}

BOOL KIniReader::GetString(const char* sec, const char* keyName, char* val, int len)
{
	m_textReader.Reset();

	char* pkey;
	char* pval;
	char buf[m_maxLineSize];

	BOOL underSec = FALSE;
	if(!sec) underSec = TRUE;

	while(TRUE)
	{
		int readed = this->ReadLine(buf, m_maxLineSize);

		if(readed == -1) return FALSE;
		if(readed == 0) continue;
		ASSERT(readed < m_maxLineSize);

		const char* psec = this->GetSectionName(buf, readed);

		if(underSec)
		{
			if(psec) return FALSE;
			if(!this->GetKeyValuePair(buf, readed, pkey, pval)) continue;

			if(!pkey[0]) continue;
			if(_stricmp(keyName, pkey)) continue;

			int vallen = (int)strlen(pval);
			if(vallen >= len) return FALSE;

			memcpy(val, pval, vallen);
			val[vallen] = '\0';
			return TRUE;
		}
		else
		{
			if(psec && _stricmp(psec, sec) == 0)
			{
				underSec = TRUE;
			}
		}
	}

	return FALSE;
}

BOOL KIniReader::GetString(const char* sec, const char* keyName, const char* defVal, char* val, int len)
{
	if(this->GetString(sec, keyName, val, len)) return TRUE;

	int deflen = (int)strlen(defVal);
	if(deflen >= len) return FALSE;

	memcpy(val, defVal, deflen);
	val[deflen] = '\0';

	return TRUE;
}

BOOL KIniReader::GetSection(const char* sec, char* val, size_t len, size_t& readed)
{
	return this->GetSection(sec, val, len, '\0', readed);
}

BOOL KIniReader::GetSection(const char* sec, char* val, size_t len, char sep, size_t & readed)
{
	m_textReader.Reset();

	char buf[m_maxLineSize];

	BOOL underSec = FALSE;
	if(!sec) underSec = TRUE;

	readed = 0;

	while(TRUE)
	{
		int nbytes = this->ReadLine(buf, m_maxLineSize);

		if(nbytes == -1)
		{
			if(underSec)
			{
				if(readed > 0) val[readed-1] = '\0';
				return TRUE;
			}
			return FALSE;
		}

		if(nbytes == 0) continue;
		ASSERT(nbytes < m_maxLineSize);

		const char* psec = this->GetSectionName(buf, nbytes);

		if(underSec)
		{
			if(psec)
			{
				if(readed > 0) val[readed-1] = '\0';
				return TRUE;
			}
			if(readed + nbytes >= len) return FALSE;

			memcpy(val+readed, buf, nbytes);
			val[readed+nbytes] = sep;
			readed += nbytes + 1;
		}
		else
		{
			if(psec && _stricmp(psec, sec) == 0)
			{
				underSec = TRUE;
			}
		}
	}

	if(underSec)
	{
		if(readed > 0) val[readed-1] = '\0';
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int KIniReader::GetSectionKeys(const char* sec, KString<64>* keys, int count)
{
	KBuffer64k* pBuffer = KBuffer64k::Alloc();
	pBuffer->Reset();

	size_t readed = 0;
	int_r len = pBuffer->WriteAvail();
	char* buf = pBuffer->GetWriteBuffer();

	if(!this->GetSection(sec, buf, len, '\n', readed))
	{
		KBuffer64k::Free(pBuffer);
		return 0;
	}

	KTextMeassgeReader reader(buf, readed);

	char* line;
	char* key;
	char* val;
	int num = 0;

	while(num < count)
	{
		int n = reader.ReadLine(line, "\n");
		if( n < 0)
			break;
		if(!this->GetKeyValuePair(line, n, key, val))
			break;

		keys[num++] = key;
	}

	KBuffer64k::Free(pBuffer);
	return num;
}

int KIniReader::ReadLine(char* buf, int len)
{
	char tmp[4096];
	int nbytes = m_textReader.ReadLine(tmp, sizeof(tmp), "\r\n");
	if(nbytes == -1) return -1;
	if(nbytes == 0) return 0;

	ASSERT(nbytes < len);
	memcpy(buf, tmp, nbytes);
	buf[nbytes] = '\0';

	char* pline = _trim(buf);

	if(!pline[0]) return 0;
	if(pline[0] == '#') return 0;
	int linelen = (int)strlen(pline);

	char* psep = strchr(pline, '#');
	if(psep)
	{
		psep[0] = '\0';
		pline = _trim(pline);
		linelen = (int)strlen(pline);
	}

	if(pline != &buf[0])
	{
		memmove(&buf[0], pline, linelen);
	}

	buf[linelen] = '\0';
	return linelen;
}

const char* KIniReader::GetSectionName(char* pline, int len)
{
	if(len < 2) return NULL;
	if(pline[0] != '[') return NULL;
	if(pline[len-1] != ']') return NULL;

	pline[len-1] = '\0';

	char* psec = pline + 1;
	psec = _trim(psec);

	return psec;
}

BOOL KIniReader::GetKeyValuePair(char* pline, int len, char*& pkey, char*& pval)
{
	char* pequal = strchr(pline, '=');
	if(!pequal) return FALSE;
	pequal[0] = '\0';

	pkey = _trim(&pline[0]);
	pval = _trim(pequal + 1);

	return TRUE;
}

KIniMsgReader::KIniMsgReader(const char* msg, int len):m_si((char*)msg, len)
{
	this->SetInputStream(m_si);
}

KIniMsgReader::~KIniMsgReader()
{

}

// 如果文件小于10K，全部读取到内存中
KIniFileReader::KIniFileReader(const char* filename)
{
	if(!m_siFile.Open(filename))
	{
		this->SetInputStream(m_siFile);
		m_textReader.m_eof = TRUE;
		return;
	}
	this->SetInputStream(m_siFile);
}

KIniFileReader::~KIniFileReader()
{

}

KColumnReader::KColumnReader()
{

}

KColumnReader::~KColumnReader()
{

}

void KColumnReader::Clear()
{
	m_sl.clear();
	m_textReader.Clear();
}

BOOL KColumnReader::SetInputStream(StreamInterface& si)
{
	return m_textReader.SetInputStream(si);
}

int KColumnReader::ReadLine(char*& buf)
{
	buf = &m_line[0];
	return m_textReader.ReadLine(m_line, m_maxLineSize);
}

int KColumnReader::ReadLine(char**& pCols, char delimiter)
{
	int nbytes = m_textReader.ReadLine(m_line, m_maxLineSize);
	if(nbytes == -1) return -1;
	if(nbytes == 0) return 0;

	int nRet = split(m_line, delimiter, m_sl);
	if(nRet < 1) return 0;

	pCols = m_sl.m_vals;
	return nRet;
}

int KColumnReader::ReadLine(char**& pCols, const char* delimiters)
{
	int nbytes = m_textReader.ReadLine(m_line, m_maxLineSize);
	if(nbytes == -1) return -1;
	if(nbytes == 0) return 0;
	
	int nRet = split(m_line, delimiters, m_sl);
	if(nRet < 1) return 0;

	pCols = m_sl.m_vals;
	return nRet;
}

KColumnMsgReader::KColumnMsgReader(const char* msg, int len):m_si((char*)msg, len)
{
	this->SetInputStream(m_si);
}

KColumnMsgReader::~KColumnMsgReader()
{

}

KColumnFileReader::KColumnFileReader(const char* filename)
{
	if(!m_siFile.Open(filename))
	{
		m_textReader.SetInputStream(m_siFile);
		m_textReader.m_eof = TRUE;
		return;
	}
	this->SetInputStream(m_siFile);
}

KColumnFileReader::~KColumnFileReader()
{

}

};};