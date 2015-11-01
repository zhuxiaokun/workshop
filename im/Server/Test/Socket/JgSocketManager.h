#pragma once
#include "JgClientTcpSocket.h"

namespace jg { namespace net {
//---------------------------------------------------------------
	class JgSocketManager
	{
	public:
		enum { socket_capacity = 512 };

	public:
		JgSocketManager();
		~JgSocketManager();

	public:
		bool Initialize();
		void Breathe(int ms);
		void Finalize();

	public:
		bool Attach(JgClientTcpSocket* pSocket);
		int LockSockets(JgClientTcpSocket** sockets, int count);
		void UnlockSockets(JgClientTcpSocket** sockets, int count);

	private:
		int _getAvailIndex();

	public:
		int m_socketCount;
		JgClientTcpSocket* m_sockets[socket_capacity];
	};
//---------------------------------------------------------------
}}
