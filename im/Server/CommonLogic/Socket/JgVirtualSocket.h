#pragma once
#include <SockServer/JgPacketSpecific.h>
#include "../Timer/KLogicTimer.h"
#include <KCommonStruct.h>

class JgVirtualSocket
	: public ISocketConnection
	, public JgPacketBuilder
	, public JgPacketReceiver
{
public:
	enum
	{
		cnn_timeout = 100*1000,   // msec
		close_timeout = 100*1000, // msec
	};

public:
	typedef string_512 SocketDesc;

public:
	JgVirtualSocket();
	virtual ~JgVirtualSocket();

public: // ISocketConnection
	BOOL SendPacket(int cmd, const void* data, int len)	{ return this->Send(cmd, data, len); }
	void AddRef_2()										{ this->AddRef(); }
	void ReleaseRef_2()									{ this->ReleaseRef(); }
	void Close_2()										{ this->Close(); }
	void DelayClose_2(int secs)							{ this->DelayClose(secs); }
	const KSocketAddress& GetRemoteAddress_2()			{ return this->GetRemoteAddress(); }

public:
	uint_r GetUserData() const;
	void SetUserData(uint_r udata);
	
	virtual int AddRef();
	virtual int ReleaseRef();
	
	BOOL PeerClose();

public: // fake JgServerTcpSocket
	virtual void Breathe(int msec);

	virtual BOOL Connect(const KSocketAddress& addr) { return TRUE; }
	virtual BOOL Close() = 0;

	virtual BOOL OnConnected()
	{
		if(m_cnnReady) return FALSE;
		m_peerClose = FALSE;
		m_closeTime = 0;
		m_closeRemainMs = 0;
		m_closeReason = 0;
		m_delayCloseRemain = 0;
		m_cnnReady = TRUE;
		m_denyRecv = FALSE;
		return TRUE;
	}
	virtual BOOL OnConnectError(DWORD errCode) { return TRUE; }
	virtual BOOL OnSocketError(DWORD errCode);
	virtual BOOL OnClosed() { if(!m_cnnReady) return FALSE; m_cnnReady = FALSE; return TRUE; }

	virtual BOOL DelayClose(int secs);
	virtual void Reset();

	virtual BOOL PostPacket(int cmd, const void* data, int len);
	BOOL PostFinalPacket(int cmd, const void* data, int len);

	virtual BOOL Send(int cmd, const void* data, int len) = 0;
	virtual BOOL SendImmediate(int cmd, const void* data, int len) = 0;

	virtual const KSocketAddress& GetRemoteAddress() const;
	virtual SocketDesc ToString() const = 0;

public:
	BOOL CloseByReason(int errCode);
	BOOL DelayCloseByReason(int secs, int errCode);
	BOOL PeerCloseByReason(int errCode);

public: // JgPacketReceiver
	BOOL OnAppPacket(const void* pPacket, int len, const KSocketAddress& fromAddr);

	int _ReadOnePacket(char* pBuf, int len);
	BOOL _WriteOnePacket(const void* pPacket, int len, const KSocketAddress& fromAddr);
	BOOL _WriteOnePacket_Force(const void* pPacket, int len, const KSocketAddress& fromAddr);

public:
	int ReadPackets(void* pBuff, int len, PacketPtrArray& arr);
	int ReadPackets(void* pBuff, int len, PacketPtrArray& arr, int maxPackets);

public:
	BOOL m_cnnReady;
	
	BOOL m_peerClose; // close initialed by peer

	time_t m_cnnTime;	// 发起 connect 的时间
	int m_cnnRemainMs;	// how many milli-secs remained wait for connect

	time_t m_closeTime; // 发起 close 的时间
	int m_closeRemainMs; // how many milli-secs remained wait for close
	int m_closeReason;   // 关闭的原因，错误代码

	uint_r m_udata;
	int m_delayCloseRemain;
	volatile int m_refCount;
};

class JgVirtualSocketManager
{
public:
	class sock_cmp
	{
	public:
		typedef JgVirtualSocket* ptr;
		int operator () (const ptr& a, const ptr& b) const
		{
			return (int)((int_r)a - (int_r)b);
		}
	};
	typedef JG_C::KDynSortedArray<JgVirtualSocket*,sock_cmp,1024,1024> SocketArray;

public:
	JgVirtualSocketManager();
	~JgVirtualSocketManager();

public:
	void breathe(int msec);
	bool attach(JgVirtualSocket* sock);
	void detach(JgVirtualSocket* sock);

public:
	SocketArray m_sockArr;
};