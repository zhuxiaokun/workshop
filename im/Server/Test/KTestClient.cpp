#include "KTestClient.h"
#include <System/Misc/md5new.h>
#include <KGlobalFunction.h>
KTestClient* g_client = NULL;
DWORD g_nowTicks = GetTickCount();

ChatAccount* KChatGroup::getMember(UINT64 acctID)
{
	ChatAccount acct; acct.m_acctID = acctID;
	int pos = m_members.find(&acct);
	if(pos < 0) return NULL;
	return m_members.at(pos);
}

void KChatGroup::removeMember(UINT64 acctID)
{
	ChatAccount acct; acct.m_acctID = acctID;
	int pos = m_members.find(&acct);
	if(pos < 0) return;
	ChatAccount* m = m_members.at(pos);
	m_members.erase_by_index(pos);
	ChatAccount::Free(m);
}

void KChatGroup::addMember(const ChatAccount& acct)
{
	ChatAccount* m = this->getMember(acct.m_acctID);
	if(m)
	{
		m->m_nick = acct.m_nick;
		m->m_signature = acct.m_signature;
	}
	else
	{
		m = ChatAccount::Alloc();
		memcpy(m, &acct, sizeof(acct));
		m_members.insert_unique(m);
	}
}
void KChatGroup::clearMembers()
{
	while(!m_members.empty())
	{
		ChatAccount* acct = m_members.pop_back();
		ChatAccount::Free(acct);
	}
}

void KChatGroup::clear()
{
	ChatGroup::clear();
	this->clearMembers();
}

/// KTestClient

KTestClient::KTestClient() : m_nextSequence(0),m_chatSocket(NULL)
{

}

KTestClient::~KTestClient()
{

}

bool KTestClient::initialize()
{
	if(!m_sockServer.Initialize())
	{
		printf("error: initialize sock server.\n");
		return false;
	}
	return true;
}


void KTestClient::breathe(int interval)
{
	char buf[102400];
	m_sockServer.Breathe(interval);
	while(m_chatSocket)
	{
		int num = m_chatSocket->ReadPackets(buf, sizeof(buf), m_packets);//读包
		if(num < 1) break;
		for(int i=0; i<num; i++)
		{
			this->processPacket(m_chatSocket, (JgPacketHead*)m_packets[i]);
		}
	}
}

void KTestClient::finalize()
{
	this->close();
	m_sockServer.Finalize();
}

bool KTestClient::checkConn()//检测连接是否正确
{
	if(m_chatSocket && m_chatSocket->m_cnnReady) return true;
	Log(LOG_CONSOLE, "error: chat connection not ready.");
	return false;
}

bool KTestClient::connect(const char* ip, int port)
{
	if(m_chatSocket)
	{
		Log(LOG_WARN|LOG_CONSOLE, "warn: chat socket already exists");
		return false;
	}
	m_chatSocket = new KSocket_Chat();
	if(!m_chatSocket->Connect(ip, port))
	{
		Log(LOG_ERROR|LOG_CONSOLE, "error: set remote address");
		delete m_chatSocket; m_chatSocket = NULL;
		return false;
	}

	m_chatSocket->AddRef();
	m_sockServer.Attach(m_chatSocket);

	return true;
}

bool KTestClient::login(const char* account, const char* pwd)
{
	if(!m_chatSocket || !m_chatSocket->m_cnnReady)//如果登陆时未连接就绪
	{
		Log(LOG_ERROR|LOG_CONSOLE, "error: chat socket is not ready");
		return false;
	}


	CMD5 md5; md5.Update((char*)pwd); md5.Final();
	const unsigned char* pwdMd5 = md5.Get();

	CCht_Login req;
	strcpy_k(req.acctName, sizeof(req.acctName), account);
	memcpy(&req.pwdMd5, pwdMd5, sizeof(req.pwdMd5));
	
	m_chatSocket->Send(s_nCCht_Login, &req, sizeof(req));
	return true;
}

void KTestClient::close()
{
	if(m_chatSocket && m_chatSocket->m_cnnReady)
	{
		m_chatSocket->Close();
	}
}

void KTestClient::processPacket(KSocket_Chat* pSock, JgPacketHead* pHead)
{
	m_gateChat.Process(pSock, pHead->command, pHead+1, pHead->length);
}

bool KTestClient::setAvatar(const char* filename)//设置头像
{
	if(!this->checkConn()) return false;
	return this->_sendMessageFile(JgMessageTargetType::AVATAR, 0, JgMessageContentType::IMAGE, filename);
}

bool KTestClient::say(int acctID, const char* text)//个人聊天
{
	if(!this->checkConn()) return false;
	return this->_sendMessage(JgMessageTargetType::PERSON, acctID, JgMessageContentType::TEXT, text, strlen(text));
}

bool KTestClient::sayInGroup(int grpID, const char* text)//群聊天
{
	if(!this->checkConn()) return false;
	return this->_sendMessage(JgMessageTargetType::GROUP, grpID, JgMessageContentType::TEXT, text, strlen(text));
}

bool KTestClient::shareFile(int acctID, int messageType, const char* filename)//传文件
{
	if(!this->checkConn()) return false;
	return this->_sendMessageFile(JgMessageTargetType::PERSON, acctID, messageType, filename);
}

bool KTestClient::shareFileInGroup(int grpID, int messageType, const char* filename)//群里面上传文件
{
	if(!this->checkConn()) return false;
	return this->_sendMessageFile(JgMessageTargetType::GROUP, grpID, messageType, filename);
}

bool KTestClient::addFriend(int acctID, const char* note)//添加好友
{
	if(!this->checkConn()) return false;
	CCht_AddFriend req;
	req.friendID = acctID;
	strcpy_k(req.note, sizeof(req.note), note);
	m_chatSocket->Send(s_nCCht_AddFriend, &req, sizeof(req));
	return true;
}

bool KTestClient::addFriendByName(const char* friendAcctName, const char* note)
{
	if(!this->checkConn()) return false;
	CCht_AddFriend_ByName req;
	strcpy_k(req.friendAcctName, sizeof(req.friendAcctName), friendAcctName);
	strcpy_k(req.note, sizeof(req.note), note);
	m_chatSocket->Send(s_nCCht_AddFriend_ByName, &req, sizeof(req));
	return true;
}
bool KTestClient::joinGroup(const char* grpNick)
{
	if(!this->checkConn()) return false;
	CCht_JoinGroup req;
	strcpy_k(req.grpNick, sizeof(req.grpNick), grpNick);
	m_chatSocket->Send(s_nCCht_JoinGroup, &req, sizeof(req));
	return true;
}
bool KTestClient::createGroup(const char* grpNick, const char* grpSignature)//创建群
{
	if(!this->checkConn()) return false;
	CCht_CreateGroup req;
	strcpy_k(req.nick, sizeof(req.nick), grpNick);
	strcpy_k(req.signature, sizeof(req.signature), grpSignature);
	m_chatSocket->Send(s_nCCht_CreateGroup, &req, sizeof(req));

	m_tmpTab["CreateGroup"]["nick"] = grpNick;
	m_tmpTab["CreateGroup"]["signature"] = grpSignature;

	return true;
}

bool KTestClient::modify(const char* nick, const char* signature)//修改个人信息
{
	if(!this->checkConn()) return false;
	CCht_ModifyAccount req;
	strcpy_k(req.nick, sizeof(req.nick), nick);
	strcpy_k(req.signature, sizeof(req.signature), signature);
	m_chatSocket->Send(s_nCCht_ModifyAccount, &req, sizeof(req));

	m_tmpTab["ModifyAccount"]["nick"] = nick;
	m_tmpTab["ModifyAccount"]["signature"] = signature;

	return true;
}

bool KTestClient::removeFriend(int frdID)
{
	if(!this->checkConn()) return false;
	if(!this->getFriend(frdID))
	{
		Log(LOG_CONSOLE, "friend %llu not found", frdID);
		return false;
	}
	CCht_RemoveFriend req;
	req.frdID = frdID;
	m_chatSocket->Send(s_nCCht_RemoveFriend, &req, sizeof(req));
	return true;
}

bool KTestClient::leaveGroup(int grpID)
{
	if(!this->checkConn()) return false;
	if(!this->getGroup(grpID))
	{
		Log(LOG_CONSOLE, "group %d not found", grpID);
		return false;
	}
	CCht_LeaveGroup req;
	req.grpID = grpID;
	m_chatSocket->Send(s_nCCht_LeaveGroup, &req, sizeof(req));
	return true;
}

bool KTestClient::queryGroupDetail(int grpID)
{
	if(!this->checkConn()) return false;
	if(!this->getGroup(grpID))
	{
		Log(LOG_CONSOLE, "group %d not found", grpID);
		return false;
	}
	CCht_QueryGroupDetail req;
	req.groupID = grpID;
	m_chatSocket->Send(s_nCCht_QueryGroupDetail, &req, sizeof(req));
	return true;
}

bool KTestClient::inviteJoinGroup(int acctID, int grpID, const char* note)
{
	if(!this->checkConn()) return false;
	if(!this->getGroup(grpID))
	{
		Log(LOG_CONSOLE, "group %llu not found", grpID);
		return false;
	}

	CCht_AddGroupMember req;
	req.memberAcctID = acctID;
	req.groupID = grpID;
	strcpy_k(req.note, sizeof(req.note), note);
	m_chatSocket->Send(s_nCCht_AddGroupMember, &req, sizeof(req));

	return true;
}

bool KTestClient::queryAvatar(int acctID)
{
	if(!this->checkConn()) return false;
	CCht_QueryAvatar req;
	req.sequence = m_nextSequence++;
	req.acctID = acctID;
	m_chatSocket->Send(s_nCCht_QueryAvatar, &req, sizeof(req));
	return true;
}

bool KTestClient::queryMessage(INT64 lastPersonMessage, INT64 lastGroupMessage)//查询消息
{
	if(!this->checkConn()) return false;
	CCht_QueryMessage req;
	req.lastPersonMessage = Int642MessageID(lastPersonMessage);
	req.lastGroupMessage = Int642MessageID(lastGroupMessage);
	m_chatSocket->Send(s_nCCht_QueryMessage, &req, sizeof(req));
	return true;
}

bool KTestClient::print_friends()
{
	int n = m_friends.size();
	Log(LOG_CONSOLE, ">>>> friend count %d:", n);
	for(int i=0; i<n; i++)
	{
		ChatAccount* acct = m_friends.at(i);
		Log(LOG_CONSOLE, "\t%llu\t%s\t%s", acct->m_acctID, acct->m_nick.c_str(), acct->m_signature.c_str());
	}
	return true;
}

bool KTestClient::print_groups()
{
	int n = m_groups.size();
	Log(LOG_CONSOLE, ">>>> group count %d:", n);
	for(int i=0; i<n; i++)
	{
		KChatGroup* grp = (KChatGroup*)m_groups.at(i);
		Log(LOG_CONSOLE, "\t%llu\t%s\t%s", grp->m_groupID, grp->m_nick.c_str(), grp->m_signature.c_str());
		int m = grp->m_members.size();
		for(int k=0; k<m; k++)
		{
			ChatAccount* acct = grp->m_members.at(k);
			Log(LOG_CONSOLE, "\t\t%llu\t%s\t%s", acct->m_acctID, acct->m_nick.c_str(), acct->m_signature.c_str());
		}
	}
	return true;
}
string_64 KTestClient::toString() const
{
	char buf[64];
	int len = sprintf_k(buf, sizeof(buf), "acct:%s id:%llu", m_acctName.c_str(), m_acctID);
	return string_64(buf, len);
}

ChatAccount* KTestClient::getFriend(UINT64 acctID)
{
	ChatAccount acct; acct.m_acctID = acctID;
	int pos = m_friends.find(&acct);
	if(pos < 0) return NULL;
	return m_friends.at(pos);
}

KChatGroup* KTestClient::getGroup(UINT64 grpID)
{
	ChatGroup grp; grp.m_groupID = grpID;
	int pos = m_groups.find(&grp);
	if(pos < 0) return NULL;
	return (KChatGroup*)m_groups.at(pos);
}

void KTestClient::clear()
{
	ChatAccount::clear();
	m_tmpTab.clear();
	while(!m_friends.empty())
	{
		ChatAccount* acct = m_friends.pop_back();
		ChatAccount::Free(acct);
	}
	while(!m_groups.empty())
	{
		KChatGroup* grp = (KChatGroup*)m_groups.pop_back(); grp->clear();
		ChatGroup::Free(grp);
	}
}

bool KTestClient::_addFriend(const ChatAccount& acct)
{
	if(this->getFriend(acct.m_acctID)) return false;
	ChatAccount* frd = ChatAccount::Alloc();
	memcpy(frd, &acct, sizeof(acct));
	return m_friends.insert_unique(frd) >= 0;
}

bool KTestClient::_joinGroup(const ChatGroup& grp)
{
	KChatGroup* chatGrp = this->getGroup(grp.m_groupID);
	if(!chatGrp)
	{
		chatGrp = KChatGroup::Alloc(); chatGrp->clear();
		memcpy(chatGrp, &grp, sizeof(grp));
		m_groups.insert_unique(chatGrp);
	}
	if(!chatGrp->getMember(m_acctID))
	{
		chatGrp->addMember(*this);
	}
	return true;
}

bool KTestClient::_removeFriend(UINT64 frdID)
{
	ChatAccount acct; acct.m_acctID = frdID;
	int pos = m_friends.find(&acct);
	if(pos < 0) return false;

	ChatAccount* chatAcct = m_friends.at(pos);
	m_friends.erase_by_index(pos);
	ChatAccount::Free(chatAcct);

	return true;
}

bool KTestClient::_leaveGroup(UINT64 grpID)
{
	ChatGroup grp; grp.m_groupID = grpID;
	int pos = m_groups.find(&grp);
	if(pos < 0) return false;

	KChatGroup* chatGrp = (KChatGroup*)m_groups.at(pos);
	m_groups.erase_by_index(pos);
	chatGrp->clear();
	KChatGroup::Free(chatGrp);
	return true;
}

bool KTestClient::_sendMessage(int targetType, UINT64 targetID, int contentType, const void* data, int len)
{
	ChatAccount* receiver = NULL;
	KChatGroup* chatGrp = NULL;
	switch(targetType)
	{
	case JgMessageTargetType::GROUP:
		chatGrp = this->getGroup(targetID);
		if(!chatGrp)
		{
			Log(LOG_CONSOLE, "error: %s send message, target group:%llu not found", this->toString().c_str(), targetID);
			return false;
		} break;
	case JgMessageTargetType::PERSON:
		receiver = this->getFriend(targetID);
		if(!receiver)
		{
			Log(LOG_CONSOLE, "error: %s send message, target acct:%llu not found", this->toString().c_str(), targetID);
			return false;
		} break;
	default:
		Log(LOG_CONSOLE, "error: unknown targetType %d", targetType);
		return false;
	}

	DWORD seq = m_nextSequence++;
	printf("%u\n",m_nextSequence);
	CCht_Message_Head messageHead;
	messageHead.sequence = seq;//消息序列号
	messageHead.contentType = (JgMessageContentType::Type)contentType;//消息类型
	messageHead.messageTarget.targetType = (JgMessageTargetType::Type)targetType;
	messageHead.messageTarget.targetID = targetID;//目标ID
	messageHead.length = len;//消息长度

	m_chatSocket->Send(CCht_Message_Head::s_nCode, &messageHead, sizeof(messageHead));
	
	KBufferOutputStream<1024> so;

	const char* bptr = (const char*)data;
	int max_datalen = 1000-sizeof(CCht_Message_Body);
	while(len > 0)
	{
		int datalen = len >  max_datalen ? max_datalen : len;
		so.Seek(0);
		so.WriteDword(seq);
		so.WriteByteArray(bptr, datalen);
		len -= datalen; bptr += datalen;
		m_chatSocket->Send(CCht_Message_Body::s_nCode, so.data(), so.size());
	}

	CCht_Message_End messageEnd; messageEnd.sequence = seq;
	m_chatSocket->Send(CCht_Message_End::s_nCode, &messageEnd, sizeof(messageEnd));

	KOutgoingMessage* outMsg = KOutgoingMessage::Alloc(); outMsg->reset();
	outMsg->m_sequence = messageHead.sequence;
	outMsg->m_messageSource.sourceType = JgMessageTargetType::PERSON;
	outMsg->m_messageSource.sourceID = m_acctID;
	outMsg->m_contentType = messageHead.contentType;
	outMsg->m_messageTarget = messageHead.messageTarget;
	outMsg->m_messageFile = outMsg->buildTmpFileName();
	outMsg->m_isTmpFile = true;

	if(!outMsg->saveTmpFile(data, messageHead.length))
	{ outMsg->destroy(); return FALSE; }
	m_chatSession.m_outMessages.insert(outMsg);

	return TRUE;
}

bool KTestClient::_sendMessageFile(int targetType, UINT64 targetID, int contentType, const char* filename)
{
	KInputFileStream fi(filename);
	if(!fi.Good())
	{
		Log(LOG_CONSOLE, "error: open '%s'", filename);
		return false;
	}

	ChatAccount* receiver = NULL;
	KChatGroup* chatGrp = NULL;
	switch(targetType)
	{
	case JgMessageTargetType::GROUP:
		chatGrp = this->getGroup(targetID);
		if(!chatGrp)
		{
			Log(LOG_CONSOLE, "error: %s send message, target group:%llu not found", this->toString().c_str(), targetID);
			return false;
		} break;
	case JgMessageTargetType::PERSON:
		receiver = this->getFriend(targetID);
		if(!receiver)
		{
			Log(LOG_CONSOLE, "error: %s send message, target acct:%llu not found", this->toString().c_str(), targetID);
			return false;
		} break;
	case JgMessageTargetType::AVATAR:
		receiver = this;
		break;
	default:
		Log(LOG_CONSOLE, "error: unknown targetType %d", targetType);
		return false;
	}

	DWORD seq = m_nextSequence++;

	CCht_Message_Head messageHead;
	messageHead.sequence = seq;
	messageHead.contentType = (JgMessageContentType::Type)contentType;
	messageHead.messageTarget.targetType = (JgMessageTargetType::Type)targetType;
	messageHead.messageTarget.targetID = targetID;
	messageHead.length = 0;

	m_chatSocket->Send(CCht_Message_Head::s_nCode, &messageHead, sizeof(messageHead));

	char buf[1000];
	KBufferOutputStream<JgPacketConst::MaxAppDataSize> so;

	while(TRUE)
	{
		int datalen = (int)fi.ReadData(buf, sizeof(buf));
		if(datalen < 1) break;
		so.Seek(0);
		so.WriteDword(seq);
		so.WriteByteArray(buf, datalen);
		m_chatSocket->Send(CCht_Message_Body::s_nCode, so.data(), so.size());
	}

	CCht_Message_End messageEnd; messageEnd.sequence = seq;
	m_chatSocket->Send(CCht_Message_End::s_nCode, &messageEnd, sizeof(messageEnd));

	KOutgoingMessage* outMsg = KOutgoingMessage::Alloc(); outMsg->reset();
	outMsg->m_sequence = messageHead.sequence;
	outMsg->m_messageSource.sourceType = JgMessageTargetType::PERSON;
	outMsg->m_messageSource.sourceID = m_acctID;
	outMsg->m_contentType = messageHead.contentType;
	outMsg->m_messageTarget = messageHead.messageTarget;
	outMsg->m_messageFile = filename;
	outMsg->m_isTmpFile = false;
	m_chatSession.m_outMessages.insert(outMsg);

	return TRUE;
}

string_512 KTestClient::_buildMessageFileName(KChatMessage* chatMessage)
{
	const char* suffix = NULL;
	switch(chatMessage->m_contentType)
	{
	case JgMessageContentType::TEXT: suffix = ".txt"; break;
	case JgMessageContentType::IMAGE: suffix = ".jpeg"; break;
	case JgMessageContentType::AUDIO: suffix = ".acf"; break;
	case JgMessageContentType::VIDEO: suffix = ".mpeg"; break;
	case JgMessageContentType::SYS_COMMAND: suffix = ".bin"; break;
	default: ASSERT(false);
	}

	JgMessageID mid = chatMessage->m_messageID;


	
	int length=chatMessage->m_lengthSum;

	char path[512] = {0}; int n = 0;
	switch(chatMessage->m_messageTarget.targetType)
	{
	case JgMessageTargetType::PERSON:
		{
			n = sprintf_k(path, sizeof(path), "./Received/person-%llu/private/%08d/%04d%s",
				chatMessage->m_messageSource.sourceID, mid.yyyy_mm_dd, mid.sequence, suffix);
			break;
		}
	case JgMessageTargetType::GROUP:
		{
			n = sprintf_k(path, sizeof(path), "./Received/group-%llu/private/%08d/%04d%s",
				chatMessage->m_messageTarget.targetID, mid.yyyy_mm_dd, mid.sequence, suffix);
			break;
		}
	case JgMessageTargetType::AVATAR:
		{
			ChatAccount* acct = this->_findAccount(chatMessage->m_messageTarget.targetID);
			if(!acct)
			{
				Log(LOG_CONSOLE, "error: account:%llu not found", chatMessage->m_messageTarget.targetID);
				break;
			}
			n = sprintf_k(path, sizeof(path), "./Received/person-%llu/private/avatar/%s",
				chatMessage->m_messageTarget.targetID, acct->m_avatar.c_str());
			break;
		}
	case JgMessageTargetType::PUBLIC:
		{
			n = sprintf_k(path, sizeof(path), "./Received/person-%llu/public/%08d/%04d%s",
				chatMessage->m_messageSource.sourceID, mid.yyyy_mm_dd, mid.sequence, suffix);
			break;
		}
	}
	return string_512(path);
}

bool KTestClient::_saveMessage(KChatMessage* chatMessage)
{
	string_512 filename = this->_buildMessageFileName(chatMessage);
	KOutputFileStream fo;
	if(!fo.OpenForce(filename.c_str(), "w+b"))
	{
		Log(LOG_CONSOLE, "error: open '%s'", filename.c_str());
		return false;
	}
	fo.WriteData(chatMessage->m_messageData.data(), chatMessage->m_messageData.size());
	Log(LOG_CONSOLE, "SaveMessage: %s", filename.c_str());
	return true;
}
bool KTestClient::queryGroupByNick(const char * grpNick)
{
	if(grpNick==NULL)
	{
		Log(LOG_CONSOLE, "Input Error:The GroupNick should not be empty");
		return false;
	}
	CCht_QueryGroupByNick  req;
	strcpy_k(req.grpNick, sizeof(req.grpNick), grpNick);
	m_chatSocket->Send(s_nCCht_QueryGroupByNick,&req,sizeof(req));//向服务器发送查找群的请求
	return true;
}
bool KTestClient::queryGroupByID(int grpID)
{
	if(grpID<0)
	{
		Log(LOG_CONSOLE, "Input Error:Please put In a unsigned ID");
			return false;
	}
	KChatGroup* groupAcct=this->getGroup(grpID);

	if(groupAcct)
	{
		Log(LOG_CONSOLE, "GroupInfo:   GroupId:%llu      GroupNick: %s       GroupSignature:%s", groupAcct->m_groupID,groupAcct->m_nick.c_str(),groupAcct->m_signature.c_str());
		return true;
	}
	CCht_QueryGroupByID  req;
	req.groupID=grpID;
	m_chatSocket->Send(s_nCCht_QueryGroupByID,&req,sizeof(req));//向服务器发送查找群的请求
	return true;
}

bool KTestClient::queryAccount(int acctID)
{
			ChatAccount* acct = this->getFriend(acctID);
			if(acct)
			{
				Log(LOG_CONSOLE, "Account Info\n AccountId:%llu         NICK:%s           Avatar:%s          signature:%s", acct->m_acctID,acct->m_nick.c_str(),acct->m_avatar.c_str(),acct->m_signature.c_str());
				return true;
			}
			

			CCht_QueryAccount req;
			req.acctID = acctID;
			m_chatSocket->Send(s_nCCht_QueryAccount, &req, sizeof(req));//向服务器发送查找账户请求
			return  true;
}

bool KTestClient::queryAccountByNick(const char * acctNick)
{
	if(acctNick==NULL)
	{
		Log(LOG_CONSOLE, "Input Error:The Account Nick should not be empty");
		return false;
	}
	CCht_QueryAccountByNick req;
	strcpy_k(req.nick, sizeof(req.nick), acctNick);
	m_chatSocket->Send(s_nCCht_QueryAccountByNick,&req,sizeof(req));
	return true;
}
ChatAccount* KTestClient::_findAccount(UINT64 acctID)
{
	ChatAccount* acct = this->getFriend(acctID);
	if(acct) return acct;

	int n = m_groups.size();
	for(int i=0; i<n; i++)
	{
		KChatGroup* grp = (KChatGroup*)m_groups.at(i);
		acct = grp->getMember(acctID);
		if(acct) return acct;
	}

	return NULL;
}

