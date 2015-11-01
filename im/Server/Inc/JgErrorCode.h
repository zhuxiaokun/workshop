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
			后续添加错误码在err_num前面添加,err_num主要是记录错误码总的个数,并不作错误码返回
			添加之后,要在客户端的 JgerrMessage.h中的相应位置添加所对应的中文描述
		*/
		err_num,
	};
};



