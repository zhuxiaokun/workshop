#ifndef _K_SOCKET_BASE_H_
#define _K_SOCKET_BASE_H_

#if defined(__APPLE__)
#ifndef _USE_SELECT_
#define _USE_SELECT_
#endif
#endif

#include "KIOBuffer.h"
#include <System/SockHeader.h>
#include <System/KPlatform.h>
#include <System/Collections/KString.h>

class KSocket;
class KBaseSockServer;

namespace EnumSocketState
{
	const int none			= 0;
	const int fresh			= 1;
	const int connecting	= 2;
	const int connected		= 3;
	const int closing		= 4;	// 只有从 connected 才能到 closing，实际的关闭在_doClose()函数中执行
	const int closed		= 5;
	const int destroying	= 6;
}

#define mkip(a,b,c,d) ((a<<24)|(b<<16)|(c<<8)|d)

struct KIpMask
{
	DWORD mask;		// 掩码
	DWORD target;	// ip & mask == target

	BOOL isValid() const;
	BOOL match(DWORD ip) const;
	BOOL match(const char* ip) const;
	static KIpMask build(const char* ipAddress);
	static KIpMask build2(const char* ipAddress, const char* mask);
};

enum ePacketReason
{
	enum_packet_invalid_head = -1,
};

#pragma pack(push, 1)
struct KSocketAddress
{
	DWORD m_ip;
	WORD  m_port;

	///
	bool operator == (const KSocketAddress& o) const;
	sockaddr_in toSockAddr() const;
	JG_C::KString<64> toString() const;

	///
	static KSocketAddress FromSockAddr(const sockaddr_in& sockAddr);
	static sockaddr_in ToSockAddr(const KSocketAddress& addr);
	static KSocketAddress FromString(const char* addrStr); // 192.168.10.46:9998
	static ::System::Collections::KString<64> ToString(const KSocketAddress& addr);
	static ::System::Collections::KString<32> ToString(DWORD ip);
};
struct KUdpPacketHead
{
	KSocketAddress m_peerAddr;
	WORD m_length;
};
#pragma pack(pop)

class KSocket
{
public:
	enum SocketType
	{
		SOCK_TCP_LISTEN,
		SOCK_CLIENT_TCP,
		SOCK_SERVER_TCP,
		SOCK_UDP,
		SOCK_MAX
	};

public:
	KSocket();
	virtual ~KSocket();

public:
	static SOCKET CreateTcpSocket();
	static SOCKET CreateUdpSocket();
	static BOOL   SetNonBlock(SOCKET sock);
	static BOOL   IgnoreSigPipe(SOCKET sock);
	static DWORD  GetIpByName(const char* addrName);

public:
	void SetSendBuffer(int bytes);

	// SOCKET活跃时间，如长时间没有收发动作
	int GetActiveTimeout() const;
	void SetActiveTimeout(int secs);

	void SetUserData(DWORD userData)
	{
		m_userData = userData;
	}
	DWORD GetUserData() const
	{
		return m_userData;
	}

	void AddRef();
	void ReleaseRef();
	int GetRefCount();

public:
	KSocketAddress GetLocalAddress() const;

public:
	DWORD GetIdleTicks() const;       // Socket Close后，经过的时间
	DWORD GetLatestSendTick() const;  // 最近一次发送成功的时间，GetTickCount
	DWORD GetLatestRecvTick() const;  // 最近一次成功接收的时间，GetTickCount
	DWORD GetSendSpeed() const;       // 工作期间的发送速度 Bytes/Sec
	DWORD GetRecvSpeed() const;       // 工作期间的接收速度 Bytes/Sec
	DWORD GetSocketError() const;

public:
	// 立即关闭
	BOOL Close();

	// 延迟关闭，在维护线程中检查时间，到了就执行正真的关闭动作
	BOOL DelayClose(int delaySecs);

	// 线程不安全的，只能在发送线程中调用
	void PauseSend(int ticks);

	// Socket类型
	virtual SocketType GetSocketType() const = 0;

	// 获得Socket对象所从属的SockServer
	virtual KBaseSockServer* GetSockServer() = 0;

	// Tcp使用的发送函数
	virtual int Send(const void* packet, int len) = 0;
	
	// UDP使用的发送函数
	virtual int SendTo(const void* packet, int len, const KSocketAddress& address) = 0;

	virtual int SendImmediate(const void* packet, int len) = 0;
	virtual int SendToImmediate(const void* packet, int len, const KSocketAddress& address) = 0;

	int SendData(const void* packet, int len)
	{
		return this->Send(packet, len);
	}
	int SendDataImmediate(const void* packet, int len)
	{
		return this->SendImmediate(packet, len);
	}
	int SendDataTo(const void* packet, int len, const KSocketAddress& address)
	{
		return this->SendTo(packet, len, address);
	}
	int SendDataToImmediate(const void* packet, int len, const KSocketAddress& address)
	{
		return this->SendToImmediate(packet, len, address);
	}

	virtual BOOL Send(int cmd, const void* pData, int len) { ASSERT(FALSE); return FALSE; }
	virtual BOOL SendImmediate(int cmd, const void* pData, int len) { ASSERT(FALSE); return FALSE; }
	virtual BOOL PostPacket(int cmd, const void* pData, int len) { ASSERT(FALSE); return FALSE; }

	// 重置Socket对象的状态
	virtual void Reset();

	// SockServer销毁Socket对象是使用的函数
	// 缺省实现时Delete，如果使用了对象池技术的话，则需要重写这个函数
	virtual void Destroy();

public:
	// SOCKET在Active状态时，每隔一定时间调用一次
	// Socket对象可以再这里检查对象的活跃时间，收发是否正常，或者其他的检查
	virtual BOOL OnActiveState();
	virtual void OnBreathe(int intervalTcks);

	// processActiveQueue, processClosedQueue
	virtual void _onProcessActiveQueue(KBaseSockServer* sockServer, DWORD nowTicks, int msInterval) = 0;
	virtual void _onProcessClosedQueue(KBaseSockServer* sockServer, DWORD nowTicks, int msInterval) = 0;

public:
	// Socket对象刚关联到SockServer上时的通知
	virtual BOOL OnAttached();

	// Socket对象从SockServer卸下的通知
	virtual BOOL OnDetached();

	// 收到非法包的通知
	virtual BOOL OnInvalidPacket(const KSocketAddress* pFrom, ePacketReason reason);

	virtual BOOL OnSendData(const void* pData, int len, const KSocketAddress* pTarget);

	// 返回值表示在函数中消耗了多少个字节，小于 0 表示错误
	virtual int OnRecvData(const void* pData, int len, const KSocketAddress* pFrom);

	// Socket的IO操作发生了错误
	virtual BOOL OnSocketError(DWORD errCode);
	virtual void OnWouldBlock();

	// Tcp Listen Scket 接受了一个连接请求
	virtual BOOL OnAccepted(SOCKET sock);

	// TCP/ClientTcp socket 连接建立时的通知
	// 对于 UDP Socket，这是可以开始 IO 的时机通知
	virtual BOOL OnConnected();

	// SOCKET被关闭时的通知
	virtual BOOL OnClosed();

	// Socket对象的字符串表示
	virtual ::System::Collections::KString<256> ToString() const = 0;

	virtual BOOL _canDestroy() { return m_refCount < 1; }
	void _doClose();

public:
	// 开始IO工作前的通知，在这里把一些Buffer和滋生的一些状态重置一下
	virtual void OnStartWork();
	
	// 真正执行CloseSocket，所有sockserver中的SOCKET的close操作都会走这个函数
	void _Close();

	// 缺省的销毁对象的方式，直接Delete
	void Destroy_Delete();

public:
	// 用户数据
	DWORD m_userData;

	// 保护Socket对象内部成员的互斥量
	System::Sync::KRecursiveMutex m_mx;

	SOCKET m_sock;
	SOCKET m_sockCopy;			// 关闭SOCKET时，会把SOCKET的值拷贝到这里，从EPOLL中删除需要使用

	int m_socketState;
	volatile BOOL m_closeDesired;

	// 是否准备好了，可以进行IO操作了
	// Send操作是从外部发起的，通过这个值可以把一些过早的Send挡住
	volatile BOOL m_readyForIO;

	int m_sockIndex;          // Socket在SockServer队列中的索引号
	volatile int m_refCount;  // 这个Socket的引用计数，只有在0的时候才能被销毁

	INT64 m_sendBytes;        // 开始工作以来发送的字节数
	INT64 m_recvBytes;        // 开始工作以来接收的字节数
	DWORD m_startTick;        // SOCKET开始工作的时刻，GetTickCount
	DWORD m_lastestSendTick;  // 最近一次发送成功的时刻，GetTickCount
	DWORD m_lastestRecvTick;  // 最近一次接收成功的时刻，GetTickCount
	DWORD m_closeTick;        // the time ticks of close socket.

	DWORD m_delayCloseTick;		// 延迟关闭的时间，0表示没有设定延迟关闭
	volatile BOOL m_debugState;

	volatile BOOL m_sendPaused;
	volatile int m_sendPauseTicks;
};

// 给 TCP Socket 提供所需要的各种 Buffer
class KTcpSocketState
{
public:
	KTcpSocketState();
	virtual ~KTcpSocketState();

public:
	void Reset();
	void SetSendBufferSize(int kSize);
	void SetRemoteAddress(DWORD ip, WORD port);
	const KSocketAddress& GetRemoteAddress() const;

public:
	virtual KSocket* GetSocket();
	virtual BOOL HasDataToSend();
	virtual int PrepareSendBuffer();
	virtual int PrepareSendBuffer(int bytes);

public:
	// 当发送字节数达到vol个字节时，停止发送msWait毫秒
	int PrepareSendBuffer_Vol(int vol, int msWait);
	int PrepareSendBuffer_Vol2(int vol, int msWait);
	
public:
	KPieceBuffer<1280> m_sendBuf;			// send() 时使用的缓冲区
	KPieceBuffer<2048> m_recvBuf;			// recv() 时使用的缓冲区
	KIOBuffer m_sendIOBuffer;				// 发送数据的缓存队列
	KSocketAddress m_remoteAddress;
	int m_currSendBytes;					// 本次暂停后，已发送字节数
};

#endif
