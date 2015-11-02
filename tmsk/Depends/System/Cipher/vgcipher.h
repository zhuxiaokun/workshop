#ifndef _VG_CIPHER_H_
#define _VG_CIPHER_H_

#include "../KPlatform.h"

class KCipher
{
public:
	virtual ~KCipher() { }
	virtual void Encrypt(void* pData, int len) = 0;
	virtual void Decrypt(void* pData, int len) = 0;
};

class KVgContext
{
public:
	enum
	{
		m_keyLength = 32,
		CONST1      = 0x341FB,
		CONST2      = 0x267EC1
	};
	struct KVgCell
	{
		BYTE a, b;
	};
	typedef KVgCell KVgTable[256][256];

private:
	KVgContext();
	
public:
	~KVgContext();
	static BOOL Instance(DWORD seed, const char* keyFile);
	static BOOL Instance(DWORD seed, const void* keyData, int len);
	static KVgContext& GetInstance();

public:
	DWORD GetSeed() const;
	const void* GetKeyData() const;
	const KVgTable* GetTable() const;

private:
	BOOL ConstructTable();

private:
	DWORD m_seed;
	BYTE m_keyData[m_keyLength];
	KVgTable m_table;
	static KVgContext* m_pInst;
};

class KVgCipher : public KCipher
{
public:
	KVgCipher();
	virtual ~KVgCipher();

public:
	static BOOL InitContext(DWORD seed, const char* keyFile);
	static BOOL InitContext(DWORD seed, const void* keyData, int len);

public:
	void Encrypt(void* pData, int len);
	void Decrypt(void* pData, int len);

public:
	DWORD m_seed;
	const BYTE* m_keyData;
	const KVgContext::KVgTable* m_pTable;
};

class KVarKeyVgCipher : public KVgCipher
{
public:
	KVarKeyVgCipher();
	~KVarKeyVgCipher();

public:
	void Encrypt(void* pData, int len);
	void Decrypt(void* pData, int len);

public:
	// 为派生类提供变化Key的可能
	virtual int TransferEncKey(int key) = 0;
	virtual int TransferDecKey(int key) = 0;
};

#endif