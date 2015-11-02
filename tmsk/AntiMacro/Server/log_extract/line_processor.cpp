#include "line_processor.h"
#include <System/Collections/KTree.h>
#include <System/Misc/KStream.h>
#include "Application.h"
#include <PacketGate/KPacket_extractor_dispatcher.h>

typedef JG_C::KMapByTree<ccstr,ProcessFunction,KccstriCmp> VerbFunctionMap;
static VerbFunctionMap g_verbFuncMap;

void default_lineProcesor(time_t t, const char* verb, KPropertyMap& m);
ProcessFunction getVerbFunction(const char* verb)
{
	VerbFunctionMap::iterator it = g_verbFuncMap.find(verb);
	if(it == g_verbFuncMap.end())
		return NULL;
	return it->second;
}

void process_log_line(const char* line)
{
	char* pc = (char*)line;
	char* ss[256];
	int n = split(pc, " \t", ss, 256);
	if(n < 3) return;
	
	string_256 dateStr(ss[0]);
	dateStr.append(' ').append(ss[1]);
	dateStr[dateStr.size()-1] = '\0';

	KDatetime dt;
	if(!dt.SetDatetimeString(dateStr.c_str()))
		return;

	int fn = 2;
	time_t t = dt.Time();
	const char* verb = ss[fn];

	ProcessFunction proc = getVerbFunction(verb);
	if(!proc)
		proc = default_lineProcesor;

	KPropertyMap m;
	for(int i=fn+1; i<n; i++)
		m.set(ss[i]);

	proc(t, verb, m);
}

static void process_login_log(time_t t, const char* verb, KPropertyMap& m)
{
	if(!m.has("server")) return;
	if(!m.has("account")) return;
	if(!m.has("player")) return;
	if(!m.has("level")) return;
	if(!m.has("map")) return;
	if(!m.has("ip")) return;
	if(!m.has("dev_id")) return;
	if(!m.has("platform")) return;

	KPropertyMap m2(m);
	m2.set("time", t);
	m2.set("verb", verb);

	char buf[2048];
	int n = m2.serialize(buf, sizeof(buf));
	g_pApp->sendToLogDispatcher(s_nED_Verb, buf, n+1);
}

static void process_logout_log(time_t t, const char* verb, KPropertyMap& m)
{
	if(!m.has("server")) return;
	if(!m.has("account")) return;
	if(!m.has("player")) return;
	if(!m.has("ip")) return;
	if(!m.has("dev_id")) return;

	KPropertyMap m2(m);
	m2.set("time", t);
	m2.set("verb", verb);

	char buf[2048];
	int n = m2.serialize(buf, sizeof(buf));
	g_pApp->sendToLogDispatcher(s_nED_Verb, buf, n+1);
}

static void process_trade_log(time_t t, const char* verb, KPropertyMap& m)
{
	if(!m.has("server")) return;
	if(!m.has("source")) return;
	if(!m.has("target")) return;
	if(!m.has("srcPlayer")) return;
	if(!m.has("dstPlayer")) return;
	if(!m.has("sviplevel")) return;
	if(!m.has("tviplevel")) return;
	if(!m.has("sip")) return;
	if(!m.has("tip")) return;
	if(!m.has("friendDegree")) return;
	if(!m.has("in")) return;
	if(!m.has("out")) return;
	if(!m.has("map")) return;

	KPropertyMap m2(m);
	m2.set("time", t);
	m2.set("verb", verb);

	char buf[2048];
	int n = m2.serialize(buf, sizeof(buf));
	g_pApp->sendToLogDispatcher(s_nED_Verb, buf, n+1);
}

void init_process_table()
{
	g_verbFuncMap["login"] = process_login_log;
	g_verbFuncMap["logout"] = process_logout_log;
	g_verbFuncMap["trade"] = process_trade_log;
}

static void default_lineProcesor(time_t t, const char* verb, KPropertyMap& m)
{
	char buf[2048];
	KPropertyMap m2(m);
	m2.set("time", t);
	m2.set("verb", verb);
	int n = m2.serialize(buf, sizeof(buf));
	g_pApp->sendToLogDispatcher(s_nED_Verb, buf, n+1);
}
