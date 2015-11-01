#include "KGlobalFunction.h"
#include <System/Misc/rand.h>
#include <math.h>
#include <System/Log/log.h>

string_512 toString(const KPassport& passport)
{
	char buf[512];
	int len = sprintf_k(buf, sizeof(buf), "%llu", passport.value);
	return string_512(buf, len);
}

static ::System::Misc::CRandom g_random;

int randbignum(int num)
{
	int result = g_random.GetRandom(0, num);
	return result;
}

int randsmallnum(int num)
{
	int result = g_random.GetRandom(0, num);
	return result;
}

int randrange(int beg, int end)
{
	return g_random.GetRandom(beg, end);
}

DWORD getRandom()
{
	return g_random.GetRandom();
}

bool fequal(float x, float y)
{
	return fabs(x-y) < 0.0001f;
}

void AssertFile(const char* lpszFileName)
{
	Log(LOG_FATAL, "open %s failed.", lpszFileName);
	ASSERT(FALSE);
}

bool operator == (const KPassport& a, const KPassport& b)
{
	return a.value == b.value;
}

int Text(int id, const char* text)
{
	return id;
}

bool isInRegionBox(int rx, int ry, const KRegionBox& box)
{
	if(rx < box.bgrx) return false;
	if(rx >= box.egrx) return false;
	if(ry < box.bgry) return false;
	if(ry >= box.egry) return false;
	return true;
}

static const char* objKindNames[] = {
	"wokWorldObj",
	"wokCharacter",
	"wokPlayer",
	"wokNpc",
	"wokGroundItem",
	"wokCount",
};

const char* objKind2Name(int/*eWorldObjectKind*/ kind)
{
	if(kind < 0 || kind >= wokCount) return "wokOutOfRange";
	return objKindNames[kind];
}

INT64 MessageID2Int64(const JgMessageID& messageID)
{
	return (INT64)messageID.yyyy_mm_dd * 10 * 100000000 + messageID.sequence;
}

JgMessageID Int642MessageID(INT64 val)
{
	JgMessageID mid;
	mid.yyyy_mm_dd = (int)(val / (10 * 100000000));
	mid.sequence = (int)(val % (10 * 100000000));
	return mid;
}

