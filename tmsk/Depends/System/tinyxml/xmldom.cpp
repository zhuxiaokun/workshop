#include "../Log/log.h"
#include "xmldom.h"
#include "../Misc/StrUtil.h"
#include "../KPlatform.h"
#include <locale.h>

StringValue::StringValue()
{
}

StringValue::StringValue(const char* val)
{
	if(val)
	{	
		m_val = val;
	}
}

StringValue::StringValue(const KXmlString& val):m_val(val)
{
}

StringValue::StringValue(const StringValue& o):m_val(o.m_val)
{
}

StringValue::~StringValue()
{
}

StringValue& StringValue::operator=(const StringValue& o)
{
	if(this == &o) return *this;
	m_val = o.m_val;
	return *this;
}

StringValue& StringValue::operator=(const char* val)
{
	if(!val) m_val.clear();
	else m_val = val;
	return *this;
}

StringValue& StringValue::operator=(const KXmlString& val)
{
	m_val = val;
	return *this;
}

BOOL StringValue::IgnoreCaseEqual(const StringValue& o) const
{
	return this->IgnoreCaseEqual(o.m_val.c_str());
}

BOOL StringValue::IgnoreCaseEqual(const KXmlString& val) const
{
	return this->IgnoreCaseEqual(val.c_str());
}

BOOL StringValue::IgnoreCaseEqual(const char* val) const
{
	const char* s = m_val.c_str();
	return _stricmp(s, val) == 0;
}

int StringValue::Integer() const
{
	if(!isnumber3(m_val.c_str())) return 0;
	return str2int(m_val.c_str());
}

INT64 StringValue::Int64() const
{
	if(!isnumber3(m_val.c_str())) return 0;
	return str2i64(m_val.c_str());
}

float StringValue::Float() const
{
	if(!isfloat(m_val.c_str())) return 0.0f;
	return (float)atof(m_val.c_str());
}

double StringValue::Double() const
{
	if(!isfloat(m_val.c_str())) return 0;
	return atof(m_val.c_str());
}

BOOL StringValue::Bool() const
{
	return strcmp(m_val.c_str(), "True") == 0
		|| strcmp(m_val.c_str(), "true") == 0 
		|| strcmp(m_val.c_str(), "yes") == 0
		|| strcmp(m_val.c_str(), "1") == 0;
}

const char* StringValue::c_str() const
{
	return m_val.c_str();
}

const char* StringValue::cstr() const
{
	return m_val.c_str();
}

const KXmlString& StringValue::str() const
{
	return m_val;
}

int StringValue::Size() const
{
	return (int)m_val.size();
}

BOOL StringValue::operator==(const StringValue& o) const
{
	return m_val == o.m_val;
}

BOOL StringValue::operator==(const KXmlString& val) const
{
	return m_val == val;
}

BOOL StringValue::operator==(const char* val) const
{
	return strcmp(m_val.c_str(), val) == 0;
}

XMLAttribute::XMLAttribute()
{
}

XMLAttribute::XMLAttribute(const char* name, const char* val):m_name(name),m_value(val)
{
}

XMLAttribute::~XMLAttribute()
{
}

XMLAttributeColl::XMLAttributeColl()
{
}

XMLAttributeColl::~XMLAttributeColl()
{
}

int XMLAttributeColl::Size() const
{
	return m_attrs.size();
}

const XMLAttribute* XMLAttributeColl::GetAttribute(int idx) const
{
	int c = this->Size();
	if(idx < 0 || idx >= c) return NULL;
	return &m_attrs[idx];
}

const XMLAttribute* XMLAttributeColl::GetAttribute(const char* name) const
{
	int c = m_attrs.size();
	for(int i=0; i<c; i++)
	{
		const XMLAttribute& attr = m_attrs[i];
		if(attr.m_name.icompare(name) == 0) return &attr;
	}
	return NULL;
}

XMLAttributeColl& XMLAttributeColl::AddAttribute(const char* name, const char* val)
{
	int c = m_attrs.size();
	for(int i=0; i<c; i++)
	{
		XMLAttribute& attr = m_attrs[i];
		if(attr.m_name.icompare(name) == 0)
		{
			attr.m_value = val;
			return *this;
		}
	}
	m_attrs.push_back(XMLAttribute(name, val));
	return *this;
}

XMLNodeColl::XMLNodeColl()
{
}

XMLNodeColl::XMLNodeColl(const XMLNodeColl& o):m_nodePtrs(o.m_nodePtrs)
{
}

XMLNodeColl::~XMLNodeColl()
{
}

XMLNodeColl& XMLNodeColl::operator=(const XMLNodeColl& o)
{
	if(this == &o) return *this;
	m_nodePtrs = o.m_nodePtrs;
	return *this;
}

int XMLNodeColl::Size() const
{
	return m_nodePtrs.size();
}

XMLDomNode* XMLNodeColl::GetNode(int idx)
{
	int c = this->Size();
	if(idx < 0 || idx >= c) return NULL;
	return m_nodePtrs[idx];
}

const XMLDomNode* XMLNodeColl::GetNode(int idx) const
{
	int c = this->Size();
	if(idx < 0 || idx >= c) return NULL;
	return m_nodePtrs[idx];
}

XMLNodeColl& XMLNodeColl::AddNode(XMLDomNode* pNode)
{
	int c = this->Size();
	for(int i=0; i<c; i++)
	{
		if(this->GetNode(i) == pNode)
			return *this;
	}
	m_nodePtrs.push_back(pNode);
	return *this;
}

XMLDomNode* XMLNodeColl::RemoveNode(int idx)
{
	int c = m_nodePtrs.size();
	if(idx < 0 || idx >= c) return NULL;
	XMLDomNode* pNode = m_nodePtrs[idx];
	m_nodePtrs.erase(idx);
	return pNode;
}

XMLTextNode::XMLTextNode()
{
}

XMLTextNode::XMLTextNode(const StringValue& text):m_text(text)
{
}

XMLTextNode::~XMLTextNode()
{
}

XMLTextNode& XMLTextNode::AddText(const char* p, int len)
{
	m_text.m_val.append(p, len);
	return *this;
}

const StringValue& XMLTextNode::GetText() const
{
	return m_text;
}

XMLElementNode::XMLElementNode()
{
}

XMLElementNode::XMLElementNode(const char* tagName):m_tagName(tagName)
{
}

XMLElementNode::~XMLElementNode()
{
	this->Clear();
}

void XMLElementNode::Clear()
{
	m_tagName.clear();
	m_attrs.m_attrs.clear();
	int c = m_children.Size();
	for(int i=0; i<c; i++)
	{
		XMLDomNode* pNode = m_children.GetNode(i);
		delete pNode;
	}
	m_children.m_nodePtrs.clear();
}

KXmlString XMLElementNode::ToString(int level) const
{
	KXmlString s;
	
	for(int i=0; i<level; i++) s.append("  ");

	s.append(1,'<').append(m_tagName.c_str());
	if(m_attrs.Size()) s.append(1, ' ');
	for(int i=0; i<m_attrs.Size(); i++)
	{
		const XMLAttribute* pattr = m_attrs.GetAttribute(i);
		if(i) s.append(1, ' ');
		s.append(pattr->m_name.c_str()).append(1,'=').append(1,'\"').append(pattr->m_value.c_str()).append(1,'\"');
	}
	if(!m_children.Size())
	{
		s.append("/>");
		return s;
	}

	s.append(1,'>');
	if(m_children.Size() == 1 && m_children.GetNode(0)->IsTextNode())
	{
		s.append(this->GetText()->c_str());
		s.append("</").append(m_tagName.c_str()).append(1,'>');
		return s;
	}
	
	s.append(1,'\n');
	for(int i=0; i<m_children.Size(); i++)
	{
		const XMLDomNode* pNode = m_children.GetNode(i);
		KXmlString s2 = pNode->ToString(level+1);
		s.append(s2);
		s.append(1,'\n');
	}
	for(int i=0; i<level; i++) s.append("  ");
	s.append("</").append(m_tagName.c_str()).append(1,'>');

	return s;
}

BOOL XMLElementNode::WriteTo(StreamInterface& s, int level) const
{
	for(int i=0; i<level; i++) s.WriteData("\t", 1);

	s.WriteData("<", 1);
	s.WriteData(m_tagName.c_str(), m_tagName.size());
	
	if(m_attrs.Size()) s.WriteData(" ", 1);
	for(int i=0; i<m_attrs.Size(); i++)
	{
		const XMLAttribute* pattr = m_attrs.GetAttribute(i);
		if(i) s.WriteData(" ", 1);
		s.WriteData(pattr->m_name.c_str(), pattr->m_name.size());
		s.WriteData("=", 1);
		s.WriteData("\"", 1);
		s.WriteData(pattr->m_value.c_str(), pattr->m_value.Size());
		s.WriteData("\"", 1);
	}
	if(!m_children.Size())
	{
		s.WriteData("/>", 2);
		return TRUE;
	}

	s.WriteData(">", 1);
	if(m_children.Size() == 1 && m_children.GetNode(0)->IsTextNode())
	{
		m_children.GetNode(0)->WriteTo(s);
// 		const StringValue* pVal = this->GetText();
// 		s.WriteData(pVal->c_str(), pVal->Size());
		s.WriteData("</", 2);
		s.WriteData(m_tagName.c_str(), m_tagName.size());
		s.WriteData(">", 1);
		return TRUE;
	}

	s.WriteData("\n", 1);
	for(int i=0; i<m_children.Size(); i++)
	{
		const XMLDomNode* pNode = m_children.GetNode(i);
		pNode->WriteTo(s, level+1);
		s.WriteData("\n", 1);
	}
	
	for(int i=0; i<level; i++) s.WriteData("\t", 1);

	s.WriteData("</", 2);
	s.WriteData(m_tagName.c_str(), m_tagName.size());
	s.WriteData(">", 1);

	return TRUE;
}

XMLElementNode& XMLElementNode::SetTagName(const char* tagName)
{
	m_tagName = tagName;
	return *this;
}

XMLElementNode& XMLElementNode::AddAttribute(const char* name, const char* val)
{
	IStringTranslator* pTranslator = IStringTranslator::GetGlobalTranslator();
	if(pTranslator)
	{
		const char* s = pTranslator->Translate(val);
		if(s && s != val)
		{
			val = s;
		}
	}

	m_attrs.AddAttribute(name, val);
	return *this;
}

XMLElementNode& XMLElementNode::AddChild(XMLDomNode* pNode)
{
	IStringTranslator* pTranslator = IStringTranslator::GetGlobalTranslator();
	if(pTranslator)
	{
		XMLTextNode* textNode = pNode->ToTextNode();
		if(textNode)
		{
			const char* src = textNode->m_text.c_str();
			const char* s = pTranslator->Translate(src);
			if(s && s != src)
			{
				textNode->m_text = s;
			}
		}
	}

	pNode->m_pParentNode = this;
	m_children.AddNode(pNode);

	return *this;
}

XMLElementNode& XMLElementNode::SetAttribute(const char* name, int val)
{
	char cTmp[64];
	sprintf_k(cTmp, sizeof(cTmp), "%d", val);
	return this->SetAttribute(name, cTmp);
}

XMLElementNode& XMLElementNode::SetAttribute(const char* name, const char* val)
{
	return this->AddAttribute(name, val);
}

XMLElementNode& XMLElementNode::SetAttribute(const char* name, INT64 val)
{
	char cTmp[64];
	sprintf_k(cTmp, sizeof(cTmp), "%lld", val);
	return this->SetAttribute(name, cTmp);
}

XMLElementNode& XMLElementNode::SetAttribute(const char* name, float val)
{
	char cTmp[64];
	sprintf_k(cTmp, sizeof(cTmp), "%.2f", val);
	return this->SetAttribute(name, cTmp);
}

BOOL XMLElementNode::RemoveChild(int index)
{
	int c = m_children.Size();
	if(index < 0 || index >= c) return FALSE;
	XMLDomNode* pNode = m_children.RemoveNode(index);
	if(pNode) delete pNode;
	return pNode != NULL;
}

BOOL XMLElementNode::RemoveChildren(const char* tagName)
{
	BOOL found = FALSE;
	int c = m_children.Size();
	for(int i=c-1; i>=0; i--)
	{
		XMLElementNode* pNode = this->GetChild(i);
		if(pNode->m_tagName.icompare(tagName) == 0)
		{
			this->RemoveChild(i);
			found = TRUE;
		}
	}
	return found;
}

BOOL XMLElementNode::RemoveChild(XMLElementNode* pChild)
{
	int c = m_children.Size();
	for(int i=c-1; i>=0; i--)
	{
		XMLElementNode* pNode = this->GetChild(i);
		if((void*)pNode == (void*)pChild)
		{
			this->RemoveChild(i);
			return TRUE;
		}
	}
	return FALSE;
}

XMLTextNode* XMLElementNode::GetTextChild()
{
	int c = m_children.Size();
	for(int i=0; i<c; i++)
	{
		XMLDomNode* pNode = m_children.GetNode(i);
		if(pNode->IsTextNode())
		{
			return pNode->ToTextNode();
		}
	}
	return NULL;
}

const StringValue* XMLElementNode::GetText() const
{
	int c = m_children.Size();
	for(int i=0; i<c; i++)
	{
		const XMLDomNode* pNode = m_children.GetNode(i);
		if(pNode->IsTextNode())
		{
			return &pNode->ToTextNode()->GetText();
		}
	}
	return NULL;
}

const XMLAttribute* XMLElementNode::GetAttribute(const char* name) const
{
	return m_attrs.GetAttribute(name);
}

const XMLNodeColl& XMLElementNode::GetChildren() const
{
	return m_children;
}

XMLElementNode* XMLElementNode::GetChild(int childIdx)
{
	XMLDomNode* pNode = m_children.GetNode(childIdx);
	if(!pNode) return NULL;
	return pNode->ToElementNode();
}

XMLElementNode* XMLElementNode::GetChild(const char* tagName)
{
	int c = m_children.Size();
	for(int i=0; i<c; i++)
	{
		XMLDomNode* pNode = m_children.GetNode(i);
		if(pNode->IsTextNode()) continue;
		XMLElementNode* pElemNode = pNode->ToElementNode();
		if(pElemNode->m_tagName.icompare(tagName) == 0)
			return pElemNode;
	}
	return NULL;
}

XMLNodeColl XMLElementNode::GetChildren(const char* tagName)
{
	XMLNodeColl coll;
	int c = m_children.Size();
	for(int i=0; i<c; i++)
	{
		XMLDomNode* pNode = m_children.GetNode(i);
		if(pNode->IsTextNode()) continue;
		XMLElementNode* pElemNode = pNode->ToElementNode();
		if(pElemNode->m_tagName.icompare(tagName) == 0)
		{
			coll.AddNode(pNode);
		}
	}
	return coll;
}

XMLElementNode* XMLElementNode::Query(const char* path)
{
	KXmlString tagName;

	int beg = 0;
	int nameLen;
	int len = (int)strlen(path);

	XMLElementNode* pRoot = this;
	XMLElementNode* pElem = pRoot;

	while(beg < len)
	{
		const char* pSep = strchr(path+beg, '/');
		if(pSep)
		{
			nameLen = int(pSep - path) - beg;
			tagName.assign(path+beg, nameLen);
			pElem = pElem->GetChild(tagName.c_str());
			if(!pElem) return NULL;
		}
		else
		{
			nameLen = len - beg;
			tagName.assign(path+beg, nameLen);
			pElem = pElem->GetChild(tagName.c_str());
			return pElem;
		}
		beg += nameLen + 1;
	}
	return NULL;
}

XMLNodeColl XMLElementNode::QueryChildren(const char* path)
{
	KXmlString tagName;

	int beg = 0;
	int nameLen;
	int len = (int)strlen(path);

	XMLElementNode* pRoot = this;
	XMLElementNode* pElem = pRoot;

	while(beg < len)
	{
		const char* pSep = strchr(path+beg, '/');
		if(pSep)
		{
			nameLen = int(pSep - path) - beg;
			tagName.assign(path+beg, nameLen);
			pElem = pElem->GetChild(tagName.c_str());
			if(!pElem) return XMLNodeColl();
		}
		else
		{
			nameLen = len - beg;
			tagName.assign(path+beg, nameLen);
			return pElem->GetChildren(tagName.c_str());
		}
		beg += nameLen + 1;
	}
	return XMLNodeColl();
}

XMLNodeColl XMLElementNode::QueryChildren2(const char* path)
{
	char cpath[1024];
	strcpy_k(cpath, sizeof(cpath), path);

	char* ss[512];
	int n = split(cpath, '/', ss, 512);

	XMLNodeColl collRst;
	XMLNodeColl collTmp;

	collTmp.AddNode(this);
	int start = 0, end = collTmp.Size();

	for(int i=0; i<n; i++)
	{
		const char* realPath = ss[i];
		for(int k=start; k<end; k++)
		{
			XMLDomNode* pNode = collTmp.GetNode(k);
			XMLElementNode* pElemNode = pNode->ToElementNode();
			XMLNodeColl& coll = pElemNode->m_children;

			for(int j=0; j<coll.Size(); j++)
			{
				XMLDomNode* pChildNode = coll.GetNode(j);
				if(!pChildNode->IsTextNode())
				{
					if(pChildNode->ToElementNode()->m_tagName.icompare(realPath) == 0)
					{
						if(i != n-1)
						{
							collTmp.AddNode(pChildNode);
						}
						else
						{
							collRst.AddNode(pChildNode);
						}

					}
				}
			}
		}
		start = end;
		end = collTmp.Size();
	}

	return collRst;
}

const StringValue* XMLElementNode::QueryAtt(const char* path)
{
	const char* pDot = strchr(path, '.');
	if(!pDot)
	{
		const XMLAttribute* pAtt = this->GetAttribute(path);
		if(!pAtt) return NULL;
		return &pAtt->m_value;
	}
	else
	{
		int len = int(pDot - path);
		KXmlString strpath(path, len);
		XMLElementNode* pElem = this->Query(strpath.c_str());
		if(!pElem) return NULL;

		const XMLAttribute* pAtt = pElem->GetAttribute(pDot+1);
		if(!pAtt) return NULL;
		return &pAtt->m_value;
	}
	return NULL;
}

void XMLElementNode::RemoveTextChild()
{
	int c = m_children.Size();
	for(int i=0; i<c; i++)
	{
		XMLDomNode* pNode = m_children.GetNode(i);
		if(pNode->IsTextNode())
		{
			RemoveChild(i);
			break;
		}
	}
}

XMLDomParser::XMLDomParser()
{
	m_parser = NULL;
	m_pRootNode = NULL;
}

XMLDomParser::~XMLDomParser()
{
	this->Finalize();
}

BOOL XMLDomParser::Parse(StreamInterface& s, XMLElementNode& root)
{
	this->Reset();
	size_t pos = s.tell();

	DWORD flag = 0;
	int_r n2 = s.ReadData(&flag, 3);
	if(n2 == 3 && flag == utf8_signature)
	{
		const char* encoding = "UTF-8";
		if(stricmp(encoding, m_encoding.c_str()) && m_parser)
		{
			XML_ParserFree(m_parser);
			m_parser = NULL;
		}
		m_encoding = encoding;
	}
	else
	{
		s.Seek(pos);
	}

	if(!m_parser)
	{
		if(m_encoding.empty()) m_encoding = "GB2312";
		m_parser = XML_ParserCreate(m_encoding.c_str());
		if(!m_parser) return FALSE;
		XML_SetUserData(m_parser, this);
		XML_SetElementHandler(m_parser, &StartElementHandler, &EndElementHandler);
		XML_SetCharacterDataHandler(m_parser, &CharacterDataHandler);
	}

	if(root.m_tagName.empty())
		root.SetTagName("XMLRoot");

	m_pRootNode = &root;
	m_stack.push(m_pRootNode);

	//const char* orglocale = NULL;
	//if(m_encoding.icompare("GB2312") == 0)
	//	orglocale = setLocaleToGbk();
	//else if(g_encoding == encode_ansii && m_encoding.icompare("UTF-8") == 0)
	//	orglocale = setLocaleToGbk();

	int_r n;
	char cBuffer[2048];
	while(n = s.ReadData(&cBuffer, 2048), n>=0)
	{
		BOOL isFinal = (n < 2048);
		XML_Status status = XML_Parse(m_parser, cBuffer, (int)n, isFinal);
		if(status == XML_STATUS_ERROR)
		{
			//if(orglocale) setlocale(LC_CTYPE, orglocale);
			sprintf_k(cBuffer, sizeof(cBuffer), "%s at line:%d col:%d",
				XML_ErrorString(XML_GetErrorCode(m_parser)),
				XML_GetCurrentLineNumber(m_parser),
				XML_GetCurrentColumnNumber(m_parser));
			m_errMsg = cBuffer;
			return FALSE;
		}
		if(isFinal) break;
	}
	//if(orglocale) setlocale(LC_CTYPE, orglocale);
	return TRUE;
}

const char* XMLDomParser::GetErrorMsg() const
{
	return m_errMsg.c_str();
}

BOOL XMLDomParser::Initialize(const char* encoding)
{
	const char encGB2312[] = {'G','B','2','3','1','2',0};
	if(encoding) m_encoding = encoding;
	else m_encoding = encGB2312;
	//ASSERT_I(IStringTranslator::GetGlobalTranslator());
	return TRUE;
}

void XMLDomParser::Finalize()
{
	if(m_parser)
	{
		XML_ParserFree(m_parser);
		m_parser = NULL;
	}
}

void XMLDomParser::Reset()
{
	m_errMsg.clear();
	if(m_parser)
	{
		if(!m_encoding.empty()) XML_ParserReset(m_parser, m_encoding.c_str());
		else XML_ParserReset(m_parser, NULL);
		XML_SetUserData(m_parser, this);
		XML_SetElementHandler(m_parser, &StartElementHandler, &EndElementHandler);
		XML_SetCharacterDataHandler(m_parser, &CharacterDataHandler);
	}
	m_pRootNode = NULL;
	while(!m_stack.empty()) m_stack.pop();
}

const char* XMLDomParser::GetEncoding()
{
	return m_encoding.c_str();
}

BOOL XMLDomParser::IsSpace(char ch) const
{
	return (ch==' ' || ch=='\t' || ch=='\r' || ch=='\n');
}

const char* XMLDomParser::Trim(const char* s, int len, int& nRetLen)
{
	int beg = 0;
	int end = len;
	while(beg<end && this->IsSpace(s[beg])) beg++;
	while(end>beg && this->IsSpace(s[end-1])) end--;
	nRetLen = end - beg;
	return (s+beg);
}

BOOL XMLDomParser::OnStartElement(const XML_Char *name, const XML_Char **atts)
{
	XMLElementNode* pParent = m_stack.top();

	XMLElementNode* pElemNode = new XMLElementNode();
	pElemNode->SetTagName(name);

	int i = 0;
	while(atts[i])
	{
		const char* n = atts[i];
		const char* val = atts[i+1];
		pElemNode->AddAttribute(n, val);
		i += 2;
	}

	pParent->AddChild(pElemNode);
	m_stack.push(pElemNode);
	return TRUE;
}

BOOL XMLDomParser::OnEndElement(const XML_Char *name)
{
	// 清除空的TextNode by Allen, 20131012
	XMLElementNode* p = m_stack.top();
	XMLTextNode* pText = p->GetTextChild();
	if (pText)
	{
		int len;
		KXmlString& text = pText->m_text.m_val;
		this->Trim(text.c_str(), (int)text.size(), len);
		if (!len)
		{
			p->RemoveTextChild();
		}
		else
		{
			text.resize(len);
		}
	}

	m_stack.pop();
	return TRUE;
}

BOOL XMLDomParser::OnCharacterData(const XML_Char *s, int len)
{
	XMLElementNode* pParent = m_stack.top();
	XMLTextNode* p = pParent->GetTextChild();
	if(!p)
	{
		// 严格遵循XML语法，不再Trim，否则会有bug by Allen 20131012
// 		s = this->Trim(s, len, len);
// 		if(!len) return TRUE;

		p = new XMLTextNode();
		pParent->AddChild(p);
	}
	p->AddText(s, len);
	return TRUE;
}

void XMLDomParser::StartElementHandler(void *userData, const XML_Char *name, const XML_Char **atts)
{
	XMLDomParser* p = (XMLDomParser*)userData;
	p->OnStartElement(name, atts);
}

void XMLDomParser::EndElementHandler(void *userData, const XML_Char *name)
{
	XMLDomParser* p = (XMLDomParser*)userData;
	p->OnEndElement(name);
}

void XMLDomParser::CharacterDataHandler(void *userData, const XML_Char *s, int len)
{
	XMLDomParser* p = (XMLDomParser*)userData;
	p->OnCharacterData(s, len);
}