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

bool parseLogLine(char* line, KPropertyMap& lmap, time_t* tp, const char** verbp)
{
	char* pc = (char*)line;
	char* ss[256];
	int n = split(pc, ' ', ss, 256);
	if(n < 3) return false;

	string_256 dateStr(ss[0]);
	dateStr.append(' ').append(ss[1]);
	dateStr[dateStr.size()-1] = '\0';

	KDatetime dt;
	if(!dt.SetDatetimeString(dateStr.c_str()))
		return false;

	int fn = 2;
	time_t t = dt.Time(); *tp = t;
	const char* verb = ss[fn]; *verbp = verb;

	KPropertyMap& m = lmap; m.clear();
	for(int i=fn+1; i<n; i++)
		m.set(ss[i]);

	m.set("time", (int)t);
	m.set("ervb", verb);

	return true;
}

void process_log_line(time_t lineTime, const char* verb, KPropertyMap& lmap)
{
	ProcessFunction proc = getVerbFunction(verb);
	if(!proc) proc = default_lineProcesor;
	proc(lineTime, verb, lmap);
}

void process_trade(time_t, const char*, KPropertyMap&);
void init_process_table()
{
	g_verbFuncMap["login"] = default_lineProcesor;
	g_verbFuncMap["logout"] = default_lineProcesor;
	g_verbFuncMap["trade"] = process_trade;
}

static void default_lineProcesor(time_t t, const char* verb, KPropertyMap& m)
{
	return;
	//bool ret = false;
	//g_pApp->retTableCall<bool,KPropertyMap*>(ret, "process", &m);
}

static void process_trade(time_t t, const char* verb, KPropertyMap& m)
{
	g_pApp->m_analysis.on_tradeLine(t, m);
}

