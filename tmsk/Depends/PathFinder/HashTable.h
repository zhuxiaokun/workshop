/********************************************************************
created:	2009/02/05
filename: 	Athena\Main\Src\Common\LogicWorld\Src\DataStructure\HashTable.h
file path:	Athena\Main\Src\Common\LogicWorld\Src\DataStructure
file base:	HashTable
file ext:	h
copyright:	AuroraGame
author:		Allen Wang

purpose:	A hash table used by A star to fast locate node
*********************************************************************/

#pragma once
#include "System/Log/log.h"
#ifdef _DEBUG_DETAIL
#	include <iostream>
#endif //_DEBUG_DETAIL

//#define SORTED_LIST
#define _NODE_POOL
//#define _NODE_POOL_USED_COMPACT
//#define USING_KHASH

#ifdef USING_KHASH
#	include <System/Collections/KHashFunction.h>
#endif

#ifdef _NODE_POOL
#	ifndef _NODE_POOL_USED_COMPACT
#		include "MemoryPool.h"
#	endif
#endif

namespace DataStructureAbout{

#ifdef USING_KHASH
	template<typename _TDATA, int _SIZE = 1024>
#else
	template<typename _TDATA, int _SIZE = 1001>
#endif

	class hash_table{

#ifdef _NODE_POOL
#	define _NODE_POOL_SIZE (_SIZE * 8)
#	ifdef _NODE_POOL_USED_COMPACT
#		define _NODE_POOL_USED_SIZE ((_NODE_POOL_SIZE >> 5) + 1)
#	endif
#endif

	public:
		const static int SIZE = _SIZE;

		hash_table(){
//			_value = new NODE*[_SIZE];
			memset(&_value, 0, _SIZE * sizeof(NODE*));
#ifdef _NODE_POOL
#	ifdef _NODE_POOL_USED_COMPACT
			memset(_bNodeUsed, 0, _NODE_POOL_USED_SIZE * sizeof(unsigned int));
			_nodePoolPos = 0;
			_nodePoolCount = 0;
#	else
//			memset(_bNodeUsed, 0, _NODE_POOL_SIZE * sizeof(bool));
//	 		for (int i = 0; i < _NODE_POOL_SIZE; i++){
//	 			_pNodeStack[i] = &(_nodePool[i]);
//	 		}
			_nodePool.Create(_NODE_POOL_SIZE);
#	endif
#endif

#ifdef _DEBUG_DETAIL
			testCollisionCount = 0;
			testMaxCollisionCount = 0;
#endif
		}

		~hash_table(){
			Clear();
//			delete [] _value;
#ifdef _DEBUG_DETAIL
			std::cout << "collision = " << testCollisionCount << std::endl;
			std::cout << "max depth = " << testMaxCollisionCount << std::endl;
#endif //_DEBUG_DETAIL
		}

#if defined(_NODE_POOL) && !defined(_NODE_POOL_USED_COMPACT)
		void SetCapacity(DWORD capacity)
		{
			ASSERT(capacity > 0);
			_nodePool.Create(capacity);
		}
#endif
		void Clear(){
#ifdef _NODE_POOL
			memset(_value, 0, _SIZE * sizeof(NODE*));
#	ifdef _NODE_POOL_USED_COMPACT
			memset(_bNodeUsed, 0, _NODE_POOL_USED_SIZE * sizeof(unsigned int));
			_nodePoolPos = 0;
			_nodePoolCount = 0;
#	else
//			memset(_bNodeUsed, 0, _NODE_POOL_SIZE * sizeof(bool));
//	 		for (int i = 0; i < _NODE_POOL_SIZE; i++){
//	 			_pNodeStack[i] = &(_nodePool[i]);
//	 		}
			_nodePool.Clear();
#	endif
#else
			for (int i = 0; i < _SIZE; i++){
				if (_value[i]){
					NODE* p = _value[i];
					NODE* q = p->next;
					delete p;
					while (q){
						p = q;
						q = q->next;
						p->next = NULL;
						delete p;
					}
					_value[i] = NULL;
				}
			}
#endif
		}

		_TDATA Find(int key){
			int pos = _Hash(key);
#ifdef SORTED_LIST
			if (_value[pos]){
				NODE* p = _value[pos];
				while (p){
					if (p->key < key){
						p = p->next;
					}else if (p->key == key){
						return p->data;
					}else{
						return NULL;
					}
				}
			}
#else
			if (_value[pos]){
				NODE* p = _value[pos];
				while (p){
					if (p->key == key){
						return p->data;
					}
					p = p->next;
				}
			}
#endif //SORTED_LIST
			return NULL;
		}

		_TDATA Delete(int key){
			int pos = _Hash(key);
			if (_value[pos]){
				NODE* p = _value[pos];
				if (p->key == key){
					_value[pos] = p->next;
					_TDATA d = p->data;
					p->next = NULL;
#ifdef _NODE_POOL
					_ReleaseNode(p);
#else
					delete p;
#endif
					return d;
				}else{
					while(p->next){
						if (p->next->key == key){
							NODE* q = p->next;
							p->next = q->next;
							_TDATA d = q->data;
							q->next = NULL;
#ifdef _NODE_POOL
							_ReleaseNode(q);
#else
							delete q;
#endif
							return d;
						}
						p = p->next;
					}
				}
			}
			return NULL;
		}

		void Insert(int key, _TDATA data){
			int pos = _Hash(key);
#ifdef _DEBUG_DETAIL
			if (_value[pos]){
				testCollisionCount++;
			}
#endif // _DEBUG_DETAIL
#ifdef _NODE_POOL
			NODE* p = _GetNewNode();
#else
			NODE* p = new NODE;
#endif
			if (p == NULL)
			{
				//ÄÚ´æÂú
				ASSERT(FALSE);
				return;
			}
			p->key = key;
			p->data = data;
#ifdef SORTED_LIST
			if (!_value[pos]
			|| p->key < _value[pos]->key){
				p->next = _value[pos];
				_value[pos] = p;
			}else{
				NODE* q = _value[pos];
				while (q->next && q->next->key < p->key){
					q = q->next;
				}
				p->next = q->next;
				q->next = p;
			}
#else
			p->next = _value[pos];
			_value[pos] = p;
#endif
#ifdef _DEBUG_DETAIL
			{
				int n = 0;
				NODE* p = _value[pos];
				while(p){
					p = p->next;
					n++;
				}
				if (n > testMaxCollisionCount){
					testMaxCollisionCount = n;
				}
			}
#endif //_DEBUG_DETAIL
		}
	private:
		inline int _Hash(int key){
#ifdef USING_KHASH
//	 		unsigned int nHashValue = JHash<const unsigned int *, unsigned int, unsigned int>()
//	 			((const unsigned int*) &key, sizeof(key), JHASH_GOLDEN_RATIO_68);
//	 		return nHashValue & 1023;
			const unsigned int* k = (const unsigned int*)&key;
			UINT a, b, c, len;

			a = b = JHASH_GOLDEN_RATIO;
			c = JHASH_GOLDEN_RATIO_68 + k[0];
			len = sizeof(UINT) / 4;

			while (len >= 3) {
				a += k[0];
				b += k[1];
				c += k[2];
				__jhash_mix(a, b, c);
				k += 3; len -= 3;
			}

			c += sizeof(UINT) * 4;

			switch (len) 
			{
			case 2 : b += k[1];
			case 1 : a += k[0];
			};

			__jhash_mix(a,b,c);

			return c & 1023;

#else
			return key % _SIZE;
#endif
		}

		// for test reason, they are public
		//private:
	public:
		struct NODE{
			int key;
			_TDATA data;
			NODE* next;
			NODE(){
				next = NULL;
			}
		};
//		NODE** _value;
		NODE* _value[_SIZE];

#ifdef _NODE_POOL
#	ifdef _NODE_POOL_USED_COMPACT

	private:
		NODE _nodePool[_NODE_POOL_SIZE];
		int _nodePoolPos;
		int _nodePoolCount;
		unsigned int _bNodeUsed[_NODE_POOL_USED_SIZE];

#	else //_NODE_POOL_USED_COMPACT
//		bool _bNodeUsed[_NODE_POOL_SIZE];
//		NODE* _pNodeStack[_NODE_POOL_SIZE];
	private:
		MemoryPool<NODE> _nodePool;
#	endif // _NODE_POOL_USED_COMPACT


		inline void _SetUsed(int pos){
#	ifdef _NODE_POOL_USED_COMPACT
			_bNodeUsed[pos >> 5] |= (1 << (pos & 0x1f));
#	else //_NODE_POOL_USED_COMPACT
//			_bNodeUsed[pos] = true;
#	endif // _NODE_POOL_USED_COMPACT
		}

		inline void _ClearUsed(int pos){
#	ifdef _NODE_POOL_USED_COMPACT
			_bNodeUsed[pos >> 5] &= ~(1 << (pos & 0x1f));
#	else //_NODE_POOL_USED_COMPACT
//			_bNodeUsed[pos] = false;
#	endif // _NODE_POOL_USED_COMPACT
		}

		inline bool _IsUsed(int pos){
#	ifdef _NODE_POOL_USED_COMPACT
			return (_bNodeUsed[pos >> 5] & (1 << (pos & 0x1f))) != 0;
#	else //_NODE_POOL_USED_COMPACT
//			return _bNodeUsed[pos];
#	endif // _NODE_POOL_USED_COMPACT
		}
// #define _SetUsed(pos)	{_bNodeUsed[pos >> 5] |= (1 << (pos & 0x1f));}
// 
// #define _ClearUsed(pos)	{_bNodeUsed[pos >> 5] &= ~(1 << (pos & 0x1f));}
// 
// #define _IsUsed(pos)	(_bNodeUsed[pos >> 5] & (1 << (pos & 0x1f)))


		inline NODE* _GetNewNode(){
#	ifdef _NODE_POOL_USED_COMPACT
			if (_nodePoolCount == _NODE_POOL_SIZE){
				assert(false);
				return NULL;
			}
//			while (_bNodeUsed[_nodePoolPos]){
			while (_IsUsed(_nodePoolPos)){
				_nodePoolPos++;
				if (_nodePoolPos == _NODE_POOL_SIZE){
					_nodePoolPos = 0;
				}
			}
//			_bNodeUsed[_nodePoolPos] = true;
			_SetUsed(_nodePoolPos);
			_nodePoolCount++;
			return &_nodePool[_nodePoolPos];
#	else
//	 		assert(_nodePoolPos< _NODE_POOL_SIZE);
//	 		return _pNodeStack[_nodePoolPos++];
			return _nodePool.New();
#	endif //_NODE_POOL_USED_COMPACT
		}

		inline void _ReleaseNode(NODE* pNode){
#	ifdef _NODE_POOL_USED_COMPACT
			assert(_IsUsed(pNode - _nodePool));
			assert(pNode - _nodePool < _NODE_POOL_SIZE);
			_nodePoolCount--;
			_ClearUsed((int)(pNode - _nodePool));
#	else
//	 		assert(_nodePoolPos > 0);
//	 		_pNodeStack[--_nodePoolPos] = pNode;
			_nodePool.Delete(pNode);
#	endif //_NODE_POOL_USED_COMPACT
		}

#endif

#ifdef _DEBUG_DETAIL
	public:
		int testCollisionCount;
		int testMaxCollisionCount;
#endif

	};

}
