#include "JgVirtualSocket.h"
#include <GlobalErrorCode.h>

JgVirtualSocket::JgVirtualSocket()
	: m_cnnReady(FALSE)
	, m_peerClose(FALSE)
	, m_cnnTime(0)
	, m_cnnRemainMs(0)
	, m_closeTime(0)
	, m_closeRemainMs(0)
	, m_closeReason(0)
	, m_udata(0)
	, m_delayCloseRemain(0)
	, m_refCount(0)
{
	
}

JgVirtualSocket::~JgVirtualSocket()
{

}

uint_r JgVirtualSocket::GetUserData() const
{
	return m_udata;
}

void JgVirtualSocket::SetUserData(uint_r udata)
{
	m_udata = udata;
}

int JgVirtualSocket::AddRef()
{
	return ++m_refCount;
}

int JgVirtualSocket::ReleaseRef()
{
	return --m_refCount;
}

BOOL JgVirtualSocket::PeerClose()
{
	if(m_closeTime) return TRUE;
	m_peerClose = TRUE;
	return this->Close();
}

void JgVirtualSocket::Breathe(int msec)
{
	if(m_cnnReady && m_delayCloseRemain)
	{
		m_delayCloseRemain = kmax(m_delayCloseRemain-msec, 0);
		if(!m_delayCloseRemain)
			this->Close();
	}
	else if(!m_cnnReady && m_cnnTime && m_cnnRemainMs)
	{
		m_cnnRemainMs = kmax(m_cnnRemainMs-msec, 0);
		if(!m_cnnRemainMs)
			this->OnConnectError(err_Connect_Timeout);
	}
	else if(m_closeTime && m_closeRemainMs)
	{
		m_closeRemainMs = kmax(m_closeRemainMs-msec, 0);
		if(!m_closeRemainMs)
		{
			this->OnSocketError(err_Close_Timeout);
			this->OnClosed();
		}
	}
}

BOOL JgVirtualSocket::OnSocketError(DWORD errCode)
{
	Log(LOG_ERROR, "OnSocketError: %s errCode:%u", this->ToString().c_str(), errCode);
	return TRUE;
}

BOOL JgVirtualSocket::DelayClose(int secs)
{
	if(!m_cnnReady || m_closeTime) return TRUE;
	if(m_delayCloseRemain) return TRUE;
	m_delayCloseRemain = kmax(secs*1000, 1);
	return TRUE;
}

void JgVirtualSocket::Reset()
{
	m_recvIoBuff.Reset();
	m_denyRecv = FALSE;
	m_cnnReady = FALSE;
	m_peerClose = FALSE;
	m_cnnTime = 0;
	m_cnnRemainMs = 0;
	m_closeTime = 0;
	m_closeRemainMs = 0;
	m_closeReason = 0;
	m_udata = 0;
	m_delayCloseRemain = 0;
	m_refCount = 0;
}

BOOL JgVirtualSocket::PostPacket(int cmd, const void* pData, int len)
{
	KSocketAddress fromAddr = {0, 0};
	ASSERT(len >= 0 && len <= JgPacketConst::MaxNetDataSize);
	if(m_denyRecv) return FALSE;

	char buffer[JgPacketConst::NetPacketSize];
	JgPacketHead* pHead = (JgPacketHead*)&buffer;
	pHead->flag = 0;
	pHead->length = len;
	pHead->command = cmd;
	if(pData && len > 0) memcpy(pHead+1, pData, len);
	if(!this->_WriteOnePacket_Force(&buffer, sizeof(JgPacketHead)+len, fromAddr))
	{
		Log(LOG_WARN, "warn: post packet cmd:%u len:%u sock:%s", cmd, len, this->ToString().c_str());
		return FALSE;
	}
	return TRUE;
}

BOOL JgVirtualSocket::PostFinalPacket(int cmd, const void* pData, int len)
{
	KSocketAddress fromAddr = {0, 0};
	ASSERT(len >= 0 && len <= JgPacketConst::MaxNetDataSize);
	if(m_denyRecv) return FALSE; m_denyRecv = TRUE;

	char buffer[JgPacketConst::NetPacketSize];
	JgPacketHead* pHead = (JgPacketHead*)&buffer;
	pHead->flag = 0;
	pHead->length = len;
	pHead->command = cmd;
	if(pData && len > 0) memcpy(pHead+1, pData, len);
	if(!this->_WriteOnePacket_Force(&buffer, sizeof(JgPacketHead)+len, fromAddr))
	{
		Log(LOG_WARN, "warn: post packet cmd:%u len:%u sock:%s", cmd, len, this->ToString().c_str());
		return FALSE;
	}

	return TRUE;
}

BOOL JgVirtualSocket::CloseByReason(int errCode)
{
	if(!m_cnnReady || m_closeTime) return TRUE;
	m_closeReason = errCode;
	return this->Close();
}

BOOL JgVirtualSocket::DelayCloseByReason(int secs, int errCode)
{
	if(!m_cnnReady || m_closeTime) return TRUE;
	if(m_delayCloseRemain) return TRUE;
	m_closeReason = errCode;
	return this->DelayClose(secs);
}

BOOL JgVirtualSocket::PeerCloseByReason(int errCode)
{
	if(m_closeTime) return TRUE;
	m_closeReason = errCode;
	return this->PeerClose();
}

BOOL JgVirtualSocket::OnAppPacket(const void* pPacket, int len, const KSocketAddress& fromAddr)
{
	return m_denyRecv ? FALSE : this->_WriteOnePacket(pPacket, len, fromAddr);
	//return JgPacketReceiver::OnAppPacket(pPacket, len, fromAddr);
}

BOOL JgVirtualSocket::_WriteOnePacket(const void* pPacket, int len, const KSocketAddress& fromAddr)
{
	//m_mx.Lock();
	int writeAvail = m_recvIoBuff.WriteAvail();
	if(writeAvail < len)
	{
		//m_mx.Unlock();
		Log(LOG_ERROR, "error: recv buffer full, sock:%s", this->ToString().c_str());
		return FALSE;
	}
	m_recvIoBuff.Write(pPacket, len);
	//m_mx.Unlock();

	return TRUE;
}

int JgVirtualSocket::_ReadOnePacket(char* pDst, int len)
{
	JgPacketHead packetHead;

	int nBytes = m_recvIoBuff.Peek(&packetHead, sizeof(packetHead));
	if(nBytes < sizeof(JgPacketHead))
	{
		ASSERT(nBytes == 0);
		return 0;
	}

	if(!packetHead.HasTail())
	{
		int packetLength = sizeof(JgPacketHead) + packetHead.Length();
		if(len < packetLength) return 0;

		nBytes = m_recvIoBuff.Read(pDst, packetLength);
		ASSERT(nBytes == packetLength);
		return packetLength;
	}
	else
	{
		int length1 = sizeof(JgPacketHead) + packetHead.Length() + sizeof(JgPacketTail);
		if(length1+JgPacketConst::MaxTailDataSize > len) return 0;

		nBytes = m_recvIoBuff.Read(pDst, length1);
		ASSERT(nBytes == length1);

		JgPacketTail* pTail = (JgPacketTail*)(pDst + sizeof(JgPacketHead) + packetHead.Length());
		pDst += length1;

		int tailLength = pTail->Length();
		if(tailLength)
		{
			nBytes = m_recvIoBuff.Read(pDst, tailLength);
			ASSERT(nBytes == tailLength);
		}

		return length1 + tailLength;
	}
}

BOOL JgVirtualSocket::_WriteOnePacket_Force(const void* pPacket, int len, const KSocketAddress& fromAddr)
{
	//m_mx.Lock();
	int nBytes = m_recvIoBuff.Write_Force(pPacket, len);
	//m_mx.Unlock();
	ASSERT(nBytes == len);
	return TRUE;
}

int JgVirtualSocket::ReadPackets(void* pBuff, int len, PacketPtrArray& arr)
{
	arr.clear();
	if(m_recvIoBuff.Size() < sizeof(JgPacketHead)) return 0;

	int nBytes;
	char* pDst = (char*)pBuff;

	//m_mx.Lock();
	while(TRUE)
	{
		nBytes = this->_ReadOnePacket(pDst, len);
		if(nBytes < 1) break;

		arr.push_back(pDst);

		pDst += nBytes;
		len -= nBytes;
	}
	//m_mx.Unlock();

	return arr.size();
}

int JgVirtualSocket::ReadPackets(void* pBuff, int len, PacketPtrArray& arr, int maxPackets)
{
	arr.clear();
	if(m_recvIoBuff.Size() < sizeof(JgPacketHead)) return 0;

	int nBytes;
	char* pDst = (char*)pBuff;

	//m_mx.Lock();
	while(arr.size() < maxPackets)
	{
		nBytes = this->_ReadOnePacket(pDst, len);
		if(nBytes < 1) break;

		arr.push_back(pDst);

		pDst += nBytes;
		len -= nBytes;
	}
	//m_mx.Unlock();

	return arr.size();
}

const KSocketAddress& JgVirtualSocket::GetRemoteAddress() const
{
	static KSocketAddress addr = {0,0};
	return addr;
}

///

JgVirtualSocketManager::JgVirtualSocketManager()
{

}

JgVirtualSocketManager::~JgVirtualSocketManager()
{

}

void JgVirtualSocketManager::breathe(int msec)
{
	int n = m_sockArr.size();
	for(int i=n-1; i>=0; i--)
	{
		JgVirtualSocket* p = m_sockArr[i];
		p->Breathe(msec);
	}
}

bool JgVirtualSocketManager::attach(JgVirtualSocket* sock)
{
	if(m_sockArr.insert_unique(sock) >= 0)
	{
		sock->AddRef();
		return true;
	}
	return false;
}

void JgVirtualSocketManager::detach(JgVirtualSocket* sock)
{
	if(m_sockArr.erase_by_value(sock))
	{
		sock->ReleaseRef();
		return;
	}
}