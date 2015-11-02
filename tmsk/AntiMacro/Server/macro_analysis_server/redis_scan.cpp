#include "redis_scan.h"
#include <System/Log/log.h>
#include <System/Misc/StrUtil.h>

//// KAccountNameArray

KNameArray::KNameArray()
	: m_nextPos(0)
	, m_current(NULL)
{

}

KNameArray::~KNameArray()
{
	this->reset();
}

bool KNameArray::avail() const
{
	return m_nextPos < CAPACITY;
}

int KNameArray::size() const
{
	return m_nextPos;
}

const char* KNameArray::append(const char* name)
{
	if(!this->avail())
		return NULL;

	size_t len = strlen(name) + 1;
	const char* nm = this->_alloc(name, len);
	m_buffer[m_nextPos++] = nm;

	return nm;
}

const char* KNameArray::at(int idx) const
{
	ASSERT(idx >= 0 && idx < m_nextPos);
	return m_buffer[idx];
}

void KNameArray::reset()
{
	list_node::reset();
	m_nextPos = 0;
	while(!m_bufferList.empty())
		KNameBuffer::Free(m_bufferList.pop_front());
}

const char* KNameArray::_alloc(const char* str, size_t len)
{
	if(!m_current)
	{
		m_current = KNameBuffer::Alloc();
	}
	if((size_t)m_current->WriteAvail() < len)
	{
		m_bufferList.push_back(m_current);
		m_current = KNameBuffer::Alloc();
	}
	const char* p = (const char*)m_current->GetWriteBuffer();
	m_current->Write(str, len);
	return p;
}

//// KLargeNameList::iterator::

KLargeNameList::iterator::iterator():m_index(0),m_arr(NULL)
{

}

KLargeNameList::iterator::iterator(const iterator& o):m_index(o.m_index),m_arr(o.m_arr)
{

}

KLargeNameList::iterator::~iterator()
{

}

KLargeNameList::iterator& KLargeNameList::iterator::operator = (const iterator& o)
{
	if(this == &o) return *this;
	m_index = o.m_index;
	m_arr = o.m_arr;
	return *this;
}

KLargeNameList::iterator& KLargeNameList::iterator::operator ++ ()
{
	if(!m_arr) return *this;
	if(m_index+1 < m_arr->size())
	{
		m_index += 1;
		return *this;
	}
	m_index = 0;
	m_arr = m_arr->m_nextNode;
	return *this;
}

KLargeNameList::iterator& KLargeNameList::iterator::operator ++ (int)
{
	if(!m_arr) return *this;
	if(m_index+1 < m_arr->size())
	{
		m_index += 1;
		return *this;
	}
	m_index = 0;
	m_arr = m_arr->m_nextNode;
	return *this;
}

bool KLargeNameList::iterator::operator == (const iterator& o) const
{
	return m_index == o.m_index && m_arr == o.m_arr;
}

bool KLargeNameList::iterator::operator != (const iterator& o) const
{
	return m_index != o.m_index || m_arr != o.m_arr;
}

const char* KLargeNameList::iterator::operator * () const
{
	if(!m_arr) return NULL;
	return m_arr->m_buffer[m_index];
}

//// KLargeNameList

KLargeNameList::KLargeNameList()
{

}

KLargeNameList::~KLargeNameList()
{
	this->reset();
}

int KLargeNameList::size() const
{
	int n = (int)m_list.size();
	if(n < 1)
	{
		return 0;
	}
	else if(n == 1)
	{
		return m_list.rbegin()->size();
	}
	else
	{
		return (n - 1)*KNameArray::CAPACITY + m_list.rbegin()->size();
	}
}

const char* KLargeNameList::append(const char* name)
{
	KNameArray* arr = m_list.rbegin();
	if(!arr)
	{
		arr = KNameArray::Alloc();
		m_list.push_back(arr);
	}
	if(!arr->avail())
	{
		arr = KNameArray::Alloc();
		m_list.push_back(arr);
	}
	return arr->append(name);
}

void KLargeNameList::reset()
{
	while(!m_list.empty())
	{
		KNameArray* p = m_list.pop_front();
		p->reset();
		KNameArray::Free(p);
	}
}

bool KLargeNameList::foreach(lua_func func, l_stackval ctx)
{
	iterator it = this->begin();
	for(; it != this->end(); it++)
	{
		const char* name = *it;
		if(!func.r_invoke<bool,const char*,l_stackval>(name, ctx))
			return false;
	}
	return true;
}

//// KRedis

KRedis::KRedis():m_ctx(NULL)
{

}

KRedis::KRedis(const KRedis& o):m_ctx(o.m_ctx)
{

}

KRedis::KRedis(redisContext* c) : m_ctx(c)
{

}

KRedis::~KRedis()
{

}

bool KRedis::isReady()
{
	redisReply* reply = NULL;
	redisContext* c = m_ctx;
	redisAppendCommand(c, "ping");
	int nRet = redisGetReply(c, (void**)&reply);

	if(nRet != REDIS_OK || !reply)
	{
		if(reply) freeReplyObject(reply);
		return false;
	}
	
	bool b = stricmp(reply->str, "PONG") == 0;
	if(reply) freeReplyObject(reply);
	
	return true;
}

bool KRedis::scan(const char* pattern, scan_visitor& v)
{
	size_t pattern_len = strlen(pattern);

	redisReply* reply = NULL;
	redisContext* c = m_ctx;

	int cursor = 0;
	bool done = false;

	while(1)
	{
		redisAppendCommand(c, "scan %d match %s count 10", cursor, pattern);
		int nRet = redisGetReply(c, (void**)&reply);
		if(nRet != REDIS_OK || !reply)
		{
			Log(LOG_ERROR, "error: scan for %s, %s", pattern, c->errstr);
			if(reply) freeReplyObject(reply);
			return false;
		}
		if(reply->type != REDIS_REPLY_ARRAY || reply->elements != 2)
		{
			Log(LOG_ERROR, "error: scan for %s, %s", pattern, c->errstr);
			if(reply) freeReplyObject(reply);
			return false;
		}

		cursor = str2int(reply->element[0]->str);

		redisReply* valReply = reply->element[1];
		for(int i=0; i<(int)valReply->elements; i++)
		{
			const char* k = valReply->element[i]->str;
			if(!v.accept(k))
			{
				done = true;
				break;
			}
		}

		freeReplyObject(reply);
		if(!cursor || done)
			break;
	}

	return true;
}

bool KRedis::hscan(const char* key, const char* pattern, hscan_visitor& v)
{
	size_t pattern_len = strlen(pattern);

	redisReply* reply = NULL;
	redisContext* c = m_ctx;

	int cursor = 0;
	bool done = false;

	while(1)
	{
		redisAppendCommand(c, "hscan %s %d match %s count 10", key, cursor, pattern);
		int nRet = redisGetReply(c, (void**)&reply);
		if(nRet != REDIS_OK || !reply)
		{
			Log(LOG_ERROR, "error: hscan for %s %s, %s", key, pattern, c->errstr);
			if(reply) freeReplyObject(reply);
			return false;
		}
		if(reply->type != REDIS_REPLY_ARRAY || reply->elements != 2)
		{
			Log(LOG_ERROR, "error: hscan for %s, %s", key, pattern, c->errstr);
			if(reply) freeReplyObject(reply);
			return false;
		}

		cursor = str2int(reply->element[0]->str);

		redisReply* valReply = reply->element[1];
		for(int i=0; i<(int)valReply->elements; i+=2)
		{
			const char* field = valReply->element[i]->str;
			const char* val = valReply->element[i+1]->str;
			if(!v.accept(field, val))
			{
				done = true;
				break;
			}
		}

		freeReplyObject(reply);
		if(!cursor || done)
			break;
	}

	return true;
}

bool KRedis::lrange(const char* key, int start, int end, lscan_visitor& v)
{
	int idx = start;
	const int step = 32;
	bool done = false;

	while(1)
	{
		if(!this->_lrange(key, idx, idx+step-1, v, &done))
			return false;

		if(done)
			break;

		idx += step;
		if(end >= 0 && idx > end)
			break;
	}
	return true;
}

bool KRedis::_lrange(const char* key, int start, int end, lscan_visitor& v, bool* done)
{
	redisReply* reply = NULL;
	redisContext* c = m_ctx;

	redisAppendCommand(c, "lrange %s %d %d", key, start, end);
	int nRet = redisGetReply(c, (void**)&reply);
	if(nRet != REDIS_OK || !reply)
	{
		Log(LOG_ERROR, "error: lrange for %s %d %d, %s", key, start, end, c->errstr);
		if(reply) freeReplyObject(reply);
		return false;
	}
	if(reply->type != REDIS_REPLY_ARRAY)
	{
		Log(LOG_ERROR, "error: lrange for %s %d %d, %s", key, start, end, c->errstr);
		if(reply) freeReplyObject(reply);
		return false;
	}

	if(!reply->elements)
	{
		*done = true;
	}
	else
	{
		for(int i=0; i<(int)reply->elements; i++)
		{
			const char* val = reply->element[i]->str;
			if(!v.accept(val))
			{
				*done = true;
				break;
			}
		}
	}

	freeReplyObject(reply);
	return true;
}

bool KRedis::hget(const char* key, const char* field, char* val, size_t len)
{
	redisReply* reply = NULL;
	redisContext* c = m_ctx;

	redisAppendCommand(c, "hget %s %s", key, field);
	int nRet = redisGetReply(c, (void**)&reply);
	if(nRet != REDIS_OK || !reply)
	{
		Log(LOG_ERROR, "error: hget for %s %s, %s", key, field, c->errstr);
		if(reply) freeReplyObject(reply);
		return false;
	}

	strcpy_k(val, len, reply->str);

	freeReplyObject(reply);
	return true;
}

bool KRedis::del(const char** keys, int count)
{
	redisReply* reply = NULL;
	redisContext* c = m_ctx;

	for(int i=0; i<count; i++)
		redisAppendCommand(c, "del %s", keys[i]);
	
	for(int i=0; i<count; i++)
	{
		int nRet = redisGetReply(c, (void**)&reply);
		if(nRet != REDIS_OK || !reply)
		{
			Log(LOG_ERROR, "error: del for %s, %s", keys[i], c->errstr);
			if(reply) freeReplyObject(reply);
			return false;
		}
		freeReplyObject(reply);
	}

	return true;
}

bool KRedis::hdel(const char* key, const char** fields, int count)
{
	redisReply* reply = NULL;
	redisContext* c = m_ctx;

	for(int i=0; i<count; i++)
		redisAppendCommand(c, "hdel %s %s", key, fields[i]);

	for(int i=0; i<count; i++)
	{
		int nRet = redisGetReply(c, (void**)&reply);
		if(nRet != REDIS_OK || !reply)
		{
			Log(LOG_ERROR, "error: hdel for %s %s, %s", key, fields[i], c->errstr);
			if(reply) freeReplyObject(reply);
			return false;
		}
		freeReplyObject(reply);
	}

	return true;
}

bool KRedis::ltrim(const char* key, int start, int end)
{
	redisReply* reply = NULL;
	redisContext* c = m_ctx;

	redisAppendCommand(c, "ltrim %s %d %d", key, start, end);
	int nRet = redisGetReply(c, (void**)&reply);
	if(nRet != REDIS_OK || !reply)
	{
		Log(LOG_ERROR, "error: ltrim %s %d %d, %s", key, start, end, c->errstr);
		if(reply) freeReplyObject(reply);
		return false;
	}
	freeReplyObject(reply);

	return true;
}
