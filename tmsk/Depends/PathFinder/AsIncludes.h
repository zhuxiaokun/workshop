/********************************************************************
	created:	2008/01/03
	created:	3:1:2008   15:39
	filename: 	Athena\Main\Src\Common\LogicWorld\Src\DataStructure\AsIncludes.h
	file path:	Athena\Main\Src\Common\LogicWorld\Src\DataStructure
	file base:	AsIncludes
	file ext:	h
	author:		xueyan
	
	purpose:	A*算法提供出来的头文件
*********************************************************************/


#pragma once
//#include "GlobalDefine.h"
//#define _DEBUG_DETAIL
//#define _USING_DOUBLE
#ifdef _USING_DOUBLE
#	define VALUE_TYPE double
#else
#	define VALUE_TYPE int
#endif // _USING_DOUBLE


namespace DataStructureAbout
{

	#ifndef NULL
	#define NULL 0
	#endif

	#define ASNL_ADDOPEN		0
	#define ASNL_STARTOPEN		1
	#define ASNL_DELETEOPEN		2
	#define ASNL_ADDCLOSED		3

	#define ASNC_INITIALADD		0
	#define ASNC_OPENADD_UP		1
	#define ASNC_OPENADD		2
	#define ASNC_CLOSEDADD_UP	3
	#define ASNC_CLOSEDADD		4
	#define ASNC_NEWADD			5

	#define  ASTAR_MAX_CHILDREN	8

	class _asNode {
		public:
			_asNode(int a = -1,int b = -1) : x(a), y(b), number(0), nStepNum(0), numchildren(0) {
				parent = next = NULL; dataptr = NULL;
				memset(children, 0, sizeof(children));
			}
			void Init()
			{
				x = 0;
				y = 0;
				number = 0;
				nStepNum = 0;
				numchildren = 0;
				parent = next = NULL;
				dataptr = NULL;
				memset(children, 0, sizeof(children));
			}

		VALUE_TYPE	f,g,h;
		int			x,y;
		int			numchildren;
		int			number;
		int			nStepNum;//增加步深的概念,默认是0就是第一个根结点
		_asNode		*parent;
		_asNode		*next;
		_asNode		*children[ASTAR_MAX_CHILDREN];
		void		*dataptr;
	};


	struct _asStack {
		_asNode	 *data;
		_asStack *next;
	};

typedef VALUE_TYPE(*_asFunc)(_asNode *, _asNode *, int, void *);
typedef int (*_createChildFunc)(const _asNode *, _asNode*, void*);

} // end namespace DataStructureAbout

using namespace DataStructureAbout;
