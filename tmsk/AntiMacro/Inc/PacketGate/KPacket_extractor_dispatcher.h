#pragma once

enum Protocol_Extract_Dispatcher
{
	s_nED_PacketStart = 0,

	s_nED_Connected,
	s_nED_ConnClosed,
	s_nED_ConnError,

	s_nED_Verb,

	s_nED_PacketEnd,
};

enum Protocol_Dispatcher_Extract
{
	s_nDE_PacketStart,
	
	s_nDE_Connected,
	s_nDE_ConnClosed,
	s_nDE_ConnError,

	s_nDE_Verb_Ack, //‘› ±≤ª”√
	s_nDE_Busy,

	s_nDE_PacketEnd,
};

#pragma pack(push,1)

struct ED_Verb
{
	enum { s_nCode = s_nED_Verb };
	// json string with length(16)
};

struct DE_Busy
{
	enum { s_nCode = s_nDE_Busy };
	int busy; // 0 or 1
};

#pragma pack(pop)
