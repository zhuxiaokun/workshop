#include "CppTable.h"

namespace LMREG
{
	// ----------------- variant -----------------------
	variant::variant():vt(vt_nil)
	{

	}

	variant::variant(const variant& o):vt(o.vt)
	{
		switch(vt)
		{
		case vt_nil:
			break;
		case vt_bool:
			this->b = o.b;
			break;
		case vt_int32:
			this->n = o.n;
			break;
		case vt_uint32:
			this->un = o.un;
			break;
		case vt_int64:
			this->ll = o.ll;
			break;
		case vt_uint64:
			this->ull = o.ull;
			break;
		case vt_float:
			this->f = o.f;
			break;
		case vt_double:
			this->d = o.d;
			break;
		case vt_pointer:
			this->p = o.p;
			break;
		case vt_string:
		case vt_d_string:
			{
				this->vt = vt_nil;
				const char* s = o; int l = o.length();
				this->setbinary(s, l);
			} break;
		case vt_table:
			{
				this->tab = new CppTable(*o.tab);
			}
			break;
		case vt_object:
			{
				memcpy(&this->obj, &o.obj, sizeof(o.obj));
			} break;
		}
	}

	variant::~variant()
	{
		this->clear();
	}

	bool variant::isnil() const
	{
		return this->vt == vt_nil;
	}

	bool variant::istable() const
	{
		return vt == vt_table;
	}

	bool variant::isstring() const
	{
		return vt == vt_string || vt == vt_d_string;
	}

	bool variant::isobject() const
	{
		return vt == vt_object;
	}

	variant& variant::setnil()
	{
		this->clear();
		return *this;
	}

	variant& variant::setstring(const char* val, int len)
	{
		return this->setbinary(val, len);
	}

	variant& variant::setbinary(const void* val, int len)
	{
		switch(vt)
		{
		case vt_nil:
			{
				if(len < stack_string_len)
				{
					this->vt = vt_string;
					this->str.first = len;
					memcpy(&this->str.second, val, len);
					this->str.second[len] = '\0';
				}
				else
				{
					this->vt = vt_d_string;
					this->dstr.first = len;
					this->dstr.second = (char*)malloc_k(len+1);
					memcpy(this->dstr.second, val, len);
					this->dstr.second[len] = '\0';
				}
			} break;
		case vt_string:
			{
				if(len < stack_string_len)
				{
					this->str.first = len;
					memcpy(&this->str.second, val, len);
					this->str.second[len] = '\0';
				}
				else
				{
					this->clear();
					this->setbinary(val, len);
				}
			} break;
		case vt_d_string:
			{
				if(len < stack_string_len)
				{
					this->clear();
					this->setbinary(val, len);
				}
				else
				{
					if(len <= this->dstr.first)
					{
						this->dstr.first = len;
						memcpy(this->dstr.second, val, len);
						this->dstr.second[len] = '\0';
					}
					else
					{
						this->clear();
						this->setbinary(val, len);
					}
				}
			} break;
		default:
			{
				this->clear();
				this->setbinary(val, len);
			} break;
		}
		return *this;
	}

	variant& variant::setobject(const char* typeName, void* val)
	{
		this->clear();
		this->vt = vt_object;
		sprintf_k((char*)&this->obj.first, sizeof(this->obj.first), typeName);
		this->obj.second = val;
		return *this;
	}

	variant& variant::operator = (const variant& o)
	{
		if(this == &o) return *this;

		switch(o.vt)
		{
		case vt_nil:
			{
				this->clear();
			} break;
		case vt_bool:
			{
				this->clear();
				vt = vt_bool;
				this->b = o.b;
			} break;
		case vt_int32:
			{
				this->clear();
				vt = vt_int32;
				this->n = o.n;
			} break;
		case vt_uint32:
			{
				this->clear();
				vt = vt_uint32;
				this->un = o.un;
			} break;
		case vt_int64:
			{
				this->clear();
				vt = vt_int64;
				this->ll = o.ll;
			} break;
		case vt_uint64:
			{
				this->clear();
				vt = vt_uint64;
				this->ull = o.ull;
			} break;
		case vt_float:
			{
				this->clear();
				vt = vt_float;
				this->f = o.f;
			} break;
		case vt_pointer:
			{
				this->clear();
				vt = vt_pointer;
				this->p = o.p;
			} break;
		case vt_double:
			{
				this->clear();
				vt = vt_double;
				this->d = o.d;
			} break;
		case vt_string:
			{
				this->setbinary(o.str.second, o.str.first);
			} break;
		case vt_d_string:
			{
				this->setbinary(o.dstr.second, o.dstr.first);
			} break;
		case vt_table:
			{
				this->clear();
				this->vt = vt_table;
				this->tab = new CppTable(*o.tab);
			} break;
		case vt_object:
			{
				this->clear();
				this->vt = vt_object;
				memcpy(&this->obj, &o.obj, sizeof(o.obj));
			} break;
		default:
			ASSERT(FALSE);
			break;
		}
		return *this;
	}

	variant::operator int () const
	{
		typedef int my_t;
		switch(vt)
		{
		case vt_bool:
			return this->b ? 1 : 0;
		case vt_int32:
			return (my_t)this->n;
		case vt_uint32:
			return (my_t)this->un;
		case vt_int64:
			return (my_t)this->ll;
		case vt_uint64:
			return (my_t)this->ull;
		case vt_float:
			return (my_t)this->f;
		case vt_double:
			return (my_t)this->d;
		}
		ASSERT(FALSE);
		return 0;
	}

	variant::operator unsigned int () const
	{
		typedef unsigned int my_t;
		switch(vt)
		{
		case vt_bool:
			return this->b ? 1 : 0;
		case vt_int32:
			return (my_t)this->n;
		case vt_uint32:
			return (my_t)this->un;
		case vt_int64:
			return (my_t)this->ll;
		case vt_uint64:
			return (my_t)this->ull;
		case vt_float:
			return (my_t)this->f;
		case vt_double:
			return (my_t)this->d;
		}
		ASSERT(FALSE);
		return 0;
	}

	variant::operator long () const
	{
		typedef long my_t;
		switch(vt)
		{
		case vt_bool:
			return this->b ? 1 : 0;
		case vt_int32:
			return (my_t)this->n;
		case vt_uint32:
			return (my_t)this->un;
		case vt_int64:
			return (my_t)this->ll;
		case vt_uint64:
			return (my_t)this->ull;
		case vt_float:
			return (my_t)this->f;
		case vt_double:
			return (my_t)this->d;
		}
		ASSERT(FALSE);
		return 0;
	}

	variant::operator unsigned long () const
	{
		typedef unsigned long my_t;
		switch(vt)
		{
		case vt_bool:
			return this->b ? 1 : 0;
		case vt_int32:
			return (my_t)this->n;
		case vt_uint32:
			return (my_t)this->un;
		case vt_int64:
			return (my_t)this->ll;
		case vt_uint64:
			return (my_t)this->ull;
		case vt_float:
			return (my_t)this->f;
		case vt_double:
			return (my_t)this->d;
		}
		ASSERT(FALSE);
		return 0;
	}

	variant::operator double () const
	{
		typedef double my_t;
		switch(vt)
		{
		case vt_bool:
			return this->b ? 1 : 0;
		case vt_int32:
			return (my_t)this->n;
		case vt_uint32:
			return (my_t)this->un;
		case vt_int64:
			return (my_t)this->ll;
		case vt_uint64:
			return (my_t)this->ull;
		case vt_float:
			return (my_t)this->f;
		case vt_double:
			return (my_t)this->d;
		}
		ASSERT(FALSE);
		return 0;
	}

	variant::operator long long () const
	{
		typedef long long my_t;
		switch(vt)
		{
		case vt_bool:
			return this->b ? 1 : 0;
		case vt_int32:
			return (my_t)this->n;
		case vt_uint32:
			return (my_t)this->un;
		case vt_int64:
			return (my_t)this->ll;
		case vt_uint64:
			return (my_t)this->ull;
		case vt_float:
			return (my_t)this->f;
		case vt_double:
			return *(my_t*)&this->d;
		}
		ASSERT(FALSE);
		return 0;
	}

	variant::operator unsigned long long () const
	{
		typedef unsigned long long my_t;
		switch(vt)
		{
		case vt_bool:
			return this->b ? 1 : 0;
		case vt_int32:
			return (my_t)this->n;
		case vt_uint32:
			return (my_t)this->un;
		case vt_int64:
			return (my_t)this->ll;
		case vt_uint64:
			return (my_t)this->ull;
		case vt_float:
			return (my_t)this->f;
		case vt_double:
			return *(my_t*)&this->d;
		}
		ASSERT(FALSE);
		return 0;
	}

	variant::operator const CppTable& () const
	{
		ASSERT(vt == vt_table && this->tab);
		return *this->tab;
	}

	variant::operator const char* () const
	{
		switch(vt)
		{
		case vt_string: return &this->str.second[0];
		case vt_d_string: return this->dstr.second;
		}
		ASSERT(FALSE);
		return NULL;
	}

	variant::operator void* ()
	{
		ASSERT(vt == vt_pointer);
		return this->p;
	}

	variant::operator const void* () const
	{
		ASSERT(vt == vt_pointer);
		return this->p;
	}

	int variant::length() const
	{
		switch(vt)
		{
		case vt_string: return this->str.first;
		case vt_d_string: return this->dstr.first;
		}
		ASSERT(FALSE);
		return 0;
	}

	variant& variant::operator = (bool val)
	{
		this->clear();
		this->vt = vt_bool;
		this->b = val;
		return *this;
	}

	variant& variant::operator = (char val)
	{
		return this->setbinary(&val, 1);
	}

	variant& variant::operator = (unsigned char val)
	{
		return this->setUint32(val);
	}

	variant& variant::operator = (short val)
	{
		return this->setInt32(val);
	}

	variant& variant::operator = (unsigned short val)
	{
		return this->setUint32(val);
	}

	variant& variant::operator = (int val)
	{
		return this->setInt32(val);
	}

	variant& variant::operator = (unsigned int val)
	{
		return this->setUint32(val);
	}

	variant& variant::operator = (long val)
	{
		if(sizeof(long) == sizeof(int))
		{
			return this->setInt32(val);
		}
		else
		{
			return this->operator=((INT64)val);
		}
	}

	variant& variant::operator = (unsigned long val)
	{
		if(sizeof(long) == sizeof(int))
		{
			return this->setUint32(val);
		}
		else
		{
			return this->operator=((UINT64)val);
		}
	}

	variant& variant::operator = (long long val)
	{
		this->clear();
		vt = vt_int64;
		this->ll = val;
		return *this;
	}

	variant& variant::operator = (unsigned long long val)
	{
		this->clear();
		vt = vt_uint64;
		this->ull = val;
		return *this;
	}

	variant& variant::operator = (float val)
	{
		this->clear();
		vt = vt_float;
		this->f = val;
		return *this;
	}

	variant& variant::operator = (double val)
	{
		this->clear();
		this->vt = vt_double;
		this->d = val;
		return *this;
	}

	variant& variant::operator = (char* val)
	{
		return this->setbinary(val, (int)strlen(val));
	}

	variant& variant::operator = (const char* val)
	{
		return this->setbinary(val, (int)strlen(val));
	}

	variant& variant::operator = (const CppTable& val)
	{
		this->clear();
		this->vt = vt_table;
		this->tab = new CppTable(val);
		return *this;
	}

	variant& variant::operator = (void* val)
	{
		this->clear();
		this->vt = vt_pointer;
		this->p = val;
		return *this;
	}

	variant& variant::operator = (const void* val)
	{
		this->clear();
		this->vt = vt_pointer;
		this->p = (void*)val;
		return *this;
	}
	// 先生成一个CppTable,然后返回cppTable[name]
	variant& variant::operator [] (const char* name)
	{
		if(this->vt != vt_table)
		{
			this->clear();
			this->vt = vt_table;
			this->tab = new CppTable();
		}
		return this->tab->operator[](name);
	}

	void variant::clear()
	{
		switch(vt)
		{
		case vt_d_string:
			{
				free_k(this->dstr.second);
			} break;
		case vt_table:
			{
				delete this->tab;
			} break;
		}
		vt = vt_nil;
	}
	variant& variant::setInt32(int val)
	{
		this->clear();
		this->vt = vt_int32;
		this->n = val;
		return *this;
	}

	variant& variant::setUint32(unsigned int val)
	{
		this->clear();
		this->vt = vt_uint32;
		this->un = val;
		return *this;
	}

	// ---- _CppTable_base -----

	_CppTable_base::_CppTable_base():m_size(0)
	{

	}

	_CppTable_base::_CppTable_base(const _CppTable_base& o):m_size(0)
	{
		this->operator = (o);
	}

	_CppTable_base::~_CppTable_base()
	{
		this->clear();
	}

	_CppTable_base& _CppTable_base::operator = (const _CppTable_base& o)
	{
		if(this == &o)
			return *this;
		
		this->clear();

		int c = max_base_value < o.m_size ? max_base_value : o.m_size;
		for(int i=0; i<c; i++)
		{
			m_baseVals[i] = o.m_baseVals[i];
			m_size++;
		}

		c = o.m_extraVals.size();
		for(int i=0; i<c; i++)
		{
			NameValue* nv = new NameValue(*o.m_extraVals[i]);
			m_extraVals.push_back(nv);
			m_size++;
		}

		return *this;
	}

	int _CppTable_base::size() const
	{
		return m_size;
	}

	void _CppTable_base::clear()
	{
		int c = max_base_value < m_size ? max_base_value : m_size;
		for(int i=0; i<c; i++)
		{
			m_baseVals[i].second.clear();
		}

		c = m_extraVals.size();
		for(int i=0; i<c; i++)
		{
			delete m_extraVals[i];
		}
		
		m_extraVals.clear();
		m_size = 0;
	}

	const char* _CppTable_base::getKey(int idx) const
	{
		ASSERT(idx >= 0 && idx < m_size);
		if(idx < max_base_value)
			return m_baseVals[idx].first.c_str();
		idx -= max_base_value;
		return m_extraVals[idx]->first.c_str();
	}

	const variant& _CppTable_base::getVal(int idx) const
	{
		ASSERT(idx >= 0 && idx < m_size);
		if(idx < max_base_value)
			return m_baseVals[idx].second;
		idx -= max_base_value;
		return m_extraVals[idx]->second;
	}

	variant& _CppTable_base::operator [] (int idx)
	{
		ASSERT(idx >= 0 && idx < m_size);
		
		if(idx < max_base_value)
			return m_baseVals[idx].second;

		idx -= max_base_value;
		return m_extraVals[idx]->second;
	}

	const variant& _CppTable_base::operator [] (int idx) const
	{
		ASSERT(idx >= 0 && idx < m_size);

		if(idx < max_base_value)
			return m_baseVals[idx].second;

		idx -= max_base_value;
		return m_extraVals[idx]->second;
	}

	variant& _CppTable_base::operator [] (const char* name)
	{
		int c = max_base_value < m_size ? max_base_value : m_size;
		for(int i=0; i<c; i++)
		{
			NameValue& nv = m_baseVals[i];
			if(!nv.first.compare(name))
				return nv.second;
		}

		c = m_extraVals.size();
		for(int i=0; i<c; i++)
		{
			NameValue* nv = m_extraVals[i];
			if(!nv->first.compare(name))
				return nv->second;
		}

		return this->_add(name);
	}

	const variant& _CppTable_base::operator [] (const char* name) const
	{
		const NameValue* nv = this->_find(name);
		ASSERT(nv);
		return nv->second;
	}

	variant& _CppTable_base::setbinary(const char* name, const void* val, int len)
	{
		variant& v = this->operator[](name);
		v.setbinary(val, len);
		return v;
	}

	variant& _CppTable_base::setobject(const char* name, void* obj, const char* typeName)
	{
		variant& v = this->operator[](name);
		return v.setobject(typeName, obj);
	}

	variant& _CppTable_base::_add(const char* name)
	{
		if(m_size < max_base_value)
		{
			int pos = m_size++;
			NameValue& nv = m_baseVals[pos];
			nv.first = name;
			nv.second.clear();
			return nv.second;
		}
		else
		{
			m_size++;
			int pos = m_extraVals.size();
			NameValue* pval = new NameValue(Name(name),variant());
			m_extraVals.push_back(pval);
			return pval->second;
		}
	}

	_CppTable_base::NameValue* _CppTable_base::_find(const char* name)
	{
		int c = max_base_value < m_size ? max_base_value : m_size;
		for(int i=0; i<c; i++)
		{
			NameValue& nv = m_baseVals[i];
			if(!nv.first.compare(name))
				return &nv;
		}

		c = m_extraVals.size();
		for(int i=0; i<c; i++)
		{
			NameValue* nv = m_extraVals[i];
			if(!nv->first.compare(name))
				return nv;
		}

		return NULL;
	}

	const _CppTable_base::NameValue* _CppTable_base::_find(const char* name) const
	{
		int c = max_base_value < m_size ? max_base_value : m_size;
		for(int i=0; i<c; i++)
		{
			const NameValue& nv = m_baseVals[i];
			if(!nv.first.compare(name))
				return &nv;
		}

		c = m_extraVals.size();
		for(int i=0; i<c; i++)
		{
			const NameValue* nv = m_extraVals[i];
			if(!nv->first.compare(name))
				return nv;
		}

		return NULL;
	}

	const _CppTable_base::NameValue& _CppTable_base::at(int idx) const
	{
		ASSERT(idx >= 0 && idx < m_size);
		if(idx < max_base_value) return m_baseVals[idx];
		idx -= max_base_value;
		return *m_extraVals[idx];
	}

	/// CppTable
	CppTable::CppTable()
	{
        
    }

    CppTable::CppTable(const CppTable& o):base_type(o)
	{

    }
	
	CppTable::~CppTable()
	{
		this->_baseType().~base_type();
	}
	
	CppTable& CppTable::operator = (const CppTable& o)
	{
		if(this == &o) return *this;
		this->_baseType() = o._baseType();
		return *this;
	}
	
	int CppTable::size() const
	{
		return this->_baseType().size();
	}
	
	void CppTable::clear()
	{
		this->_baseType().clear();
	}
	
	const char* CppTable::getKey(int idx) const
	{
		return this->_baseType().getKey(idx);
	}
	
	const variant& CppTable::getVal(int idx) const
	{
		return this->_baseType().getVal(idx);
	}

	const CppTable::NameValue& CppTable::at(int idx) const
	{
		return this->_baseType().at(idx);
	}
	
	variant& CppTable::operator [] (int idx)
	{
		return this->_baseType()[idx];
	}
	
	const variant& CppTable::operator [] (int idx) const
	{
		return this->_baseType()[idx];
	}
	
	variant& CppTable::operator [] (const char* name)
	{
		return this->_baseType()[name];
	}
	
	const variant& CppTable::operator [] (const char* name) const
	{
		return this->_baseType()[name];
	}
	
	variant& CppTable::setbinary(const char* name, const void* val, int len)
	{
		return this->_baseType().setbinary(name, val, len);
	}
	
	variant& CppTable::setobject(const char* name, void* obj, const char* typeName)
	{
		return this->_baseType().setobject(name, obj, typeName);
	}
}