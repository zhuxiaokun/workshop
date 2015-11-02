#include <string.h>
#include "vgcipher.h"
#include "../Misc/KStream.h"

KVgContext* KVgContext::m_pInst = NULL;

KVgContext::KVgContext()
{

}

KVgContext::~KVgContext()
{

}

BOOL KVgContext::Instance(DWORD seed, const char* keyFile)
{
	char vgKey[m_keyLength];
	KInputFileStream fi;
	if(!fi.Open(keyFile)) return FALSE;
	int_r n = fi.ReadData(&vgKey, m_keyLength);
	if(n != m_keyLength) return FALSE;
	return KVgContext::Instance(seed, (const void*)&vgKey, m_keyLength);
}

BOOL KVgContext::Instance(DWORD seed, const void* keyData, int len)
{
	ASSERT(!m_pInst);
	KVgContext* pCtx = new KVgContext();
	ASSERT(pCtx);
	pCtx->m_seed = seed;
	ASSERT(len >= sizeof(pCtx->m_keyData));
	memcpy(&pCtx->m_keyData, keyData, sizeof(pCtx->m_keyData));
	pCtx->ConstructTable();
	m_pInst = pCtx;
	return TRUE;
}

KVgContext& KVgContext::GetInstance()
{
	ASSERT(m_pInst);
	return *m_pInst;
}

DWORD KVgContext::GetSeed() const
{
	return m_seed;
}

const void* KVgContext::GetKeyData() const
{
	return &m_keyData;
}

const KVgContext::KVgTable* KVgContext::GetTable() const
{
	return &m_table;
}

BOOL KVgContext::ConstructTable()
{
	unsigned char bIsUsed[256];
	register unsigned int i = 0, j = 0;
	register unsigned char k;
	register unsigned int seed = m_seed;

	for(i=0; i<256; i++)
	{
		memset(bIsUsed, 0, 256);
		for(j=0; j<256; j++)
		{
			while(TRUE)
			{
				seed *= CONST1;
				seed += CONST2;
				k = (((seed >> 16) & 0x7FFF)) & 0xFF;
				if(!bIsUsed[k])
				{
					m_table[i][j].a = k;
					bIsUsed[k] = 1;
					break;
				}
			}
		}
	}

	for(i=0; i<256; i++)
	{
		for(j=0; j<256; j++)
		{
			for(int k=0; k<256; k++)
			{
				if(m_table[i][k].a == j)
				{
					m_table[i][j].b = k;
					break;
				}
			}
		}
	}

	return TRUE;
}

KVgCipher::KVgCipher()
{
	KVgContext& ctx = KVgContext::GetInstance();
	m_seed = ctx.GetSeed();
	m_keyData = (const BYTE*)ctx.GetKeyData();
	m_pTable = ctx.GetTable();
}

KVgCipher::~KVgCipher()
{

}

BOOL KVgCipher::InitContext(DWORD seed, const char* keyFile)
{
	return KVgContext::Instance(seed, keyFile);
}

BOOL KVgCipher::InitContext(DWORD seed, const void* keyData, int len)
{
	return KVgContext::Instance(seed, keyData, len);
}

void KVgCipher::Encrypt(void* pData, int len)
{
	for(int i=0; i<len; i++)
	{
		register int val = ((BYTE*)pData)[i];
		register int key = m_keyData[i & 0x1f];
		((BYTE*)pData)[i] = (*m_pTable)[key][val].a;
	}
}

void KVgCipher::Decrypt(void* pData, int len)
{
	for(int i=0; i<len; i++)
	{
		register int val = ((BYTE*)pData)[i];
		register int key = m_keyData[i & 0x1f];
		((BYTE*)pData)[i] = (*m_pTable)[key][val].b;
	}
}

KVarKeyVgCipher::KVarKeyVgCipher()
{

}

KVarKeyVgCipher::~KVarKeyVgCipher()
{

}

void KVarKeyVgCipher::Encrypt(void* pData, int len)
{
	for(int i=0; i<len; i++)
	{
		register int val = ((BYTE*)pData)[i];
		register int key = m_keyData[i & 0x1f];
		key = this->TransferEncKey(key) & 0xff;
		((BYTE*)pData)[i] = (*m_pTable)[key][val].a;
	}
}

void KVarKeyVgCipher::Decrypt(void* pData, int len)
{
	for(int i=0; i<len; i++)
	{
		register int val = ((BYTE*)pData)[i];
		register int key = m_keyData[i & 0x1f];
		key = this->TransferDecKey(key) & 0xff;
		((BYTE*)pData)[i] = (*m_pTable)[key][val].b;
	}
}