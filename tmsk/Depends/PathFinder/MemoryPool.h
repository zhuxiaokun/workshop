/********************************************************************
created:	2009/02/05
filename: 	Athena\Main\Src\Common\LogicWorld\Src\DataStructure\MemoryPool.h
file path:	Athena\Main\Src\Common\LogicWorld\Src\DataStructure
file base:	MemoryPool
file ext:	h
copyright:	AuroraGame
author:		Allen Wang

purpose:	A fast memory pool that used by A star to create node
*********************************************************************/

#pragma once

#include <System/Memory/KObjectPool.h>
#include <System/Log/log.h>

#ifdef _ASTAR_OWNER_MEMORY_POOL
template <typename _T, int _SIZE = 10000>
class MemoryPool{
public:
	MemoryPool(unsigned int initSize = 0){
//		unsigned int initSize = _SIZE;
		if (initSize == 0){
			_nodes = NULL;
			_pStack = NULL;
			_size = 0;
		}else{
			_nodes = NULL;
			Create(initSize);
		}
	}

	~MemoryPool(){
		if (_nodes){
			delete[] _nodes;
			delete[] _pStack;
		}
	}

	// Warning: all data in old space will be deleted!
	inline void Create(unsigned int newSize){
		ASSERT(newSize != 0);
		if (_nodes){
			delete[] _nodes;
			delete[] _pStack;
		}
		_nodes = new _T[newSize];
		ASSERT(_nodes);
		_pStack = new _T*[newSize];
		ASSERT(_pStack);
		_size = newSize;
		Clear();
	}

	inline void Clear(){
		_pos = 0;
		for (unsigned int i = 0; i < _size; i++){
			_pStack[i] = &_nodes[i];
		}
	}

	inline _T* New(){
		//if memory pool is full, return NULL
		if (_pos >= _size) return NULL;
// 		ASSERT(_pos < _size);
		return _pStack[_pos++];
	}

	inline void Delete(_T* pObj){
		ASSERT((unsigned int)(pObj - _nodes) < _size);
		_pStack[--_pos] = pObj;

	};

private:
 	_T* _nodes;
 	_T** _pStack;
	unsigned int _pos;
	unsigned int _size;
};
#else
template <typename _T, int _SIZE = 10000>
class MemoryPool
{
public:
	MemoryPool(unsigned int initSize = 0)
	{
	}

	~MemoryPool()
	{
	}

	inline void Create(unsigned int newSize)
	{
	}

	inline void Clear()
	{
		_objPool.clear();
	}

	inline _T* New()
	{
		return _objPool.Get();
	}

	inline void Delete(_T* pObj)
	{
		_objPool.Release(pObj);
	};
private:
	::System::Memory::KObjectPool2<_T, _SIZE> _objPool;
};
#endif // _ASTAR_OWNER_MEMORY_POOL
