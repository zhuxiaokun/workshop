#include "KWordTree.h"
#include "KCharset.h"
#include "../File/KTextReader.h"

DECLARE_SIMPLE_TYPE(charset::i_char_node);

namespace charset
{

class i_char_cmp
{
public:	int operator () (const i_char_node& a, const i_char_node& b) const
		{ return (int)(a.chr - b.chr); }
};

typedef ::System::Collections::KDynSortedArray<i_char_node,i_char_cmp, 32> ArrayCharNode;


KWordTree::KWordTree():m_root(NULL),m_root_chars(NULL)
{

}

KWordTree::~KWordTree()
{
	while(!m_none_pool_memory.empty())
		free_k(m_none_pool_memory.pop_back());
}

BOOL KWordTree::loadFile(KCharset* cs, StreamInterface& si)
{
	JG_F::KTextReader r;
	if(!r.SetInputStream(si))
		return FALSE;

	char line[1024];
	while(1)
	{
		int n = r.ReadLine(line, sizeof(line));
		if(!n) continue; if(n < 0) break;
		char* word = _trim(line);
		this->addWord(cs, word);
	}

	this->_re_construct_tree();
	return TRUE;
}

BOOL KWordTree::addWord(KCharset* cs, const char* word)
{
	BOOL exists;
	unsigned uchr;
	char_node* node;
	BOOL firstChar = TRUE;
	
	while(*word)
	{
		int n = this->to_uchar(cs, word, &uchr);
		if(n < 1) return FALSE; word += n;
		if(firstChar)
		{
			node = this->addFirstChar(uchr, &exists);
			if(exists && !node->children) return TRUE;
			firstChar = FALSE;
		}
		else
		{
			node = this->addChar(node, uchr, &exists);
			if(exists && !node->children) return TRUE;
		}
	}
	this->endWord(node);
	return TRUE;
}

BOOL KWordTree::match(KCharset* cs, const char* str)
{
	int_r char_count = 0;
	const int_r max_char_count = memory_piece_size/sizeof(uchar32) - 1;

	uchar32* ustr = (uchar32*)m_i_memory_pool.Alloc();
	while(*str && char_count < max_char_count)
	{
		uchar32 uchar;
		int n = this->to_uchar(cs, str, &uchar); str += n;
		if(n < 1)
		{
			m_i_memory_pool.Free(ustr);
			return TRUE;
		}
		ustr[char_count++] = uchar;
	}	ustr[char_count] = 0;

	const uchar32* matched;
	BOOL ret = !!this->_match_uchar(cs, ustr, &matched);

	m_i_memory_pool.Free(ustr);
	return ret;
}

BOOL KWordTree::match2(KCharset* cs, const char* str, char* matched, int len)
{
	int_r char_count = 0;
	const int_r max_char_count = memory_piece_size/sizeof(uchar32) - 1;

	uchar32* ustr = (uchar32*)m_i_memory_pool.Alloc();
	while(*str && char_count < max_char_count)
	{
		uchar32 uchar;
		int n = this->to_uchar(cs, str, &uchar); str += n;
		if(n < 1)
		{
			m_i_memory_pool.Free(ustr);
			return TRUE;
		}
		ustr[char_count++] = uchar;
	}	ustr[char_count] = 0;

	const uchar32* umatched;
	int_r matched_char_count = this->_match_uchar(cs, ustr, &umatched);
	if(!matched_char_count)
	{
		m_i_memory_pool.Free(ustr);
		return FALSE;
	}

	while(len > 0)
	{
		int n = this->from_uchar(cs, *umatched, matched, len);
		if(n < 1) break; umatched++; len -= n; matched += n;
	}	*matched = '\0';

	m_i_memory_pool.Free(ustr);
	return TRUE;
}

BOOL KWordTree::matchReplace(KCharset* cs, char* str, int chr)
{
	int_r char_count = 0;
	const int_r max_char_count = memory_piece_size/sizeof(uchar32) - 1;

	char* ostr = str;
	uchar32* ustr = (uchar32*)m_i_memory_pool.Alloc();
	while(*str && char_count < max_char_count)
	{
		uchar32 uchar;
		int n = this->to_uchar(cs, str, &uchar); str += n;
		if(n < 1)
		{
			m_i_memory_pool.Free(ustr);
			return TRUE;
		}
		ustr[char_count++] = uchar;
	}	ustr[char_count] = 0;

	const uchar32* matched;
	uchar32* oustr = ustr;
	while(*ustr)
	{
		int_r matched_char_count = this->_match_uchar(cs, ustr, &matched);
		if(!matched_char_count) break; ustr = (uchar32*)matched;
		while(matched_char_count--) *ustr++ = chr;
	}

	if(ustr != oustr)
	{
		str = ostr;
		ustr = oustr;
		while(*ustr)
		{
			int n = this->from_uchar(cs, *ustr++, str, 8);
			if(n < 1) break; str += n;
		}	*str = '\0';
		m_i_memory_pool.Free(oustr);
		return TRUE;
	}
	else
	{
		m_i_memory_pool.Free(oustr);
		return FALSE;
	}
}

char_node* KWordTree::addFirstChar(unsigned int chr, BOOL* exists)
{
	if(!m_root)
	{
		m_root = this->allocNode(chr);
		*exists = FALSE;
		return m_root;
	}
	else
	{
		return this->addSibling(m_root, chr, exists);
	}
}

char_node* KWordTree::addChar(char_node* parent, unsigned int chr, BOOL* exists)
{
	return this->addChild(parent, chr, exists);
}

BOOL KWordTree::endWord(char_node* node)
{
	char_node* child = node->children;
	if(!child) return TRUE;
	this->_removeNodeTree(child);
	node->children = NULL;
	return TRUE;
}

char_node* KWordTree::allocNode(unsigned int chr)
{
	char_node* node = m_nodePool.Alloc();
	node->chr = chr;
	node->sibling = NULL;
	node->children = NULL;
	return node;
}

void KWordTree::freeNode(char_node* node)
{
	m_nodePool.Free(node);
}

void KWordTree::_removeNodeTree(char_node* node)
{
	char_node* p = node;
	while(p)
	{
		char_node* p2 = p; p = p->sibling;
		if(p2->children)
		{
			this->_removeNodeTree(p2->children);
			p2->children = NULL;
		}	this->freeNode(p2);
	}
}

int KWordTree::to_uchar(KCharset* cs, const char* src, unsigned int* uchar)
{
	if(cs)
	{
		int srclen = src[0] ? (src[1] ? 2 : 1) : 0;
		return cs->to_uchar(src, srclen, uchar);
	}
	return utf8ToUnicode(src, uchar);
}

int KWordTree::from_uchar(KCharset* cs, unsigned int uchar, char* dst, int dstlen)
{
	if(cs) return cs->from_uchar(uchar, dst, dstlen);
	return unicodeToUtf8(uchar, dst, dstlen);
}

char_node* KWordTree::findChar(char_node* list, unsigned int chr)
{
	char_node* node = list;
	while(node)
	{
		if(node->chr == chr)
			return node;
		node = node->sibling;
	}
	return NULL;
}

char_node* KWordTree::addChild(char_node* parent, unsigned int chr, BOOL* exists)
{
	ASSERT(parent);
	char_node* sibling = parent->children;

	if(!sibling)
	{
		sibling = this->allocNode(chr);
		parent->children = sibling;
		*exists = FALSE;
		return sibling;
	}

	return this->addSibling(sibling, chr, exists);
}

char_node* KWordTree::addSibling(char_node* sibling, unsigned int chr, BOOL* exists)
{
	ASSERT(sibling);
	char_node* node = sibling;
	while(1)
	{
		if(node->chr == chr)
		{
			*exists = TRUE;
			return node;
		}

		if(!node->sibling)
			break;

		node = node->sibling;
	}

	char_node* nnode = this->allocNode(chr);
	node->sibling = nnode;
	*exists = FALSE;
	return nnode;
}

void* KWordTree::_acquireMemory(size_t len)
{
	if(len > memory_piece_size)
	{
		void* p = malloc_k(len);
		m_none_pool_memory.push_back(p);
		return p;
	}
	else
	{
		if(m_data_buff._avail() < len)
		{
			void* p = m_i_memory_pool.Alloc();
			m_data_buff.attach(p, memory_piece_size);
		}
		void* ret = m_data_buff._end();
		m_data_buff.seek(m_data_buff.m_size+len);
		return ret;
	}
}

i_char_node_arr* KWordTree::_build_char_arr(char_node* node)
{
	int_r sum = 0;
	char_node* n = node; while(n) { sum++; n = n->sibling; }

	size_t len = i_char_node_arr::calc_size(sum);
	i_char_node_arr* i_arr = (i_char_node_arr*)this->_acquireMemory(len);

	i_arr->count = sum;
	ArrayCharNode arr; arr.attach(i_arr->chars, (int)sum, 0);

	i_char_node i_node;
	n = node; while(n)
	{
		i_node.chr = n->chr;
		i_node.children = n->children ? this->_build_char_arr(n->children) : NULL;
		arr.insert(i_node);
		n = n->sibling;
	}

	return i_arr;
}

void KWordTree::_re_construct_tree()
{
	while(!m_none_pool_memory.empty())
		free_k(m_none_pool_memory.pop_back());
	m_i_memory_pool.clear();
	m_data_buff.attach(m_i_memory_pool.Alloc(), memory_piece_size);
	m_root_chars = NULL;
	if(!m_root) return;
	m_root_chars = this->_build_char_arr(m_root);
}

int_r KWordTree::_match_uchar(KCharset* cs, const uchar32* ustr, const uchar32** matched)
{
	i_char_node_arr* i_arr = m_root_chars;
	if(!i_arr) return 0;
	while(*ustr)
	{
		i_char_node* node = i_arr->find(*ustr);
		if(!node) { ustr++; continue; }
		*matched = ustr;
		if(!node->children) return 1;
		if(!*++ustr) return 0;
		int_r len = this->_match_uchar_from(cs, node->children, ustr);
		if(len) return 1+len;
	}
	return 0;
}

int_r KWordTree::_match_uchar_from(KCharset* cs, i_char_node_arr* root, const uchar32* ustr)
{
	i_char_node* node = root->find(*ustr);
	if(!node) return 0;
	if(!node->children) return 1;
	if(!*++ustr) return 0;
	int_r n2 = this->_match_uchar_from(cs, node->children, ustr);
	return n2 ? 1+n2 : 0;
}

}
