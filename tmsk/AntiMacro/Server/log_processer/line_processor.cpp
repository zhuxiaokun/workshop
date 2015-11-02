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
	char cpy[1024];
	strcpy_k(cpy, sizeof(cpy), line);

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

	static KPropertyMap m;

	m.clear();
	m.set("time", (int)t);
	m.set("verb", verb);

	for(int i=fn+1; i<n; i++)
		m.set(ss[i]);

	if(strcmp(verb,"trade") == 0 && m.size() < 6)
		ASSERT(FALSE);

	proc(t, verb, m);
}

void init_process_table()
{
	g_verbFuncMap["login"] = default_lineProcesor;
	g_verbFuncMap["logout"] = default_lineProcesor;
	g_verbFuncMap["trade"] = default_lineProcesor;
}

static void default_lineProcesor(time_t t, const char* verb, KPropertyMap& m)
{
	bool ret = false;
	g_pApp->retTableCall<bool,KPropertyMap*>(ret, "process", &m);
}
