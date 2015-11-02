#ifndef _XML_DOM_H_
#define _XML_DOM_H_

#include "../KType.h"
#include "xmlstream.h"
#include "expat.h"
#include "../Collections/KString.h"
#include "../Collections/KVector.h"
#include "../Collections/KStack.h"
#include "../Misc/StrUtil.h"

#ifdef ToString
#undef ToString
#endif

typedef ::System::Collections::KDString<32> KXmlString;

class StringValue
{
public:
	StringValue();
	StringValue(const char* val);
	StringValue(const KXmlString& val);
	StringValue(const StringValue& o);
	~StringValue();

public:
	StringValue& operator=(const StringValue& o);
	StringValue& operator=(const char* val);
	StringValue& operator=(const KXmlString& val);
	BOOL operator==(const StringValue& o) const;
	BOOL operator==(const KXmlString& val) const;
	BOOL operator==(const char* val) const;
	BOOL IgnoreCaseEqual(const StringValue& o) const;
	BOOL IgnoreCaseEqual(const KXmlString& val) const;
	BOOL IgnoreCaseEqual(const char* val) const;

public:
	int Integer() const;
	INT64 Int64() const;
	float Float() const;
	double Double() const;
	BOOL Bool() const;
	const char* c_str() const;
	const char* cstr() const;
	const KXmlString& str() const;
	int Size() const;

public:
	KXmlString m_val;
};

class XMLTextNode;
class XMLElementNode;
class XMLDomNode
{
public:
	XMLDomNode()
	:m_pParentNode(NULL)
	{
	}

	virtual ~XMLDomNode() {}
	virtual BOOL IsTextNode() const = 0;
	virtual void Clear() = 0;
	virtual KXmlString ToString(int level=0) const = 0;
	virtual BOOL WriteTo(StreamInterface& s, int level=0) const = 0;

	XMLTextNode* ToTextNode()
	{
		if(this->IsTextNode()) return (XMLTextNode*)this;
		return NULL;
	}

	XMLElementNode* ToElementNode()
	{
		if(!this->IsTextNode()) return (XMLElementNode*)this;
		return NULL;
	}

	const XMLTextNode* ToTextNode() const
	{
		if(this->IsTextNode()) return (XMLTextNode*)this;
		return NULL;
	}

	const XMLElementNode* ToElementNode() const
	{
		if(!this->IsTextNode()) return (XMLElementNode*)this;
		return NULL;
	}

	XMLDomNode* GetParentNode()
	{
		return m_pParentNode;
	}
	
	const XMLDomNode* GetParentNode() const
	{
		return m_pParentNode;
	}
public:
	XMLDomNode* m_pParentNode;
};

class XMLAttribute
{
public:
	XMLAttribute();
	XMLAttribute(const char* name, const char* val);
	~XMLAttribute();

public:
	::System::Collections::KString<64> m_name;
	StringValue m_value;
};

class XMLAttributeColl
{
public:
	XMLAttributeColl();
	~XMLAttributeColl();

public:
	int Size() const;
	const XMLAttribute* GetAttribute(int idx) const;
	const XMLAttribute* GetAttribute(const char* name) const;
	XMLAttributeColl& AddAttribute(const char* name, const char* val);

public:
	::System::Collections::KVector<XMLAttribute> m_attrs;
};

class XMLNodeColl
{
public:
	XMLNodeColl();
	XMLNodeColl(const XMLNodeColl& o);
	~XMLNodeColl();
	XMLNodeColl& operator=(const XMLNodeColl& o);

public:
	int Size() const;
	XMLDomNode* GetNode(int idx);
	const XMLDomNode* GetNode(int idx) const;
	XMLNodeColl& AddNode(XMLDomNode* pNode);
	XMLDomNode* RemoveNode(int idx);

public:
	::System::Collections::KVector<XMLDomNode*> m_nodePtrs;
};

class XMLTextNode : public XMLDomNode
{
public:
	XMLTextNode();
	XMLTextNode(const StringValue& text);
	~XMLTextNode();
	BOOL IsTextNode() const { return TRUE; }
	void Clear() { m_text.m_val.clear(); }
	KXmlString ToString(int level=0) const
	{
		KXmlString s;
		for(int i=0; i<level; i++) s.append("  ");
		return s.append(m_text.str());
	}
	BOOL WriteTo(StreamInterface& s, int level=0) const
	{
		for(int i=0; i<level; i++) s.WriteData("\t", 1);
		const static char CDATA_BEGIN[] = "<![CDATA[";
		const static char CDATA_END[] = "]]>";
		if (m_text.Size() > 0)
		{
			if (!s.WriteData(CDATA_BEGIN, sizeof(CDATA_BEGIN) - 1)) return FALSE;
			if (!s.WriteData(m_text.c_str(), m_text.Size())) return FALSE;
			if (!s.WriteData(CDATA_END, sizeof(CDATA_END) - 1)) return FALSE;
		}
		return TRUE;
	}

public:
	XMLTextNode& AddText(const char* p, int len);
	const StringValue& GetText() const;

public:
	StringValue m_text;
};

// 只包含文字的XML Element，只有一个子节点XMLTextNode
class XMLElementNode : public XMLDomNode
{
public:
	XMLElementNode();
	XMLElementNode(const char* tagName);
	~XMLElementNode();
	
	BOOL IsTextNode() const { return FALSE; }
	void Clear();
	KXmlString ToString(int level=0) const;
	BOOL WriteTo(StreamInterface& s, int level=0) const;

public:
	XMLElementNode& SetTagName(const char* tagName);
	XMLElementNode& AddAttribute(const char* name, const char* val);
	XMLElementNode& AddChild(XMLDomNode* pNode);

	XMLElementNode& SetAttribute(const char* name, int val);
	XMLElementNode& SetAttribute(const char* name, const char* val);
	XMLElementNode& SetAttribute(const char* name, INT64 val);
	XMLElementNode& SetAttribute(const char* name, float val);
	
	BOOL RemoveChild(int index);
	BOOL RemoveChildren(const char* tagName);
	BOOL RemoveChild(XMLElementNode* pChild);
	void RemoveTextChild();

public:
	XMLTextNode* GetTextChild();
	const StringValue* GetText() const;
	const XMLAttribute* GetAttribute(const char* name) const;
	const XMLNodeColl& GetChildren() const;
	XMLElementNode* GetChild(int childIdx);
	XMLElementNode* GetChild(const char* tagName);
	XMLNodeColl GetChildren(const char* tagName);

	// path is TestData/AddArticle/ArticleID
	XMLElementNode* Query(const char* path);
	XMLNodeColl QueryChildren(const char* path);
	XMLNodeColl QueryChildren2(const char* path);

	// path such as TestData/AddArticle/ArticleID.Value
	const StringValue* QueryAtt(const char* path);

public:
	::System::Collections::KString<64> m_tagName;
	XMLAttributeColl m_attrs;
	XMLNodeColl m_children;
};

class XMLDomParser
{
public:
	XMLDomParser();
	~XMLDomParser();

public:
	BOOL Parse(StreamInterface& s, XMLElementNode& root);
	const char* GetErrorMsg() const;

public:
	// encoding:
	// ISO-8859-1 US-ASCII UTF-8 UTF-16 UTF-16BE UTF-16LE GB2312
	BOOL Initialize(const char* encoding=NULL);
	void Finalize();
	void Reset();
	
	const char* GetEncoding();


private:
	BOOL IsSpace(char ch) const;
	const char* Trim(const char* s, int len, int& nRetLen);

public:
	BOOL OnStartElement(const XML_Char *name, const XML_Char **atts);
	BOOL OnEndElement(const XML_Char *name);
	BOOL OnCharacterData(const XML_Char *s, int len);

public:
	static void StartElementHandler(void *userData, const XML_Char *name, const XML_Char **atts);
	static void EndElementHandler(void *userData, const XML_Char *name);
	static void CharacterDataHandler(void *userData, const XML_Char *s, int len);

private:
	::System::Collections::KString<64> m_encoding;
	XML_Parser m_parser;
	XMLElementNode* m_pRootNode;
	::System::Collections::KStack<XMLElementNode*> m_stack;
	KXmlString m_errMsg;
};

#endif