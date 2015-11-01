#include "KCommonSession.h"
#include <System/tinyxml/xmldom.h>
#include <KCommonDefine.h>
#include <System/Log/log.h>

KCommonSession::KCommonSession(void)
{
}

KCommonSession::~KCommonSession(void)
{
}

const char* KCommonSession::GetString( int n ) const
{
	StringSessionMap::const_iterator it = m_stringSessionMap.find(n);
	if (it == m_stringSessionMap.end())
	{
		return "";
	}
	return it->second.c_str();
}

void KCommonSession::SetString( int n, const char* str )
{
	m_stringSessionMap[n] = str;
}

float KCommonSession::GetFloat( int n ) const
{
	FloatSessionMap::const_iterator it = m_floatSessionMap.find(n);
	if (it == m_floatSessionMap.end())
	{
		return 0.0f;
	}
	return it->second;
}

void KCommonSession::SetFloat( int n, float f )
{
	m_floatSessionMap[n] = f;
}

int KCommonSession::GetInt( int n ) const
{
	IntSessionMap::const_iterator it = m_intSessionMap.find(n);
	if (it == m_intSessionMap.end())
	{
		return 0;
	}
	return it->second;
}

void KCommonSession::SetInt( int n, int val )
{
	m_intSessionMap[n] = val;
}

INT KCommonSession::GetIntByDworlds(DWORD dwType, DWORD dwSessionID) const
{
	TypeSessionMap::const_iterator it = m_typeSessionMap.find(dwType);
	if (it == m_typeSessionMap.end())return 0;

	KDwordSession session;
	session.dwSessionID = dwSessionID;
	int pos = it->second.find(session);
	if(pos >= 0) return it->second.at(pos).nVlaue;
	return 0;
}

void KCommonSession::SetIntByDworlds(DWORD dwType, DWORD dwSessionID, INT nValue)
{
	DworldArray& dworldArray = m_typeSessionMap[dwType];
	KDwordSession session;
	session.dwSessionID = dwSessionID;
	session.nVlaue = nValue;
	int pos = dworldArray.find(session);
	if(pos >= 0)
	{
		dworldArray[pos].nVlaue = nValue;
	}
	else
	{
		dworldArray.insert_unique(session);
	}
}

INT64 KCommonSession::GetInt64( int n ) const
{
	Int64SessionMap::const_iterator it = m_int64SessionMap.find(n);
	if (it == m_int64SessionMap.end())
	{
		return 0;
	}
	return it->second;
}

void KCommonSession::SetInt64( int n, INT64 val )
{
	m_int64SessionMap[n] = val;
}

void KCommonSession::Clear()
{
	m_floatSessionMap.clear();
	m_int64SessionMap.clear();
	m_intSessionMap.clear();
	m_stringSessionMap.clear();
	m_typeSessionMap.clear();
}

BOOL KCommonSession::Serialize(KMsgOutputStream &oStream)
{
	ASSERT_RETURN(oStream.WriteWord(m_intSessionMap.size()), FALSE);
	for (IntSessionMap::iterator it = m_intSessionMap.begin();
		it != m_intSessionMap.end(); it++)
	{
		ASSERT_RETURN(oStream.WriteInt(it->first), FALSE);
		ASSERT_RETURN(oStream.WriteInt(it->second), FALSE);
	}

	ASSERT_RETURN(oStream.WriteWord(m_int64SessionMap.size()), FALSE);
	for (Int64SessionMap::iterator it = m_int64SessionMap.begin();
		it != m_int64SessionMap.end(); it++)
	{
		ASSERT_RETURN(oStream.WriteInt(it->first), FALSE);
		ASSERT_RETURN(oStream.WriteInt64(it->second), FALSE);
	}

	ASSERT_RETURN(oStream.WriteWord(m_floatSessionMap.size()), FALSE);
	for (FloatSessionMap::iterator it = m_floatSessionMap.begin();
		it != m_floatSessionMap.end(); it++)
	{
		ASSERT_RETURN(oStream.WriteInt(it->first), FALSE);
		ASSERT_RETURN(oStream.WriteFloat(it->second), FALSE);
	}

	ASSERT_RETURN(oStream.WriteWord(m_stringSessionMap.size()), FALSE);
	for (StringSessionMap::iterator it = m_stringSessionMap.begin();
		it != m_stringSessionMap.end(); it++)
	{
		ASSERT_RETURN(oStream.WriteInt(it->first), FALSE);
		ASSERT_RETURN(oStream.WriteString(it->second.c_str()), FALSE);
	}

	{
		INT nTypeSessionSize = 0;
		for (TypeSessionMap::iterator it = m_typeSessionMap.begin();
			it != m_typeSessionMap.end(); it++)
		{
			DworldArray& dworldArray = it->second;
			nTypeSessionSize += dworldArray.size();
		}

		ASSERT_RETURN(oStream.WriteWord(nTypeSessionSize), FALSE);
		for (TypeSessionMap::iterator it = m_typeSessionMap.begin();
			it != m_typeSessionMap.end(); it++)
		{
			DworldArray& dworldArray = it->second;
			for (INT i=0;i<dworldArray.size();i++)
			{
				ASSERT_RETURN(oStream.WriteDword(it->first), FALSE);
				ASSERT_RETURN(oStream.WriteDword(dworldArray.at(i).dwSessionID), FALSE);
				ASSERT_RETURN(oStream.WriteInt(dworldArray.at(i).nVlaue), FALSE);
			}			
		}
	}
	return TRUE;
}

BOOL KCommonSession::UnSerialize(KMsgInputStream &oStream)
{
	Clear();

	WORD count = 0;
	oStream.ReadWord(count);
	for (int i = 0; i < count; i++)
	{
		int id;
		int val;
		oStream.ReadInt(id);
		oStream.ReadInt(val);
		SetInt(id, val);
	}
	oStream.ReadWord(count);
	for (int i = 0; i < count; i++)
	{
		int id;
		INT64 val;
		oStream.ReadInt(id);
		oStream.ReadInt64(val);
		SetInt64(id, val);
	}
	oStream.ReadWord(count);
	for (int i = 0; i < count; i++)
	{
		int id;
		float val;
		oStream.ReadInt(id);
		oStream.ReadFloat(val);
		SetFloat(id, val);
	}
	oStream.ReadWord(count);
	for (int i = 0; i < count; i++)
	{
		int id;
		char buffer[4096];
		oStream.ReadInt(id);
		oStream.ReadString(buffer, sizeof(buffer));
		SetString(id, buffer);
	}
	{
		oStream.ReadWord(count);
		for (int i = 0; i < count; i++)
		{
			DWORD dwType;
			DWORD dwSessionID;
			INT nValue;
			oStream.ReadDword(dwType);
			oStream.ReadDword(dwSessionID);
			oStream.ReadInt(nValue);
			SetIntByDworlds(dwType, dwSessionID, nValue);
		}
	}
	return TRUE;
}