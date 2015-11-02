#include "JgPacketSpecific.h"
#include <System/Log/log.h>
#include <System/Memory/KThreadLocalStorage.h>
#include <System/Misc/StrUtil.h>

using namespace System::Sync;

static bool _checkAppPacket(JgPacketHead* pHead, int len)
{
	if(pHead->flag)
	{
		int headLength = (int)sizeof(JgPacketHead) + (int)pHead->length;
		if(len < headLength + (int)sizeof(JgPacketTail)) return false;
		JgPacketTail* pTail = (JgPacketTail*)((char*)pHead + headLength);
		int tailLength = (int)sizeof(JgPacketTail) + (int)pTail->length;
		return headLength + tailLength == len;
	}
	else
	{
		int packetLength = (int)sizeof(JgPacketHead) + (int)pHead->length;
		return packetLength == len;
	}
}

// 返回值:
//   >0  : 包长度
//   0   : 包没有完成
//   -1  : 包结构数据错误
int JgPacketDeclare::CheckLength(JgPacketHead* pHead, int len)
{
	if(!pHead->HasTail())
	{
		int length = pHead->Length();
		length += sizeof(JgPacketHead);

		if(length > JgPacketConst::NetPacketSize) return -1;
		if(len < length) return 0;
		return length;
	}
	else
	{
		int length = sizeof(JgPacketHead) + pHead->Length();

		if(length+sizeof(JgPacketTail) > JgPacketConst::NetPacketSize) return -1;
		if(len < length+(int)sizeof(JgPacketTail)) return 0;

		JgPacketTail* pTail = (JgPacketTail*)((char*)pHead+length);
		length += sizeof(JgPacketTail) + pTail->Length();

		if(length > JgPacketConst::NetPacketSize) return -1;
		if(len < length) return 0;
		return length;
	}
}

BOOL JgPacketDeclare::ValidateAppPacket(JgPacketHead* pHead, int len)
{
	if(len < sizeof(JgPacketHead)) return FALSE;
	if(len > JgPacketConst::AppPacketSize) return FALSE;

	if(!pHead->HasTail())
	{
		return sizeof(JgPacketHead) + pHead->Length() == len;
	}
	else
	{
		int length1 = sizeof(JgPacketHead) + pHead->Length();
		if(len < length1+(int)sizeof(JgPacketTail)) return FALSE;

		JgPacketTail* pTail = (JgPacketTail*)((char*)pHead + length1);
		return length1+sizeof(JgPacketTail)+pTail->Length() == len;
	}
}

int JgPacketDeclare::GetLength(JgPacketHead* pHead)
{
	if(!pHead->HasTail())
	{
		return sizeof(JgPacketHead) + pHead->Length();
	}
	else
	{
		int len = sizeof(JgPacketHead) + pHead->Length();
		JgPacketTail* pTail = (JgPacketTail*)((char*)pHead + len);
		return len + sizeof(JgPacketTail) + pTail->Length();
	}
}

WORD JgPacketDeclare::CalcChecksum(const void* pData, int len)
{
	register WORD ans = 0;
	register DWORD sum = JgPacketConst::Checksum_Initial;
	register const BYTE* p = (const BYTE*)pData;

	while(len > 1)
	{
		*(BYTE*)&ans = *p++;
		*((BYTE*)&ans + 1) = *p++;
		sum += ans;
		len -= 2;
	}

	if(len)
	{
		ans = 0;
		*(BYTE*)&ans = *p;
		sum += ans;
	}

	sum = (sum >> 16) + (sum & 0x0FFFF);
	sum += (sum >> 16);
	ans = (WORD)~sum;

	return ans;
}

/// JgPacketBuffer
JgPacketBuffer::JgPacketBuffer()
{
	m_buffer = &m_buildBuffer_1;
}

JgPacketBuffer::~JgPacketBuffer()
{

}

int JgPacketBuffer::write(const void* data, int len)
{
	return (int)m_buffer->Write(data, len);
}

int JgPacketBuffer::read(void* buf, int len)
{
	return (int)m_buffer->Read(buf, len);
}

int JgPacketBuffer::dataSize() const
{
	return (int)m_buffer->ReadAvail();
}

const void* JgPacketBuffer::data() const
{
	return m_buffer->GetBuffer();
}

int JgPacketBuffer::size() const
{
	return (int)m_buffer->m_wp;
}

void JgPacketBuffer::resize(int length)
{
	m_buffer->m_rp = 0;
	m_buffer->m_wp = length;
}

const void* JgPacketBuffer::swap(int* bufferSize)
{
	const void* dataPtr = this->data();
	*bufferSize = this->size();
	if(m_buffer != &m_buildBuffer_1)
	{
		m_buffer = &m_buildBuffer_1;
		m_buffer->Reset();
		return dataPtr;
	}
	else
	{
		m_buffer = &m_buildBuffer_2;
		m_buffer->Reset();
		return dataPtr;
	}
}

BOOL JgPacketBuffer::innerBuild(int cmd, const void* data, int len)
{
	m_buffer->Reset();
	JgPacketHead head;
	head.flag = 0; head.length = len; head.command = cmd;
	size_t nRet = this->write(&head, sizeof(JgPacketHead));
	if(nRet != sizeof(JgPacketHead)) return FALSE;
	if(len)
	{
		size_t nBytes = this->write(data, len);
		return nBytes == len;
	}
	return TRUE;
}

void JgPacketBuffer::Reset()
{
	m_buildBuffer_1.Reset();
	m_buildBuffer_2.Reset();
}

//// JgPacketBuilder
JgPacketBuilder::JgPacketBuilder()
{
}

JgPacketBuilder::~JgPacketBuilder()
{

}

BOOL JgPacketBuilder::_Build(JgPacketBuffer* pPacketBuffer, int cmd, const void* pData, int len)
{
	return pPacketBuffer->innerBuild(cmd, pData, len);
}

BOOL JgPacketBuilder::Build(JgPacketBuffer* pPacketBuffer, int cmd, const void* pData, int len)
{
	pPacketBuffer->Reset();
	if(!this->_Build(pPacketBuffer, cmd, pData, len))
		return FALSE;

	int packetLength = (int)pPacketBuffer->size();
	if(packetLength <= JgPacketConst::NetPacketSize) return TRUE;
	ASSERT(packetLength <= JgPacketConst::AppPacketSize);

	JgPacketHead packetHead; packetHead.flag = 0;
	const char* packetData = (const char*)pPacketBuffer->swap(&packetLength);
	{
		packetHead.command = JgPacketConst::Assemble;
		packetHead.length = JgPacketConst::MaxNetDataSize;
		if(pPacketBuffer->write(&packetHead, sizeof(packetHead)) != sizeof(packetHead)) return FALSE;
		int bytes = pPacketBuffer->write(packetData, JgPacketConst::MaxNetDataSize);
		if(bytes != JgPacketConst::MaxNetDataSize) return FALSE;
		packetData += bytes; packetLength -= bytes;
	}
	while(packetLength > 0)
	{
		int want = kmin(packetLength, (int)JgPacketConst::MaxNetDataSize);
		packetHead.length = want;
		packetHead.command = packetLength > want ? JgPacketConst::Assemble : JgPacketConst::AssembleEnd;
		if(pPacketBuffer->write(&packetHead, sizeof(packetHead)) != sizeof(packetHead)) return FALSE;
		int bytes = pPacketBuffer->write(packetData, want);
		if(bytes != want) return FALSE;
		packetData += bytes; packetLength -= bytes;
	}

	return TRUE;
}

void JgPacketBuilder::Output(JgPacketBuffer* pPacketBuffer, const void*& pData, int& len)
{
	pData = pPacketBuffer->data();
	len = pPacketBuffer->size();
}

/// JgPacketReceiver

JgPacketReceiver::JgPacketReceiver()
	: m_pSock(NULL)
	, m_denyRecv(FALSE)
{

}

JgPacketReceiver::~JgPacketReceiver()
{
	m_recvIoBuff.Reset();
	m_assembleBuffer.reset();
}

void JgPacketReceiver::SetRecvBuffSize(int kSize)
{
	m_recvIoBuff.SetCapacity(kSize);
}

// 一次读取尽量多的包，直到缓冲区满，返回读到的包数量
int JgPacketReceiver::ReadPackets(void* pBuff, int len, PacketPtrArray& arr)
{
	arr.clear();
	if(m_recvIoBuff.Size() < sizeof(JgPacketHead)) return 0;

	int nBytes;
	char* pDst = (char*)pBuff;

	m_mxRecv.Lock();
	while(TRUE)
	{
		nBytes = this->_ReadOnePacket(pDst, len);
		if(nBytes < 1) break;

		arr.push_back(pDst);

		pDst += nBytes;
		len -= nBytes;
	}
	m_mxRecv.Unlock();

	return arr.size();
}

int JgPacketReceiver::ReadPackets(void* pBuff, int len, PacketPtrArray& arr, int maxPackets)
{
	arr.clear();
	if(m_recvIoBuff.Size() < sizeof(JgPacketHead)) return 0;

	int nBytes;
	char* pDst = (char*)pBuff;

	m_mxRecv.Lock();
	while(arr.size() < maxPackets)
	{
		nBytes = this->_ReadOnePacket(pDst, len);
		if(nBytes < 1) break;

		arr.push_back(pDst);

		pDst += nBytes;
		len -= nBytes;
	}
	m_mxRecv.Unlock();

	return arr.size();
}

void JgPacketReceiver::SetRecvBufferPool(KIOBuffPool_2048& pool)
{
	m_recvIoBuff.SetBufferPool(pool);
}

void JgPacketReceiver::SetAssemBuffPool(JgPacketReceiver::AssemBuffPool& pool)
{
	m_assembleBuffer.m_pool = &pool;
}

int JgPacketReceiver::ReadAvail()
{
	return m_recvIoBuff.Size();
}

// 收到一个完整的网络包
BOOL JgPacketReceiver::OnRecvComplete(const void* pPacket, int len, const KSocketAddress* pFrom)
{
	JgPacketHead* pHead = (JgPacketHead*)pPacket;
	
	int cmd = pHead->command;
	if(m_assembleBuffer.empty()) // not in assembling
	{
		switch(cmd)
		{
		case JgPacketConst::Assemble:
			{
				if(pHead->HasTail() || pHead->length != JgPacketConst::MaxNetDataSize)
				{
					KSocket* pSock = this->GetSocket();
					Log(LOG_ERROR, "error: invalid assemble packet, %s", pSock->ToString().c_str());
					pSock->Close();
					return FALSE;
				}
				m_assembleBuffer.write(pHead+1, pHead->length);
			}	break;
		case JgPacketConst::AssembleEnd:
			{
				KSocket* pSock = this->GetSocket();
				Log(LOG_ERROR, "error: not in assemble packet, %s", pSock->ToString().c_str());
				pSock->Close();
				return FALSE;
			}
		default:
			{
				if(pHead->length > JgPacketConst::MaxNetDataSize)
				{
					KSocket* pSock = this->GetSocket();
					Log(LOG_ERROR, "error: normal packet length exceed, %s", pSock->ToString().c_str());
					pSock->Close();
					return FALSE;
				}
				this->OnAppPacket(pHead, len, *pFrom);
			}	break;
		}
		return TRUE;
	}
	else // in assembling
	{
		switch(cmd)
		{
		case JgPacketConst::Assemble:
			{
				if(pHead->HasTail() || pHead->length != JgPacketConst::MaxNetDataSize)
				{
					KSocket* pSock = this->GetSocket();
					Log(LOG_ERROR, "error: invalid assemble packet, %s", pSock->ToString().c_str());
					pSock->Close();
					return FALSE;
				}
				m_assembleBuffer.write(pHead+1, pHead->length);
				if(m_assembleBuffer.size() > JgPacketConst::AppPacketSize)
				{
					KSocket* pSock = this->GetSocket();
					Log(LOG_ERROR, "error: invalid assemble packet, len:%d, %s", m_assembleBuffer.size(), pSock->ToString().c_str());
					pSock->Close();
					return FALSE;
				}
			}	break;
		case JgPacketConst::AssembleEnd:
			{
				if(pHead->HasTail() || pHead->length > JgPacketConst::MaxNetDataSize)
				{
					KSocket* pSock = this->GetSocket();
					Log(LOG_ERROR, "error: invalid assembleEnd packet, %s", pSock->ToString().c_str());
					pSock->Close();
					return FALSE;
				}

				m_assembleBuffer.write(pHead+1, pHead->length);

				int packetLength = m_assembleBuffer.size();
				if(packetLength > JgPacketConst::AppPacketSize)
				{
					KSocket* pSock = this->GetSocket();
					Log(LOG_ERROR, "error: invalid assemble packet, len:%d, %s", packetLength, pSock->ToString().c_str());
					pSock->Close();
					return FALSE;
				}
				
				char buf[1024];
				{
					JgSockServer* sockServer = (JgSockServer*)this->GetSocket()->GetSockServer();
					JgPacketBuffer* packetBuffer = sockServer->GetThreadPacketBuffer(); packetBuffer->Reset();
					
					while(1)
					{
						int n = m_assembleBuffer.read(buf, sizeof(buf));
						if(n > 0) packetBuffer->write(buf, n);
						if(n != sizeof(buf)) break;
					}

					JgPacketHead* pHead = (JgPacketHead*)packetBuffer->data();
					if(!_checkAppPacket(pHead, packetBuffer->size()))
					{
						KSocket* pSock = this->GetSocket();
						Log(LOG_ERROR, "error: invalid assemble packet, len:%d, %s", packetBuffer->size(), pSock->ToString().c_str());
						pSock->Close();
						return FALSE;
					}

					BOOL bRet = this->OnAppPacket(pHead, packetBuffer->size(), *pFrom);
					m_assembleBuffer.reset();
					return bRet;
				}
			}	break;
		default:
			{
				KSocket* pSock = this->GetSocket();
				Log(LOG_ERROR, "error: assemble packet not complete, %s", pSock->ToString().c_str());
				pSock->Close();
				return FALSE;
			}
		}
		return TRUE;
	}
}

void JgPacketReceiver::ClearReceiver()
{
	m_assembleBuffer.reset();
	m_mxRecv.Lock();
	m_recvIoBuff.Reset();
	m_denyRecv = FALSE;
	m_mxRecv.Unlock();
}

// 当收到一个完整的应用程序包
BOOL JgPacketReceiver::OnAppPacket(const void* pPacket, int len, const KSocketAddress& fromAddr)
{
	if(!this->WriteOnePacket(pPacket, len, fromAddr))
	{
		Log(LOG_ERROR, "error: write packet len:%u, sock:%s", len, m_pSock->ToString().c_str());
		return FALSE;
	}
	return TRUE;
}

BOOL JgPacketReceiver::WriteOnePacket(const void* pPacket, int len, const KSocketAddress& fromAddr)
{
	BOOL ret = FALSE;
	m_mxRecv.Lock();
	if(!m_denyRecv) ret = this->_WriteOnePacket(pPacket, len, fromAddr);
	m_mxRecv.Unlock();
	return ret;
}

BOOL JgPacketReceiver::WriteOnePacket_Force(const void* pPacket, int len, const KSocketAddress& fromAddr)
{
	BOOL ret = FALSE;
	m_mxRecv.Lock();
	if(!m_denyRecv) ret = this->_WriteOnePacket_Force(pPacket, len, fromAddr);
	m_mxRecv.Unlock();
	return ret;
}

BOOL JgPacketReceiver::post_packet(int cmd, const void* data, int len)
{
	JgSockServer* sockServer = (JgSockServer*)this->GetSocket()->GetSockServer();
	ASSERT(sockServer);
	KSocketAddress addr = {0, 0};
	JgPacketBuffer* packetBuffer = sockServer->GetThreadPacketBuffer(); packetBuffer->Reset();
	packetBuffer->innerBuild(cmd, data, len);
	BOOL ret = FALSE; m_mxRecv.Lock();
	if(!m_denyRecv) ret = this->_WriteOnePacket_Force(packetBuffer->data(), packetBuffer->size(), addr);
	m_mxRecv.Unlock();
	return ret;
}

BOOL JgPacketReceiver::post_final_packet(int cmd, const void* data, int len)
{
	JgSockServer* sockServer = (JgSockServer*)this->GetSocket()->GetSockServer();
	ASSERT(sockServer);
	KSocketAddress addr = {0, 0};
	JgPacketBuffer* packetBuffer = sockServer->GetThreadPacketBuffer(); packetBuffer->Reset();
	packetBuffer->innerBuild(cmd, data, len);
	BOOL ret = FALSE; m_mxRecv.Lock(); if(!m_denyRecv)
	{
		m_denyRecv = TRUE;
		ret = this->_WriteOnePacket_Force(packetBuffer->data(), packetBuffer->size(), addr);
	}
	m_mxRecv.Unlock();
	return ret;
}

/// JgTcpPacketReceiver

JgTcpPacketReceiver::JgTcpPacketReceiver()
{

}

JgTcpPacketReceiver::~JgTcpPacketReceiver()
{

}

BOOL JgTcpPacketReceiver::_WriteOnePacket(const void* pPacket, int len, const KSocketAddress& fromAddr)
{
	int writeAvail = m_recvIoBuff.WriteAvail();
	if(writeAvail < len)
	{
		Log(LOG_ERROR, "error: tcp socket recv buffer full, sock:%s", m_pSock->ToString().c_str());
		return FALSE;
	}
	m_recvIoBuff.Write(pPacket, len);
	return TRUE;
}

BOOL JgTcpPacketReceiver::_WriteOnePacket_Force(const void* pPacket, int len, const KSocketAddress& fromAddr)
{
	int nBytes = m_recvIoBuff.Write_Force(pPacket, len);
	ASSERT(nBytes == len);
	return TRUE;
}

int JgTcpPacketReceiver::_ReadOnePacket(char* pDst, int len)
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

JgUdpPacketReceiver::JgUdpPacketReceiver()
{

}

JgUdpPacketReceiver::~JgUdpPacketReceiver()
{

}

BOOL JgUdpPacketReceiver::_WriteOnePacket(const void* pPacket, int len, const KSocketAddress& fromAddr)
{
	JgPacketHead* pHead = (JgPacketHead*)pPacket;

	int cmd = pHead->Command();
	if(cmd == JgPacketConst::Assemble || cmd == JgPacketConst::AssembleEnd)
	{
		Log(LOG_WARN, "warn: udp socket does not support assemble packet from %s",
			KSocketAddress::ToString(fromAddr).c_str());
		return FALSE;
	}

	// 先写包内容，再写来源地址

	int nBytes = 0;
	int wanted = len + sizeof(KSocketAddress);

	//m_mxRecv.Lock();

	int writeAvail = m_recvIoBuff.WriteAvail();
	if(writeAvail < wanted)
	{
		//m_mxRecv.Unlock();
		Log(LOG_WARN, "warn: recv buffer full, sock:%s", m_pSock->ToString().c_str());
		return FALSE;
	}

	m_recvIoBuff.Write(pPacket, len);
	m_recvIoBuff.Write(&fromAddr, sizeof(KSocketAddress));

	//m_mxRecv.Unlock();
	return TRUE;
}

BOOL JgUdpPacketReceiver::_WriteOnePacket_Force(const void* pPacket, int len, const KSocketAddress& fromAddr)
{
	JgPacketHead* pHead = (JgPacketHead*)pPacket;

	int cmd = pHead->Command();
	if(cmd == JgPacketConst::Assemble || cmd == JgPacketConst::AssembleEnd)
	{
		Log(LOG_WARN, "warn: udp socket does not support assemble packet from %s",
			KSocketAddress::ToString(fromAddr).c_str());
		return FALSE;
	}

	// 先写包内容，再写来源地址

	int nBytes;
	//int wanted = len + sizeof(KSocketAddress);

	//m_mxRecv.Lock();
	
	nBytes = m_recvIoBuff.Write_Force(pPacket, len);
	ASSERT(nBytes == len);

	nBytes = m_recvIoBuff.Write_Force(&fromAddr, sizeof(KSocketAddress));
	ASSERT(nBytes == sizeof(KSocketAddress));
	
	//m_mxRecv.Unlock();
	return TRUE;
}

int JgUdpPacketReceiver::_ReadOnePacket(char* pDst, int len)
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
		int wanted = sizeof(JgPacketHead) + packetHead.Length() + sizeof(KSocketAddress);
		if(len < wanted) return 0;

		nBytes = m_recvIoBuff.Read(pDst, wanted);
		ASSERT(nBytes == wanted);
		return nBytes;
	}
	else
	{
		int length1 = sizeof(JgPacketHead) + packetHead.Length() + sizeof(JgPacketTail);
		if(len < length1+JgPacketConst::MaxTailDataSize+(int)sizeof(KSocketAddress)) return 0;

		nBytes = m_recvIoBuff.Read(pDst, length1);
		ASSERT(nBytes == length1);

		JgPacketTail* pTail = (JgPacketTail*)(pDst + sizeof(JgPacketHead) + packetHead.Length());
		pDst += length1;

		int tailLength = pTail->Length() + sizeof(KSocketAddress);
		nBytes = m_recvIoBuff.Read(pDst, tailLength);
		ASSERT(nBytes == tailLength);

		return length1 + tailLength;
	}
}

JgServerTcpSocket::JgServerTcpSocket()
{
	JgPacketReceiver::m_pSock = this;
}

JgServerTcpSocket::~JgServerTcpSocket()
{

}

BOOL JgServerTcpSocket::Send(int cmd, const void* pData, int len)
{
	JgSockServer* pSockServer = (JgSockServer*)this->GetSockServer();
	JgPacketBuffer* pPacketBuffer = pSockServer->GetThreadPacketBuffer();

	if(!this->Build(pPacketBuffer, cmd, pData, len))
	{
		Log(LOG_WARN, "warn: build packet, cmd:%u len:%u sock:%s",
			cmd, len, this->ToString().c_str());
		return FALSE;
	}

	int packetLen;
	const void* pPacket;
	this->Output(pPacketBuffer, pPacket, packetLen);

	return ((KSocket*)this)->Send(pPacket, packetLen) > 0;
}

BOOL JgServerTcpSocket::SendImmediate(int cmd, const void* pData, int len)
{
	JgSockServer* pSockServer = (JgSockServer*)this->GetSockServer();
	JgPacketBuffer* pPacketBuffer = pSockServer->GetThreadPacketBuffer();

	if(!this->Build(pPacketBuffer, cmd, pData, len))
	{
		Log(LOG_WARN, "warn: build packet, cmd:%u len:%u sock:%s",
			cmd, len, this->ToString().c_str());
		return FALSE;
	}

	int packetLen;
	const void* pPacket;
	this->Output(pPacketBuffer, pPacket, packetLen);

	return ((KSocket*)this)->SendImmediate(pPacket, packetLen) > 0;
}

const KSocketAddress& JgServerTcpSocket::GetRemoteAddress() const
{
	return m_remoteAddress;
}

void JgServerTcpSocket::Reset()
{
	this->ClearReceiver();
	KServerTcpSocket::Reset();
}

BOOL JgServerTcpSocket::OnAttached()
{
	JgSockServer* pSockServer = (JgSockServer*)this->GetSockServer();
	this->SetRecvBufferPool(pSockServer->m_recvPool);
	this->SetAssemBuffPool(pSockServer->m_assemblePool);
	this->ClearReceiver();
	return TRUE;
}

int JgServerTcpSocket::OnRecvData(const void* pData, int len, const KSocketAddress* pFrom)
{
	int nCount = 0;
	const char* p = (const char*)pData;
	while(len >= sizeof(JgPacketHead))
	{
		int packetLength = JgPacketDeclare::CheckLength((JgPacketHead*)p, len);
		if(packetLength < 0)
		{
			Log(LOG_WARN, "warn: check length, len:%u sock:%s", len, this->ToString().c_str());
			return -1;
		}
		if(!packetLength) break;

		if(!this->OnRecvComplete(p, packetLength, pFrom))
		{
			Log(LOG_WARN, "warn: on recv complete, len:%u sock:%s", len, this->ToString().c_str());
			return -2;
		}

		p += packetLength;
		len -= packetLength;
		nCount += packetLength;
	}
	return nCount;
}

BOOL JgServerTcpSocket::OnDetached()
{
	this->Reset();
	return TRUE;
}

void JgServerTcpSocket::OnStartWork()
{
	this->ClearReceiver();
	KServerTcpSocket::OnStartWork();
}

JgUdpSocket::JgUdpSocket()
{
	JgPacketReceiver::m_pSock = this;	
}

JgUdpSocket::~JgUdpSocket()
{

}

BOOL JgUdpSocket::SendTo(int cmd, const void* pData, int len, const KSocketAddress& peerAddr)
{
	JgSockServer* pSockServer = (JgSockServer*)this->GetSockServer();
	JgPacketBuffer* pPacketBuffer = pSockServer->GetThreadPacketBuffer();

	if(!this->Build(pPacketBuffer, cmd, pData, len))
	{
		Log(LOG_WARN, "warn: build cmd:%u len:%u sock:%s", cmd, len, this->ToString().c_str());
		return FALSE;
	}

	int packetLen;
	const void* pPacket;
	this->Output(pPacketBuffer, pPacket, packetLen);

	return ((KSocket*)this)->SendTo(pPacket, packetLen, peerAddr) > 0;
}

BOOL JgUdpSocket::SendToImmediate(int cmd, const void* pData, int len, const KSocketAddress& peerAddr)
{
	JgSockServer* pSockServer = (JgSockServer*)this->GetSockServer();
	JgPacketBuffer* pPacketBuffer = pSockServer->GetThreadPacketBuffer();

	if(!this->Build(pPacketBuffer, cmd, pData, len))
	{
		Log(LOG_WARN, "warn: build cmd:%u len:%u sock:%s", cmd, len, this->ToString().c_str());
		return FALSE;
	}

	int packetLen;
	const void* pPacket;
	this->Output(pPacketBuffer, pPacket, packetLen);

	return ((KSocket*)this)->SendToImmediate(pPacket, packetLen, peerAddr) > 0;
}

void JgUdpSocket::Reset()
{
	this->ClearReceiver();
	KUdpSocket::Reset();
}

BOOL JgUdpSocket::OnAttached()
{
	JgSockServer* pSockServer = (JgSockServer*)this->GetSockServer();
	this->SetRecvBufferPool(pSockServer->m_recvPool);
	this->SetAssemBuffPool(pSockServer->m_assemblePool);
	this->ClearReceiver();
	return TRUE;
}

int JgUdpSocket::OnRecvData(const void* pData, int len, const KSocketAddress* pFrom)
{
	int nCount = 0;
	const char* p = (const char*)pData;
	while(len >= sizeof(JgPacketHead))
	{
		int packetLength = JgPacketDeclare::CheckLength((JgPacketHead*)p, len);
		if(packetLength < 0)
		{
			Log(LOG_WARN, "warn: check length, len:%u, sock:%s", len, this->ToString().c_str());
			return -1;
		}

		if(!packetLength) break;

		if(!this->OnRecvComplete(p, packetLength, pFrom))
		{
			Log(LOG_WARN, "warn: on recv complete, len:%u, sock:%s", len, this->ToString().c_str());
			return -2;
		}

		p += packetLength;
		len -= packetLength;
		nCount += packetLength;
	}
	return nCount;
}

BOOL JgUdpSocket::OnSocketError(DWORD errCode)
{
	Log(LOG_WARN, "warn: %s, %u:%s", this->ToString().c_str(), errCode, strerror_r2(errCode).c_str());
	return TRUE;
}

BOOL JgUdpSocket::OnInvalidPacket(const KSocketAddress* pFrom, ePacketReason reason)
{
	Log(LOG_WARN, "warn: invalid packet reason:%u from:%s", reason, KSocketAddress::ToString(*pFrom).c_str());
	return TRUE;
}

//BOOL JgUdpSocket::OnRecvComplete(const void* pPacket, int len, const KSocketAddress* pFrom)
//{
//	if(!m_receiver.OnNetUdpPacket(pPacket, len, *pFrom))
//	{
//		Log(LOG_WARN, "warn: %s recv buffer full", this->ToString().c_str());
//		//this->Close(); // 和TCPsocket不一样，这里不关闭
//		return FALSE;
//	}
//	return TRUE;
//}

BOOL JgUdpSocket::OnDetached()
{
	this->Reset();
	return TRUE;
}

JgClientTcpSocket::JgClientTcpSocket()
{
	JgPacketReceiver::m_pSock = this;
}

JgClientTcpSocket::~JgClientTcpSocket()
{

}

BOOL JgClientTcpSocket::Send(int cmd, const void* pData, int len)
{
	JgSockServer* pSockServer = (JgSockServer*)this->GetSockServer();
	JgPacketBuffer* pPacketBuffer = pSockServer->GetThreadPacketBuffer();

	if(!this->Build(pPacketBuffer, cmd, pData, len))
	{
		Log(LOG_WARN, "warn: build cmd:%u len:%u sock:%s", cmd, len, this->ToString().c_str());
		return FALSE;
	}

	int packetLen;
	const void* pPacket;
	this->Output(pPacketBuffer, pPacket, packetLen);

	return ((KSocket*)this)->Send(pPacket, packetLen) > 0;
}

BOOL JgClientTcpSocket::SendImmediate(int cmd, const void* pData, int len)
{
	JgSockServer* pSockServer = (JgSockServer*)this->GetSockServer();
	JgPacketBuffer* pPacketBuffer = pSockServer->GetThreadPacketBuffer();

	if(!this->Build(pPacketBuffer, cmd, pData, len))
	{
		Log(LOG_WARN, "warn: build cmd:%u len:%u sock:%s", cmd, len, this->ToString().c_str());
		return FALSE;
	}

	int packetLen;
	const void* pPacket;
	this->Output(pPacketBuffer, pPacket, packetLen);

	return ((KSocket*)this)->SendImmediate(pPacket, packetLen) > 0;
}

void JgClientTcpSocket::Reset()
{
	this->ClearReceiver();
	KClientTcpSocket::Reset();
}

BOOL JgClientTcpSocket::OnAttached()
{
	JgSockServer* pSockServer = (JgSockServer*)this->GetSockServer();
	this->SetRecvBufferPool(pSockServer->m_recvPool);
	this->SetAssemBuffPool(pSockServer->m_assemblePool);
	this->ClearReceiver();
	return TRUE;
}

int JgClientTcpSocket::OnRecvData(const void* pData, int len, const KSocketAddress* pFrom)
{
	int nCount = 0;
	const char* p = (const char*)pData;
	while(len >= sizeof(JgPacketHead))
	{
		int packetLength = JgPacketDeclare::CheckLength((JgPacketHead*)p, len);
		if(packetLength < 0)
		{
			Log(LOG_WARN, "warn: check length, len:%u sock:%s", len, this->ToString().c_str());
			return -1;
		}

		if(!packetLength) break;

		if(!this->OnRecvComplete(p, packetLength, pFrom))
		{
			Log(LOG_WARN, "warn: on recv complete, len:%u sock:%s", len, this->ToString().c_str());
			return -2;
		}

		p += packetLength;
		len -= packetLength;
		nCount += packetLength;
	}
	return nCount;
}

BOOL JgClientTcpSocket::OnDetached()
{
	this->Reset();
	return TRUE;
}

void JgClientTcpSocket::OnStartWork()
{
	this->ClearReceiver();
	KClientTcpSocket::OnStartWork();
}

JgSockServer::JgSockServer()
{
	m_sendTls = -1;
}

JgSockServer::~JgSockServer()
{

}

BOOL JgSockServer::Initialize(int socketCapacity, int ioThreadNum)
{
	KThreadLocalStorage& tls = KThreadLocalStorage::GetInstance();

	m_sendTls = tls.AllocStorage();
	if(m_sendTls == -1)
	{
		Log(LOG_ERROR, "error: alloc packet sender thread local storage");
		return FALSE;
	}

	BOOL bRet = KSockServer::Initialize(socketCapacity, ioThreadNum);
	return bRet;
}

void JgSockServer::Finalize()
{
	KSockServer::Finalize();
}

BOOL JgSockServer::BuildPacket(JgPacketBuilder* builder, int cmd, const void* data, int len, const void** outPacket, int* outLen)
{
	JgPacketBuffer* pPacketBuffer = this->GetThreadPacketBuffer();
	if(!builder->Build(pPacketBuffer, cmd, data, len))
		return FALSE;
	builder->Output(pPacketBuffer, *outPacket, *outLen);
	return TRUE;
}

int JgSockServer::LockSockets(JgSocketFilter* pFilter, KSocketArray& arrSocks)
{
	arrSocks.clear();

	m_mxSockets.Lock();
	for(int i=0; i<m_socketCapacity; i++)
	{
		KSocket* pSock = m_ppActiveSock[i];
		if(!pSock) pSock = m_ppClosedSock[i];

		if(!pSock) continue;

		if(pFilter->Accept(pSock))
		{
			pSock->AddRef();
			arrSocks.push_back(pSock);
		}
	}
	m_mxSockets.Unlock();

	return arrSocks.size();
}

void JgSockServer::UnlockSockets(KSocketArray& arrSocks)
{
	for(int i=0; i<arrSocks.size(); i++)
	{
		KSocket* pSock = arrSocks[i];
		pSock->ReleaseRef();
	}
}

int JgSockServer::LockSockets(JgSocketFilter* filter, JgPacketReceiver* socks[], int count)
{
	int num = 0;
	m_mxSockets.Lock();
	for(int i=0; i<m_socketCapacity && num<count; i++)
	{
		KSocket* pSock = m_ppActiveSock[i];
		if(!pSock) pSock = m_ppClosedSock[i];
		if(!pSock) continue;
		if(!filter->Accept(pSock)) continue;

		KSocket::SocketType sockType = pSock->GetSocketType();
		switch(sockType)
		{
		case KSocket::SOCK_UDP:
			{
				pSock->AddRef();
				socks[num++] = (JgPacketReceiver*)(JgUdpSocket*)pSock;
			} break;
		case KSocket::SOCK_SERVER_TCP:
			{
				pSock->AddRef();
				socks[num++] = (JgPacketReceiver*)(JgServerTcpSocket*)pSock;
			} break;
		case KSocket::SOCK_CLIENT_TCP:
			{
				pSock->AddRef();
				socks[num++] = (JgPacketReceiver*)(JgClientTcpSocket*)pSock;
			} break;
		}
	}
	m_mxSockets.Unlock();
	return num;
}

void JgSockServer::UnlockSockets(JgPacketReceiver* socks[], int count)
{
	for(int i=0; i<count; i++)
	{
		socks[i]->GetSocket()->ReleaseRef();
	}
}

int JgSockServer::LockSockets(JgServerSocketArray& arrServer, JgClientSocketArray& arrClient, JgUdpSocketArray& arrUdp)
{
	arrServer.clear();
	arrClient.clear();
	arrUdp.clear();

	m_mxSockets.Lock();
	for(int i=0; i<m_socketCapacity; i++)
	{
		KSocket* pSock = m_ppActiveSock[i];
		if(!pSock) pSock = m_ppClosedSock[i];

		if(!pSock) continue;
		
		// 因为在应用层，收到KSocket::OnClosed消息时会给自己发一个包，在处理这个包的时候，会解除这个Socket的引用
		// 所以即使是关闭了的Socket也可能被返回，只要它的接收缓冲区里面有数据

		KSocket::SocketType sockType = pSock->GetSocketType();
		switch(sockType)
		{
		case KSocket::SOCK_SERVER_TCP:
			{
				JgServerTcpSocket* pServerSock = (JgServerTcpSocket*)pSock;
				if(pServerSock->ReadAvail())
				{
					pSock->AddRef();
					arrServer.push_back(pServerSock);
				}
				break;
			}
		case KSocket::SOCK_CLIENT_TCP:
			{
				JgClientTcpSocket* pClientSock = (JgClientTcpSocket*)pSock;
				if(pClientSock->ReadAvail())
				{
					pSock->AddRef();
					arrClient.push_back(pClientSock);
				}
				break;
			}
		case KSocket::SOCK_UDP:
			{
				JgUdpSocket* pUdpSock = (JgUdpSocket*)pSock;
				if(pUdpSock->ReadAvail() > 0)
				{
					pSock->AddRef();
					arrUdp.push_back(pUdpSock);
				}
			}
			break;
		}
	}
	m_mxSockets.Unlock();

	return arrServer.size() + arrClient.size() + arrUdp.size();
}

void JgSockServer::UnlockSockets(JgServerSocketArray& arrServer, JgClientSocketArray& arrClient, JgUdpSocketArray& arrUdp)
{
	for(int i=0; i<arrServer.size(); i++)
	{
		KSocket* pSock = arrServer[i];
		pSock->ReleaseRef();
	}
	for(int i=0; i<arrClient.size(); i++)
	{
		KSocket* pSock = arrClient[i];
		pSock->ReleaseRef();
	}
	for(int i=0; i<arrUdp.size(); i++)
	{
		KSocket* pSock = arrUdp[i];
		pSock->ReleaseRef();
	}
}

JgPacketBuffer* JgSockServer::GetThreadPacketBuffer()
{
	TlsSenderData* pTlsData = NULL;
	static KThreadLocalStorage& tls = KThreadLocalStorage::GetInstance();

	BOOL bRet = tls.GetValue(m_sendTls, (void*&)pTlsData);
	if(pTlsData && pTlsData->prefixFlag == tls_prefix_flag)
	{	// 已经初始化好了
		return &pTlsData->packetBuffer;
	}

	pTlsData = new TlsSenderData();
	pTlsData->prefixFlag = tls_prefix_flag;

	bRet = tls.SetValue(m_sendTls, pTlsData);
	ASSERT(bRet);

	return &pTlsData->packetBuffer;
}