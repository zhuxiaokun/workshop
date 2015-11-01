#pragma once
#include <KCommonStruct.h>
#include <Packetgate/common_packet.h>
#include <JgMessageDefine.h>

int randbignum(int num);
int randsmallnum(int num);
int randrange(int beg, int end);
DWORD getRandom();

string_512 toString(const KPassport& passport);

bool fequal(float x, float y);
void AssertFile(const char* lpszFileName);

bool operator == (const KPassport& a, const KPassport& b);

int Text(int id, const char* text);

bool isInRegionBox(int rx, int ry, const KRegionBox& box);
const char* objKind2Name(int/*eWorldObjectKind*/ kind);

INT64 MessageID2Int64(const JgMessageID& messageID);
JgMessageID Int642MessageID(INT64 val);

template <typename T> void sort(T* arr, int size, bool (*less_func)(T a,T b))
{
	for(int i=0; i<size; i++)
	{
		T& t = arr[i];
		for(int k=i+1; k<size; k++)
		{
			T& t2 = arr[k];
			if(less_func(t2, t))
			{
				T cpy(t);
				t = t2;
				t2 = cpy;
			}
		}
	}
	return;
}

template <typename T> void sort(T* arr, int size, bool (*less_func)(T& a,T& b))
{
	for(int i=0; i<size; i++)
	{
		T& t = arr[i];
		for(int k=i+1; k<size; k++)
		{
			T& t2 = arr[k];
			if(less_func(t2, t))
			{
				T cpy(t);
				t = t2;
				t2 = cpy;
			}
		}
	}
	return;
}

template <typename T> void sort(T* arr, int size, bool (*less_func)(const T& a,const T& b))
{
	for(int i=0; i<size; i++)
	{
		T& t = arr[i];
		for(int k=i+1; k<size; k++)
		{
			T& t2 = arr[k];
			if(less_func(t2, t))
			{
				T cpy(t);
				t = t2;
				t2 = cpy;
			}
		}
	}
	return;
}
