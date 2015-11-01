#include "JgClientTcpSocket.h"

namespace jg { namespace net {
//--------------------------------------------------------------------------
	JgClientTcpSocket::JgClientTcpSocket():m_assembling(false)
	{

	}

	JgClientTcpSocket::~JgClientTcpSocket()
	{
		
	}

	bool JgClientTcpSocket::_buildPacket(int cmd, const void* data, int len, PieceBufferStream& output)
	{
		ASSERT(cmd >= JgPacketConst::PredefineCommandEnd);
		ASSERT(cmd <= 0xffff);
		ASSERT(len <= JgPacketConst::MaxAppDataSize);

		//output.reset();

		if(len <= JgPacketConst::MaxNetDataSize)
		{
			JgPacketHead head;
			head.flag = 0;
			head.command = cmd;
			head.length = len;

			output.WriteData(&head, sizeof(head));
			if(len > 0) 
				output.WriteData(data, len);
		}
		else
		{
			const char* ptr = (const char*)data;
			while(len > 0)
			{
				int want; JgPacketHead head;

				if((void*)ptr == data)
				{
					want = (int)JgPacketConst::MaxNetDataSize - sizeof(JgPacketHead); len -= want;
					head.flag = 0; head.command = JgPacketConst::Assemble; head.length = want;
					output.WriteData(&head, sizeof(head));
					head.flag = 0; head.command = cmd; head.length = len;
					output.WriteData(&head, sizeof(head));
					output.WriteData(ptr, want); ptr += want;
				}
				else
				{
					want = kmin(len, (int)JgPacketConst::MaxNetDataSize); len -= want;
					head.flag = 0; head.command = len > 0 ? JgPacketConst::Assemble : JgPacketConst::AssembleEnd; head.length = want;
					output.WriteData(&head, sizeof(head));
					output.WriteData(ptr, want); ptr += want;
				}
			}
		}

		return true;
	}

	bool JgClientTcpSocket::Send(int cmd, const void* data, int len)
	{
		if(!this->IsReady()) return false;
		return this->_buildPacket(cmd, data, len, m_sendPieceBuff);
	}

	bool JgClientTcpSocket::PostPacket(int cmd, const void* data, int len)
	{
		ASSERT(cmd >= JgPacketConst::PredefineCommandEnd);
		ASSERT(cmd <= 0xffff);
		ASSERT(len <= JgPacketConst::MaxAppDataSize);

		JgPacketHead head;
		head.flag = 0; head.command = cmd; head.length = len;
		m_recvPieceBuff.WriteData(&head, sizeof(head));
		if(len > 0) m_recvPieceBuff.WriteData(data, len);

		return true;
	}

	int JgClientTcpSocket::ReadPackets(void* pBuff, int len, JgPacketArray& arr)
	{
		JgPacketHead head;

		int packet_count = 0;
		char* ptr = (char*)pBuff;
		int recv_len = m_recvPieceBuff.size();

		arr.clear();
		while(len >= sizeof(JgPacketHead) && recv_len >= sizeof(JgPacketHead))
		{
			int n = m_recvPieceBuff.peek(&head, sizeof(JgPacketHead)); ASSERT(n == sizeof(JgPacketHead));
			int packet_len = JgPacketDeclare::GetLength(&head);
			
			if(recv_len < packet_len) break;
			if(len < packet_len) break;
			
			int readed = m_recvPieceBuff.ReadData(ptr, packet_len); ASSERT(readed == packet_len);
			arr.push_back((JgPacketHead*)ptr);
			
			ptr += packet_len;
			len -= packet_len;
			recv_len -= packet_len;
			packet_count++;
		}

		return packet_count;
	}

	int JgClientTcpSocket::ReadPackets(void* pBuff, int len, JgPacketArray& arr, int maxPackets)
	{
		JgPacketHead head;

		int packet_count = 0;
		char* ptr = (char*)pBuff;
		int recv_len = m_recvPieceBuff.size();

		arr.clear();
		while(packet_count < maxPackets && len >= sizeof(JgPacketHead) && recv_len >= sizeof(JgPacketHead))
		{
			int n = m_recvPieceBuff.peek(&head, sizeof(JgPacketHead)); ASSERT(n == sizeof(JgPacketHead));
			int packet_len = JgPacketDeclare::GetLength(&head);

			if(recv_len < packet_len) break;
			if(len < packet_len) break;

			int readed = m_recvPieceBuff.ReadData(ptr, packet_len); ASSERT(readed == packet_len);
			arr.push_back((JgPacketHead*)ptr);

			ptr += packet_len;
			len -= packet_len;
			recv_len -= packet_len;
			packet_count++;
		}

		return packet_count;
	}

	int JgClientTcpSocket::OnRecvData(void* data, int count)
	{
		int offset = 0;
		const char* ptr = (const char*)data;

		while(count >= sizeof(JgPacketHead))
		{
			JgPacketHead* head = (JgPacketHead*)ptr;
			int result = JgPacketDeclare::CheckLength(head, count);
			if(result == 0) return offset;
			if(result < 0) return -1;
			this->_onNetPacket(head->command, head->length, head+1);
			offset += result; ptr += result; count -= result;
		}
		return offset;
	}

	void JgClientTcpSocket::Reset()
	{
		m_assembling = false;
		m_recvPieceBuff.reset();
		m_assemblePieceBuff.reset();
		ClientTcpSocket::Reset();
	}

	bool JgClientTcpSocket::_create()
	{
		m_assembling = false;
		m_recvPieceBuff.reset();
		m_assemblePieceBuff.reset();
		return ClientTcpSocket::_create();
	}

	void JgClientTcpSocket::_onNetPacket(int cmd, int len, const void* data)
	{
		if(m_assembling)
		{
			switch(cmd)
			{
			case JgPacketConst::Assemble:
				if(len != JgPacketConst::MaxNetDataSize)
				{
					Log(LOG_ERROR, "error: %s invalid assemble packet len %d", this->ToString().c_str(), len);
					this->Close();
					return;
				}
				m_assemblePieceBuff.write(data, len);
				break;
			case JgPacketConst::AssembleEnd:
				if(len > JgPacketConst::MaxNetDataSize)
				{
					Log(LOG_ERROR, "error: %s invalid assemble-end packet len %d", this->ToString().c_str(), len);
					this->Close();
					return;
				}
				m_assemblePieceBuff.write(data, len);
				while(true)
				{
					char tmpBuff[1024];
					int bytes = m_assemblePieceBuff.read(tmpBuff, sizeof(tmpBuff));
					if (bytes < 1) break;
					m_recvPieceBuff.write(tmpBuff, bytes);
				}
				m_assembling = false;
				break;
			default:
				Log(LOG_ERROR, "error: %s un-terminated assemble packet cmd:%d len:%d", this->ToString().c_str(), cmd, len);
				this->Close();
				break;
			}
		}
		else // not in asembling
		{
			switch (cmd)
			{
			case JgPacketConst::Assemble:
				{
					if(len != JgPacketConst::MaxNetDataSize)
					{
						Log(LOG_ERROR, "error: %s invalid assemble packet len %d", this->ToString().c_str(), len);
						this->Close();
						return;
					}
					m_assembling = true;
					m_assemblePieceBuff.write(data, len);
				} break;
			case JgPacketConst::AssembleEnd:
				{
					Log(LOG_ERROR, "error: %s unexpected AssembleEnd packet", this->ToString().c_str());
					this->Close();
				} break;
			default:
				{
					JgPacketHead head;
					head.flag = 0; head.command = cmd; head.length = len;
					m_recvPieceBuff.write(&head, sizeof(head));
					m_recvPieceBuff.write(data, len);
				}
				break;
			}
		}
	}

//--------------------------------------------------------------------------
} }
