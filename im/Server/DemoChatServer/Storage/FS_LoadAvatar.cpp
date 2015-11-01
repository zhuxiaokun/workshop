#include "FS_LoadAvatar.h"
#include "../KDemoChatServerApp.h"
#include <JgErrorCode.h>
#include <System/File/KFile.h>
#include "../Database/DB_SetAccountAvatar.h"

FS_LoadAvatar::FS_LoadAvatar()
{
	m_result = 0;
	m_reqSeq = 0;
	m_source = NULL;
	m_target = NULL;
}

FS_LoadAvatar::~FS_LoadAvatar()
{

}

void FS_LoadAvatar::execute()
{
	string_512 filepath = m_target->buildAvatarFilePath(m_target->m_avatar.c_str());
	KInputFileStream fi;
	if(!fi.Open(filepath.c_str(), "rb"))
	{
		m_result = GetLastError();
		Log(LOG_ERROR, "error: open '%s', %s", filepath.c_str(), strerror_r2(m_result).c_str());
		m_result = JgErrorCode::err_OpenFile;
		return;
	}

	int n; char buf[1024];
	while(n = fi.ReadData(buf, sizeof(buf)), n > 0)
		m_avatarData.WriteData(buf, n);

	return;
}

void FS_LoadAvatar::onFinish()
{
	ChtC_QueryAvatar_Rsp rsp;
	memset(&rsp, 0, sizeof(rsp));
	rsp.sequence = m_reqSeq;
	rsp.result = m_result;
	rsp.acctID = m_target->m_acctID;
	if(!m_result) strcpy_k(rsp.avatarName, sizeof(rsp.avatarName), m_target->m_avatar.c_str());
	m_source->send(ChtC_QueryAvatar_Rsp::s_nCode, &rsp, sizeof(rsp));

	if(!m_result)
	{
		JgMessageTarget target;
		target.targetType = JgMessageTargetType::AVATAR;
		target.targetID = m_target->m_acctID;
		m_source->sendMessage(target, JgMessageContentType::IMAGE, m_avatarData);
	}
}

void FS_LoadAvatar::reset()
{
	m_result = 0;
	m_reqSeq = 0;
	m_source = NULL;
	m_target = NULL;
	m_avatarData.reset();
}

void FS_LoadAvatar::destroy()
{
	this->reset();
	FS_LoadAvatar::Free(this);
}
