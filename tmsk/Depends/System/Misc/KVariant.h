#ifndef _K_VARIANT_H_
#define _K_VARIANT_H_

#include "../KType.h"

struct KVariant
{
	enum
	{
		vt_null,		// 没有值
		vt_int32,
		vt_int64,
		vt_float32,
		vt_float64,
		vt_pointer,
		vt_string,
		vt_count
	} vt;

	union
	{
		int    valInt32;
		INT64  valInt64;
		float  valFloat32;
		double valFloat64;
		void*  valPointer;
		char*  valString;
	};

	void (*release)(void*); // 如果是指针值或者字符串值时，提供释放的功能

	KVariant():vt(vt_null),release(NULL) {}
	~KVariant()
	{
		if(release)
		{
			if(vt == vt_pointer)
			{
				release(valPointer);
			}
			else if(vt == vt_string)
			{
				release(valString);
			}
		}
	}

	void Clear()
	{
		if(release)
		{
			if(vt == vt_pointer)
			{
				release(valPointer);
			}
			else if(vt == vt_string)
			{
				release(valString);
			}
		}
		vt = vt_null;
		release = NULL;
	}

	BOOL IsNull() const        { return vt == vt_null;   }
	void Set(int val)          { vt = vt_int32;   valInt32   = val; }
	void Set(INT64 val)        { vt = vt_int64;   valInt64   = val; }
	void Set(float val)        { vt = vt_float32; valFloat32 = val; }
	void Set(double val)       { vt = vt_float64; valFloat64 = val; }
	void Set(void* val)        { vt = vt_pointer; valPointer = val; }
	void Set(char* val)        { vt = vt_string;  valString  = val; }
};

#endif