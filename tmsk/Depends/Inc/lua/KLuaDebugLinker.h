#pragma once
#include <map>
// using namespace std;
#define MAX_BREAKPOINT 65536
#define MAX_VALUE_COUNT 65536
#define MAX_SHAREDMEM 1024 * 1024 * 20

#define LMSG_DEBUG_START WM_USER+'L'+'S'
#define LMSG_DEBUG_END WM_USER+'L'+'E'
#define LMSG_DEBUG_LIST WM_USER+'L'+'L'
#define LMSG_DEBUG_CKPV WM_USER+'L'+'V'
#define LMSG_DEBUG_EXIT	WM_USER+'E'+'X'

#define strlen0(p) (strlen(p)+1)

class CSpinInterlocked
{
public:
	void init()
	{
		m_bLock = FALSE;
	}

	void lock()
	{
		//while (m_bLock) Sleep(1);
		//m_bLock = TRUE;
	}

	void unlock()
	{
		//m_bLock = FALSE;
	}

	BOOL m_bLock;
};

struct LDebugMsg
{
	DWORD msg, uParam, uProcessID;
	long lParam;
};

class CPGEMemPool
{
public:
	static const int c_blocksize = 7;
	CPGEMemPool()
	{
		m_pMainData = NULL;
		m_dwDatSize = 0;
		m_dwBlockSize = 0;
		m_hFile = NULL;
		InitializeCriticalSection(&cs_lock);
	}

	~CPGEMemPool()
	{
		Destory();
	}

	static CPGEMemPool* Instance(BOOL bre = 0)
	{
		static CPGEMemPool* p = NULL;
		if (!p) p = new CPGEMemPool;
		if (bre) 
		{
			delete p;
			p = NULL;
		}
		return p;
	}

	void Destory()
	{

	}

	BOOL Init(DWORD datsize, char* cmem)
	{
		LPVOID ap = cmem;
		m_dwDatSize = datsize;
		m_dwBlockSize = 1<<c_blocksize;
		m_dwFlagSize = (datsize>>c_blocksize);
		m_pFlag = (BYTE*)ap;
		m_pMainData = m_pFlag+m_dwFlagSize;
		memset(m_pFlag, 0, m_dwFlagSize);
		return TRUE;
	}

	void* malloc(size_t size)
	{
		EnterCriticalSection(&cs_lock);
		BYTE* p = NULL;
		if (size+m_dwFlagSize > m_dwDatSize) 
		{
			LeaveCriticalSection(&cs_lock);
			return 0;
		}
		int count = 0, need = (size>>c_blocksize)+1;
		for (int i=0; i<(int)m_dwFlagSize; ++i)
		{
			if (!(*(m_pFlag+i)))
			{ 
				if (++count == need) 
				{
					memset(m_pFlag+(i-need+1), 0x1, need);
					p = m_pMainData+((i-need+1)<<c_blocksize);
					*((DWORD*)p) = need;
					p += 4;
					break;
				}
			}
			else count = 0;
		}
		LeaveCriticalSection(&cs_lock);
		return p;

	}	

	void free(void* p)
	{
		EnterCriticalSection(&cs_lock);
		BYTE* c = ((BYTE*)p)-4;
		int s = (c-m_pMainData)>>c_blocksize;
		int l = *((DWORD*)c);
		memset(m_pFlag+s, 0, l);
		LeaveCriticalSection(&cs_lock);
	}

	void* d2v(DWORD dad)
	{
		return m_pFlag + dad;
	}

	DWORD v2d(void* pad)
	{
		return ((BYTE*)pad) - m_pFlag;
	}


	BYTE* m_pMainData;
	DWORD m_dwDatSize;
	DWORD m_dwBlockSize;
	HANDLE m_hFile;
	BYTE* m_pFlag;
	DWORD m_dwFlagSize;
	CRITICAL_SECTION cs_lock;

};


enum KLDL_DEBUG_MSG
{
	KLDL_MSG_BREAK = WM_USER + 5,
};


DWORD ELFHash(const char*str);

struct KLDL_BreakItem
{
	KLDL_BreakItem(char* f, int l)
	{
		char* fn = (char*)CPGEMemPool::Instance()->malloc(strlen0(f));
		strcpy(fn, f);
		file = CPGEMemPool::Instance()->v2d(fn);
		line = l;
	}
	DWORD	file;
	DWORD	line;
	char* GetFileName()
	{
		return (char*)CPGEMemPool::Instance()->d2v(file);
	}
};

struct KLDL_ValueItem
{
	DWORD	name;
	DWORD	value;
	WORD	len;
	WORD	type;
};

struct KLDL_Operation
{
	BYTE bnext;
	BYTE bstep;
	HWND hwndDebuger;
};

struct KLDL_LocalPtr
{
	DWORD ptr[255];
	DWORD count;
};

struct PubVName
{
	char name[64];
	void operator = (PubVName& v)
	{
		strcpy(name, v.name);
	}
};

struct KLDL_PublicPtr
{
	PubVName pnm[255];
	DWORD ptr[255];
	DWORD count;
	void AddValue(char* name)
	{
		strcpy(pnm[count++].name, name);
	}

	void DeleteValue(char* name)
	{
		int n = -1;
		for(int i=0; i<255; ++i)
		{
			if (strcmp(pnm[i].name, name) == 0)
			{
				count--;
				n = i;
			}
			if (n != -1)
			{
				for (int j=i; j<255; ++j)
					pnm[j] = pnm[j+1];
			}
		}
	}
};

class KLuaDebugLinker
{
public:
	typedef std::map<DWORD, KLuaDebugLinker*> LinkerMap;
	KLuaDebugLinker()
		: m_hShared(0)
		, m_pMem(0)
		, m_pBreak(0)
		, m_pValue(0) {}

	static LinkerMap* GetLmap()
	{
		static LinkerMap* llmap = NULL;
		if (!llmap) llmap = new LinkerMap;
		return llmap;
	}

	static KLuaDebugLinker* GetLinkerFromID(DWORD uID)
	{
		LinkerMap* llmap = GetLmap();
		KLuaDebugLinker* p = (*llmap)[uID];
		if (!p)
		{
			p = new KLuaDebugLinker;
			p->Init(uID);
			(*llmap)[uID] = p;
		}
		return p;

	}

	static void DestoryLinkerFromID(DWORD uID)
	{
		LinkerMap* llmap = GetLmap();
		KLuaDebugLinker* p = (*llmap)[uID];
		if (p)
		{
			llmap->erase(uID);
			p->Destory();
			delete p;
			p = NULL;
		}
	}

	void Init(DWORD uID)
	{
		char buf[100];
		sprintf(buf, "Lua_Debug_Mf_%d", uID);
		if (!(m_hShared = OpenFileMapping(FILE_MAP_WRITE,
			FALSE, buf)))
		{
			m_hShared = CreateFileMapping((HANDLE)0xFFFFFFFF,
				NULL, PAGE_READWRITE, 0, MAX_SHAREDMEM, buf); 

			m_pMem = (char*)MapViewOfFile((HANDLE)m_hShared, 
				FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, MAX_SHAREDMEM);

			memset(m_pMem, 0, MAX_SHAREDMEM);
		}

		if (!m_pMem)
			m_pMem = (char*)MapViewOfFile((HANDLE)m_hShared, 
				FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, MAX_SHAREDMEM);

		m_pOper = (KLDL_Operation*)m_pMem;
		m_pBreak = (KLDL_BreakItem*)(m_pMem + 1024);
		m_pValue = (KLDL_ValueItem*)(m_pMem + 1024 + sizeof(KLDL_BreakItem) * MAX_BREAKPOINT);

		m_pBuf = m_pMem + 1024 + sizeof(KLDL_BreakItem) * MAX_BREAKPOINT 
			+ sizeof(KLDL_ValueItem) * MAX_VALUE_COUNT;

		CPGEMemPool::Instance()->Init(MAX_SHAREDMEM - (1024 + sizeof(KLDL_BreakItem) * MAX_BREAKPOINT 
			+ sizeof(KLDL_ValueItem) * MAX_VALUE_COUNT), m_pBuf);
	
	}

	void Destory()
	{
		UnmapViewOfFile(m_pMem);
		CloseHandle(m_hShared);
		m_hShared = 0;
		m_pMem = 0;
		m_pBreak = 0;
		m_pValue = 0;
		CPGEMemPool::Instance(1);
	}

	void SetBreakPoint(char* file, int line)
	{
		m_plocker->lock();
		char* cf = _strlwr(file);
		for (int i=0; i<MAX_BREAKPOINT; ++i)
		{
			if (m_pBreak[i].file == 0)
			{
				char* fn = (char*)CPGEMemPool::Instance()->malloc(strlen0(file));
				strcpy(fn, cf);
				m_pBreak[i].file = CPGEMemPool::Instance()->v2d(fn);
				m_pBreak[i].line = line;
				break;
			}
			else
			{
				char* fn = (char*)CPGEMemPool::Instance()->d2v(m_pBreak[i].file);
				if (strcmp(fn, cf) == 0 && line == m_pBreak[i].line)
				{
					break;
				}
			}
		}
		m_plocker->unlock();
	}

	void ClearBreakPoint(char* file, int line, BOOL bcall)
	{
		m_plocker->lock();
		char* cf = _strlwr(file);
		if (bcall)
		{
			for (int i=0; i<MAX_BREAKPOINT; ++i)
			{
				char* fn = (char*)CPGEMemPool::Instance()->d2v(m_pBreak[i].file);
				CPGEMemPool::Instance()->free(fn);
			}
			memset(m_pBreak, 0, sizeof(KLDL_BreakItem) * MAX_BREAKPOINT);
		}
		else
		{
			for (int i=0; i<MAX_BREAKPOINT; ++i)
			{
				if (m_pBreak[i].file)
				{
					char* fn = (char*)CPGEMemPool::Instance()->d2v(m_pBreak[i].file);
					if (strcmp(fn, cf) == 0 && line == m_pBreak[i].line)
					{
						CPGEMemPool::Instance()->free(fn);
						m_pBreak[i].line = 0;
						m_pBreak[i].file = 0;
						break;
					}
				}
			}
		}
		m_plocker->unlock();
	}

	BOOL IsBreaked(char* file, int line)
	{
		m_plocker->lock();
		char* cf = _strlwr(file);
		for (int i=0; i<MAX_BREAKPOINT; ++i)
		{
			if (m_pBreak[i].file)
			{
				char* fn = (char*)CPGEMemPool::Instance()->d2v(m_pBreak[i].file);
				if (strcmp(fn, cf) == 0 && line == m_pBreak[i].line)
				{
					return TRUE;
				}
			}

		}
		m_plocker->unlock();

		return FALSE;
	}

	int SetValue(char* name, void* v, int type, int len)
	{
		m_plocker->lock();
		int n = -1;
		char *pn = NULL, *pv = NULL;
		for (int i=0; i<MAX_VALUE_COUNT; ++i)
		{

			if (!m_pValue[i].name) 
			{
				if (n==-1) n = i;
			}
			else
			{
				pn = (char*)CPGEMemPool::Instance()->d2v(m_pValue[i].name);
				if (strcmp(pn, name) == 0)
				{
					pv = (char*)CPGEMemPool::Instance()->d2v(m_pValue[i].value);

					CPGEMemPool::Instance()->free(pv);
					pv = (char*)CPGEMemPool::Instance()->malloc(len);
					memcpy(pv, v, len);

					m_pValue[i].value = CPGEMemPool::Instance()->v2d(pv);
					return i;
					m_plocker->unlock();
				}
			}
		}
		
		pn = (char*)(CPGEMemPool::Instance()->malloc(strlen0(name)));
		strcpy(pn, name);
	
		pv = (char*)CPGEMemPool::Instance()->malloc(len);
		memcpy(pv, v, len);

		m_pValue[n].name =  CPGEMemPool::Instance()->v2d(pn);
		m_pValue[n].value =  CPGEMemPool::Instance()->v2d(pv);
		m_pValue[n].len = len;
		m_pValue[n].type = type;

		m_plocker->unlock();
		return n;
	}

	void ClearValue(char* name = 0, int index = -1)
	{
		m_plocker->lock();
		char *pn = NULL, *pv = NULL;
		if (name)
		{
			for (int i=0; i<MAX_VALUE_COUNT; ++i)
			{
				if (m_pValue[i].name)
				{
					pn =  (char*)CPGEMemPool::Instance()->d2v(m_pValue[i].name);
					if (pn && strcmp(pn, name) == 0)
					{
						pv = (char*)CPGEMemPool::Instance()->d2v(m_pValue[i].value);
						CPGEMemPool::Instance()->free(pv);
						CPGEMemPool::Instance()->free(pn);
						memset(&(m_pValue[index]), 0, sizeof(KLDL_ValueItem)); 
					}
				}
			}
		}
		else if (index != -1)
		{
			if (m_pValue[index].name)
			{
				pn = (char*)CPGEMemPool::Instance()->d2v(m_pValue[index].name);
				pv = (char*)CPGEMemPool::Instance()->d2v(m_pValue[index].value);
				CPGEMemPool::Instance()->free(pv);
				CPGEMemPool::Instance()->free(pn);
				memset(&(m_pValue[index]), 0, sizeof(KLDL_ValueItem)); 
			}
		}
		m_plocker->unlock();
	}

	void* GetValue(char* name, int& type, int& len)
	{
		m_plocker->lock();
		char *pn = NULL, *pv = NULL;
		if (name)
		{
			for (int i=0; i<MAX_VALUE_COUNT; ++i)
			{
				if (m_pValue[i].name)
				{
					pn =  (char*)CPGEMemPool::Instance()->d2v(m_pValue[i].name);
					if (pn && strcmp(pn, name) == 0)
					{
						type = m_pValue[i].type;
						len = m_pValue[i].len;
						return CPGEMemPool::Instance()->d2v(m_pValue[i].value);
					}
				}
			}
		}
		m_plocker->unlock();
		return NULL;
	}

	void* GetValue(int index, int& type, int& len, char*& name)
	{
		m_plocker->lock();
		if (index != -1)
		{
			if (m_pValue[index].name)
			{
				type = m_pValue[index].type;
				len = m_pValue[index].len;
				name = (char*)CPGEMemPool::Instance()->d2v(m_pValue[index].name);
				return CPGEMemPool::Instance()->d2v(m_pValue[index].value);
			}
		}
		m_plocker->unlock();
		return NULL;
	}
	
	KLDL_Operation* GetOper()
	{
		return m_pOper;
	}


	HANDLE				m_hShared; // 共享内存句柄
	char*				m_pMem;
	KLDL_BreakItem*		m_pBreak;
	KLDL_ValueItem*		m_pValue;
	KLDL_Operation*		m_pOper;
	char*				m_pBuf;
	CSpinInterlocked*	m_plocker;
};