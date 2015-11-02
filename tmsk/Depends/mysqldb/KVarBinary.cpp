#include "KVarBinary.h"
#include "KTableCache.h"
#include <System/File/KFile.h>
#include <System/Misc/StrUtil.h>

using namespace System::Memory;
using namespace System::File;

// KMMapRecordset ///////////////////////////////////////

KMmapRecordset::KMmapRecordset()
{

}

KMmapRecordset::~KMmapRecordset()
{
	this->finalize();
}

bool KMmapRecordset::initialize(KVarBinaryMemory& vbm, int fno)
{
	int actualRecSize = sizeof(Record) + vbm.m_recSize;
	actualRecSize = (actualRecSize+sizeof(long)-1)&(~(sizeof(long)-1));

	char fname[512];
	this->_getName(vbm, fno, fname);
	DWORD len = sizeof(RecordsetHeader) + actualRecSize * vbm.m_stepCapacity;
	if(!m_mmap.Initialize(fname, 0, len, FALSE))
	{
		Log(LOG_ERROR, "error: initialize KMMapRecordset(recSize:%d,capacity:%d,fname:%s)", actualRecSize, vbm.m_stepCapacity, fname);
		return false;
	}

	KMMap::eFileOpenStatus openStatus = m_mmap.GetOpenStatus();
	RecordsetHeader* pHeader = (RecordsetHeader*)m_mmap.GetMemory();
	switch(openStatus)
	{
	case KMMap::enum_CreateNew:
	case KMMap::enum_ModifyExists:
		{
			pHeader->usedCount = 0;
			pHeader->firstFree = invalid_record_no;
			pHeader->recSize = actualRecSize;
			pHeader->capacity = vbm.m_stepCapacity;
		} break;
	};

	m_mrs.Attach(*pHeader, pHeader+1);
	return true;
}

void KMmapRecordset::finalize()
{
	m_mrs.Detach();
	m_mmap.Finalize();
}

void KMmapRecordset::clear()
{
	m_mrs.Clear();
}

const char* KMmapRecordset::_getName(KVarBinaryMemory& vbm, int fno, char* buf)
{
	char dir[512];
	char cleanName[128];

	if(!fno)
	{
		strcpy(buf, vbm.m_fname);
	}
	else
	{
		KFileUtil::dirname_r(vbm.m_fname, dir);
		const char* base = KFileUtil::basename_r(vbm.m_fname);
		KFileUtil::cleanname_r(base, cleanName);
		const char* suffix = strchr(base, '.');
		if(!suffix) suffix = "";
		sprintf(buf, "%s/%s_%d%s", dir, cleanName, fno, suffix);
	}

	return buf;
}

KMmapRecordset::Record* KMmapRecordset::_handle2Rec(HANDLE hRec)
{
	ASSERT(hRec != INVALID_HREC);
	return (Record*)m_mrs.HandleFromRecNo((int)(uint_r)hRec);
}

HANDLE KMmapRecordset::_allocRecord()
{
	HANDLE h = m_mrs.AllocRecord();
	if(!h) return INVALID_HREC;

	HANDLE hRec = (HANDLE)m_mrs.RecNoFromHandle(h);
	Record* pRec = this->_handle2Rec(hRec);

	pRec->usedTag = 1;
	pRec->next = invalid_record_no;
	pRec->actualSize = 0;

	return hRec;
}

void KMmapRecordset::_freeRecord(HANDLE hRec)
{
	if(hRec == INVALID_HREC) return;
	Record* pRec = this->_handle2Rec(hRec);
	m_mrs.FreeRecord(pRec);
}

// KVarBinaryMemory ///////////////////////////////////////////////

KVarBinaryMemory::KVarBinaryMemory()
{

}

KVarBinaryMemory::~KVarBinaryMemory()
{
	this->finalize();
}

bool KVarBinaryMemory::initialize(const char* fname, int pieceSize, int capacity)
{
	char fpath[512];
	getfullpath(fpath, fname, sizeof(fpath));
	
	int i = 0;
	while(fpath[i])
	{
		if(fpath[i] == '\\')
			fpath[i] = '/';
		i += 1;
	}

	ASSERT(m_mrsArray.empty());

	char semName[512];
	strcpy(semName, fpath);
	i = 1; while(semName[i])
	{
		if(semName[i] == '/') semName[i] = '_';
		i++;
	}
	
	if(!m_mx.Initialize(semName))
	{
		printf_k("error: create ProcessMutex '%s', %s\n", semName, strerror_r2(GetLastError()).c_str());
		ASSERT(m_mx.Initialize(semName));
	}

	strcpy(m_fname, fname);
	m_recSize = (pieceSize+sizeof(long)-1) & (~(sizeof(long)-1));
	m_stepCapacity = capacity;

	if(!this->initCtrlFile())
		return false;
	
	return true;
}

void KVarBinaryMemory::finalize()
{
	KAutoProcessMutex mx(m_mx);

	int n = m_mrsArray.size();
	for(int i=0; i<n; i++)
	{
		KMmapRecordset* mrs = m_mrsArray[i];
		mrs->finalize();
		delete mrs;
	}

	m_mrsArray.clear();

	if(m_ctrlFile)
	{
		m_ctrlMmap.Finalize();
		m_ctrlFile = NULL;
	}
}

void KVarBinaryMemory::clear()
{
	char fn[512];
	int n = m_mrsArray.size();
	for(int i=0; i<n; i++)
	{
		KMmapRecordset* mrs = m_mrsArray[i];
		mrs->_getName(*this, i, fn);
		mrs->finalize();
		remove(fn);
	}

	m_ctrlFile->fileCount = 0;
	//m_ctrlMmap.Finalize();
	//this->_getCtrlFname(fn);
	//remove(fn);
	return;
}

void KVarBinaryMemory::clearDataOnly()
{
	int n = m_mrsArray.size();
	for(int i=0; i<n; i++)
	{
		KMmapRecordset* mrs = m_mrsArray[i];
		mrs->clear();
	}
	return;
}

HANDLE KVarBinaryMemory::allocMemory(int size)
{
	m_tmpHarray.clear();
	int n = (size+m_recSize-1)/m_recSize;

	KAutoProcessMutex mx(m_mx);

	for(int i=0; i<n; i++)
	{
		HANDLE h = this->_allocOneRecord();
		if(h == INVALID_HREC)
		{
			this->_freeTmpArray(0);
			return INVALID_HREC;
		}
		m_tmpHarray.push_back(h);
	}

	this->_linkTmpArray(0);
	return m_tmpHarray[0];
}

HANDLE KVarBinaryMemory::reallocMemory(HANDLE hRec, int size)
{
	KAutoProcessMutex mx(m_mx);
	return this->_reallocMemory(hRec, size);
}

HANDLE KVarBinaryMemory::assureMemory(HANDLE hRec, int size)
{
	KAutoProcessMutex mx(m_mx);
	this->_syncWithCtrlFile();
	int n = this->_link2TmpArray(hRec);
	int capacity = n * m_recSize;
	if(size <= capacity) return hRec;
	return this->_reallocMemory(hRec, size);
}

bool KVarBinaryMemory::checkMemory(HANDLE hRec)
{
	KAutoProcessMutex mx(m_mx);

	this->_syncWithCtrlFile();
	int n = this->_link2TmpArray(hRec);
	for(int i=0; i<n; i++)
	{
		HANDLE h = m_tmpHarray[i];
		if(!this->_checkOneHandle(h))
			return false;
	}

	return true;
}

int KVarBinaryMemory::getDataSize(HANDLE hRec)
{
	KAutoProcessMutex mx(m_mx);

	this->_syncWithCtrlFile();
	int n = this->_link2TmpArray(hRec);

	int size = 0;
	for(int i=0; i<n; i++)
	{
		HANDLE h = m_tmpHarray[i];
		Record* pRec = this->_handle2Record(h);
		size += pRec->actualSize;
	}

	return size;
}

void KVarBinaryMemory::freeMemory(HANDLE hRec)
{
	this->reallocMemory(hRec, 0);
}

bool KVarBinaryMemory::_setValue(HANDLE hRec, const void* val, int len)
{
	int n = this->_link2TmpArray(hRec);
	ASSERT(n*m_recSize >= len);

	int pos = 0;
	const char* p = (const char*)val;

	for(int i=0; i<n; i++)
	{
		HANDLE h = m_tmpHarray[i];
		Record* pRec = this->_handle2Record(h);
		int bytes = len < m_recSize ? len : m_recSize;
		this->_setRecordValue(pRec, p+pos, bytes);
		pos += bytes; len -= bytes;
		if(len < 1) break;
	}

	return true;
}

bool KVarBinaryMemory::_setValue(HANDLE hRec, int pos, const void* val, int len)
{
	int n = this->_link2TmpArray(hRec);
	int capacity = n * m_recSize;
	ASSERT(pos+len <= capacity);

	const char* p = (const char*)val;
	int from_n = pos / m_recSize;

	int writes = 0;
	pos = pos % m_recSize;

	for(int i=from_n; i<n; i++)
	{
		HANDLE h = m_tmpHarray[i];
		Record* pRec = this->_handle2Record(h);
		int bytes = len < m_recSize ? len : m_recSize;
		if(pos)
		{
			this->_setRecordValue(pRec, pos, p+writes, bytes);
			pos = 0;
		}
		else
		{
			this->_setRecordValue(pRec, p+writes, bytes);
		}
		len -= bytes; writes += bytes;
		if(len < 1) break;
	}
	return true;
}

int KVarBinaryMemory::_getValue(HANDLE hRec, void* val, int len)
{
	int n = this->_link2TmpArray(hRec);
	if(!n) return 0;

	int bytes = 0;
	char* p = (char*)val;

	for(int i=0; i<n; i++)
	{
		HANDLE h = m_tmpHarray[i];
		Record* pRec = this->_handle2Record(h);
		int l = len <= pRec->actualSize ? len : pRec->actualSize;
		memcpy(p+bytes, &pRec->data, l);
		bytes += l; len -= l;
		if(len < 1) break;
	}

	return bytes;
}

int KVarBinaryMemory::_getValue(HANDLE hRec, int pos, void* val, int len)
{
	int n = this->_link2TmpArray(hRec);
	if(!n) return 0;

	int bytes = 0;
	char* p = (char*)val;

	int from_n = pos / m_recSize;
	pos = pos % m_recSize;

	for(int i=from_n; i<n; i++)
	{
		HANDLE h = m_tmpHarray[i];
		Record* pRec = this->_handle2Record(h);

		int l;
		if(pos)
		{
			l = pRec->actualSize - pos;
			l = len <= l ? len : l;
			memcpy(p+bytes, &pRec->data[pos], l);
			pos = 0;
		}
		else
		{
			l = len <= pRec->actualSize ? len : pRec->actualSize;
			memcpy(p+bytes, &pRec->data, l);
		}
		
		bytes += l; len -= l;
		if(len < 1) break;
	}

	return bytes;
}

HANDLE KVarBinaryMemory::_reallocMemory(HANDLE hRec, int size)
{
	this->_syncWithCtrlFile();

	int n = this->_link2TmpArray(hRec);
	int want_n = (size+m_recSize-1)/m_recSize;
	if(n == want_n) return hRec;

	if(n > want_n)
	{// free some pieces
		this->_freeTmpArray(want_n);
	}
	else
	{// alloc some piece
		for(int i=n; i<want_n; i++)
		{
			HANDLE h = this->_allocOneRecord();
			if(h == INVALID_HREC)
			{
				this->_freeTmpArray(want_n);
				return INVALID_HREC;
			}
			m_tmpHarray.push_back(h);
		}
		int beg = n > 0 ? n-1 : 0;
		this->_linkTmpArray(beg);
	}

	return want_n ? m_tmpHarray[0] : INVALID_HREC;
}

void KVarBinaryMemory::_setRecordValue(Record* pRec, const void* p, int len)
{
	ASSERT(len <= m_recSize);
	memcpy(&pRec->data, p, len);
	pRec->actualSize = len;
}

void KVarBinaryMemory::_setRecordValue(Record* pRec, int pos, const void* p, int len)
{
	ASSERT(pos+len <= m_recSize);
	memcpy(&pRec->data[pos], p, len);
	if(pRec->actualSize < pos+len)
	{
		pRec->actualSize = pos + len;
	}
}

KMmapRecordset* KVarBinaryMemory::_createRecordset(int fileno, bool sync)
{
	KMmapRecordset* prs = new KMmapRecordset(); ASSERT(prs);
	ASSERT(prs->initialize(*this, fileno));
	m_mrsArray[fileno] = prs;
	if(!sync) m_ctrlFile->fileCount++;
	return prs;
}

KVarBinaryMemory::Record* KVarBinaryMemory::_handle2Record(HANDLE hRec)
{
	KMmapRecordset* prs = NULL;
	HANDLE subHandle = this->_subHandle(hRec, &prs);
	if(!prs) return NULL;
	return prs->_handle2Rec(subHandle);
}

HANDLE KVarBinaryMemory::_allocOneRecord()
{
	this->_syncWithCtrlFile();
	int n = m_mrsArray.size();

	for(int i=0; i<n; i++)
	{
		KMmapRecordset* prs = m_mrsArray[i];
		HANDLE h = prs->_allocRecord();
		if(h != INVALID_HREC)
		{
			return this->_uniHandle(h, i);
		}
	}
	{
		KMmapRecordset* prs = this->_createRecordset(n, false);
		ASSERT(prs);

		HANDLE h = prs->_allocRecord();
		if(h == INVALID_HREC) return INVALID_HREC;
		return this->_uniHandle(h, n);
	}
}

void KVarBinaryMemory::_freeOneRecord(HANDLE hRec)
{
	this->_syncWithCtrlFile();
	
	KMmapRecordset* mrs = NULL;
	HANDLE subHandle = this->_subHandle(hRec, &mrs);
	if(!mrs)
	{
		Log(LOG_ERROR, "error: KVarBinaryMemory::_freeOneRecord(%d)", (int)(uint_r)hRec);
		return;
	}

	mrs->_freeRecord(subHandle);
}

bool KVarBinaryMemory::_checkOneHandle(HANDLE hRec)
{
	if(hRec == INVALID_HREC)
		return false;

	int recNo = (int)(uint_r)hRec;
	if(recNo < 0) return false;
	
	int n = m_mrsArray.size();
	int mrs_n = recNo/m_stepCapacity;
	if(mrs_n >= n) return false;

	int subNo = recNo % m_stepCapacity;
	KMmapRecordset* prs = m_mrsArray[mrs_n];
	if(!prs) return false;

	Record* pRec = prs->_handle2Rec((HANDLE)subNo);
	if(!pRec->usedTag) return false;

	return true;
}

HANDLE KVarBinaryMemory::_linkTmpArray(int beg)
{
	int n = m_tmpHarray.size();
	ASSERT(beg < n);

	for(int i=beg; i<n-1; i++)
	{
		HANDLE h = m_tmpHarray[i];
		HANDLE nexth = m_tmpHarray[i+1];
		Record* pRec = this->_handle2Record(h);
		pRec->next = (int)(uint_r)nexth;
	}
	{
		HANDLE h = m_tmpHarray[n-1];
		Record* pRec = this->_handle2Record(h);
		pRec->next = invalid_record_no;
	}

	return m_tmpHarray[beg];
}

int KVarBinaryMemory::_link2TmpArray(HANDLE hRec)
{
	m_tmpHarray.clear();
	while(hRec != INVALID_HREC)
	{
		m_tmpHarray.push_back(hRec);
		Record* pRec = this->_handle2Record(hRec);
		hRec = (HANDLE)pRec->next;
	}
	return m_tmpHarray.size();
}

void KVarBinaryMemory::_freeTmpArray(int beg)
{
	int n = m_tmpHarray.size();
	
	if(beg > 0)
	{
		HANDLE h = m_tmpHarray[beg-1];
		Record* pRec = this->_handle2Record(h);
		pRec->next = invalid_record_no;
	}

	for(int i=beg; i<n; i++)
	{
		HANDLE h = m_tmpHarray[i];
		this->_freeOneRecord(h);
	}

	return;
}

HANDLE KVarBinaryMemory::_subHandle(HANDLE hRec, KMmapRecordset** pp)
{
	int recNo = (DWORD)(uint_r)hRec;

	int n = m_mrsArray.size();
	int mrs_n = recNo/m_stepCapacity;
	ASSERT(mrs_n < n);
	
	int subNo = recNo % m_stepCapacity;
	if(pp) *pp = m_mrsArray[mrs_n];
	return (HANDLE)subNo;
}

HANDLE KVarBinaryMemory::_uniHandle(HANDLE hRec, int fno)
{
	DWORD n = (DWORD)(uint_r)hRec;
	return (HANDLE)(n+fno*m_stepCapacity);
}

const char* KVarBinaryMemory::_getCtrlFname(char* buf)
{
	char dir[512];
	char cleanName[128];

	KFileUtil::dirname_r(m_fname, dir);
	const char* base = KFileUtil::basename_r(m_fname);
	KFileUtil::cleanname_r(base, cleanName);
	const char* suffix = strchr(base, '.');
	if(!suffix) suffix = "";

	sprintf(buf, "%s/%s_ctrl%s", dir, cleanName, suffix);
	return buf;
}

bool KVarBinaryMemory::_syncWithCtrlFile()
{
	int n = m_mrsArray.size();
	if(n < m_ctrlFile->fileCount)
	{
		for(int i=n; i<m_ctrlFile->fileCount; i++)
		{
			this->_createRecordset(i, true);
		}
	}	
	return true;
}

bool KVarBinaryMemory::initCtrlFile()
{
	char fname[512];
	this->_getCtrlFname(fname);

	KAutoProcessMutex mx(m_mx);

	if(!m_ctrlMmap.Initialize(fname, 0, sizeof(CtrlFile), FALSE))
	{
		Log(LOG_ERROR, "error: initialize ctrl file %s", fname);
		return false;
	}

	m_ctrlFile = (CtrlFile*)m_ctrlMmap.GetMemory();

	KMMap::eFileOpenStatus openStatus = m_ctrlMmap.GetOpenStatus();
	if(openStatus == KMMap::enum_CreateNew)
	{
		m_ctrlFile->fileCount = 0;
	}
	
	return true;
}

// KVarBinary /////////////////////////////////////////////

KVarBinary::KVarBinary()
	: m_vbm(NULL)
	, m_hRec(INVALID_HREC)
	, m_hptr(&m_hRec)
{

}

KVarBinary::KVarBinary(const KVarBinary& o)
	: m_vbm(o.m_vbm)
	, m_hRec(o.m_hRec)
	, m_hptr(&m_hRec)
{
	if(o.m_hptr != &o.m_hRec)
	{
		m_hptr = o.m_hptr;
	}
}

KVarBinary::KVarBinary(KVarBinaryMemory& vbm)
	: m_vbm(&vbm)
	, m_hRec(INVALID_HREC)
	, m_hptr(&m_hRec)
{

}

KVarBinary::KVarBinary(KVarBinaryMemory& vbm, HANDLE hRec)
	: m_vbm(NULL)
	, m_hRec(INVALID_HREC)
	, m_hptr(&m_hRec)
{
	this->attach(vbm, hRec);
}

KVarBinary::KVarBinary(KVarBinaryMemory& vbm, HANDLE* hptr)
	: m_vbm(&vbm)
	, m_hRec(INVALID_HREC)
	, m_hptr(hptr)
{
	this->attach(vbm, hptr);
}

KVarBinary::~KVarBinary()
{

}

KVarBinary& KVarBinary::operator = (const KVarBinary& o)
{
	if(this == &o) return *this;
	
	m_vbm = o.m_vbm;
	m_hRec = o.m_hRec;

	if(o.m_hptr == &o.m_hRec)
	{	m_hptr = &m_hRec;	}
	else
	{	m_hptr = o.m_hptr;	}
	
	return *this;
}

bool KVarBinary::isValid() const
{
	return m_vbm != NULL;
}

bool KVarBinary::hasData() const
{
	return this->_handle() != INVALID_HREC;
}

bool KVarBinary::checkMemory()
{
	if(this->_handle() == INVALID_HREC) return true;
	return m_vbm->checkMemory(this->_handle());
}

void KVarBinary::attach(KVarBinaryMemory& vbm)
{
	this->attach(vbm, INVALID_HREC);
}

void KVarBinary::attach(KVarBinaryMemory& vbm, HANDLE hRec)
{
	m_vbm = &vbm;
	m_hRec = hRec;
	m_hptr = &m_hRec;
	if(this->_handle() != INVALID_HREC)
	{
		m_vbm->assureMemory(this->_handle(), 0);
	}
}

void KVarBinary::attach(KVarBinaryMemory& vbm, HANDLE* hptr)
{
	m_vbm = &vbm;
	m_hptr = hptr;
	if(this->_handle() != INVALID_HREC)
	{
		m_vbm->assureMemory(this->_handle(), 0);
	}
}

void KVarBinary::clear()
{
	if(this->_handle() != INVALID_HREC)
	{
		m_vbm->freeMemory(this->_handle());
		this->_setHandle(INVALID_HREC);
	}
}

bool KVarBinary::setValue(const void* val, int len)
{
	if(len < 1)
	{
		this->clear();
		return true;
	}

	HANDLE h = m_vbm->reallocMemory(this->_handle(), len);
	if(h == INVALID_HREC) return false;

	this->_setHandle(h);
	return m_vbm->_setValue(this->_handle(), val, len);
}

bool KVarBinary::setValue(int pos, const void* val, int len)
{
	this->_setHandle(m_vbm->assureMemory(this->_handle(), pos+len));
	return m_vbm->_setValue(this->_handle(), pos, val, len);
}

int KVarBinary::getValue(void* val, int len)
{
	if(this->_handle() == INVALID_HREC) return 0;
	return m_vbm->_getValue(this->_handle(), val, len);
}

int KVarBinary::getValue(int pos, void* val, int len)
{
	if(this->_handle() == INVALID_HREC) return 0;
	return m_vbm->_getValue(this->_handle(), pos, val, len);
}

int KVarBinary::size() const
{
	if(this->_handle() == INVALID_HREC) return 0;
	return m_vbm->getDataSize(this->_handle());
}