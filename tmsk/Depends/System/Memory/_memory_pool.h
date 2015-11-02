#pragma once
#include "../KType.h"
#include "../KMacro.h"
#include "../Collections/KSDList.h"
#include "../Log/log.h"

namespace System { namespace Memory {

	#if defined(__x64__)
	#define _check_point (0xFFFFEEEEFFFFEEEE)
	#else
	#define _check_point (0xFFFFEEEE)
	#endif
	#define _max_size(a,b) ((a)>(b)?(a):(b))
	#define _invalid_ptr (void*)(~0)
	#define _aligned_usize(usize) _max_size((usize+sizeof(void*)-1)&(~(sizeof(void*)-1)),sizeof(void*)*2)

	template <size_t usize,size_t capacity> struct _memory_piece
	{
		///
		struct unit_type { char x[_aligned_usize(usize)]; };
		struct unit_node { uint_r checkPoint; unit_node* m_pNextNode; };

		///
		int_r avail;
		unit_node* free_unit_list;
		int_r free_unit_num;
		unit_type units[capacity];

		///
		_memory_piece()
			: avail(capacity)
			, free_unit_list(NULL)
			, free_unit_num(0)
		{

		}
		void clear()
		{
			avail = capacity;
			free_unit_list = NULL;
			free_unit_num = 0;
		}
		BOOL has(void* ptr) const
		{
			unit_type* u = (unit_type*)ptr;
			return u >= &units[0] && u < &units[capacity];
		}
		void* allocUnit()
		{
			if(free_unit_list)
			{
				unit_node* unode = free_unit_list;
				free_unit_list = unode->m_pNextNode;
				free_unit_num--;
				return unode;
			}
			if(!avail) return NULL;
			return &units[--avail];
		}
		BOOL freeUnit(void* ptr)
		{
			if(!this->has(ptr)) return FALSE;
			unit_node* unode = (unit_node*)ptr;
			unode->m_pNextNode = free_unit_list;
			free_unit_list = unode;
			free_unit_num++;
			return TRUE;
		}
		BOOL isAvail() const
		{
			return free_unit_list || avail > 0;
		}
		BOOL empty() const
		{
			return free_unit_num+avail >= capacity;
		}
	};

	template <size_t usize> struct _dyn_memory_piece
	{
		struct unit_type { char x[_aligned_usize(usize)]; };
		struct unit_node { uint_r checkPoint; unit_node* m_pNextNode; };

		int_r avail, capacity;
		unit_node* free_unit_list;
		int_r free_unit_num;
		unit_type* units;

		_dyn_memory_piece()
			: avail(0)
			, capacity(0)
			, units(NULL)
			, free_unit_list(NULL)
			, free_unit_num(0)
		{

		}
		~_dyn_memory_piece()
		{
			if(units)
			{
				delete [] units;
				units = NULL;
			}
		}
		void init(size_t n)
		{
			capacity = n;
			avail = capacity;
			units = new unit_type[n];
			free_unit_list = NULL;
			free_unit_num = 0;
		}
		void clear()
		{
			avail = capacity;
			free_unit_list = NULL;
			free_unit_num = 0;
		}
		BOOL has(void* ptr)
		{
			unit_type* u = (unit_type*)ptr;
			return u >= &units[0] && u < &units[capacity];
		}
		void* allocUnit()
		{
			if(free_unit_list)
			{
				unit_node* unode = free_unit_list;
				free_unit_list = unode->m_pNextNode;
				free_unit_num--;
				return unode;
			}
			if(!avail) return NULL;
			return &units[--avail];
		}
		BOOL freeUnit(void* ptr)
		{
			if(!this->has(ptr)) return FALSE;
			unit_node* unode = (unit_node*)ptr;
			unode->m_pNextNode = free_unit_list;
			free_unit_list = unode;
			free_unit_num++;
			if(free_unit_num+avail >= capacity)
			{
				avail = capacity;
				free_unit_list = NULL;
				free_unit_num = 0;
			}
			return TRUE;
		}
		BOOL isAvail() const
		{
			return free_unit_list || avail > 0;
		}
		BOOL empty() const
		{
			return free_unit_num+avail >= capacity;
		}
	};
	template <size_t usize,size_t capacity> struct _memory_piece_node : public _memory_piece<usize,capacity>
	{
		typedef _memory_piece<usize,capacity> base_type;
		typedef typename base_type::unit_node unit_node;
		_memory_piece_node* m_pNextNode;
	};
	template <size_t usize,size_t step,typename lock=JG_S::KNoneLock> class _memory_pool
	{
	public:
		typedef lock lock_type;
		typedef _memory_piece_node<usize,step> piece_type;
		typedef typename piece_type::unit_node unit_node;
		typedef JG_C::KSDSelfList<piece_type> piece_list;
		typedef JG_C::KSDSelfList<unit_node> unit_list;

	public:
		lock_type  m_lock;
		piece_list m_full_list;
		piece_list m_avail_list;
		unit_list  m_free_unit_list;

	public:
		_memory_pool()
		{

		}
		~_memory_pool()
		{
			this->reset();
		}
		void reset()
		{
			m_lock.Lock();
			m_free_unit_list.clear();
			piece_type* piece;
			while(piece=m_avail_list.pop_front(), piece)
				delete piece;
			while(piece=m_full_list.pop_front(), piece)
				delete piece;
			m_lock.Unlock();
		}
		void clear()
		{
			m_lock.Lock();
			m_free_unit_list.clear();
			piece_type* piece = m_avail_list.begin();
			for(; piece; piece=m_avail_list.next(piece)) { piece->clear(); }
			while(piece=m_full_list.pop_front(), piece) { piece->clear(); m_avail_list.push_back(piece); }
			m_lock.Unlock();
		}
		void* Alloc()
		{
			m_lock.Lock();
			unit_node* ptr = m_free_unit_list.pop_front();
			if(ptr)
			{
				m_lock.Unlock();
				ptr->checkPoint = 0;
				return ptr;
			}
			piece_type* piece = m_avail_list.begin();
			if(!piece)
			{
				piece = new piece_type();
				//{
				//	char buf[1024];
				//	backtrace(buf, sizeof(buf));
				//	Log(LOG_WARN, "new-pool-piece: usize:%u step:%u where:%s", (int)usize, (int)step, buf);
				//}
				m_avail_list.push_back(piece);
			}
			ptr = (unit_node*)piece->allocUnit();
			if(!piece->isAvail())
			{
				m_avail_list.pop_front();
				m_full_list.push_back(piece);
			}
			m_lock.Unlock();
			ptr->checkPoint = 0;
			return ptr;
		}
		void Free(void* ptr)
		{
			unit_node* unode = (unit_node*)ptr;
			ASSERT(unode->checkPoint != _check_point);
			unode->checkPoint = _check_point;
			m_lock.Lock();
			m_free_unit_list.push_back(unode);
			m_lock.Unlock();
		}
		int_r _totalPiece() const
		{
			return m_full_list.size()+m_avail_list.size();
		}
	};

	template <size_t usize,size_t step,typename lock=JG_S::KNoneLock>
	class _shrinkable_memory_pool : public _memory_pool<usize,step,lock>
	{
	public:
		typedef _memory_pool<usize,step,lock> base_type;
		typedef typename base_type::piece_type piece_type;
		typedef typename base_type::unit_node unit_node;
		typedef typename base_type::piece_list piece_list;
		typedef typename base_type::unit_list unit_list;

	public:
		void Free(void* ptr)
		{
			//ASSERT(this->_has(ptr));
			unit_node* unode = (unit_node*)ptr;
			ASSERT(unode->checkPoint != _check_point);
			unode->checkPoint = _check_point;
			base_type::m_lock.Lock();
			base_type::m_free_unit_list.push_back(unode);
			if(this->_needShrink()) this->_shrink();
			base_type::m_lock.Unlock();
		}

	protected:
		BOOL _has(void* ptr) const
		{
			const piece_type* piece = base_type::m_full_list.begin();
			for(; piece; piece=base_type::m_full_list.next(piece))
				if(piece->has(ptr)) return TRUE;
			piece = base_type::m_avail_list.begin();
			for(; piece; piece=base_type::m_avail_list.next(piece))
				if(piece->has(ptr)) return TRUE;
			return FALSE;
		}
		BOOL _needShrink() const
		{
			int_r piece_count = this->_totalPiece();
			int_r free_obj_count = base_type::m_free_unit_list.size();
			return (piece_count > 3) && (free_obj_count/(int_r)step) >= (piece_count/2);
		}
		void _shrink()
		{
			piece_type* piece;
			piece_list tmpList, emptyList;
			int_r pieceCount = this->_totalPiece();

			// full list
			while(piece=base_type::m_full_list.pop_front(), piece)
			{
				if(this->_shrinkPiece(piece))
				{
					emptyList.push_back(piece);
				}
				else if(piece->isAvail())
				{
					base_type::m_avail_list.push_back(piece);
				}
				else
				{
					tmpList.push_back(piece);
				}
				if(base_type::m_free_unit_list.empty()) break;
			}
			while(piece=tmpList.pop_front(), piece) base_type::m_full_list.push_back(piece);

			// avail list
			while(piece=base_type::m_avail_list.pop_front(), piece)
			{
				if(this->_shrinkPiece(piece))
				{
					emptyList.push_back(piece);
				}
				else
				{
					tmpList.push_back(piece);
				}
				if(base_type::m_free_unit_list.empty()) break;
			}
			while(piece=tmpList.pop_front(), piece) base_type::m_avail_list.push_back(piece);

			ASSERT(base_type::m_free_unit_list.empty());

			// empty list
			while(piece=emptyList.pop_front(), piece)
			{
				if(pieceCount > 3)
				{
					delete piece;
					pieceCount--;
				}
				else
				{
					base_type::m_avail_list.push_back(piece);
				}
			}
		}
		BOOL _shrinkPiece(piece_type* piece)
		{
			unit_node* u;
			unit_list tmpList;
			while(u=base_type::m_free_unit_list.pop_front(), u)
			{
				if(piece->freeUnit(u))
				{
					if(piece->empty())
					{
						piece->clear();
						while(u=tmpList.pop_front(), u)
							base_type::m_free_unit_list.push_back(u);
						return TRUE;
					}
				}
				else
				{
					tmpList.push_back(u);
				}
			}

			while(u=tmpList.pop_front(), u)
				base_type::m_free_unit_list.push_back(u);

			return FALSE;
		}
	};

	template <typename T,size_t step,bool shrinkable=false,typename lock=JG_S::KNoneLock> class _object_pool
	{
	public:
		typedef T object_type;
		typedef lock lock_type;
		typedef JG_C::SDListNode<object_type> object_node;
		typedef _memory_pool<sizeof(object_node),step,JG_S::KNoneLock> _pool_type;
		typedef _shrinkable_memory_pool<sizeof(object_node),step,JG_S::KNoneLock> _shrinkable_pool_type;
		typedef typename _type_switch_<shrinkable,_shrinkable_pool_type,_pool_type>::type memory_pool_type;
		typedef JG_C::KSDSelfList<object_node> object_list;

	public:
		lock_type m_lock;
		object_list m_free_obj_list;
		memory_pool_type m_memory_pool;

	public:
		_object_pool()
		{

		}
		~_object_pool()
		{
			this->reset();
		}
		void reset()
		{
			m_lock.Lock();
			object_node* node;
			while(node=m_free_obj_list.pop_front(), node)
				((object_type*)node)->~object_type();
			m_memory_pool.reset();
			m_lock.Unlock();
		}
		void clear()
		{
			m_lock.Lock();
			object_node* node;
			while(node=m_free_obj_list.pop_front(), node)
				((object_type*)node)->~object_type();
			m_memory_pool.clear();
			m_lock.Unlock();
		}
		object_type* Alloc()
		{
			m_lock.Lock();
			object_node* node = m_free_obj_list.pop_front();
			if(node)
			{
				m_lock.Unlock();
				node->m_pNextNode = (object_node*)_invalid_ptr;
				return (object_type*)node;
			}
			node = (object_node*)m_memory_pool.Alloc();
			ASSERT(node);
			new (node) object_type;
			m_lock.Unlock();
			node->m_pNextNode = (object_node*)_invalid_ptr;
			return (object_type*)node;
		}
		void Free(object_type* obj)
		{
			object_node* node = (object_node*)obj;
			if(node->m_pNextNode != _invalid_ptr)
			{
				ASSERT_I(!" !!!!!!!!!!! free the pointer more than once !!!!!!!!!!!");
				return;
			}
			//ASSERT(node->m_pNextNode==_invalid_ptr);
			m_lock.Lock();
			m_free_obj_list.push_back(node);
			if(shrinkable && this->_needShrink())
			{
				while(node=m_free_obj_list.pop_front(), node)
				{
					((object_type*)node)->~object_type();
					uint_r* pp = (uint_r*)node; pp[0] = 0;
					m_memory_pool.Free(node);
				}
			}
			m_lock.Unlock();
		}

	protected:
		BOOL _needShrink() const
		{
			int_r free_obj_count = m_free_obj_list.size();
			int_r piece_count = m_memory_pool._totalPiece();
			return piece_count > 3 && (free_obj_count/(int_r)step) >= (piece_count/2);
		}
	};

	template <typename T,size_t step,bool shrinkable=false,typename lock=JG_S::KNoneLock>
	class _autoreset_object_pool
	{
	public:
		typedef T object_type;
		typedef lock lock_type;
		typedef _memory_pool<sizeof(object_type),step,JG_S::KNoneLock> _pool_type;
		typedef _shrinkable_memory_pool<sizeof(object_type),step,JG_S::KNoneLock> _shrinkable_pool_type;
		typedef typename _type_switch_<shrinkable,_shrinkable_pool_type,_pool_type>::type memory_pool_type;
		typedef typename memory_pool_type::unit_node unit_node;

	public:
		lock_type m_lock;
		memory_pool_type m_memory_pool;

	public:
		_autoreset_object_pool()
		{

		}
		~_autoreset_object_pool()
		{
	
		}
		void reset()
		{
			m_lock.Lock();
			m_memory_pool.reset();
			m_lock.Unlock();
		}
		void clear()
		{
			m_lock.Lock();
			m_memory_pool.clear();
			m_lock.Unlock();
		}
		object_type* Alloc()
		{
			m_lock.Lock();
			object_type* node = (object_type*)m_memory_pool.Alloc();
			ASSERT(node);
			m_lock.Unlock();
			new (node) object_type;
			return (object_type*)node;
		}
		void Free(object_type* obj)
		{
			unit_node* u = (unit_node*)obj;
			ASSERT(u->checkPoint != _check_point);
			obj->~object_type();
			m_lock.Lock();
			m_memory_pool.Free(obj);
			m_lock.Unlock();
		}
	};

	template <typename T,size_t step,bool shrinkable=false,typename L=::System::Sync::KNoneLock>
	class _portable_object_pool
	{
	public:
		typedef _object_pool<T,step,shrinkable,L> object_pool_type;
		static object_pool_type* m_staticPool;
		static object_pool_type* _get_object_pool() { return m_staticPool; }
		static T* Alloc()
		{
			object_pool_type* pool = _get_object_pool();
			return pool->Alloc();
		}
		static void Free(T* ptr)
		{
			object_pool_type* pool = _get_object_pool();
			pool->Free(ptr);
		}
	};

	template <typename T,size_t step,bool shrinkable,typename L>
		typename _portable_object_pool<T,step,shrinkable,L>::object_pool_type*
			_portable_object_pool<T,step,shrinkable,L>::m_staticPool
				= new typename _portable_object_pool<T,step,shrinkable,L>::object_pool_type();

	template <typename T,size_t step,bool shrinkable=false,typename L=::System::Sync::KNoneLock>
	class _portable_autoreset_object_pool
	{
	public:
		typedef _autoreset_object_pool<T,step,shrinkable,L> object_pool_type;
		static object_pool_type* m_staticPool;
		static object_pool_type* _get_object_pool() { return m_staticPool; }
		static T* Alloc()
		{
			object_pool_type* pool = _get_object_pool();
			return pool->Alloc();
		}
		static void Free(T* ptr)
		{
			object_pool_type* pool = _get_object_pool();
			pool->Free(ptr);
		}
	};

	template <typename T,size_t step,bool shrinkable,typename L> 
		typename _portable_autoreset_object_pool<T,step,shrinkable,L>::object_pool_type*
			_portable_autoreset_object_pool<T,step,shrinkable,L>::m_staticPool
				= new typename _portable_autoreset_object_pool<T,step,shrinkable,L>::object_pool_type();

	#undef _check_point
	#undef _max_size
	#undef _invalid_ptr
	#undef _aligned_usize
}}
