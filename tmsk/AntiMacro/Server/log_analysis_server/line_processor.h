#pragma once
#include <lua/KLuaWrap.h>
#include <System/Misc/StrUtil.h>
#include <System/Misc/KDatetime.h>
#include "app_defines.h"
#include <System/Collections/KTree.h>
#include <CommSession/KPropertyMap.h>

typedef void (*ProcessFunction)(time_t t, const char* verb, KPropertyMap& m);

void init_process_table();
ProcessFunction getVerbFunction(const char* verb);

bool parseLogLine(char* line, KPropertyMap& lmap, time_t* tp, const char** verbp);
void process_log_line(time_t lineTime, const char* verb, KPropertyMap& lmap);

