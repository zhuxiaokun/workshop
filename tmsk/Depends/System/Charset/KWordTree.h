#pragma once
#include "../KType.h"
#include "../KMacro.h"
#include "../Memory/KStepObjectPool.h"
#include "../Memory/KDataBuffer.h"

class KCharset;
class StreamInterface;

namespace charset
{
	enum { memory_piece_size = 4096 };
	struct char_node
	{
		char_node* sibling;
		char_node* children;
		unsigned int chr;
	};
	struct char_node_cmp
	{
		typedef char_node* ptr;
		int operator () (const ptr& a, const ptr& b) const
		{
			return (int)(a->chr - b->chr);
		}
	};
	
	#pragma warning(disable:4200)
	struct i_char_node_arr;
	struct i_char_node
	{
		i_char_node_arr* children;
		unsigned int chr;
	};
	struct i_char_node_arr
	{
		int_r count;
		i_char_node chars[0];
		///
		static size_t calc_size(int_r count)
		{
			return offsetof(i_char_node_arr,chars) + count*sizeof(i_char_node);
		}
		size_t size() const
		{
			return offsetof(i_char_node_arr,chars) + count*sizeof(i_char_node);
		}
		i_char_node* find(unsigned int chr)
		{
			if(count < 3)
			{
				for(int_r i=0; i<count; i++)
					if(chars[i].chr == chr) return &chars[i];
				return NULL;
			}
			else
			{
				int_r b = 0;
				int_r e = count-1;
				int_r m = (e-b+1)/2 + b;
				while(1)
				{
					unsigned int mchr = chars[m].chr;
					if(chr < mchr)
					{
						e = m - 1;
					}
					else if(chr == mchr)
					{
						return &chars[m];
					}
					else
					{
						b = m + 1;
					}
					if(e - b < 2)
					{
						for(int_r i=b; i<=e; i++)
							if(chars[i].chr == chr)
								return &chars[i];
						return NULL;
					}
					m = (e-b+1)/2 + b;
				}
				return NULL;
			}
		}
	};
	#pragma warning(default:4200)

	typedef unsigned int uchar32;
	typedef ::System::Collections::KDynSortedArray<uchar32,KUintCompare<uchar32>,256> uchar32_array;
	typedef ::System::Memory::_shrinkable_memory_pool<memory_piece_size,4> i_memory_pool;
	typedef ::System::Memory::_autoreset_object_pool<char_node,256> char_node_pool;

	class KWordTree
	{
		friend class ::KCharset;
	public:
		char_node* m_root;
		i_char_node_arr* m_root_chars;
		char_node_pool m_nodePool;
		i_memory_pool m_i_memory_pool;
		::System::Memory::KDataBuffer m_data_buff;
		::System::Collections::DynArray<void*> m_none_pool_memory;

	public:
		KWordTree();
		~KWordTree();

	public:
		BOOL loadFile(KCharset* cs, StreamInterface& si);
		BOOL addWord(KCharset* cs, const char* word);

	public:
		BOOL match(KCharset* cs, const char* str);
		BOOL match2(KCharset* cs, const char* str, char* matched, int len);
		BOOL matchReplace(KCharset* cs, char* str, int chr);

	protected:
		char_node* addFirstChar(unsigned int chr, BOOL* exists);
		char_node* addChar(char_node* parent, unsigned int chr, BOOL* exists);
		BOOL endWord(char_node* node);

	private:
		char_node* allocNode(unsigned int chr);
		void freeNode(char_node* node);
		void _removeNodeTree(char_node* node);
		
		int to_uchar(KCharset* cs, const char* src, unsigned int* uchar);
		int from_uchar(KCharset* cs, unsigned int uchar, char* dst, int dstlen);

		char_node* findChar(char_node* list, unsigned int chr);
		char_node* addChild(char_node* parent, unsigned int chr, BOOL* exists);
		char_node* addSibling(char_node* sibling, unsigned int chr, BOOL* exists);

		void* _acquireMemory(size_t len);
		i_char_node_arr* _build_char_arr(char_node* node);
		void _re_construct_tree();

		int_r _match_uchar(KCharset* cs, const uchar32* ustr, const uchar32** matched);
		int_r _match_uchar_from(KCharset* cs, i_char_node_arr* root, const uchar32* ustr);
	};
}
