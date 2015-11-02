#ifndef _STREAM_INTERFACE_H_
#define _STREAM_INTERFACE_H_

#include "../KType.h"

class StreamInterface
{
public:
	virtual ~StreamInterface() {}
	virtual int_r ReadData(void* value, size_t length) = 0;
	virtual int_r WriteData(const void* value, size_t length) = 0;
	virtual BOOL Seek(size_t pos) = 0;
	virtual size_t tell() const = 0;
	virtual void Close() = 0;
	virtual void flush() = 0;
};

#endif