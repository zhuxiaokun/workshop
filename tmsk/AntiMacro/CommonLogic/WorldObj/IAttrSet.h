#pragma once

#include <System/KType.h>
#include <System/KMacro.h>
#include <string.h>

class IAttrSet
{
public:
	virtual ~IAttrSet() { }
	virtual void clearAttrs()									{ return; }
	virtual bool hasAttr(int attrId) const						{ return false; }
	virtual int getAttrValue(int attrId, int defVal)			{ return defVal; }
	virtual void setAttrValue(int attrId, int val)				{ return; }
	virtual int setAttrParam(int attrId, int paramId, int val)	{ return 0; }
};

template <int startAttr,int endAttr> class KAttrSet_1
{
public:
	struct Attr_1 { int val; };

public:
	KAttrSet_1()
	{
		memset(&m_attrs, 0, sizeof(m_attrs));
	}
	~KAttrSet_1()
	{

	}

public:
	void clearAttrs()
	{
		memset(&m_attrs, 0, sizeof(m_attrs));
	}
	bool hasAttr(int attrId) const
	{
		return attrId >= startAttr && attrId < endAttr;
	}
	int getAttrValue(int attrId, int defVal)
	{
		if(!this->hasAttr(attrId)) return defVal;
		return m_attrs[attrId-startAttr].val;
	}
	void setAttrValue(int attrId, int val)
	{
		if(!this->hasAttr(attrId)) return;
		m_attrs[attrId-startAttr].val = val;
	}
	int setAttrParam(int attrId, int paramId, int val)
	{
		ASSERT(!this->hasAttr(attrId));
		return this->getAttrValue(attrId, 0);
	}

public:
	Attr_1 m_attrs[endAttr-startAttr];
};

template <int startAttr,int endAttr> class KAttrSet_2
{
public:
	struct Attr_2
	{
		int		val;						// 当前值
		int		baseVal;					// 基础值			paramId:1
		int		extraAddVal;				// 附加当前值		paramId:2
		short	basePercent;				// 附加基础百分比	paramId:3
		short	extraPercent;				// 附加当前百分比	paramId:4
		int		globalAddVal;				// 全局修正值		paramId:5
	};

public:
	KAttrSet_2()
	{
		memset(&m_attrs, 0, sizeof(m_attrs));
	}
	~KAttrSet_2()
	{

	}

public:
	void clearAttrs()
	{
		memset(&m_attrs, 0, sizeof(m_attrs));
	}
	bool hasAttr(int attrId) const
	{
		return attrId >= startAttr && attrId < endAttr;
	}
	int getAttrValue(int attrId, int defVal)
	{
		if(!this->hasAttr(attrId)) return defVal;
		return m_attrs[attrId-startAttr].val;
	}
	void setAttrValue(int attrId, int val)
	{
		if(!this->hasAttr(attrId)) return;
		m_attrs[attrId-startAttr].val = val;
	}
	int setAttrParam(int attrId, int paramId, int val)
	{
		if(!this->hasAttr(attrId)) return 0;
		Attr_2& attr = m_attrs[attrId-startAttr];
		switch(paramId)
		{
		case 1:
			attr.baseVal = val; break;
		case 2:
			attr.extraAddVal = val; break;
		case 3:
			attr.basePercent = val; break;
		case 4:
			attr.extraPercent = val; break;
		case 5:
			attr.globalAddVal = val; break;
		default: return 0;
		}
		return this->_calcAttrValue(attr);
	}

private:
	int _calcAttrValue(Attr_2& attr)
	{
		// TODO: using formula to calc attr value
		// ...
		return attr.val;
	}

public:
	Attr_2 m_attrs[endAttr-startAttr];
};
