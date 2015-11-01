#pragma once

#include <JgErrorCode.h>



///////////////////////////////////返回的错误码所对应的文字描述//////////////////////////////////////////////
struct    JgErrorMessage
{
	static const char  * getErrorMeaage(int err_num)
	{
	    static const char *  errText[]=
	    {
		    "操作成功",
			"无效服务器地址",
			"连接错误",
			"操作超时",
			"Canceled Error",
			"无效状态",
		    "未知账户名",
			"密码与账户名不匹配",
			"所要添加的好友不存在",
			"查找群失败",
			"您不是该群成员,无法添加群成员",
			"您已经是群成员",
		    "对方拒绝了你的添加请求",	
			"打开文件出错",
			"写文件出错",
			"创建SQLStatement出错",
			"SQL参数出错",
			"执行SQL语句出错",
		    "账户查找失败!",
			"未设置头像",
			"添加自己为好友失败",
			"对方不在线",
			"未找到相关数据",
			"所要创建的群已经存在",
			"创建群失败",
			"修改账户信息失败",
			"删除好友失败"
	};
		const char * other_err="未知错误类型";
		const char* errMsg = NULL;
		if(err_num < 0) errMsg = other_err;
		else if(err_num >= JgErrorCode::err_num) errMsg = other_err;
		else errMsg = errText[err_num];
		return errMsg;
	}

	static string_512 getUtf8ErrorMessage(int err_num)
	{
		const char* msg = getErrorMeaage(err_num);
#if defined(WIN32)
		return utf8ToLocalString<512>(msg);
#elif defined(__APPLE__)
		return localToUtf8String<512>(msg);
#else
		return localToUtf8String<512>(msg);
#endif
	}
	//printf("%s\n", JgErrorMessage::getUtf8ErrorMessage(100).c_str());
};
