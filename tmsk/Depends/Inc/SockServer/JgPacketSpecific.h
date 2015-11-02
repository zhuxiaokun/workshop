#ifndef _JG_PACKET_SPECIFIC_H_
#define _JG_PACKET_SPECIFIC_H_

#include "KSockets.h"
#include "KIocpSockServer.h"
#include "KEpollSockServer.h"
#include "KSelectSockServer.h"
#include <System/Cipher/vgcipher.h>

class ISocketConnection
{
public:
	virtual ~ISocketConnection() { }
	virtual BOOL SendPacket(int cmd, const void* data, int len) { return FALSE; }
	virtual void AddRef_2()     { return; }
	virtual void ReleaseRef_2() { return; }
	virtual void Close_2()      { return; }
	virtual void DelayClose_2(int secs) { return; }
	virtual const KSocketAddress& GetRemoteAddress_2() { static KSocketAddress addr = {0,0}; return addr; }
};

struct JgPacketHead
{
	DWORD flag    : 1;	// 第一个BIT是标志位，指示是否携带 Tail 数据
	DWORD length  : 15;
	DWORD command : 16;

//////////////////////////////////////////////
	BOOL HasTail() const
	{
		return this->flag;
	}
	int Length() const
	{
		return this->length;
	}
	int Command() const
	{
		return this->command;
	}
};

// 网络包的附加数据，主要用于反外挂
// 这个部分可以没有
struct JgPacketTail
{
	DWORD length   : 10;	// 可以为 0
	DWORD command  : 6;
	DWORD checksum : 16;	// 逻辑数据的校验和，上行包使用

//////////////////////////////////////////////
	int Length() const
	{
		return this->length;
	}
	int Command() const
	{
		return this->command;
	}
	int Checksum() const
	{
		return this->checksum;
	}
	const void* Data() const
	{
		return this + 1;
	}
	void* Data()
	{
		return this + 1;
	}
};

class JgPacketCipher
{
public:
	virtual ~JgPacketCipher() { }
	virtual void Encrypt(JgPacketHead* pHead) = 0;
	virtual void Decrypt(JgPacketHead* pHead) = 0;
};

class JgPacketConst
{
public:
	enum
	{
		Checksum_Initial	= 0x5AA5,
		MaxAppDataSize		= 32*1024-1,
		AppPacketSize		= MaxAppDataSize + sizeof(JgPacketHead),// 应用程序最大数据包长度
		NetPacketSize		= 1024,									// 网络传输的最大数据包长度
		MaxNetDataSize		= NetPacketSize-sizeof(JgPacketHead),	// 网络包最大数据长度
		MaxTailDataSize		= 1024,

		// 构造包时使用
		BuildBuffSize = AppPacketSize + 1024,
		//SplitBuffSize = BuildBuffSize + 256,
	};
	enum
	{
		PredefineCommandStart = 0x0fff9,
		Assemble              = 0x0fff9,
		AssembleEnd           = 0x0fffa,
		Ping                  = 0x0fffb,
		PingAck               = 0x0fffc,
		Ack                   = 0x0fffd,
		Heartbeat             = 0x0fffe,
		PredefineCommandEnd   = 0x0ffff,
	};
};

class JgPacketDeclare
{
public:
	// 返回值:
	//   >0  : 包长度
	//   0   : 包没有完成
	//   -1  : 包结构数据错误
	static int  CheckLength(JgPacketHead* pHead, int len);

	// 主要用于检查组装后的包的有效性
	static BOOL ValidateAppPacket(JgPacketHead* pHead, int len);

	// 根据包头，得到整个包的长度
	static int GetLength(JgPacketHead* pHead);

	// 计算一段数据的校验和
	static WORD CalcChecksum(const void* pData, int len);
};

// 每个线程使用一个，从TLS中获得
class JgPacketBuffer
{
public:
	typedef KPieceBuffer<JgPacketConst::BuildBuffSize> KPacketBuildBuffer;

public:
	JgPacketBuffer();
	~JgPacketBuffer();

public:
	int write(const void* data, int len);
	int read(void* buf, int len);
	int dataSize() const;
	const void* data() const;
	int size() const;
	void resize(int length);
	const void* swap(int* bufferSize);
	BOOL innerBuild(int cmd, const void* data, int len);

public:
	void Reset();

public:
	KPacketBuildBuffer* m_buffer;
	KPacketBuildBuffer m_buildBuffer_1;
	KPacketBuildBuffer m_buildBuffer_2;
};

class JgPacketBuilder
{
public:
	JgPacketBuilder();
	virtual ~JgPacketBuilder();

public:
	// 派生类重载这个函数实现 附加数据、包加密 等功能
	virtual BOOL _Build(JgPacketBuffer* pPacketBuffer, int cmd, const void* pData, int len);

public:
	virtual BOOL Build(JgPacketBuffer* pPacketBuffer, int cmd, const void* pData, int len);
	void Output(JgPacketBuffer* pPacketBuffer, const void*& pData, int& len);

public:
	//BOOL m_splitted;
	//JgPacketBuffer::KPacketBuildBuffer* m_pBuildBuffer;
	//JgPacketBuffer::KPacketSplitBuffer* m_pSplitBuffer;
};

class JgPacketReceiver
{
public:
	typedef System::Collections::DynArray<void*> PacketPtrArray;
	typedef System::Memory::_autoreset_object_pool<KPieceBufferTmpl<5120>,128,false,System::Sync::KMTLock> AssemBuffPool;
	class AssembleBuffer : public KPieceListBuffer<5120>
	{
	public: AssemBuffPool* m_pool;
		AssembleBuffer():m_pool(NULL) { }
		piece_type* allocPiece() { return m_pool->Alloc(); }
		void freePiece(piece_type* p) { m_pool->Free(p); }
	};

public:
	JgPacketReceiver();
	virtual ~JgPacketReceiver();

public: // 仅仅为了提供友好接口
	KSocket* GetSocket() { return m_pSock; }
	int _getSocketType() const { return m_pSock->GetSocketType() ; }
	DWORD _getUserData() const { return m_pSock->GetUserData(); }

public:
	void SetRecvBuffSize(int kSize);
	void SetRecvBufferPool(KIOBuffPool_2048& pool);
	void SetAssemBuffPool(AssemBuffPool& pool);
	int  ReadAvail();

	// 当收到一个完整的网络包
	BOOL OnRecvComplete(const void* pPacket, int len, const KSocketAddress* pFrom);

	// 一次读取尽量多的包，直到缓冲区满，返回读到的包数量
	int ReadPackets(void* pBuff, int len, PacketPtrArray& arr);
	int ReadPackets(void* pBuff, int len, PacketPtrArray& arr, int maxPackets);

	void ClearReceiver();

protected:
	// 当收到一个完整的应用程序包
	// 派生类可以重载这个函数以实现 包检查、解密 等功能
	virtual BOOL OnAppPacket(const void* pPacket, int len, const KSocketAddress& fromAddr);

	// 需要考虑锁
	BOOL WriteOnePacket(const void* pPacket, int len, const KSocketAddress& fromAddr);
	BOOL WriteOnePacket_Force(const void* pPacket, int len, const KSocketAddress& fromAddr);

	// 假定已经在锁定范围内
	virtual int _ReadOnePacket(char* pBuf, int len) = 0;
	virtual BOOL _WriteOnePacket(const void* packet, int len, const KSocketAddress& fromAddr) = 0;
	virtual BOOL _WriteOnePacket_Force(const void* packet, int len, const KSocketAddress& fromAddr) = 0;

protected:
	BOOL post_packet(int cmd, const void* data, int len);
	BOOL post_final_packet(int cmd, const void* data, int len);

public:
	System::Sync::KThreadMutex m_mxRecv;
	KIOBuffer m_recvIoBuff;			// 接收缓冲区，里面都是应用程序包
	AssembleBuffer m_assembleBuffer;

	// 和哪个Socket关联
	KSocket* m_pSock;
	BOOL m_denyRecv;
};

class JgTcpPacketReceiver : public JgPacketReceiver
{
public:
	JgTcpPacketReceiver();
	virtual ~JgTcpPacketReceiver();

protected:
	BOOL _WriteOnePacket(const void* pPacket, int len, const KSocketAddress& fromAddr);
	BOOL _WriteOnePacket_Force(const void* pPacket, int len, const KSocketAddress& fromAddr);
	int _ReadOnePacket(char* pBuf, int len);
};

class JgUdpPacketReceiver : public JgPacketReceiver
{
public:
	JgUdpPacketReceiver();
	virtual ~JgUdpPacketReceiver();

protected:
	BOOL _WriteOnePacket(const void* pPacket, int len, const KSocketAddress& fromAddr);
	BOOL _WriteOnePacket_Force(const void* pPacket, int len, const KSocketAddress& fromAddr);
	int _ReadOnePacket(char* pBuf, int len);
};

class JgServerTcpSocket
	: public KServerTcpSocket
	, public JgPacketBuilder
	, public JgTcpPacketReceiver
{
public:
	friend class JgSockServer;

public:
	JgServerTcpSocket();
	~JgServerTcpSocket();

public:
	BOOL Send(int cmd, const void* pData, int len);
	BOOL SendImmediate(int cmd, const void* pData, int len);

	// 给自己的接收队列发包
	BOOL PostPacket(int cmd, const void* pData, int len) { return post_packet(cmd, pData, len); }
	// 这是最后一个包了，之后不再收包
	BOOL PostFinalPacket(int cmd, const void* pData, int len) { return post_final_packet(cmd, pData, len); }

	const KSocketAddress& GetRemoteAddress() const;

public:
	void Reset();
	BOOL OnAttached();
	BOOL OnDetached();
	void OnStartWork();
	int OnRecvData(const void* pData, int len, const KSocketAddress* pFrom);
};

class JgClientTcpSocket
	: public KClientTcpSocket
	, public JgPacketBuilder
	, public JgTcpPacketReceiver
{
public:
	friend class JgSockServer;

public:
	JgClientTcpSocket();
	~JgClientTcpSocket();

public:
	BOOL Send(int cmd, const void* pData, int len);
	BOOL SendImmediate(int cmd, const void* pData, int len);

	// 给自己的接收队列发包
	BOOL PostPacket(int cmd, const void* pData, int len) { return post_packet(cmd, pData, len); }
	// 这是最后一个包了，之后不再收包
	BOOL PostFinalPacket(int cmd, const void* pData, int len) { return post_final_packet(cmd, pData, len); }

public:
	void Reset();
	BOOL OnAttached();
	BOOL OnDetached();
	void OnStartWork();
	int OnRecvData(const void* pData, int len, const KSocketAddress* pFrom);
};

class JgUdpSocket
	: public KUdpSocket
	, public JgPacketBuilder
	, public JgUdpPacketReceiver
{
public:
	friend class JgSockServer;

public:
	JgUdpSocket();
	~JgUdpSocket();

public:
	BOOL SendTo(int cmd, const void* pData, int len, const KSocketAddress& peerAddr);
	BOOL SendToImmediate(int cmd, const void* pData, int len, const KSocketAddress& peerAddr);
	
	// 给自己的接收队列发包
	BOOL PostPacket(int cmd, const void* pData, int len) { return post_packet(cmd, pData, len); }
	// 这是最后一个包了，之后不再收包
	BOOL PostFinalPacket(int cmd, const void* pData, int len) { return post_final_packet(cmd, pData, len); }

public:
	void Reset();
	BOOL OnAttached();
	BOOL OnDetached();
	int OnRecvData(const void* pData, int len, const KSocketAddress* pFrom);
	BOOL OnSocketError(DWORD errCode);
	BOOL OnInvalidPacket(const KSocketAddress* pFrom, ePacketReason reason);
};

class JgSocketFilter
{
public:
	virtual ~JgSocketFilter() { }
	virtual BOOL Accept(KSocket* pSock) = 0;
};

class JgSockServer : public KSockServer
{
public:
	friend class JgPacketReceiver;
	friend class JgServerTcpSocket;
	friend class JgClientTcpSocket;
	friend class JgUdpSocket;

	typedef System::Collections::DynArray<KSocket*>           KSocketArray;
	typedef System::Collections::DynArray<JgServerTcpSocket*> JgServerSocketArray;
	typedef System::Collections::DynArray<JgClientTcpSocket*> JgClientSocketArray;
	typedef System::Collections::DynArray<JgUdpSocket*>       JgUdpSocketArray;

private:
	static const DWORD tls_prefix_flag = 0xAA55AA55;
	struct TlsSenderData
	{
		DWORD prefixFlag;
		JgPacketBuffer packetBuffer;
	};

public:
	JgSockServer();
	~JgSockServer();

public:
	BOOL Initialize(int socketCapacity, int ioThreadNum);
	void Finalize();

public:
	BOOL BuildPacket(JgPacketBuilder* builder, int cmd, const void* data, int len, const void** outPacket, int* outLen);

public:
	// 根据过滤器得到Socket数组
	int LockSockets(JgSocketFilter* pFilter, KSocketArray& arrSocks);
	void UnlockSockets(KSocketArray& arrSocks);

	int LockSockets(JgSocketFilter* filter, JgPacketReceiver* socks[], int count);
	void UnlockSockets(JgPacketReceiver* socks[], int count);

	// 获得当前所有活跃的Socket对象
	// 在返回每个Socket的时候会给Socket增加一个引用计数
	int LockSockets(JgServerSocketArray& arrServer, JgClientSocketArray& arrClient, JgUdpSocketArray& arrUdp);

	// 给引用计数减1，和LockSockets成对调用
	void UnlockSockets(JgServerSocketArray& arrServer, JgClientSocketArray& arrClient, JgUdpSocketArray& arrUdp);

public:
	// 从 TLS 中得到 JgPacketBuffer
	JgPacketBuffer* GetThreadPacketBuffer();

public:
	int m_sendTls;
	KIOBuffPool_2048 m_recvPool;
	JgPacketReceiver::AssemBuffPool m_assemblePool;
};

#endif
