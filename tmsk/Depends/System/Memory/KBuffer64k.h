#ifndef _K_BUFFER64K_H_
#define _K_BUFFER64K_H_

#include "RingBuffer.h"
#include "KStepObjectPool.h"
#include "RingBuffer.h"

class KBuffer64k
	: public KPieceBuffer<64*1024>
	, public ::System::Memory::KPortableStepPool<KBuffer64k,16>
{
public:
	static char* WriteData(const void* pData, size_t len);

public:
	typedef ::System::Collections::DynArray<KBuffer64k*> KBufferArray;
	static KBufferArray m_bufferArray;
};

#endif