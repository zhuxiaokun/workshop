#pragma once
#include <EventSystem/KEventEntity.h>

enum AppTimers
{
	AppTimer_SearchLogFile = 1000,
	AppTimer_SearchNextLogFile = 1001,
	AppTimer_ReadLogFile = 1002,
	AppTimer_WaitDispatcher = 1003,
};
