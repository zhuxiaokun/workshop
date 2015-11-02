#ifndef _K_MSG_TRANSLATE_H_
#define _K_MSG_TRANSLATE_H_

#include <System/Collections/KString.h>

//
// 去转义，认为和源码一个级别
// 对StringManager表中读取出来的字符串进行转换
// \r	->	'\r'
// \n	->	'\n'
// \t	->	'\t'
// \b	->	'\b'
// #\n, #\t 不做转换，留给UI处理
//
template <int capacity> const char* TranslateSourceMessage(const char* s, ::System::Collections::KString<capacity>& str)
{
	char ch1, ch2, ch3;
	int p1 = 0, p2, p3;

	str.clear();
	while(ch1 = s[p1], ch1)
	{
		p2 = p1 + 1;
		p3 = p1 + 2;

		if(ch1 == '#')
		{
			ch2 = s[p2];
			if(ch2 == '\\')
			{
				ch3 = s[p3];
				if(ch3 == 'n' || ch3 == 't')
				{
					str.append(&s[p1], 3);
					p1 += 3;
					continue;
				}
			}
		}
		else if(ch1 == '\\')
		{
			ch2 = s[p2];
			if(ch2 == 'r')
			{
				str.append('\r');
				p1 += 2;
				continue;
			}
			else if(ch2 == 'n')
			{
				str.append('\n');
				p1 += 2;
				continue;
			}
			else if(ch2 == 't')
			{
				str.append('\t');
				p1 += 2;
				continue;
			}
			else if(ch2 == 'b')
			{
				str.append('\b');
				p1 += 2;
				continue;
			}

			str.append(ch2);
			p1 += 2;
			continue;
		}
		//else if(ch1 == '%')
		//{
		//	ch2 = s[p2];
		//	if(ch2 == '%')
		//	{
		//		str.append('%');
		//		p1 += 2;
		//		continue;
		//	}
		//}

		str.append(ch1);
		p1 += 1;
	}

	return str.c_str();
}

#endif
