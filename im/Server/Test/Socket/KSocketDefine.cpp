#include "KSocketDefine.h"

//---------------------------------------------------------
KSocket_Chat::KSocket_Chat():m_cnnReady(FALSE)
{
	this->SetUserData(0);
	//this->SetSendBufferSize(1024*10);
	//this->SetRecvBuffSize(1024*10);
	//this->DisableReconnect();
}

KSocket_Chat::~KSocket_Chat()
{

}

void KSocket_Chat::Reset()
{
	m_cnnReady = FALSE;
	jg::net::JgClientTcpSocket::Reset();
}

BOOL KSocket_Chat::OnConnected()
{
	m_cnnReady = TRUE;
	this->PostPacket(s_nChtC_Connected, NULL, 0);
	return TRUE;
}

BOOL KSocket_Chat::OnConnectError( DWORD errCode )
{
	this->PostPacket(s_nChtC_ConnError, NULL, 0);
	return TRUE;
}

BOOL KSocket_Chat::OnClosed()
{
	m_cnnReady = FALSE;
	this->PostPacket(s_nChtC_ConnClosed, NULL, 0);
	return TRUE;
}

//string_256 KSocket_Chat::ToString() const
//{
//	string_64 str = m_remoteAddress.toString();
//	return string_256(str.c_str(), str.size());
//}
