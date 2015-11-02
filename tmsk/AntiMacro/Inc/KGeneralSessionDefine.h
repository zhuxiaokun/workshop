#ifndef _K_GENERAL_SESSION_DEFINE_H_
#define _K_GENERAL_SESSION_DEFINE_H_

#include <System/KType.h>
#include <System/KMacro.h>

// gereral session types defined here
const int general_session_type_openUI = 1;
const int general_session_type_updateUI = 2;
const int general_session_type_closeUI = 3;

// activity notice type/s
const int activity_notice_player_pos = 1;
const int activity_notice_player_reqLeave = 2;
const int activity_notice_timePeriod = 3;

// stream session keys/s
const DWORD general_session_key_layoutName = -10000;
const DWORD general_session_key_recvTime = -10001;		// 收到命令的时间 (time)
														// 当Loading阶段收到不能在此时处理的包，需要保存起来
														// 并记下当前时间
const DWORD general_session_key_denyLoading = -10002;	// 是否拒绝在loading状态下处理

template <int x> void _export_general_session_const_to_lua()
{
	// general session packet type/s
	export_enum_to_lua(general_session_type_openUI);
	export_enum_to_lua(general_session_type_updateUI);
	export_enum_to_lua(general_session_type_closeUI);

	// activity notice type/s
	export_enum_to_lua(activity_notice_player_pos);
	export_enum_to_lua(activity_notice_player_reqLeave);
	export_enum_to_lua(activity_notice_timePeriod);

	// general session/s
	export_enum_to_lua(general_session_key_layoutName);
	export_enum_to_lua(general_session_key_recvTime);
	export_enum_to_lua(general_session_key_denyLoading);
}
#define export_general_session_const_to_lua() _export_general_session_const_to_lua<0>()

#endif
