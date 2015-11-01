#pragma once

enum KGlobalErrorCodeDefine_base	// 0-999
{
	// 请不要在中间插入
	err_NormalSuccess = 0,			// 成功(泛用)
	err_NormalFailed,				// 失败(泛用)
	err_StringLen_error,			// 字符串长度错误
	err_PacketLen_error,			// 数据包长度错误
	err_DB_error,					// 数据库错误
	err_invalid_sql_statement,		// sql语句错误
	err_execute_sql,
	err_no_data_found,
	err_Unknown,					// 未知错误

	err_Connect_Timeout,
	err_Close_Timeout,

	err_basecount,
	err_base_end = 1000,
};

enum KGlobalErrorCodeDefine_login	// 1000-9999
{
	// 请不要在中间插入
	err_Account_notfound = err_base_end,			// 用户名未找到
	err_Password_error,				// 密码不正确
	err_Already_online,				// 已经登陆
	err_OTP_error,					// OTP错误
	err_Version_error,				// 版本错误
	err_DateBase_error,				// 数据库错误
	err_IllegalCharValid,			// 含有敏感字符
	err_ServerClose,				// 系统维护中
	err_Validate_Error,				// 认证错误
	err_Account_Problem,			// 玩家账号存在问题
	err_Account_Unuseful,			// 玩家的账号不能利用
	err_NotActiveAccount,			// 未激活
	err_LoginFaild,					// 登录失败

	err_BindIPError,				// 绑定IP不符合要求
	err_AccountBlocked,				// 帐号被封停
	err_AccountBlockedForver,		// 帐号被永久封停
	err_WallowProtect,				// 防沉迷
	err_AdultUnApply,				// 未登记1
	err_NotAdultUnApply,			// 未登记2

	err_Character_exist,			// 角色名已存在
	err_Character_notexist,			// 角色名不存在
	err_Character_namenull,			// 角色名空
	err_Character_notfound,			// 角色未找到
	err_Character_overcount,		// 角色数超过上限
	err_Equip_error,				// 装备错误
	err_CharacterInfo_error,		// 角色信息错误
	err_Nationality_full,			// 选择的国籍人满为患
	err_Guild_Master,				// 是帮主或者堂主
	err_Guild_Answer,				// 是公会的响应标志
	err_Character_NeedChangeName,	// 角色需要改名
	err_CanNot_ChangeName,			// 不允许改名

	err_EnterGame_nopoint,			// 玩家进入时worldserver的错误代码
	err_EnterGame_nobornpoint,		// 找不到出生点
	err_EnterGame_noworldtypefound,	// 找不到地图对应的world
	err_EnterGame_not_allowCreate,	// 不能创建新副本
	err_EnterGame_mapctrlnotfound,	// 找不到地图脚本
	err_EnterGame_no_world_found,	// 找不到指定的world
	err_EnterGame_game_server_full,	// 游戏服务器人满
	err_EnterGame_wplayer_notfound,	// worldserver上玩家不存在(肯定是逻辑出问题了)
	err_EnterGame_wplayer_state_error,		// worldserver上玩家状态错误
	err_EnterGame_playerstoragefailed,		// 加载玩家副本状态信息错误

	err_LinePlayerFull,						// 线路人满,不能排队
	err_PASSWORDPROTECT_NOT_VALIDATION,		// 密保未认证
	err_PASSWORDPROTECT_ERROR,				// 密保错误
	err_GamePlaying_NotFind_Rsp,			// 玩家不在线(应该是严重错误)
	err_EnterGameError,						// 进入游戏服务器错误

	err_LoadPlayerData_timeout,		// 读取玩家数据超时
	err_LoadActiveData,				// 在DataServer上加载Active的数据
	err_LoadData_NoCapacity,		// cache的总量用光了
	err_LoadData_Sql,				// sql错误
	err_DataServer_NotAvail,		// data server not available

	// 普通登录错误请在这之前添加 尽量避免插入
	err_EnterGame_unionMap,				// 不能直接进入跨服战场
	err_EnterGame_unionServer_notAvail,	// 跨服服务器不可用
	err_wplayer_already_exist,			// worldserver上玩家已经存在

	err_DeleteChar_MatchTeam_Leader,	// 战队队长闪角色	
	err_LinePlayerFull_ask,						// 线路人满,可以排队
	err_NoDataAgent,						// 没有找到DataAgent

	// 日本 特殊段
	err_JAPAN_08					= 3000,		// 验证错误

	// JJ平台特殊段
	err_JJ_PARAM_ERROR				= 3100,		// 参数错误		HTTP 400
	err_JJ_APP_CHECK_ERROR,						// 应用验证失败 HTTP 401
	err_JJ_CALL_SERVICE_ERROR,					// 调用服务失败 -99

	// 韩国 特殊段
	err_KOREA_VALIDATE_ERROR		= 3200,		// 认证失败
	err_KOREA_OTHER_ERROR,						// 其他错误
	err_KOREA_SRDPASS_NOT_EXISTS,				// 二级密码不存在

	err_Unknown_Language,

	err_login_end = 10000,
};

enum KGlobalErrorCodeDefine_baselogic	// 10000-10999
{
	err_player_not_found = err_login_end,

	err_FlushRecord_NotFound,					// DataServer上没找到记录
	err_DeleteRecord_NotFound,					// 要删除的记录没有找到
	err_FlushRecord_Inactive,					// flush一条在DataServer非激活的记录
	err_FlushRecord_Mismatch,					// loadTime不匹配

	err_TablceCache_NoCapacity,					// TablceCache容量不足
	err_DeleteActiveRecord,						// 不允许删除激活的记录
	err_DirectDelete_ActiveRecord,				// 不允许直接删除激活的记录
	err_DirectUpdate_ActiveRecord,				// 不允许直接更新激活的记录
	err_DirectInsert_AlreadyExist,				// 要插入的记录已存在

	err_baselogic_end = 11000,
};

enum KGlobalErrorCodeDefine_logic	// 11000-19999
{
	err_player_logic_begin = err_baselogic_end,

	// cash result
	err_Prepaid_Result_Success,	// 预付成功
	err_Prepaid_Result_Failed,	// 预付失败
	err_AddCash_Result_Success,	// 充值成功
	err_AddCash_Result_Failed,	// 充值失败

	// NPC fight
	err_Npc_CanNotFight,		// 当前不能战斗
	err_Npc_CastSkill,			// 不能施放技能

	// money result
	err_AddMoney_Success,		// 增加Money成功
	err_AddMoney_Failed,		// 增加Money失败

	// friend result
	err_target_Not_Online,		// 目标不在线

	// mail result
	err_M_Net_Error,			// 网络出错
	err_M_Db_Error,				// 数据库出错
	err_M_Query_In_Part,		// 后续还有邮件未传送
	err_M_Query_Accomplish,		// 后续再无邮件未传送
	err_M_Send_No_Space,		// 收件人信箱已满
	err_M_Send_No_Space_RTS,	// 收件人信箱已满，已退给发件人
	err_M_Send_No_Receiver,		// 收件人不存在
	err_M_Send_No_Receiver_RTS,	// 收件人不存在，已退给发件人
	err_M_Send_No_Enough_Money,	// 无法扣除足够的金钱
	err_M_Send_Attachment_Oper,	// 附件物品操作出错
	err_M_Send_Level_Limit,		// 发邮件等级限制
	err_M_Send_MailNum_Limit,	// 超过当天可发邮件数
	err_M_Send_Blacklist_Block,	// 黑名单规则屏蔽
	err_M_Send_Switch_Off,		// 邮件发送开关关闭
	err_M_Delete_Fee_Nonzero,	// 带有附件并且需要提取费用的邮件，不能直接删除
	err_M_Fetch_Bag_No_Space,	// 背包已满，无空间
	err_M_Fetch_No_Enough_Money,// 无法扣除足够的提取费用
	err_M_Fetch_Attach_Error,	// 附件收取失败
	err_M_Fetch_Switch_Off,		// 附件收取开关关闭
	err_M_RTS_Type_Unmatch,		// 此类邮件不支持退信
	err_M_RTS_Type_No_response,	// 此类邮件无需回应
	err_M_RTS_OK,				// 该邮件已退信
	err_M_AttachType_Unsupport,	// 不兼容的附件类型

	err_M_Select_Attach_Unknown,	//增删附件未知错误
	err_M_Select_Attach_ItemLack,	//附件物品缺失
	err_M_Select_Attach_ItemLock,	//附件物品被锁定
	err_M_Select_Attach_ItemBind,	//附件物品被绑定
	err_M_Select_Attach_CannotMail,	//物品不可邮寄
	err_M_Select_Attach_InsertFail,	//加入附件失败
	err_M_Select_Attach_DeleteFail,	//删除附件失败
	err_M_Select_Attach_LockFail,	//锁定附件失败
	err_M_Select_Attach_UnlockFail,	//解锁附件失败
	err_M_Select_Attach_OverFlow,	//附件buffer溢出
	err_M_Select_Attach_FormulaExchange,
	// chat result

	// item result

	err_logiccount,

	err_logic_end,
};
