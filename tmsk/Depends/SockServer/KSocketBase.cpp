#include "KSocketBase.h"
#include <System/Log/log.h>
#include <System/Misc/StrUtil.h>
#include "KSockServer.h"

using namespace ::System::Collections;
using namespace System::Sync;

BOOL KIpMask::isValid() const
{
	static KIpMask invalidMask = {0xffffffff, 0xffffffff};
	return memcmp(this, &invalidMask, sizeof(KIpMask)) != 0;
}

BOOL KIpMask::match(DWORD ip) const
{
	return (mask & ip) == target;
}

BOOL KIpMask::match( const char* ip ) const
{
	char buf[32];
	strcpy_k(buf, sizeof(buf), ip);

	char* abcd[4];
	int n = split(buf, '.', abcd, 4);
	if(n != 4) return FALSE;

	DWORD a = str2int(abcd[0]);
	DWORD b = str2int(abcd[1]);
	DWORD c = str2int(abcd[2]);
	DWORD d = str2int(abcd[3]);

	DWORD dwIP = mkip(a, b, c, d);

	return match(dwIP);
}

KIpMask KIpMask::build(const char* ipAddress)
{
	char buf[32];
	strcpy_k(buf, sizeof(buf), ipAddress);

	KIpMask ipMask = {0xffffffff, 0xffffffff};

	char* abcd[4];
	int n = split(buf, '.', abcd, 4);
	if(n != 4) return ipMask;

	::System::Collections::KString<16> star("*");

	::System::Collections::KString<16> sa = abcd[0];
	::System::Collections::KString<16> sb = abcd[1];
	::System::Collections::KString<16> sc = abcd[2];
	::System::Collections::KString<16> sd = abcd[3];

	DWORD a = sa == star ? 0 : str2int(sa.c_str());
	DWORD b = sb == star ? 0 : str2int(sb.c_str());
	DWORD c = sc == star ? 0 : str2int(sc.c_str());
	DWORD d = sd == star ? 0 : str2int(sd.c_str());

	DWORD ma = sa == star ? 0 : 255;
	DWORD mb = sb == star ? 0 : 255;
	DWORD mc = sc == star ? 0 : 255;
	DWORD md = sd == star ? 0 : 255;

	ipMask.mask = mkip(ma, mb, mc, md);
	ipMask.target = mkip(a, b, c, d);

	return ipMask;
}


KIpMask KIpMask::build2( const char* ipAddress, const char* mask )
{
	KIpMask ipMask = {0xffffffff, 0xffffffff};

	char buf[32];
	strcpy_k(buf, sizeof(buf), ipAddress);		

	char* abcd[4];
	int n = split(buf, '.', abcd, 4);
	if(n != 4) return ipMask;

	DWORD a = str2int(abcd[0]);
	DWORD b = str2int(abcd[1]);
	DWORD c = str2int(abcd[2]);
	DWORD d = str2int(abcd[3]);	

	strcpy_k(buf, sizeof(buf), mask);

	n = split(buf, '.', abcd, 4);
	if(n != 4) return ipMask;

	DWORD ma = str2int(abcd[0]);
	DWORD mb = str2int(abcd[1]);
	DWORD mc = str2int(abcd[2]);
	DWORD md = str2int(abcd[3]);

	ipMask.target = mkip(a, b, c, d);
	ipMask.mask = mkip(ma, mb, mc, md);	

	return ipMask;
}

bool KSocketAddress::operator == (const KSocketAddress& o) const
{
	return m_ip == o.m_ip && m_port == o.m_port;
}

sockaddr_in KSocketAddress::toSockAddr() const
{
	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = htonl(m_ip);
	sockAddr.sin_port = htons(m_port);
	return sockAddr;
}

JG_C::KString<64> KSocketAddress::toString() const
{
	char cTmp[64];
	sockaddr_in sockAddr = this->toSockAddr();
	sprintf_k(cTmp, sizeof(cTmp), "%s:%u", inet_ntoa(sockAddr.sin_addr), m_port);
	return cTmp;
}

KSocketAddress KSocketAddress::FromSockAddr(const sockaddr_in& sockAddr)
{
	KSocketAddress addr;
	addr.m_ip = ntohl(sockAddr.sin_addr.s_addr);
	addr.m_port = ntohs(sockAddr.sin_port);
	return addr;
}

sockaddr_in KSocketAddress::ToSockAddr(const KSocketAddress& addr)
{
	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = htonl(addr.m_ip);
	sockAddr.sin_port = htons(addr.m_port);
	return sockAddr;
}

// 192.168.10.46:9998
KSocketAddress KSocketAddress::FromString(const char* addrStr)
{
	KSocketAddress sockAddr = {0,0};
	KIpAddr ipAddr = str2ipaddr(addrStr);
	if(!ipAddr.isValid()) return sockAddr;
	sockAddr.m_ip = ipAddr.ip;
	sockAddr.m_port = ipAddr.port;
	return sockAddr;
}

KString<64> KSocketAddress::ToString(const KSocketAddress& addr)
{
	char cTmp[64];
	sockaddr_in sockAddr = KSocketAddress::ToSockAddr(addr);
	sprintf(cTmp, "%s:%u", inet_ntoa(sockAddr.sin_addr), addr.m_port);
	return cTmp;
}

KString<32> KSocketAddress::ToString(DWORD ip)
{
	DWORD nip = htonl(ip);
	return inet_ntoa(*(in_addr*)&nip);
}

KSocket::KSocket()
	: m_sock(INVALID_SOCKET)
	, m_closeDesired(FALSE)
	, m_socketState(EnumSocketState::none)
{
	KSocket::Reset();
}

KSocket::~KSocket()
{
	this->_Close();
}

SOCKET KSocket::CreateTcpSocket()
{
#if defined(WIN32) || defined(WINDOWS)
	SOCKET sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
#else
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
#endif
	if(sock != INVALID_SOCKET)
	{
		KSocket::SetNonBlock(sock);
		KSocket::IgnoreSigPipe(sock);
	}
	return sock;
}

SOCKET KSocket::CreateUdpSocket()
{
#if defined(WIN32) || defined(WINDOWS)
	SOCKET sock = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
#else
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
#endif
	if(sock != INVALID_SOCKET)
	{
		KSocket::SetNonBlock(sock);
		KSocket::IgnoreSigPipe(sock);
	}
	return sock;
}

BOOL KSocket::SetNonBlock(SOCKET sock)
{
#if defined(WIN32) || defined(WINDOWS)
	unsigned long ul = 1;
	int nRet = ioctlsocket(sock, FIONBIO, (unsigned long*)&ul);
#else
	int oldFlags = fcntl(sock, F_GETFL);
	if(oldFlags == -1)
	{
		DWORD errCode = GetLastError();
		Log(LOG_ERROR, "error: get socket flag, %s", strerror_r2(errCode).c_str());
		oldFlags = 0;
	}
	int nRet = fcntl(sock, F_SETFL, oldFlags | O_NONBLOCK);
#endif

	if (nRet)
	{
		DWORD errCode = GetLastError();
		Log(LOG_ERROR, "error: set socket %u non block, %s", sock, strerror_r2(errCode).c_str());
		return FALSE;
	}
	return TRUE;
}

BOOL KSocket::IgnoreSigPipe(SOCKET sock)
{
#if defined(WIN32) || defined(WINDOWS)
	return TRUE;
#else
	//int set = 1;
	//int nRet = setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
	//return nRet != -1;
	return TRUE;
#endif
}

DWORD KSocket::GetIpByName(const char* addrName)
{
#if defined(WIN32) || defined(WINDOWS)
	unsigned long ip = inet_addr(addrName);
	if(ip != INADDR_NONE) return ntohl(ip);
#else
	in_addr_t ip = inet_addr(addrName);
	if(ip != INADDR_NONE) return ntohl(ip);
#endif

	struct hostent* pHostent = gethostbyname(addrName);
	if(!pHostent)
	{
		DWORD errCode = GetLastError();
		Log(LOG_WARN, "warn: gethostname %s, %s", addrName, strerror_r2(errCode).c_str());
		return 0;
	}

	if(pHostent->h_addrtype != AF_INET)
	{
		Log(LOG_WARN, "warn: %s not a AF_INET address", addrName);
		return 0;
	}

	int i;
	DWORD nips[64];
	for(i = 0; i < 64 && pHostent->h_addr_list[i] != NULL; i++)
	{
		nips[i] = *(DWORD*)pHostent->h_addr_list[i];
	}

	DWORD now = GetTickCount();
	if(now & 0x80000000) now &= 0x7fffffff;

	int idx = now % i;
	DWORD nip = nips[idx];
	return ntohl(nip);
}

void KSocket::SetSendBuffer(int bytes)
{
	socklen_t sendbuflen = bytes;
	socklen_t len = sizeof(sendbuflen);
	setsockopt(m_sock, SOL_SOCKET, SO_SNDBUF, (const char*)&sendbuflen, len);
}

DWORD KSocket::GetSocketError() const
{
	int err = 0;
	socklen_t len = sizeof(err);
	getsockopt(m_sock, SOL_SOCKET, SO_ERROR, (char*)&err, &len);
	return err;
}

BOOL KSocket::Close()
{
	//if(!m_closeDesired)
	//{
	//	char buf[2048]; backtrace(buf, sizeof(buf));
	//	Log(LOG_WARN, "warn: KSocket::Close() %s at: %s", ToString().c_str(), buf);
	//}
	m_closeDesired = TRUE;
	return TRUE;
}

BOOL KSocket::DelayClose(int delaySecs)
{
	const int SD_MODE_BOTH  = 0;
	const int SD_MODE_READ  = -1;
	const int SD_MODE_WRITE = -2;

	m_mx.Lock();
	if(m_sock == INVALID_SOCKET)
	{
		m_mx.Unlock();
		return FALSE;
	}

	if(delaySecs > 0)
	{
		//if(!m_delayCloseTick)
		//{
		//	char buf[2048]; backtrace(buf, sizeof(buf));
		//	Log(LOG_WARN, "warn: KSocket::DelayClose(%d) %s at: %s", delaySecs, ToString().c_str(), buf);
		//}
		m_delayCloseTick = GetTickCount() + delaySecs*1000;
		m_mx.Unlock();
	}
	else
	{	// shutdown read and write
#if defined(WIN32)
		const int SHUT_RD = SD_RECEIVE;
		const int SHUT_WR = SD_SEND;
		const int SHUT_RDWR = SD_BOTH;
#endif
		//{//////////////////////////////////////////////
		//	char buf[2048]; backtrace(buf, sizeof(buf));
		//	Log(LOG_WARN, "warn: KSocket::DelayClose(%d) %s at: %s", delaySecs, ToString().c_str(), buf);
		//}
		switch(delaySecs)
		{
		case SD_MODE_BOTH:
			shutdown(m_sock, SHUT_RDWR);
			break;
		case SD_MODE_READ:
			shutdown(m_sock, SHUT_RD);
			break;
		case SD_MODE_WRITE:
			shutdown(m_sock, SHUT_WR);
			break;
		default:
			shutdown(m_sock, SHUT_RDWR);
			break;
		}
		m_mx.Unlock();
	}

	return TRUE;
}

void KSocket::PauseSend(int ticks)
{
	m_sendPauseTicks += ticks;
	m_sendPaused = TRUE;
}

void KSocket::Destroy_Delete()
{
	delete this;
}

void KSocket::Reset()
{
	this->_Close();

	// 用户数据留给用户自己清除
	//m_userData = 0;
	
	m_closeDesired = FALSE;
	m_socketState = EnumSocketState::none;
	m_readyForIO = FALSE;

	m_sockIndex = -1;
	m_refCount = 0;

	m_sendBytes = 0;
	m_recvBytes = 0;
	m_startTick = 0;
	m_lastestSendTick = 0;
	m_lastestRecvTick = 0;
	m_closeTick = 0;

	m_delayCloseTick = 0;
	m_debugState = FALSE;

	m_sendPaused = FALSE;
	m_sendPauseTicks = 0;
}

void KSocket::Destroy()
{
	this->Destroy_Delete();
}

void KSocket::AddRef()
{
	m_mx.Lock();
	m_refCount += 1;
	m_mx.Unlock();
}

void KSocket::ReleaseRef()
{
	m_mx.Lock();
	m_refCount -= 1;
	m_mx.Unlock();

	ASSERT(m_refCount >= 0);
}

int KSocket::GetRefCount()
{
	return m_refCount;
}

KSocketAddress KSocket::GetLocalAddress() const
{
	KSocketAddress sockAddr = {0,0};
	if(m_sock == INVALID_SOCKET) return sockAddr;

	char buf[128];
	socklen_t addrLen = sizeof(buf);
	sockaddr_in& localAddr = *(sockaddr_in*)&buf;
	int nRet = getsockname(m_sock, (sockaddr*)&localAddr, &addrLen);
	if(nRet == -1) return sockAddr;

	return KSocketAddress::FromSockAddr(localAddr);
}

DWORD KSocket::GetIdleTicks() const
{
	DWORD now = GetTickCount();
	DWORD sendIdleTicks = now - m_lastestSendTick;
	DWORD recvIdleTicks = now - m_lastestRecvTick;

	if(sendIdleTicks > recvIdleTicks)
		return sendIdleTicks;
	return recvIdleTicks;
}

DWORD KSocket::GetLatestSendTick() const
{
	return m_lastestSendTick;
}

DWORD KSocket::GetLatestRecvTick() const
{
	return m_lastestRecvTick;
}

DWORD KSocket::GetSendSpeed() const
{
	DWORD now = GetTickCount();
	DWORD workTicks = now - m_startTick;
	return (DWORD)(m_sendBytes / workTicks);
}

DWORD KSocket::GetRecvSpeed() const
{
	DWORD now = GetTickCount();
	DWORD workTicks = now - m_startTick;
	return (DWORD)(m_recvBytes / workTicks);
}

BOOL KSocket::OnActiveState()
{
	return TRUE;
}

void KSocket::OnBreathe(int intervalTcks)
{
	if(m_sendPaused)
	{
		if(m_sendPauseTicks <= intervalTcks)
		{
			m_sendPauseTicks = 0;
			m_sendPaused = FALSE;
		}
		else
		{
			m_sendPauseTicks -= intervalTcks;
		}
	}
}

BOOL KSocket::OnAttached()
{
	return TRUE;
}

BOOL KSocket::OnDetached()
{
	return TRUE;
}

//BOOL KSocket::OnSendComplete(const void* packet, const KSocketAddress* pTarget)
//{
//	return TRUE;
//}

//BOOL KSocket::OnRecvComplete(const void* pPacket, const KSocketAddress* pFrom)
//{
//	return TRUE;
//}

BOOL KSocket::OnInvalidPacket(const KSocketAddress* pFrom, ePacketReason reason)
{
	Log(LOG_WARN, "warn: %s invalid packet from %s, reason:%u, close it",
		this->ToString().c_str(),
		KSocketAddress::ToString(*pFrom).c_str(),
		reason);
	this->Close();
	return TRUE;
}

BOOL KSocket::OnSendData(const void* pData, int len, const KSocketAddress* pTarget)
{
	return TRUE;
}

int KSocket::OnRecvData(const void* pData, int len, const KSocketAddress* pFrom)
{
	return len;
}

BOOL KSocket::OnSocketError(DWORD errCode)
{
	Log(LOG_DEBUG, "debug: %s socket error, close it, %s", this->ToString().c_str(), strerror_r2(errCode).c_str());
	this->Close();
	return TRUE;
}

void KSocket::OnWouldBlock()
{
	return;
}

BOOL KSocket::OnAccepted(SOCKET sock)
{
	ASSERT(this->GetSocketType() == KSocket::SOCK_TCP_LISTEN);
	Log(LOG_INFO, "info: %s accept a new socket %u, you should create new socket object here",
		this->ToString().c_str(), sock);
	return TRUE;
}

BOOL KSocket::OnConnected()
{
	return TRUE;
}

BOOL KSocket::OnClosed()
{
	return TRUE;
}

// IOCP  在维护线程中执行
// EPOLL 在IO线程中执行
void KSocket::_doClose()
{
	m_closeDesired = FALSE;
	m_sockCopy = m_sock;
	m_closeTick = GetTickCount();
	m_readyForIO = FALSE;

	this->_Close();

	switch(m_socketState)
	{
	case EnumSocketState::none:
		m_socketState = EnumSocketState::closed;
		break;
	case EnumSocketState::fresh:
		m_socketState = EnumSocketState::closed;
		break;
	case EnumSocketState::connecting:
		m_socketState = EnumSocketState::closed;
		break;
	case EnumSocketState::connected:
		m_socketState = EnumSocketState::closing;
		break;
	case EnumSocketState::closing:
		break;
	case EnumSocketState::closed:
		break;
	case EnumSocketState::destroying:
		break;
	default:
		ASSERT(FALSE); break;
	};
}

void KSocket::OnStartWork()
{
	ASSERT(m_sock != INVALID_SOCKET);
	m_sendBytes = 0;
	m_recvBytes = 0;
	m_startTick = GetTickCount();
	m_lastestSendTick = 0;
	m_lastestRecvTick = 0;
	m_closeTick = 0;
	m_delayCloseTick = 0;
}

void KSocket::_Close()
{
	if(m_sock != INVALID_SOCKET)
	{
		if(m_debugState)
		{
			Log(LOG_ERROR, "sockDebug: %s close", this->ToString().c_str());
		}

		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
}

KTcpSocketState::KTcpSocketState()
{
	memset(&m_remoteAddress, 0, sizeof(KSocketAddress));
	m_currSendBytes = 0;
}

KTcpSocketState::~KTcpSocketState()
{

}

void KTcpSocketState::Reset()
{
	m_sendBuf.Reset();
	m_recvBuf.Reset();
	m_sendIOBuffer.Reset();

	m_currSendBytes = 0;
	memset(&m_remoteAddress, 0, sizeof(KSocketAddress));
}

void KTcpSocketState::SetSendBufferSize(int kSize)
{
	m_sendIOBuffer.SetCapacity(kSize);
}

void KTcpSocketState::SetRemoteAddress(DWORD ip, WORD port)
{
	m_remoteAddress.m_ip = ip;
	m_remoteAddress.m_port = port;
}

const KSocketAddress& KTcpSocketState::GetRemoteAddress() const
{
	return m_remoteAddress;
}

KSocket* KTcpSocketState::GetSocket()
{
	return NULL;
}

BOOL KTcpSocketState::HasDataToSend()
{
	return m_sendIOBuffer.Size() || (m_sendBuf.m_rp < m_sendBuf.m_wp);
}

int KTcpSocketState::PrepareSendBuffer()
{
	int readed = 0;
	KSocket* pSock = this->GetSocket();

	m_sendBuf.Reset();

	int len = (int)m_sendBuf.WriteAvail();
	char* p = m_sendBuf.GetWriteBuffer();

	pSock->m_mx.Lock();
	readed = m_sendIOBuffer.Read(p, len);
	pSock->m_mx.Unlock();

	m_sendBuf.m_wp = readed;
	return readed;
}

int KTcpSocketState::PrepareSendBuffer(int bytes)
{
	int readed = 0;
	KSocket* pSock = this->GetSocket();

	m_sendBuf.Reset();
	if(bytes <= 0) return 0;

	int len = (int)m_sendBuf.WriteAvail();
	char* p = m_sendBuf.GetWriteBuffer();

	if(len > bytes) len = bytes;

	pSock->m_mx.Lock();
	readed = m_sendIOBuffer.Read(p, len);
	pSock->m_mx.Unlock();

	m_sendBuf.m_wp = readed;
	return readed;
}

int KTcpSocketState::PrepareSendBuffer_Vol(int vol, int msWait)
{
	KSocket* pSock = this->GetSocket();
	if(!pSock) return 0;

	if(pSock->m_sendPaused)
		return 0;

	int allowBytes = vol - m_currSendBytes;
	int bytes = KTcpSocketState::PrepareSendBuffer(allowBytes);

	m_currSendBytes += bytes;
	if(m_currSendBytes >= vol)
	{
		m_currSendBytes = 0;
		pSock->PauseSend(msWait);
	}

	return bytes;
}

int KTcpSocketState::PrepareSendBuffer_Vol2(int vol, int msWait)
{
	KSocket* pSock = this->GetSocket();
	if(!pSock) return 0;

	if(pSock->m_sendPaused)
		return 0;

	int bytes = KTcpSocketState::PrepareSendBuffer(vol);
	if(bytes > 0)
	{
		pSock->PauseSend(msWait);
	}

	return bytes;
}
