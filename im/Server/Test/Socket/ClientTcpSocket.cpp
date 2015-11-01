#include "ClientTcpSocket.h"
#include <System/Misc/StrUtil.h>
namespace jg { namespace net {
//--------------------------------------------------------------------------
	ClientTcpSocket::ClientTcpSocket():m_refCount(0),m_socketIndex(-1)
	{
		m_userData = 0;
		m_socket = INVALID_SOCKET; m_socket_cpy = INVALID_SOCKET;
		m_socketState =SocketState::none;
		memset(&m_localAddr, 0, sizeof(KSocketAddress));
		memset(&m_remoteAddr, 0, sizeof(KSocketAddress));
		m_sendOffset = 0;
		m_recvOffset = 0;
		m_sendBuffSize;
	}

	ClientTcpSocket::~ClientTcpSocket()
	{
		this->Reset();
	}

	bool ClientTcpSocket::Connect(const char* ip, int port)
	{
		if(port < 0 || port > 0xffff) return false;
		DWORD ipAddr = KSocket::GetIpByName(ip);
		if(!ipAddr) return false;
		KSocketAddress addr = {ipAddr, port};
		return this->Connect(addr);
	}

	bool ClientTcpSocket::Connect(const KSocketAddress& remoteAddr)
	{
		if(m_socketState != SocketState::none && m_socketState != SocketState::closed)
		{
			Log(LOG_ERROR, "error: connect with invalid state");
			//this->OnConnectError(0);
			return false;
		}
		
		this->Reset();

		this->_create();
		if (m_socketState != SocketState::ready)
		{
			Log(LOG_ERROR, "error: %s connect while not ready, state:%d", this->ToString().c_str(), m_socketState);
			//this->OnConnectError(0);
			return false;
		}

		memcpy(&m_remoteAddr, &remoteAddr, sizeof(KSocketAddress));

		Log(LOG_DEBUG, "debug: %s connecting", this->ToString().c_str());

		sockaddr_in peer = KSocketAddress::ToSockAddr(m_remoteAddr);
		int socklen = sizeof(peer);

		int nRet = connect(m_socket, (sockaddr*)&peer, socklen);

		DWORD errCode = GetLastError();
		ASSERT(errCode != 0); // non-blocking socket

		if(this->_isWouldBlock(errCode))
		{
			m_socketState = SocketState::connecting;
			return true;
		}

		Log(LOG_ERROR, "error: %s connect, code:%u %s", this->ToString().c_str(), errCode, strerror_r2(errCode).c_str());

		//this->OnConnectError(errCode);

		this->_close();
		return false;
	}

	void ClientTcpSocket::Close()
	{
		Log(LOG_DEBUG, "debug: close %s", this->ToString().c_str());

		switch (m_socketState)
		{
		case SocketState::none:
			{
				m_socketState = SocketState::closed;
				return;
			}
		case SocketState::ready:
			{
				this->_close();
				//closesocket(m_socket); m_socket = INVALID_SOCKET;
				//m_socketState = SocketState::closed;
	 			break;
			}
		case SocketState::connecting:
			{
				this->_close();
				//closesocket(m_socket); m_socket = INVALID_SOCKET;
				//m_socketState = SocketState::closed;
				break;
			}
		case SocketState::connected:
			{
			#if defined(WIN32)
				const int SHUT_RD = SD_RECEIVE;
				const int SHUT_WR = SD_SEND;
				const int SHUT_RDWR = SD_BOTH;
			#endif
				shutdown(m_socket, SHUT_WR);
				m_socketState = SocketState::closing;
				break;
			}
		case SocketState::closing:
			{
				return;
			}
		case SocketState::closed:
			{
				break;
			}
		default:
			{
				ASSERT(!"unknown socket state");
				break;
			}
		}
	}

	bool ClientTcpSocket::SendData(const char* data, int len)
	{
		if (!this->IsReady()) return false;
		m_sendPieceBuff.write(data, len);
		return this->_doSend();
	}

	void ClientTcpSocket::Breathe(int ms)
	{
		switch(m_socketState)
		{
		case SocketState::none: return;
		case SocketState::ready: return;
		case SocketState::connecting:
			{
				if(this->_checkWrite() || this->_checkRead())
				{
					m_socketState = SocketState::connected;
					this->OnConnected();
					break;
				}
				if (this->_checkError()) this->_passiveClose();
			} break;
		case SocketState::connected:
			{
				if (this->_checkWrite()) this->_doSend();
				if (this->_checkRead()) this->_doRecv();
				if (this->_checkError()) this->_passiveClose();
			} break;
		case SocketState::closing:
			{
				if (this->_checkWrite()) this->_doSend();
				if (this->_checkRead()) this->_doRecv();
				if (this->_checkError()) this->_passiveClose();
			} break;
		case SocketState::closed: return;
		}
	}

	KSocketAddress ClientTcpSocket::GetLocalAddress() const
	{
		const KSocketAddress sockAddr = {0,0};
		if(m_socket == INVALID_SOCKET) return sockAddr;

		char buf[128];
		socklen_t addrLen = sizeof(buf);
		sockaddr_in& localAddr = *(sockaddr_in*)&buf;
		int nRet = getsockname(m_socket, (sockaddr*)&localAddr, &addrLen);
		if(nRet == -1) return sockAddr;

		return KSocketAddress::FromSockAddr(localAddr);
	}

	KSocketAddress ClientTcpSocket::GetRemoteAddress() const
	{
		return m_remoteAddr;
	}

	bool ClientTcpSocket::IsReady() const
	{
		return m_socketState == SocketState::connected;
	}

	BOOL ClientTcpSocket::OnConnected()
	{
		return TRUE;
	}

	BOOL ClientTcpSocket::OnConnectError(DWORD errCode)
	{
		return TRUE;
	}

	BOOL ClientTcpSocket::OnSocketError(DWORD errCode)
	{
		return TRUE;
	}

	BOOL ClientTcpSocket::OnInvalidPacket(const KSocketAddress* pFrom, int reason)
	{
		return TRUE;
	}

	BOOL ClientTcpSocket::OnClosed()
	{
		return TRUE;
	}

	string_256 ClientTcpSocket::ToString() const
	{
		char buf[256];
		int n = sprintf_k(buf, sizeof(buf), "%d %s",
			m_socket != INVALID_SOCKET ? m_socket : m_socket_cpy,
			KSocketAddress::ToString(m_remoteAddr).c_str());
		return string_256(buf, n);
	}

	void ClientTcpSocket::Reset()
	{
		m_refCount = 0;
		m_socketIndex = -1;
		m_sendOffset = 0;
		m_recvOffset = 0;
		m_sendBuffSize = 0;
		m_sendPieceBuff.reset();
		memset(&m_localAddr, 0, sizeof(KSocketAddress));
		memset(&m_remoteAddr, 0, sizeof(KSocketAddress));
		this->_close(); m_socket_cpy = INVALID_SOCKET;
		m_socketState = SocketState::none;
	}

	void ClientTcpSocket::Destroy()
	{
		this->Reset();
		delete this;
	}

	int ClientTcpSocket::OnRecvData(void* data, int count)
	{
		return count;
	}

	void ClientTcpSocket::_close()
	{
		if(m_socket != INVALID_SOCKET)
		{
			closesocket(m_socket);
			m_socket_cpy = m_socket; m_socket = INVALID_SOCKET;
			m_socketState = SocketState::closed;
		}
	}

	bool ClientTcpSocket::_create()
	{
		m_sendOffset = 0;
		m_recvOffset = 0;
		m_sendBuffSize = 0;
		m_sendPieceBuff.reset();
		memset(&m_localAddr, 0, sizeof(KSocketAddress));
		memset(&m_remoteAddr, 0, sizeof(KSocketAddress));
		
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(m_socket == INVALID_SOCKET)
		{
			DWORD errCode = GetLastError();
			Log(LOG_ERROR, "error: create socket, code:%u %s", errCode, strerror_r2(errCode).c_str());
			return false;
		}

		this->_disableBlock();
		this->_enableLinger();
		this->_keepAlive();

		m_socketState = SocketState::ready;
		return true;
	}

	void ClientTcpSocket::_passiveClose()
	{
		switch (m_socketState)
		{
		case SocketState::none:
			{
				m_socketState = SocketState::closed;
				return;
			}
		case SocketState::ready:
			{
				this->_close();
				//closesocket(m_socket); m_socket = INVALID_SOCKET;
				//m_socketState = SocketState::closed;
				break;
			}
		case SocketState::connecting:
			{
				this->OnConnectError(0);
				this->_close();
				//closesocket(m_socket); m_socket = INVALID_SOCKET;
				//m_socketState = SocketState::closed;
				break;
			}
		case SocketState::connected:
			{
				this->_close();
				//closesocket(m_socket); m_socket = INVALID_SOCKET;
				//m_socketState = SocketState::closed;
				this->OnClosed();
				break;
			}
		case SocketState::closing:
			{
				this->_close();
				//closesocket(m_socket); m_socket = INVALID_SOCKET;
				//m_socketState = SocketState::closed;
				this->OnClosed();
				break;
			}
		case SocketState::closed:
			{
				break;
			}
		default:
			{
				ASSERT(!"unknown m_socket state");
				break;
			}
		}
	}

	int ClientTcpSocket::_recv()
	{
		int capacity = recv_buff_size - m_recvOffset;
		int bytes = recv(m_socket, &m_innerRecvBuff[m_recvOffset], capacity, 0);
		
		if(bytes == SOCKET_ERROR)
		{
			DWORD errCode = GetLastError();
			if(this->_isWouldBlock(errCode))
				return 0;

			Log(LOG_WARN, "warn: %s recv, code:%u %s", this->ToString().c_str(), errCode, strerror_r2(errCode).c_str());
			this->_passiveClose();
			return -1;
		}

		if (bytes == 0)
		{
			Log(LOG_WARN, "warn: recv: %s peer closed", this->ToString().c_str());
			this->_passiveClose();
			return -1;
		}

		m_recvOffset += bytes;
		return bytes;
	}

	int ClientTcpSocket::_send(const void* data, int len)
	{
		int bytes = send(m_socket, (const char*)data, len, 0);
		if(bytes == SOCKET_ERROR)
		{
			DWORD errCode = GetLastError();
			if(this->_isWouldBlock(errCode))
				return 0;

			Log(LOG_WARN, "warn: %s recv, code:%u %s", this->ToString().c_str(), errCode, strerror_r2(errCode).c_str());
			this->OnSocketError(errCode);
			this->_passiveClose();
			return -1;
		}
		return bytes;
	}

	bool ClientTcpSocket::_doSend()
	{
		if (!this->IsReady())
			return false;

		if(this->_checkWrite())
		{
			while (true)
			{
				while (m_sendBuffSize > 0)
				{
					int bytes = this->_send(&m_innerSendBuff[m_sendOffset], m_sendBuffSize);
					if (bytes < 1) return true;
					m_sendOffset += bytes;
					m_sendBuffSize -= bytes;
				}
				m_sendOffset = 0;
				m_sendBuffSize = m_sendPieceBuff.read(m_innerSendBuff, send_buff_size);
				if (m_sendBuffSize < 1) break;
			}
		}

		return true;
	}

	bool ClientTcpSocket::_doRecv()//接收数据
	{
		while (true)
		{
			int n = this->_recv(); if (n < 1) break;
			int used = this->OnRecvData(m_innerRecvBuff, m_recvOffset);
			if(used > 0)
			{
				int remain = m_recvOffset - used;
				if(remain > 0)
					memmove(m_innerRecvBuff, &m_innerRecvBuff[used], remain);
				m_recvOffset = remain;
			}
		}
		return true;
	}

	bool ClientTcpSocket::_checkRead()
	{
		if (m_socket == INVALID_SOCKET) return false;
		
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(m_socket, &rfds);

		struct timeval timeo;
		timeo.tv_sec = 0;
		timeo.tv_usec = 0;

		int nRet = select(m_socket+1, &rfds, NULL, NULL, &timeo);
		return nRet > 0;
	}

	bool ClientTcpSocket::_checkWrite()
	{
		if (m_socket == INVALID_SOCKET) return false;

		fd_set wfds;
		FD_ZERO(&wfds);
		FD_SET(m_socket, &wfds);
		struct timeval timeo;
		timeo.tv_sec = 0;
		timeo.tv_usec = 0;

		int nRet = select(m_socket+1, NULL, &wfds, NULL, &timeo);
		return nRet > 0;
	}

	bool ClientTcpSocket::_checkError()
	{
		if (m_socket == INVALID_SOCKET) return false;

		fd_set efds;
		FD_ZERO(&efds);
		FD_SET(m_socket, &efds);

		struct timeval timeo;
		timeo.tv_sec = 0;
		timeo.tv_usec = 0;

		int nRet = select(m_socket+1, NULL, NULL, &efds, &timeo);
		return nRet > 0;
	}

	bool ClientTcpSocket::_disableBlock()
	{
	#if defined(WIN32)
		unsigned long ul = 1;
		int nRet = ioctlsocket(m_socket, FIONBIO, (unsigned long*)&ul);
	#else
		int oldFlags = fcntl(m_socket, F_GETFL);
		if(oldFlags == -1)
		{
			DWORD errCode = GetLastError();
			Log(LOG_ERROR, "error: get socket flag, %s", strerror_r2(errCode).c_str());
			oldFlags = 0;
		}
		int nRet = fcntl(m_socket, F_SETFL, oldFlags | O_NONBLOCK);
	#endif

		if (nRet)
		{
			DWORD errCode = GetLastError();
			Log(LOG_ERROR, "error: set socket %u non block, %s", m_socket, strerror_r2(errCode).c_str());
			return false;
		}
		return true;
	}

	bool ClientTcpSocket::_enableLinger()
	{
		struct linger lg;
		lg.l_onoff = 1;
		lg.l_linger = 5;
		if(setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (const char*)&lg, sizeof(lg)))
		{
			DWORD errCode = GetLastError();
			Log(LOG_ERROR, "error: socket %d enable-alive, code:%u %s", m_socket, errCode, strerror_r2(errCode).c_str());
			return false;
			return false;
		}
		return true;
	}

	bool ClientTcpSocket::_keepAlive()
	{
		BOOL keepAlive = TRUE;
		if(setsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&keepAlive, sizeof(keepAlive)))
		{
			DWORD errCode = GetLastError();
			Log(LOG_ERROR, "error: socket %d keep-alive, code:%u %s", m_socket, errCode, strerror_r2(errCode).c_str());
			return false;
		}
		return true;
	}

	bool ClientTcpSocket::_isWouldBlock(DWORD errCode)
	{
	#if defined(WIN32)
		return errCode == WSAEWOULDBLOCK;
	#else
		return errCode == EINPROGRESS;
	#endif
	}
//--------------------------------------------------------------------------
} }
