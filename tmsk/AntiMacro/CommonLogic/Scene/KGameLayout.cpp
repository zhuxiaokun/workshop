#include "KGameLayout.h"

/// GameWorld ////////////////////////////////////

GameWorld::GameWorld()
	: m_line(0)
	, m_gameServer(0)
	, m_baseIdx(0)
	, m_count(0)
	, m_worldSetting(NULL)
{

}

GameWorld::GameWorld(int gameServer, const KWorldSetting* ws, int count)
	: m_line(0)
	, m_gameServer(gameServer)
	, m_baseIdx(0)
	, m_count(count)
	, m_worldSetting(ws)
{

}

GameWorld::GameWorld(const GameWorld& o)
	: m_line(o.m_line)
	, m_gameServer(o.m_gameServer)
	, m_baseIdx(o.m_baseIdx)
	, m_count(o.m_count)
	, m_worldSetting(o.m_worldSetting)
{

}

GameWorld& GameWorld::operator = (const GameWorld& o)
{
	if(this == &o) return *this;
	m_line = o.m_line;
	m_gameServer = o.m_gameServer;
	m_baseIdx = o.m_baseIdx;
	m_count = o.m_count;
	m_worldSetting = o.m_worldSetting;
	return *this;
}

/// KGameServerSetting ///////////////////////////

KGameServerSetting::KGameServerSetting()
{
	this->clear();
}

KGameServerSetting::~KGameServerSetting()
{

}

void KGameServerSetting::clear()
{
	m_id = 0;
	m_worlds.clear();
}

bool KGameServerSetting::accept(KGameServerSetting::IVisitor* visitor)
{
	int n = m_worlds.size();
	for(int i=0; i<n; i++)
	{
		GameWorld& world = m_worlds[i];
		if(!visitor->visit(world))
			return false;
	}
	return true;
}

int KGameServerSetting::serverID() const
{
	return m_id;
}

int KGameServerSetting::getWorldCount() const
{
	return m_worlds.size();
}

const GameWorld* KGameServerSetting::getWorld(int idx) const
{
	int n = m_worlds.size();
	if(idx < 0 || idx >= n) return NULL;
	return &m_worlds[idx];
}

const GameWorld* KGameServerSetting::findWorld(int worldType) const
{
	KWorldSetting ws; ws.m_worldType = worldType;
	GameWorld world(0, &ws,0);
	int pos = m_worlds.find(world);
	if(pos < 0) return NULL;
	return &m_worlds[pos];
}

bool KGameServerSetting::_initFromXml(KWorldSettings& wss, XMLElementNode* xml)
{
	m_id = xml->QueryAtt("id")->Integer();
	
	XMLNodeColl coll = xml->QueryChildren2("World");
	int n = coll.Size();

	for(int i=0; i<n; i++)
	{
		XMLElementNode* worldXml = (XMLElementNode*)coll.GetNode(i);
		int worldType = worldXml->QueryAtt("type")->Integer();
		int num = worldXml->QueryAtt("count")->Integer();
		KWorldSetting* pws = wss.getWorldSetting(worldType);
		if(pws)
		{
			m_worlds.insert_unique(GameWorld(m_id, pws, num));
		}
		else
		{
			Log(LOG_ERROR, "error: KGameServerSetting::_initFromXml, world of type %d not found", worldType);
		}
	}

	return true;
}

/// KGameLineSetting ////////////////////////////////

KGameLineSetting::KGameLineSetting() : m_lineID(0)
{

}

KGameLineSetting::~KGameLineSetting()
{
	this->clear();
}

void KGameLineSetting::clear()
{
	m_lineID = 0;
	int n = m_gameServers.size();
	for(int i=0; i<n; i++)
	{
		KGameServerSetting* gs = m_gameServers[i];
		gs->clear();
		KGameServerSetting::Free(gs);
	}
	m_gameServers.clear();
}

bool KGameLineSetting::accept(KGameServerSetting::IVisitor* visitor)
{
	int n = m_gameServers.size();
	for(int i=0; i<n; i++)
	{
		KGameServerSetting* gs = m_gameServers[i];
		if(!gs->accept(visitor))
			return false;
	}
	return true;
}

bool KGameLineSetting::visit(GameWorld& gw)
{
	gw.m_line = m_lineID;
	gw.m_baseIdx = gw.m_worldSetting->m_baseIdx;
	((KWorldSetting*)gw.m_worldSetting)->m_baseIdx += gw.m_count;
	return true;
}

int KGameLineSetting::lineID() const
{
	return m_lineID;
}

int KGameLineSetting::getGameServerCount() const
{
	return m_gameServers.size();
}

const KGameServerSetting* KGameLineSetting::getGameServer(int idx) const
{
	int n = m_gameServers.size();
	if(idx < 0 || idx >= n) return NULL;
	return m_gameServers[idx];
}

const KGameServerSetting* KGameLineSetting::findGameServer(int gameServerID) const
{
	KGameServerSetting gs; gs.m_id = gameServerID;
	int pos = m_gameServers.find(&gs);
	if(pos < 0) return NULL;
	return m_gameServers[pos];
}

int KGameLineSetting::getWorldCount(int worldType) const
{
	int num = 0;
	int n = m_gameServers.size();
	for(int i=0; i<n; i++)
	{
		const KGameServerSetting* gs = m_gameServers[i];
		const GameWorld* pw = gs->findWorld(worldType);
		if(pw) num += pw->m_count;
	}
	return num;
}

int KGameLineSetting::findWorlds(int worldType, GameWorld* worlds, int count) const
{
	int num = 0;
	if(count < 1) return 0;

	int n = m_gameServers.size();
	for(int i=0; i<n; i++)
	{
		const KGameServerSetting* gs = m_gameServers[i];
		const GameWorld* pw = gs->findWorld(worldType);
		if(pw)
		{
			worlds[num++] = *pw;
			if(num >= count) break;
		}
	}
	return num;
}

bool KGameLineSetting::_initFromXml(KWorldSettings& wss, XMLElementNode* xml)
{
	m_lineID = xml->QueryAtt("id")->Integer();

	XMLNodeColl coll = xml->QueryChildren2("GameServer");
	int n = coll.Size();

	for(int i=0; i<n; i++)
	{
		XMLElementNode* gsXml = (XMLElementNode*)coll.GetNode(i);
		KGameServerSetting* gs = KGameServerSetting::Alloc(); gs->clear();
		if(!gs->_initFromXml(wss, gsXml))
		{
			KGameServerSetting::Free(gs);
			continue;
		}

		if(m_gameServers.insert_unique(gs) < 0)
		{
			Log(LOG_ERROR, "error: KGameLineSetting::_initFromXml, line %d game server %d already exist.", m_lineID, gs->m_id);
			KGameServerSetting::Free(gs);
			continue;
		}
	}

	this->accept(this);
	return true;
}

/// KGameLayout ////////////////////////////////////////////////////

KGameLayout::KGameLayout()
{

}

KGameLayout::~KGameLayout()
{
	this->finalize();
}

bool KGameLayout::initialize(KWorldSettings& wss, const char* fileName)
{
	KInputFileStream fi;
	if(!fi.Open(fileName))
	{
		Log(LOG_ERROR, "error: KGameLayout::initialize(%s), open.", fileName);
		return false;
	}
	return this->initialize(wss, fi);
}

bool KGameLayout::initialize(KWorldSettings& wss, StreamInterface& fi)
{
	XMLDomParser parser;
	if(!parser.Initialize())
	{
		Log(LOG_ERROR, "error: KGameLayout::initialize, init xml parser");
		return false;
	}

	XMLElementNode rootXml;
	if(!parser.Parse(fi, rootXml))
	{
		Log(LOG_ERROR, "error: KGameLayout::initialize, parse xml, %s", parser.GetErrorMsg());
		return false;
	}

	XMLNodeColl coll = rootXml.QueryChildren2("Settings/Line");
	int n = coll.Size();

	for(int i=0; i<n; i++)
	{
		KGameLineSetting* pls = KGameLineSetting::Alloc(); pls->clear();
		XMLElementNode* lineXml = (XMLElementNode*)coll.GetNode(i);
		if(!pls->_initFromXml(wss, lineXml))
		{
			KGameLineSetting::Free(pls);
			continue;
		}
		if(m_lines.insert_unique(pls) < 0)
		{
			Log(LOG_ERROR, "error: KGameLayout::initialize, line %d already exist.", pls->m_lineID);
			KGameLineSetting::Free(pls);
			continue;
		}
	}
	return true;
}

void KGameLayout::finalize()
{
	int n = m_lines.size();
	for(int i=0; i<n; i++)
	{
		KGameLineSetting* pls = m_lines[i]; pls->clear();
		KGameLineSetting::Free(pls);
	}
	m_lines.clear();
}

bool KGameLayout::accept(KGameServerSetting::IVisitor* visitor)
{
	int n = m_lines.size();
	for(int i=0; i<n; i++)
	{
		KGameLineSetting* ls = m_lines[i];
		if(!ls->accept(visitor))
			return false;
	}
	return true;
}

int KGameLayout::getLineCount() const
{
	return m_lines.size();
}

const KGameLineSetting* KGameLayout::getLineSetting(int idx) const
{
	int n = m_lines.size();
	if(idx < 0 || idx >= n) return NULL;
	return m_lines[idx];
}

const KGameLineSetting* KGameLayout::findLineSetting(int lineID) const
{
	KGameLineSetting line; line.m_lineID = lineID;
	int pos = m_lines.find(&line);
	if(pos < 0) return NULL;
	return m_lines[pos];
}

int KGameLayout::findWorlds(int line, int worldType, GameWorld* worlds, int count) const
{
	const KGameLineSetting* ls = this->findLineSetting(line);
	if(!ls) return 0;
	return ls->findWorlds(worldType, worlds, count);
}

int KGameLayout::findGameServerWorlds(int gameServerID, GameWorld* worlds, int count) const
{
	if(count < 1)
		return 0;

	int num = 0;
	int n = m_lines.size();
	for(int i=0; i<n; i++)
	{
		const KGameServerSetting* gs = m_lines[i]->findGameServer(gameServerID);
		if(!gs) continue;

		int c = gs->getWorldCount();
		for(int k=0; k<c; k++)
		{
			const GameWorld* pwc = gs->getWorld(k);
			worlds[num++] = *pwc;
			if(num >= count)
				return num;
		}
	}

	return num;
}
