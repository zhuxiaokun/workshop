#include "Version.h"

#define _dword_2_byte_arr_(x) \
	(x&(0xff)), ((x&0xff00)>>8), ((x&0x0ff0000)>>16), ((x&0xff000000)>>24)

static const char gscPrefix[52+8] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
									_dword_2_byte_arr_(_mainVersion),
									_dword_2_byte_arr_(_subVersion),
									'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

const unsigned int& gscMainVersion = *(const unsigned int*)&gscPrefix[26];
const unsigned int& gscSubVersion = *(const unsigned int*)&gscPrefix[26+4];