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

void process_log_line(const char* line);

