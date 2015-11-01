#include "JgSocketManager.h"

namespace jg { namespace net {
//-------------------------------------------------------------
	JgSocketManager::JgSocketManager():m_socketCount(0)
	{
		memset(&m_sockets, 0, sizeof(m_sockets));
	}

	JgSocketManager::~JgSocketManager()
	{
		this->Finalize();
	}

	bool JgSocketManager::Initialize()
	{
	#if defined(WIN32)
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2,2), &wsaData);
	#endif
		return true;
	}

	void JgSocketManager::Breathe(int ms)
	{
		for(int i=0; i<socket_capacity; i++)
		{
			JgClientTcpSocket* pSocket = m_sockets[i];
			if(pSocket)
			{
				pSocket->Breathe(ms);
				if(pSocket->RefCount() == 1)
				{
					if(pSocket->m_socketState == SocketState::none
						|| pSocket->m_socketState == SocketState::closed)
					{
						pSocket->Destroy();
						m_sockets[i] = NULL;
						m_socketCount--;
					}
				}
			}
		}
	}

	void JgSocketManager::Finalize()
	{
		for(int i=0; i<socket_capacity; i++)
		{
			JgClientTcpSocket* pSocket = m_sockets[i];
			if(pSocket)
			{
				pSocket->Destroy();
				m_sockets[i] = NULL;
			}
		}
	
	#if defined(WIN32)
		WSACleanup();
	#endif
	}

	bool JgSocketManager::Attach(JgClientTcpSocket* pSocket)
	{
		int idx = this->_getAvailIndex();
		if(idx < 0)
		{
			Log(LOG_ERROR, "error: socket manager is full, can not accept more socket");
			return false;
		}
		pSocket->AddRef();
		m_sockets[idx] = pSocket;
		m_socketCount++;
		return true;
	}

	int JgSocketManager::LockSockets(JgClientTcpSocket** sockets, int count)
	{
		int n = 0;
		for(int i=0; i < socket_capacity && n < count; i++)
		{
			JgClientTcpSocket* pSocket = m_sockets[i];
			if(pSocket)
			{
				pSocket->AddRef();
				sockets[n++] = pSocket;
			}
		}
		return n;
	}

	void JgSocketManager::UnlockSockets(JgClientTcpSocket** sockets, int count)
	{
		for(int i=0; i<count; i++)
		{
			JgClientTcpSocket* pSocket = sockets[i];
			pSocket->ReleaseRef();
		}
	}

	int JgSocketManager::_getAvailIndex()
	{
		for(int i=0; i < socket_capacity; i++)
			if(!m_sockets[i]) return i;
		return -1;
	}
//-------------------------------------------------------------
} }
