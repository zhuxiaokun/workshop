#pragma once

#include <SockServer/JgPacketSpecific.h>

template <typename T,typename Sock,int startCmd,int endCmd>
class KPacketGateTmpl
{
public:
	typedef void (T::*ProcMethod)(Sock*, const void*, int);

public:
	KPacketGateTmpl()
	{
		memset(&m_methods, 0, sizeof(m_methods));
	}
	virtual ~KPacketGateTmpl()
	{

	}

public:
	BOOL Register(int cmd, ProcMethod method)
	{
		ASSERT(cmd>=startCmd && cmd<endCmd);
		m_methods[cmd-startCmd] = method;
		return TRUE;
	}
	void Process(KSocket* pSock, int cmd, const void* pData, int len)
	{
		if(cmd<startCmd || cmd>=endCmd) return;
		ProcMethod m = m_methods[cmd-startCmd];
		
		if(!m)
			return;

		(((T*)this)->*m)((Sock*)pSock, pData, len);
	}

public:
	ProcMethod m_methods[endCmd-startCmd];
};
