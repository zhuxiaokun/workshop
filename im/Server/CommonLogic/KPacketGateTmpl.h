#pragma once
#include <System/Collections/KMapByVector.h>
#include <SockServer/JgPacketSpecific.h>

#define REGISTER_CMD(CMD,method) \
	Register(CMD,method);\
	mCmdMap.insert_unique(CMD,#CMD)
	
template <typename T,typename Sock,int startCmd,int endCmd>
class KPacketGateTmpl
{
public:
	typedef Sock socket_type;
	typedef void (T::*ProcMethod)(socket_type*, const void*, int);
	class cmp { public: int operator () (const INT& a, const INT& b) const { return  (a-b); } };
	typedef System::Collections::KMapByVector2<INT,System::Collections::KString<50>,cmp> CmdMap;

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
	void Process(socket_type* pSock, int cmd, const void* pData, int len)
	{
		if(cmd<startCmd || cmd>=endCmd) return;
		ProcMethod m = m_methods[cmd-startCmd];
		if(m==NULL) 
		{
			printf("unknow cmd %d\n",cmd);
			return ;
		}
		//Log(LOG_DEBUG,"recv packet %s", mCmdMap[cmd].c_str());
		(((T*)this)->*m)((Sock*)pSock, pData, len);
	}

public:
	ProcMethod m_methods[endCmd-startCmd];
	CmdMap mCmdMap;//µ˜ ‘√¸¡Ó π”√
	
};
