#include <stdio.h>
#include <stdlib.h>
#include "StrUtil.h"
#include "../SockHeader.h"
#include "../Sync/KSync.h"
#if defined(WIN32) || defined(WINDOWS)
#include <lmerr.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#else
#include <ctype.h>
#endif
#include "../KPlatform.h"
#include <wchar.h>
#include "../Charset/KCharset.h"

static unsigned int _unicode_limits_[] = {0, 0x80, 0x800, 0x10000, 0x200000, 0x4000000, 0x80000000};

char* _trim(char* s)
{
	char* pb = s;
	char* pe = s + strlen(s) - 1;
	while((*pb==' '||*pb=='\t'||*pb=='\n'||*pb=='\r') && *pb!='\0') pb++;
	while(pe > pb && (*pe==' '||*pe=='\t'||*pe=='\n'||*pe=='\r')) pe--;
	*(pe+1) = '\0';
	return pb;
}

const char* eatup(const char* pc, const char* delimiters)
{
	while(pc[0] != '\0' && strchr(delimiters, pc[0]) != NULL) pc++;
	return pc;
}

int str2int(const char* pc)
{
	if(pc == NULL || pc[0] == '\0') return 0;

	bool negative = false;
	const char* pb = pc;
	if(pc[0] == '+' || pc[0] == '-')
	{
		pb = pc + 1;
		if(pc[0] == '-') negative = true;
	}

	size_t len = strlen(pb);
	if(len > 2 && pb[0] == '0' && toupper(pb[1]) == 'X')
		return strtoul(pc,NULL,16);

	return strtoul(pc,NULL,10);
}

INT64 str2i64(const char* pc)
{
	if(pc == NULL || pc[0] == '\0') return 0;

	bool negative = false;
	const char* pb = pc;
	if(pc[0] == '+' || pc[0] == '-')
	{
		pb = pc + 1;
		if(pc[0] == '-') negative = true;
	}

	size_t len = strlen(pb);
	if(len > 2 && pb[0] == '0' && toupper(pb[1]) == 'X')
		return _strtoi64(pc,NULL,16);

	return _strtoi64(pc,NULL,10);
}

bool isnumchar(char ch)
{
	int chr = (int)(BYTE)ch;
	static bool ch_tab[256];
	static ::System::Sync::KThreadMutex m_mx;
	static BOOL inited = FALSE;
	if(inited) return ch_tab[chr];
	::System::Sync::KAutoThreadMutex mx(m_mx);
	if(inited) return ch_tab[chr];
	memset(&ch_tab, 0, sizeof(ch_tab));
	ch_tab['0'] = ch_tab['1'] = ch_tab['2'] = ch_tab['3'] = true;
	ch_tab['4'] = ch_tab['5'] = ch_tab['6'] = ch_tab['7'] = true;
	ch_tab['8'] = ch_tab['9'] = ch_tab['a'] = ch_tab['b'] = true;
	ch_tab['c'] = ch_tab['d'] = ch_tab['e'] = ch_tab['f'] = true;
	ch_tab['A'] = ch_tab['B'] = ch_tab['C'] = ch_tab['D'] = true;
	ch_tab['E'] = ch_tab['F'] = ch_tab['+'] = ch_tab['-'] = true;
	inited = TRUE;
	return ch_tab[chr];
}

bool isdigitchar(char ch)
{
	int chr = (int)(BYTE)ch;
	static bool ch_tab[256];
	static ::System::Sync::KThreadMutex m_mx;
	static BOOL inited = FALSE;
	if(inited) return ch_tab[chr];
	::System::Sync::KAutoThreadMutex mx(m_mx);
	if(inited) return ch_tab[chr];
	memset(&ch_tab, 0, sizeof(ch_tab));
	ch_tab['0'] = ch_tab['1'] = ch_tab['2'] = ch_tab['3'] = true;
	ch_tab['4'] = ch_tab['5'] = ch_tab['6'] = ch_tab['7'] = true;
	ch_tab['8'] = ch_tab['9'] = ch_tab['+'] = ch_tab['-'] = true;
	inited = TRUE;
	return ch_tab[chr];
}

bool ishexchar(char ch)
{
	int chr = (int)(BYTE)ch;
	static bool ch_tab[256];
	static ::System::Sync::KThreadMutex m_mx;
	static BOOL inited = FALSE;
	if(inited) return ch_tab[chr];
	::System::Sync::KAutoThreadMutex mx(m_mx);
	if(inited) return ch_tab[chr];
	memset(&ch_tab, 0, sizeof(ch_tab));
	ch_tab['a'] = ch_tab['b'] = true;
	ch_tab['c'] = ch_tab['d'] = ch_tab['e'] = ch_tab['f'] = true;
	ch_tab['A'] = ch_tab['B'] = ch_tab['C'] = ch_tab['D'] = true;
	ch_tab['E'] = ch_tab['F'] = true;
	inited = TRUE;
	return ch_tab[chr];
}

bool isnumber3(const char* pc)
{
	BOOL isHex = FALSE;
	if(!pc || !pc[0]) return false;
	if(pc[0] == '0' && (pc[1]=='x' || pc[1]=='X'))
	{
		pc += 2;
		isHex = TRUE;
	}

	while(pc[0])
	{
		char chr = *pc++;
		if(!isnumchar(chr)) return FALSE;
		if(!isHex && chr<'0' && chr>'9') return FALSE;
	}

	return TRUE;
}

bool isfloat(const char* pc)
{
	if(!pc || !pc[0])
		return false;
	
	if(pc[0] == '-') pc++;
	BOOL dotFound = FALSE;

	char ch;
	while(ch=*pc++, ch)
	{
		if(ch == '.')
		{
			if(dotFound)
				return false;
			dotFound = TRUE;
			continue;
		}
		if(!isdigitchar(ch))
			return false;
	}
	return true;
}

::System::Collections::KString<512> strerror_r2(int errnum)
{
	char buf[512];
	char buf2[512];
#if defined (WIN32) || defined(WINDOWS)
	buf[0] = '\0';
	HMODULE hModule = NULL;
	static HMODULE hNetmsgMod = LoadLibraryEx(TEXT("netmsg.dll"),NULL,LOAD_LIBRARY_AS_DATAFILE);
	WORD langID = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
	DWORD flags = FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_SYSTEM;
	if(errnum >= NERR_BASE && errnum <= MAX_NERR)
	{
		hModule = hNetmsgMod;
		flags |= FORMAT_MESSAGE_FROM_HMODULE;
	}
	int len = FormatMessageA(flags, hModule, errnum, langID, buf, sizeof(buf), NULL);
#else
	const char* errMsg = NULL;
#if defined(__APPLE__)
	if(!strerror_r(errnum, &buf[256], sizeof(buf)-256))
		errMsg = &buf[0];
	else errMsg = "unknow-error";
#else
	errMsg = strerror_r(errnum, &buf[256], sizeof(buf)-256);
#endif
	int len = sprintf(buf, "code:%u message:%s", errnum, errMsg);
#endif

	_trim(buf);
	if(localToCurrent(buf, buf2, sizeof(buf2)) < 0)
	{
		::System::Collections::KString<512> retStr;
		retStr.Printf("code:%u message:%s", errnum, "unknown");
		return retStr;
	}
	
	return buf2;
}

// 会对输入的源字符串 s 进行修改
int split(char* s, const char* delimiters, KStringList& sl)
{
	sl.clear();
	sl.push_back(s);
	while(s[0])
	{
		size_t n = kmax(1, mblen_k(s, 6));
		if(n == 1 && strchr(delimiters, s[0]))
		{
			s[0] = '\0'; s++; 
			while((n=mblen_k(s,6)) == 1 && strchr(delimiters, s[0])) s++;
			sl.push_back(s);
		}
		else
		{
			s += n;
		}
	}
	return sl.size();
}

int split(char* s, char delimiter, KStringList& sl)
{
	sl.clear();
	sl.push_back(s);
	while(s[0])
	{
		size_t n = kmax(1, mblen_k(s, 6));
		if(n == 1 && s[0] == delimiter)
		{
			s[0] = '\0'; s += n;
			sl.push_back(s);
		}
		else
		{
			s += n;
		}
	}
	return sl.size();
}

int split(char* s, char delimiter, char* ss[], int maxs)
{
	int i = 0;
	ss[i++] = s;
	while(s[0] && i<maxs)
	{
		size_t n = kmax(1, mblen_k(s, 6));
		if(n == 1 && delimiter == s[0])
		{
			s[0] = '\0'; s += n;
			ss[i++] = s;
		}
		else
		{
			s += n;
		}
	}
	return i;
}

int split(char* s, const char* delimiters, char* ss[], int maxs)
{
	int i = 0;
	ss[i++] = s;
	while(s[0] && i<maxs)
	{
		size_t n = kmax(1, mblen_k(s, 6));
		if(n == 1 && strchr(delimiters, s[0]))
		{
			s[0] = '\0'; s += n;
			while((n=mblen_k(s,6)) == 1 && strchr(delimiters, s[0])) s++;
			ss[i++] = s;
		}
		else
		{
			s += n;
		}
	}
	return i;
}

BOOL CheckStrLength(const char* s, int minLength, int maxLength)
{
	ASSERT(minLength <= maxLength);
	
	int i = 0;
	while(s[i] && i < maxLength) i++;

	if(!s[i])
	{
		if(i < minLength) return FALSE;
		return TRUE;
	}

	return FALSE;
}

int stristr(const char* str, const char* pattern)
{
	int i = 0;
	int len = (int)strlen(str);
	int len2 = (int)strlen(pattern);
	if(len < len2) return -1;

	int ep = len - len2;
	while(i <= ep)
	{
		const char* subStr = str + i;
		if(_strnicmp(subStr, pattern, len2) == 0) return i;
		i++;
	}

	return -1;
}

BOOL strrep(char* str, int buflen, int beg, int count, const char* ns)
{
	int len = (int)strlen(ns);
	if(len <= count)
	{
		memcpy(str+beg, ns, len);
		int l = count - len;
		if(l > 0)
		{
			char* pDst = str + beg + len;
			char* pSrc = str + beg + count;
			int length = (int)strlen(pSrc) + 1;
			memmove(pDst, pSrc, length);
			return TRUE;
		}
		return TRUE;
	}
	else
	{
		int l = len - count;
		int lenOrg = (int)strlen(str);
		if(lenOrg + l >= buflen) return FALSE;

		char* pSrc = str + beg + count;
		char* pDst = str + beg + len;
		int length = (int)strlen(pSrc) + 1;
		memmove(pDst, pSrc, length);
		memcpy(str+beg, ns, len);
		return TRUE;
	}
}

KIpAddr str2ipaddr(const char* stripaddr)
{
	KIpAddr ipaddr;

	char buf[256];
	strcpy_k(buf, sizeof(buf), stripaddr);

	char* ss[2];
	int ns = split(buf, ':', ss, 2);
	if(ns != 2) return ipaddr;


	ipaddr.ip = ntohl(inet_addr(ss[0]));
	ipaddr.port = str2int(ss[1]);
	ipaddr.valid = 1;

	return ipaddr;
}

void toUpperCase(char* str)
{
	while(str[0])
	{
		size_t n = kmax(mblen_k(str, 6), 1);
		if(n == 1)
		{
			char chr = str[0];
			if(chr >= 'a' && chr<= 'z')
				str[0] -= 32;
		}
		str += n;
	}
}

void toLowerCase(char* str)
{
	while(str[0])
	{
		size_t n = kmax(mblen_k(str, 6), 1);
		if(n == 1)
		{
			char chr = str[0];
			if(chr >= 'A' && chr<= 'Z')
				str[0] += 32;
		}
		str += n;
	}
}

int tohexstr(const void* data, size_t len, char* buf, size_t size)
{
	ASSERT(size >= len*2);
	const char chars[] = "0123456789ABCDEF";

	BYTE chr; size_t i = 0;
	BYTE* p = (BYTE*)data;
	while(chr=p[i], i++<len)
	{
		*buf++ = chars[chr >> 4];
		*buf++ = chars[chr & 0x0f];
		size -= 2;
	}
	if(size) *buf = '\0';
	return len*2;
}

int reversehexstr(const char* str, size_t len, void* buf, size_t size)
{
	if(len & 1) return -1;
	if(size < (len>>1)) return -1;

	size_t i = 0;
	size_t c = 0;
	unsigned char* dst = (unsigned char*)buf;

	while(i < len)
	{
		char chr = str[i++];
		switch(chr)
		{
		case '0': dst[c] = 0;  break;
		case '1': dst[c] = 1;  break;
		case '2': dst[c] = 2;  break;
		case '3': dst[c] = 3;  break;
		case '4': dst[c] = 4;  break;
		case '5': dst[c] = 5;  break;
		case '6': dst[c] = 6;  break;
		case '7': dst[c] = 7;  break;
		case '8': dst[c] = 8;  break;
		case '9': dst[c] = 9;  break;
		case 'A': dst[c] = 10; break;
		case 'B': dst[c] = 11; break;
		case 'C': dst[c] = 12; break;
		case 'D': dst[c] = 13; break;
		case 'E': dst[c] = 14; break;
		case 'F': dst[c] = 15; break;
		case 'a': dst[c] = 10; break;
		case 'b': dst[c] = 11; break;
		case 'c': dst[c] = 12; break;
		case 'd': dst[c] = 13; break;
		case 'e': dst[c] = 14; break;
		case 'f': dst[c] = 15; break;
		default: return -1;
		}
		chr = str[i++];
		switch(chr)
		{
		case '0': dst[c] = (dst[c]<<4) + 0;  break;
		case '1': dst[c] = (dst[c]<<4) + 1;  break;
		case '2': dst[c] = (dst[c]<<4) + 2;  break;
		case '3': dst[c] = (dst[c]<<4) + 3;  break;
		case '4': dst[c] = (dst[c]<<4) + 4;  break;
		case '5': dst[c] = (dst[c]<<4) + 5;  break;
		case '6': dst[c] = (dst[c]<<4) + 6;  break;
		case '7': dst[c] = (dst[c]<<4) + 7;  break;
		case '8': dst[c] = (dst[c]<<4) + 8;  break;
		case '9': dst[c] = (dst[c]<<4) + 9;  break;
		case 'A': dst[c] = (dst[c]<<4) + 10; break;
		case 'B': dst[c] = (dst[c]<<4) + 11; break;
		case 'C': dst[c] = (dst[c]<<4) + 12; break;
		case 'D': dst[c] = (dst[c]<<4) + 13; break;
		case 'E': dst[c] = (dst[c]<<4) + 14; break;
		case 'F': dst[c] = (dst[c]<<4) + 15; break;
		case 'a': dst[c] = (dst[c]<<4) + 10; break;
		case 'b': dst[c] = (dst[c]<<4) + 11; break;
		case 'c': dst[c] = (dst[c]<<4) + 12; break;
		case 'd': dst[c] = (dst[c]<<4) + 13; break;
		case 'e': dst[c] = (dst[c]<<4) + 14; break;
		case 'f': dst[c] = (dst[c]<<4) + 15; break;
		default: return -1;
		}
		c++;
	}
	return c;
}

int utf8charsize(const char* s)
{
	unsigned char chr = s[0];
	if(!chr) return 0;
	if(chr < B10000000) return 1;
	if(chr < B11100000) return 2;
	if(chr < B11110000) return 3;
	if(chr < B11111000) return 4;
	if(chr < B11111100) return 5;
	if(chr < B11111110) return 6;
	return -1;
}

int utf8charcount(const char* s)
{
	int n = 0;
	while(s[0])
	{
		int len = kmax(utf8charsize(s), 1);
		s += len; n++;
	}
	return n;
}

int mblen_k(const char* s, size_t n)
{
	switch(g_encoding)
	{
	case encode_ansii:
		return mblen(s, n);
	case encode_utf8:
		return utf8charsize(s);
	//case encode_utf16_be:
	//case encode_utf16_le:
		return 2;
	default:
		return mblen(s, 4);
	}
}

int utf8ToUnicode(const char* src, unsigned int* uchr)
{
	unsigned int& ret = *uchr;
	ret = 0;

	BYTE* s = (BYTE*)src;
	unsigned char chr = s[0];
	if(!chr) return 0;

	if(chr < B10000000)
	{
		ret = chr;
		return 1;
	}
	if(chr < B11100000) // B11011111
	{
		if(!(chr & B00011111)) return -1;
		if((chr&B11000000) != B11000000) return -1;
		if(!s[1] || s[1] > B10111111) return -1;
		ret  = s[0] & B00011111; ret <<= 6;
		ret |= s[1] & B00111111;
		if(ret < _unicode_limits_[1] || ret >= _unicode_limits_[2]) return -1;
		return 2;
	}
	if(chr < B11110000) // B11101111
	{
		if(!s[1] || s[1] > B10111111) return -1;
		if(!s[2] || s[2] > B10111111) return -1;
		ret  = s[0] & B00001111; ret <<= 6;
		ret |= s[1] & B00111111; ret <<= 6;
		ret |= s[2] & B00111111;
		if(ret < _unicode_limits_[2] || ret >= _unicode_limits_[3]) return -1;
		return 3;
	}
	if(chr < B11111000) // B11110111
	{
		if(!s[1] || s[1] > B10111111) return -1;
		if(!s[2] || s[2] > B10111111) return -1;
		if(!s[3] || s[3] > B10111111) return -1;
		ret  = s[0] & B00000111; ret <<= 6;
		ret |= s[1] & B00111111; ret <<= 6;
		ret |= s[2] & B00111111; ret <<= 6;
		ret |= s[3] & B00111111;
		if(ret < _unicode_limits_[3] || ret >= _unicode_limits_[4]) return -1;
		return 4;
	}
	if(chr < B11111100) // B11111011
	{
		if(!s[1] || s[1] > B10111111) return -1;
		if(!s[2] || s[2] > B10111111) return -1;
		if(!s[3] || s[3] > B10111111) return -1;
		if(!s[4] || s[4] > B10111111) return -1;
		ret  = s[0] & B00000011; ret <<= 6;
		ret |= s[1] & B00111111; ret <<= 6;
		ret |= s[2] & B00111111; ret <<= 6;
		ret |= s[3] & B00111111; ret <<= 6;
		ret |= s[4] & B00111111;
		if(ret < _unicode_limits_[4] || ret >= _unicode_limits_[5]) return -1;
		return 5;
	}
	if(chr < B11111110) // B11111101
	{
		if(!s[1] || s[1] > B10111111) return -1;
		if(!s[2] || s[2] > B10111111) return -1;
		if(!s[3] || s[3] > B10111111) return -1;
		if(!s[4] || s[4] > B10111111) return -1;
		if(!s[5] || s[5] > B10111111) return -1;
		ret  = s[0] & B00000001; ret <<= 6;
		ret |= s[1] & B00111111; ret <<= 6;
		ret |= s[2] & B00111111; ret <<= 6;
		ret |= s[3] & B00111111; ret <<= 6;
		ret |= s[4] & B00111111; ret <<= 6;
		ret |= s[5] & B00111111;
		if(ret < _unicode_limits_[5] || ret >= _unicode_limits_[6]) return -1;
		return 6;
	}
	return -1;
}

int unicodeToUtf8(unsigned int chr, char* buffer, int len)
{
	if(chr >= 0x04000000)
	{
		if(len < 6) return -1;
		buffer[0] = 0xfc | (unsigned char)(chr>>30);		// 11111100
		buffer[1] = 0x80 | ((unsigned char)(chr>>24)&0x3f);	// 10000000 00111111
		buffer[2] = 0x80 | ((unsigned char)(chr>>18)&0x3f);	// 10000000 00111111
		buffer[3] = 0x80 | ((unsigned char)(chr>>12)&0x3f);	// 10000000 00111111
		buffer[4] = 0x80 | ((unsigned char)(chr>>6)&0x3f);	// 10000000 00111111
		buffer[5] = 0x80 | ((unsigned char)chr&0x3f);		// 10000000 00111111
		return 6;
	}
	else if(chr >= 0x00200000)
	{
		if(len < 5) return -1;
		buffer[0] = 0xf8 | (unsigned char)(chr>>24);		// 11111000
		buffer[1] = 0x80 | ((unsigned char)(chr>>18)&0x3f);	// 10000000 00111111
		buffer[2] = 0x80 | ((unsigned char)(chr>>12)&0x3f);	// 10000000 00111111
		buffer[3] = 0x80 | ((unsigned char)(chr>>6)&0x3f);	// 10000000 00111111
		buffer[4] = 0x80 | ((unsigned char)chr&0x3f);		// 10000000 00111111
		return 5;
	}
	else if(chr >= 0x00010000)
	{
		if(len < 4) return -1;
		buffer[0] = 0xf0 | (unsigned char)(chr>>18);		// 11110000
		buffer[1] = 0x80 | ((unsigned char)(chr>>12)&0x3f);	// 10000000 00111111
		buffer[2] = 0x80 | ((unsigned char)(chr>>6)&0x3f);	// 10000000 00111111
		buffer[3] = 0x80 | ((unsigned char)chr&0x3f);		// 10000000 00111111
		return 4;
	}
	else if(chr >= 0x00000800)
	{
		if(len < 3) return -1;
		buffer[0] = 0xe0 | (unsigned char)(chr>>12);		// 11100000
		buffer[1] = 0x80 | ((unsigned char)(chr>>6)&0x3f);	// 10000000 00111111
		buffer[2] = 0x80 | ((unsigned char)chr&0x3f);		// 10000000 00111111
		return 3;
	}
	else if(chr >= 0x00000080)
	{
		if(len < 2) return -1;
		buffer[0] = 0xc0 | (unsigned char)(chr>>6);			// 11000000
		buffer[1] = 0x80 | ((unsigned char)chr&0x3f);		// 10000000 00111111
		return 2;
	}
	else
	{
		if(len < 1) return -1;
		buffer[0] = 0x7f & (unsigned char)chr;				// 01111111
		return 1;
	}
}

/// IStringTranslator
IStringTranslator* IStringTranslator::m_globalTranslator = NULL;
IStringTranslator* IStringTranslator::GetGlobalTranslator()
{
	return m_globalTranslator;
}

IStringTranslator* IStringTranslator::SetGlobalTranslator(IStringTranslator* pTranslator)
{
	IStringTranslator* p = m_globalTranslator;
	m_globalTranslator = pTranslator;
	return p;
}

enum EnumLengthModifier
{
	enum_length_none,
	enum_length_h,
	enum_length_hh,
	enum_length_l,
	enum_length_ll,
	enum_length_L,
	enum_length_q,
	enum_length_j,
	enum_length_z,
	enum_length_t,
};
static size_t getSizeByModifier(EnumLengthModifier modifier, size_t len)
{
	switch(modifier)
	{
	case enum_length_none:	return len;
	case enum_length_h:		return sizeof(short);
	case enum_length_hh:	return sizeof(char);
	case enum_length_l:		return sizeof(long int);
	case enum_length_ll:	return sizeof(long long);
	case enum_length_L:		return sizeof(long double);
	case enum_length_q:		return sizeof(long long);
	case enum_length_j:		return sizeof(int);
	case enum_length_z:		return sizeof(size_t);
	case enum_length_t:		return sizeof(ptrdiff_t);
	}
	return len;
}
int scanFormatString(const char* fmt, char* nfmt, va_arg_decl argDecls[256])
{
	char chr;
	int ordinal;
	int normalOrdinal = 0;
	EnumLengthModifier lmodifier;

	size_t i = 0;
	while(chr=fmt[0], chr)
	{
		int n = kmax(mblen_k(fmt, 6), 1);
		if(n == 1 && chr == '%')
		{
			if(fmt[1] == '%')
			{
				*nfmt++ = fmt[0];
				*nfmt++ = fmt[1];
				fmt += 2;
				continue;
			}

			if(fmt[1] == '(')
			{
				*nfmt++ = fmt[0]; fmt += 2;
				ordinal = 0; lmodifier = enum_length_none;
				while(chr=fmt[0], chr>=48 && chr<58)
				{
					fmt += 1;
					ordinal *= 10;
					ordinal += chr-48;
				}
				if(fmt[0] != ')')
					return -1;
				fmt++;
				normalOrdinal = ordinal;
			}
			else
			{
				*nfmt++ = fmt[0]; fmt += 1;
				ordinal = ++normalOrdinal;
				lmodifier = enum_length_none;
			}

			/*fmt += 1;*/ int oi = (int)i;
			while(chr=fmt[0], chr && oi == i)
			{
				switch(chr)
				{
				// length modifier
				case 'h':
					if(lmodifier == enum_length_h) lmodifier = enum_length_hh;
					else lmodifier = enum_length_h;
					break;
				case 'l':
					if(lmodifier == enum_length_l) lmodifier = enum_length_ll;
					else lmodifier = enum_length_l;
					break;
				case 'L':
					lmodifier = enum_length_L;
					break;
				case 'q':
					lmodifier = enum_length_q;
					break;
				case 'j':
					lmodifier = enum_length_j;
					break;
				case 'z':
					lmodifier = enum_length_z;
					break;
				case 't':
					lmodifier = enum_length_t;
					break;
				// conversion specifier
				case 'c': // int or wint_t
				case 'C': // int or wint_t
				case 'd': // int
				case 'i': // int
				case 'o': // int
				case 'u': // int
				case 'x': // int
				case 'X': // int
					{
						va_arg_decl& arg = argDecls[i++];
						arg.ordinal = ordinal;
						arg.isFloatValue = 0;
						arg.argDataType = va_arg_decl::arg_integer;
						arg.size = (short)getSizeByModifier(lmodifier, sizeof(int));
						arg.offset = 0;
					} break;
				case 'e': // double
				case 'E': // double
				case 'f': // double
				case 'g': // double
				case 'G': // double
				case 'a': // double
				case 'A': // double
					{
						va_arg_decl& arg = argDecls[i++];
						arg.ordinal = ordinal;
						arg.isFloatValue = 1;
						arg.argDataType = va_arg_decl::arg_float;
						arg.size = sizeof(double); //(short)getSizeByModifier(lmodifier, sizeof(double));
						arg.offset = 0;
					} break;
				case 'n': // Pointer to integer 
				case 'p': // Pointer to void
					{
						va_arg_decl& arg = argDecls[i++];
						arg.ordinal = ordinal;
						arg.isFloatValue = 0;
						arg.argDataType = va_arg_decl::arg_pointer;
						arg.size = sizeof(void*); //(short)getSizeByModifier(lmodifier, sizeof(void*));
						arg.offset = 0;
					} break;
				case 's': // String 
				case 'S': // String
					{
						va_arg_decl& arg = argDecls[i++];
						arg.ordinal = ordinal;
						arg.isFloatValue = 0;
						arg.argDataType = va_arg_decl::arg_string;
						arg.size = (short)getSizeByModifier(lmodifier, sizeof(void*));
						arg.offset = 0;
					} break;
				default:
					break;
				}
				*nfmt++ = chr;
				fmt += 1;
			}
			continue;
		}
		for(int k=0; k<n; k++) *nfmt++ = fmt[k];
		fmt += n;
	}
	*nfmt = '\0';
	return (int)i;
}
static BOOL reArrangeValist(va_arg_decl* decls, size_t n, va_list args, char* nargs)
{
	char argBuffer[1024];
	va_arg_decl* declBuffer[256];
	::System::Collections::DynArray<va_arg_decl*> arr;
	arr.attach(declBuffer, 256, 0);

	for(size_t i=0; i<n; i++)
	{
		va_arg_decl& decl = decls[i];
		int ordinal = decl.ordinal;
		if(ordinal < 1 || ordinal > 256)
			return FALSE;
		arr[ordinal-1] = &decl;
	}

	size_t pos = 0;
	for(size_t i=0; i<n; i++)
	{
		va_arg_decl* decl = arr.at((int)i);
		if(!decl) return FALSE;
		if(decl->isFloatValue)
		{
			double val = va_arg(args, double);
			memcpy(argBuffer+pos, &val, sizeof(val));
		}
		else if(decl->size == sizeof(int))
		{
			int_r val = (int_r)va_arg(args, int);
			memcpy(argBuffer+pos, &val, sizeof(val));
		}
		else
		{
			long long val = va_arg(args, long long);
			memcpy(argBuffer+pos, &val, sizeof(val));
		}
		decl->offset = (int)pos;
		pos += kmax((size_t)decl->size, sizeof(int_r));
	}

	pos = 0;
	for(size_t i=0; i<n; i++)
	{
		va_arg_decl* decl = &decls[i];
		size_t size = kmax((size_t)decl->size, sizeof(int_r));
		memcpy(nargs+pos, &argBuffer[decl->offset], size);
		pos += size;
	}
	return TRUE;
}

int _vsnprintf_k(char* buf, size_t len, const char* fmt, va_list args)
{
	//return vsnprintf(buf, len, fmt, args);
	char nfmt[2048];
	char nargs[1024];
	va_arg_decl argDecls[256];
	int n = scanFormatString(fmt, nfmt, argDecls);
	if(n > 0)
	{
		va_list newArgs;
		if(!reArrangeValist(argDecls, n, args, nargs))
			return strcpy_k(buf, len, fmt);
		_init_va_list(&newArgs, &nargs);
		return vsnprintf(buf, len, nfmt, newArgs);
	}
	else
		return strcpy_k(buf, len, fmt);
}

int currentToLocal(const char* str, char* buf, size_t len)
{
	int length = 0;
#if defined(WIN32)
	if(g_encoding == encode_utf8 && validateUtf8String(str))
	{	
		unsigned int uchar;
		while(str[0])
		{
			int n = utf8ToUnicode(str, &uchar);
			if(n < 0) return -1; str += n;
			n = unicode_to_local(uchar, buf, (int)len);
			if(n < 0) return -1;
			buf += n; len -= n; length += n;
		}
	}
	else
	{
		while(str[0])
		{
			if(len < 2) return -1;
			*buf++ = *str++;
			length++; len--;
		}
	}
#else
	while(str[0])
	{
		if(len < 2) return -1;
		*buf++ = *str++;
		length++; len--;
	}
#endif
	*buf = '\0';
	return length;
}

int localToCurrent(const char* str, char* buf, size_t len)
{
	int length = 0;
#if defined(WIN32)
	if(g_encoding == encode_utf8 && !validateUtf8String(str))
	{
		int srclen = (int)strlen(str);
		length = local_to_utf8(str, srclen, buf, (int)len);
		if(length < 0) return -1;
		buf += length; len -= length;
	}
	else
	{
		while(str[0])
		{
			if(len < 2) return -1;
			*buf++ = *str++;
			length++; len--;
		}
	}
#else
	while(str[0])
	{
		if(len < 2) return -1;
		*buf++ = *str++;
		length++; len--;
	}
#endif
	ASSERT(len > 0); *buf = '\0';
	return length;
}

int tobstr(int n, int width, char* buf, int len)
{
	int shift = 1;
	width = kmin(width, 32);
	for(int i=0; i<32; i++)
	{
		if(n & (shift<<i))
		{
			if(i >= len) return -1;
			buf[31-i] = '1';
		}
		else
		{
			if(i < len) buf[31-i] = '0';
		}
	}
	int src = 32 - width;
	memmove(buf, buf+src, width);
	buf[width] = '\0';
	return width;
}

bool validateUtf8String(const char* s)
{
	int n; unsigned int uchr; char tmp[8];
	while(n = utf8ToUnicode(s,&uchr), n > 0)
	{
#if defined(WIN32)
		if(n > 1 && unicode_to_local(uchr, tmp, sizeof(tmp)) < 0)
			return false;
#endif
		s += n;
	}
	return n == 0;
}

// 假定当前 locale 是 gbk: setlocale(LC_CTYPE, "zh_CN.GB18030")
int local_to_utf8(const char* src, int srclen, char* dst, int dstlen)
{
	KCharset* cs = KCharset::getLocalCharset();
	return cs->to_utf8(&src, &srclen, &dst, &dstlen);
}

int utf8_to_local(const char* src, int srclen, char* dst, int dstlen)
{
	KCharset* cs = KCharset::getLocalCharset();
	return cs->from_utf8(&src, &srclen, &dst, &dstlen);
}

BOOL _is_utf8_string(const char* s)
{
	int n; unsigned int uchr;
	while(n = utf8ToUnicode(s, &uchr), n > 0)
	{
		if(n > 1) return FALSE;
		s += n;
	}
	return n == 0;
}

BOOL _is_gbk_string(const char* s)
{
	unsigned int uchr;
	while(s[0])
	{
		int n = utf8ToUnicode(s, &uchr);
		if(n < 0) return TRUE;
		if(n > 1) return FALSE;
		s += n;
	}
	return TRUE;
}

int unicode_to_local(unsigned int uchr, char* dst, int dstlen)
{
	KCharset* cs = KCharset::getLocalCharset();
	return cs->from_uchar(uchr, dst, dstlen);
}

int local_to_unicode(const char* src, int srclen, unsigned int* uchar)
{
	KCharset* cs = KCharset::getLocalCharset();
	return cs->to_uchar(src, srclen, uchar);
}

int utf8_to_local(const char* src, char* dst, int dstlen)
{
	char tmp[8];
	unsigned int uchr;
	int count = 0;
	const char* pc = src;
	while(*pc)
	{
		int n = utf8ToUnicode(pc, &uchr);
		if(n < 0) return -1; pc += n;
		n = unicode_to_local(uchr, tmp, sizeof(tmp));
		if(n < 0) return -1;
		if(dstlen < n) return -1;
		memcpy(dst, tmp, n);
		count += n; dst += n; dstlen -= n;
		if(dstlen < 1) break;
	}
	if(dstlen > 0) *dst = '\0';
	return count;
}

extern "C" int local_to_utf8_xml(const char **srcP, int srclen, char **dstP, int dstlen)
{
	KCharset* cs = KCharset::getLocalCharset();
	return cs->to_utf8(srcP, &srclen, dstP, &dstlen);
}

extern "C" int utf8_to_local_xml(const char **srcP, int srclen, char **dstP, int dstlen)
{
	KCharset* cs = KCharset::getLocalCharset();
	return cs->from_utf8(srcP, &srclen, dstP, &dstlen);
}

int utf16be_to_utf8(const char* src, int srclen, char* dst, int dstlen)
{
	unsigned int uchar;
	char ctmp[8]; int count = 0;
	while(uchar = (((unsigned int)(BYTE)src[0])<<8) + (unsigned int)(BYTE)src[1], uchar && srclen > 0)
	{
		int n = unicodeToUtf8(uchar, ctmp, sizeof(ctmp));
		if(n < 0) return -1; if(dstlen < n) return -1;
		memcpy(dst, ctmp, n);
		src += sizeof(wchar_16); srclen -= sizeof(wchar_16); dst+= n; dstlen -= n; count += n;
	}
	if(dstlen > 0) *dst = '\0';
	return count;
}

int utf16le_to_utf8(const char* src, int srclen, char* dst, int dstlen)
{
	unsigned int uchar;
	char ctmp[8]; int count = 0;
	while(uchar=*(wchar_16*)src, uchar && srclen > 0)
	{
		int n = unicodeToUtf8(uchar, ctmp, sizeof(ctmp));
		if(n < 0) return -1; if(dstlen < n) return -1;
		memcpy(dst, ctmp, n);
		src += sizeof(wchar_16); srclen -= sizeof(wchar_16); dst+= n; dstlen -= n; count += n;
	}
	if(dstlen > 0) *dst = '\0';
	return count;
}

int utf16be_to_local(const char* src, int srclen, char* dst, int dstlen)
{
	KCharset* cs = KCharset::getLocalCharset();
	unsigned int uchar;
	char ctmp[8]; int count = 0;
	while(uchar = (((unsigned int)(BYTE)src[0])<<8) + (unsigned int)(BYTE)src[1], uchar && srclen > 0)
	{
		int n = cs->from_uchar(uchar, ctmp, sizeof(ctmp));
		if(n < 0) return -1; if(dstlen < n) return -1;
		memcpy(dst, ctmp, n);
		src += sizeof(wchar_16); srclen -= sizeof(wchar_16); dst+= n; dstlen -= n; count += n;
	}
	if(dstlen > 0) *dst = '\0';
	return count;
}

int utf16le_to_local(const char* src, int srclen, char* dst, int dstlen)
{
	unsigned int uchar;
	KCharset* cs = KCharset::getLocalCharset();
	char ctmp[8]; int count = 0;
	while(uchar=*(wchar_16*)src, uchar && srclen > 0)
	{
		int n = cs->from_uchar(uchar, ctmp, sizeof(ctmp));
		if(n < 0) return -1; if(dstlen < n) return -1;
		memcpy(dst, ctmp, n);
		src += sizeof(wchar_16); srclen -= sizeof(wchar_16); dst+= n; dstlen -= n; count += n;
	}
	if(dstlen > 0) *dst = '\0';
	return count;
}
