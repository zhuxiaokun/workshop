#pragma once

#include <JgErrorCode.h>



///////////////////////////////////���صĴ���������Ӧ����������//////////////////////////////////////////////
struct    JgErrorMessage
{
	static const char  * getErrorMeaage(int err_num)
	{
	    static const char *  errText[]=
	    {
		    "�����ɹ�",
			"��Ч��������ַ",
			"���Ӵ���",
			"������ʱ",
			"Canceled Error",
			"��Ч״̬",
		    "δ֪�˻���",
			"�������˻�����ƥ��",
			"��Ҫ��ӵĺ��Ѳ�����",
			"����Ⱥʧ��",
			"�����Ǹ�Ⱥ��Ա,�޷����Ⱥ��Ա",
			"���Ѿ���Ⱥ��Ա",
		    "�Է��ܾ�������������",	
			"���ļ�����",
			"д�ļ�����",
			"����SQLStatement����",
			"SQL��������",
			"ִ��SQL������",
		    "�˻�����ʧ��!",
			"δ����ͷ��",
			"����Լ�Ϊ����ʧ��",
			"�Է�������",
			"δ�ҵ��������",
			"��Ҫ������Ⱥ�Ѿ�����",
			"����Ⱥʧ��",
			"�޸��˻���Ϣʧ��",
			"ɾ������ʧ��"
	};
		const char * other_err="δ֪��������";
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
