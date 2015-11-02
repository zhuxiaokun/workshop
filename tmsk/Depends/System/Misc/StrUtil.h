#pragma once
#include "../Collections/KString.h"
#include "../Collections/DynArray.h"

typedef char* KCharPtr;
typedef JG_C::KString<512> KSTRING;
typedef JG_C::DynArray<KCharPtr,64,64> KStringList;

// ordinal and data size, and arg data address
struct va_arg_decl
{
	enum { arg_integer,arg_float,arg_string,arg_pointer };
	int ordinal;
	unsigned char isFloatValue;
	unsigned char argDataType;
	short size;
	int offset;
};

char* _trim(char* s);
const char* eatup(const char* pc, const char* delimiters);
int str2int(const char* pc);
INT64 str2i64(const char* pc);

bool isnumchar(char ch);
bool ishexchar(char ch);
bool isdigitchar(char ch);

bool isnumber3(const char* pc);
bool isfloat(const char* pc);
::System::Collections::KString<512> strerror_r2(int errnum);

// 会对输入的源字符串 s 进行修改
int split(char* s, const char* delimiters, KStringList& sl);
int split(char* s, char delimiter, KStringList& sl);
int split(char* s, char delimiter, char* ss[], int maxs);
int split(char* s, const char* delimiters, char* ss[], int maxs);

// 假定 s 不是NULL
BOOL CheckStrLength(const char* s, int minLength, int maxLength);

int stristr(const char* str, const char* pattern);
BOOL strrep(char* str, int buflen, int beg, int count, const char* ns);

//大小写转换
void toUpperCase(char * str);
void toLowerCase(char * str);

// HEX string
int tohexstr(const void* data, size_t len, char* buf, size_t size);
int reversehexstr(const char* str, size_t len, void* buf, size_t size);

// 编码转换
int utf8charsize(const char* s);
int utf8charcount(const char* s);
int mblen_k(const char* s, size_t n);

extern "C" int utf8ToUnicode(const char* s, unsigned int* uchr);
extern "C" int unicodeToUtf8(unsigned int chr, char* buffer, int len);

int scanFormatString(const char* fmt, char* nfmt, va_arg_decl argDecls[256]);
int _vsnprintf_k(char* buf, size_t len, const char* fmt, va_list args);

// current program encoding string to local platform encoding string
extern "C" int currentToLocal(const char* str, char* buf, size_t len);
// local platform encoding string to current program encoding string
extern "C" int localToCurrent(const char* str, char* buf, size_t len);

// stripaddr : 192.168.10.250:3000
struct KIpAddr
{
	DWORD ip;
	WORD port;
	short valid;
	KIpAddr() : ip(0),port(0),valid(0) { }
	BOOL isValid() const { return valid; }
};
KIpAddr str2ipaddr(const char* stripaddr);

class IStringTranslator
{
public:
	virtual ~IStringTranslator() { }
	virtual const char* Translate(const char* str) = 0;

public:
	static IStringTranslator* GetGlobalTranslator();
	// return original translator
	static IStringTranslator* SetGlobalTranslator(IStringTranslator* pTranslator);

private:
	static IStringTranslator* m_globalTranslator;
};

int tobstr(int n, int width, char* buf, int len);
bool validateUtf8String(const char* s);

// 返回写入dst的字节数
int unicode_to_local(unsigned int uchar, char *dst, int dstlen);
// 返回消耗的src字节数
int local_to_unicode(const char *src, int srclen, unsigned int *uchar);

// 假定当前 locale 是 gbk: setlocale(LC_CTYPE, "zh_CN.GB18030")
extern "C" int local_to_utf8(const char* src, int srclen, char* dst, int dstlen);
extern "C" int utf8_to_local(const char* src, int srclen, char* dst, int dstlen);

int utf16be_to_utf8(const char* src, int srclen, char* dst, int dstlen);
int utf16le_to_utf8(const char* src, int srclen, char* dst, int dstlen);
int utf16be_to_local(const char* src, int srclen, char* dst, int dstlen);
int utf16le_to_local(const char* src, int srclen, char* dst, int dstlen);

int utf8_to_local(const char* src, char* dst, int dstlen);

// 仅用于 gbk locale 的上下文中
BOOL _is_utf8_string(const char* s);
BOOL _is_gbk_string(const char* s);

template <size_t n> ::System::Collections::KString<n> localToUtf8String(const char* src)
{
	char buf[n];
	typedef JG_C::KString<n> string_type;
	int len = local_to_utf8(src, (int)strlen(src), buf, n);
	return len > 0 ? string_type(buf, len) : string_type();
}

template <size_t n> ::System::Collections::KString<n> utf8ToLocalString(const char* src)
{
	char buf[n];
	typedef JG_C::KString<n> string_type;
	int len = utf8_to_local(src, buf, n);
	return len > 0 ? string_type(buf, len) : string_type();
}
