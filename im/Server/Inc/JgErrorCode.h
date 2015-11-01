#pragma once

struct JgErrorCode
{
	enum
	{
		err_Success,
		err_InvalidServerAddress,
		err_ConnectError,
		err_Timeout,
		err_Canceled,
		err_InvalidState,
		err_UnknownAccountName,
		err_PasswordMismatch,
		err_Peer_NotFound,
		err_Group_NotFound,
		err_AccessDeny,
		err_AlreadyGroupMember,
		err_PeerRefuse,
		err_OpenFile,
		err_WriteFile,
		err_SqlStatement,
		err_SqlParams,
		err_SqlExecute,
		err_AccountNotFound,
		err_AvatarNotSet,
		err_AddFriend_Self,
		err_PeerNotOnline,
		err_NoDataFound,
		err_Group_AlreadyExist,
		err_InnerError,
		err_CreateGroup,
		err_ModifyAccount,
		err_RemoveFriend,
		/*
			������Ӵ�������err_numǰ�����,err_num��Ҫ�Ǽ�¼�������ܵĸ���,�����������뷵��
			���֮��,Ҫ�ڿͻ��˵� JgerrMessage.h�е���Ӧλ���������Ӧ����������
		*/
		err_num,
	};
};



