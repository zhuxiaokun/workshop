#include "KBinarySignFile.h"

using namespace ::System::Collections;
using namespace ::System::File;

namespace KBinary_Sign_File_About
{
	/****************** KBinary_Sign_Node ******************/
	KBinary_Sign_Node::KBinary_Sign_Node()
	{
		Clear();
	}

	KBinary_Sign_Node::KBinary_Sign_Node(const KBinary_Sign_Node_Revolution &aoData)
	{
		m_dwOffsetToBase = 0;
		m_pPtr = NULL;
	}

	KBinary_Sign_Node::~KBinary_Sign_Node()
	{
		Clear();
	}

	KBinary_Sign_Node & KBinary_Sign_Node::operator = (const KBinary_Sign_Node_Revolution &aoData)
	{
		m_dwOffsetToBase = 0;
		m_pPtr = NULL;
		return *this;
	}

	BOOL KBinary_Sign_Node::Equal(const KBinary_Sign_Node_InterFace *apszNode)
	{
		if( NULL == apszNode ) return FALSE;
		if( Normal == apszNode->GetType() )
		{
			(*this) = (*(static_cast<const KBinary_Sign_Node*>(apszNode)));
		}
		else if( Revolution == apszNode->GetType() )
		{
			(*this) = (*(static_cast<const KBinary_Sign_Node_Revolution*>(apszNode)));
		}
		return TRUE;
	}

	const char* KBinary_Sign_Node::GetText() const
	{
		if( IsNull() ) return NULL;
		char *lpNamePtr = m_pPtr + sizeof(KSign_File_Count) + sizeof(KSign_File_Count);
		DWORD ldwNameOffset = 0;
		memcpy(&ldwNameOffset, lpNamePtr, sizeof(DWORD));
		lpNamePtr = m_pPtr - m_dwOffsetToBase + ldwNameOffset;
		return lpNamePtr;
	}

	size_t KBinary_Sign_Node::GetAttributeCount() const
	{
		if( IsNull() ) return 0;
		DWORD ldwAttributeCount = 0;
		memcpy(&ldwAttributeCount, m_pPtr, sizeof(KSign_File_Count));
		return ldwAttributeCount;
	}

	size_t KBinary_Sign_Node::GetChildCount() const
	{
		if( IsNull() ) return 0;
		DWORD ldwChildrenCount = 0;
		memcpy(&ldwChildrenCount, m_pPtr + sizeof(KSign_File_Count), sizeof(KSign_File_Count));
		return ldwChildrenCount;
	}

	const char* KBinary_Sign_Node::GetAttributeNameByIndex(int attrIdx) const
	{
		if( attrIdx < 0 ) return NULL;
		if( IsNull() ) return NULL;
		DWORD ldwAttributeCount = 0;
		memcpy(&ldwAttributeCount, m_pPtr, sizeof(KSign_File_Count));
		if( attrIdx >= (int)ldwAttributeCount ) return NULL;

		char *lpAttrNode = m_pPtr + sizeof(KSign_File_Count) + sizeof(KSign_File_Count) + sizeof(DWORD) + attrIdx * sizeof(KBinary_Sign_File_Attribute_Node);
		KBinary_Sign_File_Attribute_Node lAttrNode;
		memcpy(&lAttrNode, lpAttrNode, sizeof(KBinary_Sign_File_Attribute_Node));
		return m_pPtr - m_dwOffsetToBase + lAttrNode.m_dwAttributeNameOffset;
	}

	const char* KBinary_Sign_Node::GetAttributeValueByIndex(int attrIdx) const
	{
		if( attrIdx < 0 ) return NULL;
		if( IsNull() ) return NULL;
		DWORD ldwAttributeCount = 0;
		memcpy(&ldwAttributeCount, m_pPtr, sizeof(KSign_File_Count));
		if( attrIdx >= (int)ldwAttributeCount ) return NULL;

		char *lpAttrNode = m_pPtr + sizeof(KSign_File_Count) + sizeof(KSign_File_Count) + sizeof(DWORD) + attrIdx * sizeof(KBinary_Sign_File_Attribute_Node);
		KBinary_Sign_File_Attribute_Node lAttrNode;
		memcpy(&lAttrNode, lpAttrNode, sizeof(KBinary_Sign_File_Attribute_Node));
		return m_pPtr - m_dwOffsetToBase + lAttrNode.m_dwAttributeValueOffset;
	}

	const char* KBinary_Sign_Node::GetAttributeValueByName(const char* name) const
	{
		if( NULL == name ) return NULL;
		if( IsNull() ) return NULL;
		DWORD ldwAttributeCount = 0;
		memcpy(&ldwAttributeCount, m_pPtr, sizeof(KSign_File_Count));

		char *lpAttrNodeStart = m_pPtr + sizeof(KSign_File_Count) + sizeof(KSign_File_Count) + sizeof(DWORD);
		KBinary_Sign_File_Attribute_Node lAttrNode;
		for(INT i = 0; i < (int)ldwAttributeCount; ++i)
		{
			memcpy(&lAttrNode, lpAttrNodeStart + i * sizeof(KBinary_Sign_File_Attribute_Node), sizeof(KBinary_Sign_File_Attribute_Node));
			char *lpszAttrName = m_pPtr - m_dwOffsetToBase + lAttrNode.m_dwAttributeNameOffset;
			if( NULL == lpszAttrName ) continue;
			if( 0 == _stricmp(lpszAttrName, name) )
			{
				return m_pPtr - m_dwOffsetToBase + lAttrNode.m_dwAttributeValueOffset;
			}
			lAttrNode.Clear();
		}
		return NULL;
	}

	DWORD KBinary_Sign_Node::GetChildren(KBinary_Sign_Nodes &aNodes) const
	{
		if( !IsNull() )
		{
			DWORD ldwAttributeCount = 0;
			memcpy(&ldwAttributeCount, m_pPtr, sizeof(KSign_File_Count));
			DWORD ldwChildrenCount = 0;
			memcpy(&ldwChildrenCount, m_pPtr + sizeof(KSign_File_Count), sizeof(KSign_File_Count));
			if( ldwChildrenCount <= 0 ) return 0;

			KBinary_Sign_Node lNode;
			char *lpChildPtr = m_pPtr + sizeof(KSign_File_Count) + sizeof(KSign_File_Count) + sizeof(DWORD) + ldwAttributeCount * sizeof(KBinary_Sign_File_Attribute_Node);
			for(INT i = 0; i < (int)ldwChildrenCount; ++i)
			{
				memcpy(&(lNode.m_dwOffsetToBase), lpChildPtr, sizeof(DWORD));
				lNode.m_pPtr = m_pPtr - m_dwOffsetToBase + lNode.m_dwOffsetToBase;
				aNodes.push_back(lNode);
				lNode.Clear();
			}
			return ldwChildrenCount;
		}
		return 0;
	}

	BYTE KBinary_Sign_Node::GetChildType(int childIdx) const
	{
		return Normal;
	}

	BOOL KBinary_Sign_Node::GetChild(int childIdx, KBinary_Sign_Node_InterFace *apNode) const
	{
		if( NULL == apNode ) return FALSE;
		KBinary_Sign_Node lNode;
		if( !IsNull() )
		{
			DWORD ldwAttributeCount = 0;
			memcpy(&ldwAttributeCount, m_pPtr, sizeof(KSign_File_Count));
			DWORD ldwChildrenCount = 0;
			memcpy(&ldwChildrenCount, m_pPtr + sizeof(KSign_File_Count), sizeof(KSign_File_Count));
			if( ldwChildrenCount <= 0 ) return FALSE;
			if( childIdx < 0 || childIdx >= (int)ldwChildrenCount ) return FALSE;

			char *lpChildPtr = m_pPtr + sizeof(KSign_File_Count) + sizeof(KSign_File_Count) + sizeof(DWORD) + ldwAttributeCount * sizeof(KBinary_Sign_File_Attribute_Node) + childIdx * sizeof(DWORD);
			memcpy(&(lNode.m_dwOffsetToBase), lpChildPtr, sizeof(DWORD));
			lNode.m_pPtr = m_pPtr - m_dwOffsetToBase + lNode.m_dwOffsetToBase;
		}
		return apNode->Equal(&lNode);
	}

	BYTE KBinary_Sign_Node::GetChildType(const char* tagName) const
	{
		return Normal;
	}

	KBinary_Sign_Node_Revolution KBinary_Sign_Node::GetChild_Revolution(int childIdx) const
	{
		KBinary_Sign_Node_Revolution lNode_Revolution;
		GetChild(childIdx, &lNode_Revolution);
		return lNode_Revolution;
	}

	// CustomWinFile
	BOOL KBinary_Sign_Node::GetChild(const char* tagName, KBinary_Sign_Node_InterFace *apNode) const
	{
		if( NULL == apNode ) return FALSE;
		KBinary_Sign_Node lNode;
		if( !IsNull() )
		{
			DWORD ldwAttributeCount = 0;
			memcpy(&ldwAttributeCount, m_pPtr, sizeof(KSign_File_Count));
			DWORD ldwChildrenCount = 0;
			memcpy(&ldwChildrenCount, m_pPtr + sizeof(KSign_File_Count), sizeof(KSign_File_Count));
			if( ldwChildrenCount <= 0 ) return FALSE;

			char *lpChildPtr = m_pPtr + sizeof(KSign_File_Count) + sizeof(KSign_File_Count) + sizeof(DWORD) + ldwAttributeCount * sizeof(KBinary_Sign_File_Attribute_Node);
			for(INT i = 0; i < (int)ldwChildrenCount; ++i)
			{
				memcpy(&(lNode.m_dwOffsetToBase), lpChildPtr + i * sizeof(DWORD), sizeof(DWORD));
				lNode.m_pPtr = m_pPtr - m_dwOffsetToBase + lNode.m_dwOffsetToBase;
				if( lNode.IsNull() ) continue;
				if( NULL == lNode.GetText() ) continue;
				if( 0 == _stricmp( lNode.GetText(), tagName ) ) break;
				lNode.Clear();
			}
		}
		return apNode->Equal(&lNode);
	}

	KBinary_Sign_Node_Revolution KBinary_Sign_Node::GetChild_Revolution(const char* tagName) const
	{
		KBinary_Sign_Node_Revolution lNode_Revolution;
		GetChild(tagName, &lNode_Revolution);
		return lNode_Revolution;
	}

	DWORD KBinary_Sign_Node::GetChildren(const char* tagName, KBinary_Sign_Nodes &aNodes)
	{
		if( !IsNull() )
		{
			DWORD ldwAttributeCount = 0;
			memcpy(&ldwAttributeCount, m_pPtr, sizeof(KSign_File_Count));
			DWORD ldwChildrenCount = 0;
			memcpy(&ldwChildrenCount, m_pPtr + sizeof(KSign_File_Count), sizeof(KSign_File_Count));
			if( ldwChildrenCount <= 0 ) return 0;

			KBinary_Sign_Node lNode;
			char *lpChildPtr = m_pPtr + sizeof(KSign_File_Count) + sizeof(KSign_File_Count) + sizeof(DWORD) + ldwAttributeCount * sizeof(KBinary_Sign_File_Attribute_Node);
			for(INT i = 0; i < (int)ldwChildrenCount; ++i)
			{
				memcpy(&(lNode.m_dwOffsetToBase), lpChildPtr + i * sizeof(DWORD), sizeof(DWORD));
				lNode.m_pPtr = m_pPtr - m_dwOffsetToBase + lNode.m_dwOffsetToBase;
				if( lNode.IsNull() ) continue;
				if( NULL == lNode.GetText() ) continue;
				if( 0 == _stricmp( lNode.GetText(), tagName ) ) aNodes.push_back(lNode);
				lNode.Clear();
			}
			return ldwChildrenCount;
		}
		return 0;
	}

	// CustomWinManager/CustomWinFile
	BOOL KBinary_Sign_Node::Query(const char* path, KBinary_Sign_Node_InterFace *apNode) const
	{
		if( NULL == apNode ) return FALSE;
		KBinary_Sign_Node lNode;
		if( NULL == path ) return FALSE;
		KVector< KString<MAX_PATH> > lPath;
		char lszPath[MAX_PATH] = {0};
		strcpy_k(lszPath, MAX_PATH, path);
		char * lpText = NULL;
		if( 0 != strcmp("", lszPath) )
		{
			lpText = strtok(lszPath, "/");
			while(lpText) { lPath.push_back( lpText ); lpText = strtok( NULL, "/" ); }
		}

		if( 0 >= lPath.size() ) return FALSE;
		if( 1 == lPath.size() ) return GetChild(lszPath, apNode);

		KBinary_Sign_Node lChildNode = *this;
		INT i = 0;
		for(; i < lPath.size() - 1; ++i)
		{
			lChildNode.GetChild( lPath[i], apNode );
			if( apNode->IsNull() ) return FALSE;
		}
		return lChildNode.GetChild( lPath[i], apNode );
	}

	KBinary_Sign_Node_Revolution KBinary_Sign_Node::Query_Revolution(const char* path) const
	{
		KBinary_Sign_Node_Revolution lNode_Revolution;
		Query(path, &lNode_Revolution);
		return lNode_Revolution;
	}

	// CustomWinManager/CustomWinFile
	DWORD KBinary_Sign_Node::QueryChildren(const char* path, KBinary_Sign_Nodes &aNodes)
	{
		if( NULL == path ) return 0;
		KVector< KString<MAX_PATH> > lPath;
		char lszPath[MAX_PATH] = {0};
		strcpy_k(lszPath, MAX_PATH, path);
		char * lpText = NULL;
		if( 0 != strcmp("", lszPath) )
		{
			lpText = strtok(lszPath, "/");
			while(lpText) { lPath.push_back( lpText ); lpText = strtok( NULL, "/" ); }
		}

		if( 0 >= lPath.size() ) return 0;
		if( 1 == lPath.size() ) return GetChildren( lszPath, aNodes );

		KBinary_Sign_Node lChildNode = *this;
		KBinary_Sign_Node lChildTempNode;
		INT i = 0;
		for(; i < lPath.size() - 1; ++i)
		{
			lChildNode.GetChild( lPath[i] , &lChildTempNode );
			if( lChildTempNode.IsNull() ) return 0;
			else lChildNode = lChildTempNode;
		}
		return lChildNode.GetChildren( lPath[i], aNodes );
	}

	// CustomWinManager/CustomWinFile.name
	const char* KBinary_Sign_Node::QueryAtt(const char* path) const
	{
		if( NULL == path ) return NULL;
		char lszPath[MAX_PATH] = {0};
		strcpy_k(lszPath, MAX_PATH, path);

		char * lpText = NULL;
		lpText = strtok(lszPath, ".");
		if( NULL == lpText ) return NULL;
		char lszNodePath[MAX_PATH] = {0};
		strcpy_k(lszPath, MAX_PATH, lpText);

		lpText = strtok( NULL, "." );
		if( NULL == lpText ) return NULL;
		char lszAttrName[MAX_PATH] = {0};
		strcpy_k(lszAttrName, MAX_PATH, lpText);

		KBinary_Sign_Node lTargetChildNode;
		Query(lszPath, &lTargetChildNode);
		if( lTargetChildNode.IsNull() ) return NULL;

		return lTargetChildNode.GetAttributeValueByName(lszAttrName);
	}

	/****************** KBinary_Sign_Node_Revolution ******************/
	KBinary_Sign_Node_Revolution::KBinary_Sign_Node_Revolution()
	{
		Clear();
	}

	KBinary_Sign_Node_Revolution::KBinary_Sign_Node_Revolution(const KBinary_Sign_Node &aoData)
	{
		m_Title = aoData.GetText();

		for(UINT i = 0; i < aoData.GetAttributeCount(); ++i)
		{
			InsertAttribute( aoData.GetAttributeNameByIndex(i), aoData.GetAttributeValueByIndex(i) );
		}
		ASSERT_I( aoData.GetAttributeCount() == m_AttributeName.size() );
		ASSERT_I( aoData.GetAttributeCount() == m_AttributeValue.size() );

		KBinary_Sign_Node_Revolution lTemp;
		for(UINT i = 0; i < aoData.GetChildCount(); ++i)
		{
			lTemp = aoData.GetChild_Revolution(i);
			InsertAChild( lTemp );
		}
		ASSERT_I( aoData.GetChildCount() == m_ChildrenVector.size() );
	}

	KBinary_Sign_Node_Revolution::~KBinary_Sign_Node_Revolution()
	{
		Clear();
	}

	KBinary_Sign_Node_Revolution & KBinary_Sign_Node_Revolution::operator = (const KBinary_Sign_Node &aoData)
	{
		m_Title = aoData.GetText();

		for(UINT i = 0; i < aoData.GetAttributeCount(); ++i)
		{
			InsertAttribute( aoData.GetAttributeNameByIndex(i), aoData.GetAttributeValueByIndex(i) );
		}
		ASSERT_I( aoData.GetAttributeCount() == m_AttributeName.size() );
		ASSERT_I( aoData.GetAttributeCount() == m_AttributeValue.size() );

		KBinary_Sign_Node_Revolution lTemp;
		for(UINT i = 0; i < aoData.GetChildCount(); ++i)
		{
			lTemp = aoData.GetChild_Revolution(i);
			InsertAChild( lTemp );
		}
		ASSERT_I( aoData.GetChildCount() == m_ChildrenVector.size() );

		return *this;
	}

	BOOL KBinary_Sign_Node_Revolution::Equal(const KBinary_Sign_Node_InterFace *apszNode)
	{
		if( NULL == apszNode ) return FALSE;
		if( Normal == apszNode->GetType() )
		{
			(*this) = *(static_cast<const KBinary_Sign_Node*>(apszNode));
		}
		else if( Revolution == apszNode->GetType() )
		{
			(*this) = *(static_cast<const KBinary_Sign_Node_Revolution*>(apszNode));
		}
		return TRUE;
	}

	BOOL KBinary_Sign_Node_Revolution::IsNull() const
	{
		return (m_Title.empty() && m_AttributeName.size() < 1 && 
			m_AttributeName.size() < 1 && m_ChildrenVector.size() < 1);
	}

	VOID KBinary_Sign_Node_Revolution::Clear()
	{
		m_Title.clear();
		m_AttributeName.clear();
		m_AttributeValue.clear();
		m_ChildrenVector.clear();
	}

	const char* KBinary_Sign_Node_Revolution::GetText() const
	{
		return m_Title.c_str();
	}

	size_t KBinary_Sign_Node_Revolution::GetAttributeCount() const
	{
		return m_AttributeName.size();
	}

	size_t KBinary_Sign_Node_Revolution::GetChildCount() const
	{
		return m_ChildrenVector.size();
	}

	const char* KBinary_Sign_Node_Revolution::GetAttributeNameByIndex(int attrIdx) const
	{
		if( attrIdx < 0 || attrIdx >= m_AttributeName.size() ) return NULL;
		return m_AttributeName[attrIdx].c_str();
	}

	const char* KBinary_Sign_Node_Revolution::GetAttributeValueByIndex(int attrIdx) const
	{
		if( attrIdx < 0 || attrIdx >= m_AttributeName.size() ) return NULL;
		return m_AttributeValue[attrIdx].c_str();
	}

	const char* KBinary_Sign_Node_Revolution::GetAttributeValueByName(const char* name) const
	{
		if( NULL == name ) return NULL;
		for( INT i = 0; i < m_AttributeName.size(); ++i )
		{
			if( 0 == _stricmp( m_AttributeName[i].c_str(), name ) )
			{
				return m_AttributeValue[i].c_str();
			}
		}
		return NULL;
	}

	BYTE KBinary_Sign_Node_Revolution::GetChildType(int childIdx) const
	{
		if( childIdx < 0 || childIdx >= m_ChildrenVector.size() ) return Invalid;
		return m_ChildrenVector[childIdx].GetType();
	}

	BOOL KBinary_Sign_Node_Revolution::GetChild(int childIdx, KBinary_Sign_Node_InterFace *apNode) const
	{
		if( NULL == apNode ) return FALSE;
		if( childIdx < 0 || childIdx >= m_ChildrenVector.size() ) return FALSE;
		return apNode->Equal(&m_ChildrenVector[childIdx]);
	}

	BYTE KBinary_Sign_Node_Revolution::GetChildType(const char* tagName) const
	{
		for( INT i = 0; i < m_ChildrenVector.size(); ++i )
		{
			if( 0 == _stricmp( m_ChildrenVector[i].GetText(), tagName ) )
			{
				return m_ChildrenVector[i].GetType();
			}
		}
		return Invalid;
	}

	BOOL KBinary_Sign_Node_Revolution::GetChild(const char* tagName, KBinary_Sign_Node_InterFace *apNode) const
	{
		if( NULL == apNode ) return FALSE;
		for( INT i = 0; i < m_ChildrenVector.size(); ++i )
		{
			if( 0 == _stricmp( m_ChildrenVector[i].GetText(), tagName ) )
			{
				return apNode->Equal(&m_ChildrenVector[i]);
			}
		}
		return FALSE;
	}
	BOOL KBinary_Sign_Node_Revolution::Query(const char* path, KBinary_Sign_Node_InterFace *apNode) const
	{
		if( NULL == apNode || NULL == path ) return FALSE;

		if( NULL == apNode ) return FALSE;
		KBinary_Sign_Node_Revolution lNode;
		if( NULL == path ) return FALSE;
		KVector< KString<MAX_PATH> > lPath;
		char lszPath[MAX_PATH] = {0};
		strcpy_k(lszPath, MAX_PATH, path);
		char * lpText = NULL;
		if( 0 != strcmp("", lszPath) )
		{
			lpText = strtok(lszPath, "/");
			while(lpText) { lPath.push_back( lpText ); lpText = strtok( NULL, "/" ); }
		}

		if( 0 >= lPath.size() ) return FALSE;
		if( 1 == lPath.size() ) return GetChild(lszPath, apNode);

		KBinary_Sign_Node_Revolution lChildNode = *this;
		INT i = 0;
		for(; i < lPath.size() - 1; ++i)
		{
			lChildNode.GetChild( lPath[i], apNode );
			if( apNode->IsNull() ) return FALSE;
		}
		return lChildNode.GetChild( lPath[i], apNode );
	}

	const char* KBinary_Sign_Node_Revolution::QueryAtt(const char* path) const
	{
		if( NULL == path ) return NULL;
		char lszPath[MAX_PATH] = {0};
		strcpy_k(lszPath, MAX_PATH, path);

		char * lpText = NULL;
		lpText = strtok(lszPath, ".");
		if( NULL == lpText ) return NULL;
		char lszNodePath[MAX_PATH] = {0};
		strcpy_k(lszPath, MAX_PATH, lpText);

		lpText = strtok( NULL, "." );
		if( NULL == lpText ) return NULL;
		char lszAttrName[MAX_PATH] = {0};
		strcpy_k(lszAttrName, MAX_PATH, lpText);

		KBinary_Sign_Node_Revolution lTargetChildNode;
		Query(lszPath, &lTargetChildNode);
		if( lTargetChildNode.IsNull() ) return NULL;

		return lTargetChildNode.GetAttributeValueByName(lszAttrName);
	}

	VOID KBinary_Sign_Node_Revolution::SetText(const char *apszText)
	{
		if( NULL != apszText ) m_Title = apszText;
	}

	INT KBinary_Sign_Node_Revolution::InsertAttribute(const char* name, const char *apszValue)
	{
		if( NULL == name || NULL == apszValue ) return -1;
		for( INT i = 0; i < m_AttributeName.size(); ++i )
		{
			if( 0 == _stricmp( m_AttributeName[i].c_str(), name ) ) return -1;
		}

		ASSERT_I(m_AttributeName.size() == m_AttributeValue.size());
		m_AttributeName.push_back(name);
		m_AttributeValue.push_back(apszValue);
		return (m_AttributeName.size() - 1);
	}

	VOID KBinary_Sign_Node_Revolution::SetAttributeValueByName(const char* name, const char *apszValue)
	{
		if( NULL == name || NULL == apszValue ) return;
		for( INT i = 0; i < m_AttributeName.size(); ++i )
		{
			if( 0 == _stricmp( m_AttributeName[i].c_str(), name ) )
			{
				m_AttributeValue[i] = apszValue;
			}
		}
	}

	BOOL KBinary_Sign_Node_Revolution::DeleteAttributeByIndex(INT anIndex)
	{
		if( anIndex < 0 || anIndex >= m_AttributeName.size() || anIndex >= m_AttributeValue.size() ) return FALSE;
		m_AttributeName.erase(anIndex);
		m_AttributeValue.erase(anIndex);
		return TRUE;
	}

	INT KBinary_Sign_Node_Revolution::InsertAChild(KBinary_Sign_Node_Revolution &aNode)
	{
		m_ChildrenVector.push_back(aNode);
		return (m_ChildrenVector.size() - 1);
	}

	VOID KBinary_Sign_Node_Revolution::SetAChildByIndex(INT anIndex, KBinary_Sign_Node_Revolution &aNode)
	{
		if( anIndex < 0 || anIndex > m_ChildrenVector.size() ) return;
		m_ChildrenVector[anIndex] = aNode;
	}

	BOOL KBinary_Sign_Node_Revolution::DeleteAChildByIndex(INT anIndex)
	{
		if( anIndex < 0 || anIndex >= m_ChildrenVector.size() ) return FALSE;
		m_ChildrenVector.erase(anIndex);
		return TRUE;
	}

	/****************** KBinary_Sign_File_Reader ******************/
	KBinary_Sign_File_Reader::KBinary_Sign_File_Reader()
	{
		Init();
	}

	KBinary_Sign_File_Reader::~KBinary_Sign_File_Reader()
	{
		Destroy();
	}

	void KBinary_Sign_File_Reader::Init()
	{
		m_pszBuff = NULL;
		m_pszCurrentPtr = NULL;
		mSize = 0;
	}

	VOID KBinary_Sign_File_Reader::Init(char *apszBuff, size_t aBuffSize)
	{
		m_pszBuff = apszBuff;
		m_pszCurrentPtr = m_pszBuff;
		mSize = aBuffSize;
	}

	void KBinary_Sign_File_Reader::Destroy()
	{
		if(m_pszBuff)
		{
			delete [] m_pszBuff;
			m_pszBuff = NULL;
		}
		m_pszCurrentPtr = NULL;
		mSize = 0;
	}

	BOOL KBinary_Sign_File_Reader::LoadBinarySignFile(const char *apszFile)
	{
		if( NULL != m_pszBuff ) Destroy();

		mSize = KFileUtil::GetFileSize(apszFile);
		m_pszBuff = new char[mSize];
		memset(m_pszBuff, 0, sizeof(char) * mSize);
		m_pszCurrentPtr = m_pszBuff;

		ASSERT_RETURN(NULL != apszFile, FALSE);
		FILE *lpBinaryFile = JG_F::KFileUtil::OpenFile(apszFile, "rb");
		if( !lpBinaryFile ) return FALSE;
		size_t lSize = fread(m_pszBuff, sizeof(char), mSize, lpBinaryFile);
		ASSERT( lSize == mSize );
		return TRUE;
	}

	BOOL KBinary_Sign_File_Reader::SaveToXmlFile(const char *apszXmlFile)
	{
		ASSERT_RETURN(NULL != apszXmlFile, FALSE);
		KOutputFileStream lXmlFile;
		if( !lXmlFile.Open(apszXmlFile, "w+") ) return FALSE;

		KBinary_Sign_Node lRoot = GetRoot();
		if( lRoot.IsNull() ) return FALSE;

		KBinary_Sign_Node lRootChildren;
		lRoot.GetChild(0, &lRootChildren);
		if( lRootChildren.IsNull() ) return FALSE;

		XMLElementNode lstruRootChild;
		if( !SaveToXml(lRootChildren, lstruRootChild) ) return FALSE;
		lstruRootChild.WriteTo(lXmlFile);

		return TRUE;
	}

	KBinary_Sign_Node KBinary_Sign_File_Reader::GetRoot()
	{
		KBinary_Sign_Node lRoot;
		if( NULL != m_pszBuff && mSize > 0 )
		{
			lRoot.SetPtr(m_pszBuff);
			lRoot.SetOffsetToBase((DWORD)(lRoot.GetPtr() - m_pszBuff));
		}
		return lRoot;
	}

	BOOL KBinary_Sign_File_Reader::SaveToXml(KBinary_Sign_Node &aNode, XMLElementNode &aXmlNode)
	{
		if( aNode.IsNull() ) return FALSE;

		const char *lpTagName = aNode.GetText();
		aXmlNode.SetTagName(lpTagName);
		size_t lSize = aNode.GetAttributeCount();
		for(size_t i = 0; i < lSize; ++i)
		{
			aXmlNode.SetAttribute( aNode.GetAttributeNameByIndex((int)i), aNode.GetAttributeValueByIndex((int)i) );
		}

		lSize = aNode.GetChildCount();
		if( lSize > 0 )
		{
			KBinary_Sign_Node lChildNode;
			for(size_t i = 0; i < lSize; ++i)
			{
				aNode.GetChild((int)i, &lChildNode);
				if( lChildNode.IsNull() ) continue;

				XMLElementNode *lpChildXmlNode = new XMLElementNode;
				if( !SaveToXml(lChildNode, *lpChildXmlNode) ) return FALSE;

				aXmlNode.AddChild( lpChildXmlNode );
			}
		}

		return TRUE;
	}

	/****************** KBinary_Sign_Node_Writer ******************/
	KBinary_Sign_Node_Writer::KBinary_Sign_Node_Writer()
	{
		m_dwOffsetToBase = 0;
		m_dwBuffLength = 0;
		m_pBaseAddress = NULL;
		m_pCurrentAddress = NULL;
		m_byStep = 0;

		m_AttributeCount = 0;
		m_ChildrenCount = 0;
	}

	KBinary_Sign_Node_Writer::KBinary_Sign_Node_Writer(char *apBaseAddress, char *apCurrentAddress, DWORD adwLength, DWORD adwOffsetToBase)
	{
		m_dwOffsetToBase = adwOffsetToBase;
		m_dwBuffLength = adwLength;
		m_pBaseAddress = apBaseAddress;
		m_pCurrentAddress = apCurrentAddress;
		m_byStep = 0;

		m_AttributeCount = 0;
		m_ChildrenCount = 0;
	}

	KBinary_Sign_Node_Writer::~KBinary_Sign_Node_Writer()
	{
		//
	}

	BOOL KBinary_Sign_Node_Writer::IsOverflow(const char *apszPtr)
	{
		ASSERT_RETURN(NULL != apszPtr, TRUE);
		if( m_dwBuffLength <= 0 ) return TRUE;
		return (apszPtr - m_pBaseAddress + m_dwOffsetToBase >= m_dwBuffLength);
	}

	char * KBinary_Sign_Node_Writer::SetAttributeCount(KSign_File_Count anCount)
	{
		ASSERT_RETURN(0 == m_byStep, FALSE);
		ASSERT_RETURN(NULL != m_pCurrentAddress, FALSE);
		ASSERT_RETURN(FALSE == IsOverflow(m_pCurrentAddress), FALSE);
		memcpy(m_pCurrentAddress, (VOID*)&anCount, sizeof(anCount));
		m_pCurrentAddress += sizeof(anCount);
		m_AttributeCount = anCount;
		m_byStep++;
		return m_pCurrentAddress;
	}

	char * KBinary_Sign_Node_Writer::SetChildrenCount(KSign_File_Count anCount)
	{
		ASSERT_RETURN(1 == m_byStep, FALSE);
		ASSERT_RETURN(NULL != m_pCurrentAddress, FALSE);
		ASSERT_RETURN(FALSE == IsOverflow(m_pCurrentAddress), FALSE);
		memcpy(m_pCurrentAddress, (VOID*)&anCount, sizeof(anCount));
		m_pCurrentAddress += sizeof(anCount);
		m_ChildrenCount = anCount;
		m_byStep++;
		return m_pCurrentAddress;
	}

	char * KBinary_Sign_Node_Writer::SetSignName(const char *apszSignName)
	{
		ASSERT_RETURN(2 == m_byStep, FALSE);
		ASSERT_RETURN(NULL != m_pCurrentAddress, FALSE);
		ASSERT_RETURN(FALSE == IsOverflow(m_pCurrentAddress), FALSE);
		DWORD Binary_Sign_Node_Length = sizeof(KSign_File_Count) + sizeof(KSign_File_Count) + sizeof(DWORD) + 
			m_AttributeCount * sizeof(KBinary_Sign_File_Attribute_Node) + m_ChildrenCount * sizeof(DWORD);
		m_pCurrentAddress = m_pBaseAddress + Binary_Sign_Node_Length;
		ASSERT_RETURN(FALSE == IsOverflow(m_pCurrentAddress), FALSE);
		DWORD ldwNameOffsetToFileBase = Binary_Sign_Node_Length + m_dwOffsetToBase;
		memcpy(m_pBaseAddress + sizeof(KSign_File_Count) + sizeof(KSign_File_Count), (VOID*)&ldwNameOffsetToFileBase, sizeof(ldwNameOffsetToFileBase));
		memcpy(m_pCurrentAddress, (VOID*)apszSignName, strlen(apszSignName));
		m_pCurrentAddress += strlen(apszSignName);
		ASSERT_RETURN(FALSE == IsOverflow(m_pCurrentAddress), FALSE);
		*m_pCurrentAddress = '\0';
		m_pCurrentAddress++;
		m_byStep++;
		return m_pCurrentAddress;
	}

	char * KBinary_Sign_Node_Writer::SetSignName(DWORD adwSignNameOffset)
	{
		ASSERT_RETURN(2 == m_byStep, FALSE);
		ASSERT_RETURN(NULL != m_pCurrentAddress, FALSE);
		ASSERT_RETURN(FALSE == IsOverflow(m_pCurrentAddress), FALSE);
		DWORD Binary_Sign_Node_Length = sizeof(KSign_File_Count) + sizeof(KSign_File_Count) + sizeof(DWORD) + 
			m_AttributeCount * sizeof(KBinary_Sign_File_Attribute_Node) + m_ChildrenCount * sizeof(DWORD);
		m_pCurrentAddress = m_pBaseAddress + Binary_Sign_Node_Length;
		ASSERT_RETURN(FALSE == IsOverflow(m_pCurrentAddress), FALSE);
		memcpy(m_pBaseAddress + sizeof(KSign_File_Count) + sizeof(KSign_File_Count), (VOID*)&adwSignNameOffset, sizeof(adwSignNameOffset));
		m_byStep++;
		return m_pCurrentAddress;
	}

	char * KBinary_Sign_Node_Writer::SetAttibuteByIndex(DWORD adwIndex, const char *apszAttrName, const char *apszAttrValue)
	{
		ASSERT_RETURN(3 == m_byStep, FALSE);
		ASSERT_RETURN(NULL != apszAttrName, FALSE);
		ASSERT_RETURN(NULL != apszAttrValue, FALSE);
		ASSERT_RETURN(NULL != m_pCurrentAddress, FALSE);
		ASSERT_RETURN(FALSE == IsOverflow(m_pCurrentAddress), FALSE);
		DWORD ldwAttrNameOffsetByNodeBase = (DWORD)(m_pCurrentAddress - m_pBaseAddress + m_dwOffsetToBase);
		memcpy(m_pCurrentAddress, (VOID*)apszAttrName, strlen(apszAttrName));
		m_pCurrentAddress += strlen(apszAttrName);
		ASSERT_RETURN(FALSE == IsOverflow(m_pCurrentAddress), FALSE);
		*m_pCurrentAddress = '\0';
		m_pCurrentAddress++;
		ASSERT_RETURN(FALSE == IsOverflow(m_pCurrentAddress), FALSE);
		DWORD ldwAttrValueOffsetByNodeBase = (DWORD)(m_pCurrentAddress - m_pBaseAddress + m_dwOffsetToBase);
		memcpy(m_pCurrentAddress, (VOID*)apszAttrValue, strlen(apszAttrValue));
		m_pCurrentAddress += strlen(apszAttrValue);
		ASSERT_RETURN(FALSE == IsOverflow(m_pCurrentAddress), FALSE);
		*m_pCurrentAddress = '\0';
		m_pCurrentAddress++;

		DWORD Binary_Sign_Node_Length = sizeof(KSign_File_Count) + sizeof(KSign_File_Count) + sizeof(DWORD) + 
			adwIndex * sizeof(KBinary_Sign_File_Attribute_Node);
		char *lpAttributeInfoPtr = m_pBaseAddress + Binary_Sign_Node_Length;
		ASSERT_RETURN(FALSE == IsOverflow(lpAttributeInfoPtr), FALSE);
		memcpy(lpAttributeInfoPtr, (VOID*)&ldwAttrNameOffsetByNodeBase, sizeof(ldwAttrNameOffsetByNodeBase));
		lpAttributeInfoPtr += sizeof(ldwAttrNameOffsetByNodeBase);
		ASSERT_RETURN(FALSE == IsOverflow(lpAttributeInfoPtr), FALSE);
		memcpy(lpAttributeInfoPtr, (VOID*)&ldwAttrValueOffsetByNodeBase, sizeof(ldwAttrValueOffsetByNodeBase));
		lpAttributeInfoPtr += sizeof(ldwAttrValueOffsetByNodeBase);

		return m_pCurrentAddress;
	}

	char * KBinary_Sign_Node_Writer::SetAttibuteByIndex(DWORD adwIndex, DWORD adwAttrNameOffset, const char *apszAttrValue)
	{
		ASSERT_RETURN(3 == m_byStep, FALSE);
		ASSERT_RETURN(NULL != apszAttrValue, FALSE);
		ASSERT_RETURN(NULL != m_pCurrentAddress, FALSE);

		ASSERT_RETURN(FALSE == IsOverflow(m_pCurrentAddress), FALSE);
		DWORD ldwAttrValueOffsetByNodeBase = (DWORD)(m_pCurrentAddress - m_pBaseAddress + m_dwOffsetToBase);
		memcpy(m_pCurrentAddress, (VOID*)apszAttrValue, strlen(apszAttrValue));
		m_pCurrentAddress += strlen(apszAttrValue);
		ASSERT_RETURN(FALSE == IsOverflow(m_pCurrentAddress), FALSE);
		*m_pCurrentAddress = '\0';
		m_pCurrentAddress++;

		DWORD Binary_Sign_Node_Length = sizeof(KSign_File_Count) + sizeof(KSign_File_Count) + sizeof(DWORD) + 
			adwIndex * sizeof(KBinary_Sign_File_Attribute_Node);
		char *lpAttributeInfoPtr = m_pBaseAddress + Binary_Sign_Node_Length;
		ASSERT_RETURN(FALSE == IsOverflow(lpAttributeInfoPtr), FALSE);
		memcpy(lpAttributeInfoPtr, (VOID*)&adwAttrNameOffset, sizeof(adwAttrNameOffset));
		lpAttributeInfoPtr += sizeof(adwAttrNameOffset);
		ASSERT_RETURN(FALSE == IsOverflow(lpAttributeInfoPtr), FALSE);
		memcpy(lpAttributeInfoPtr, (VOID*)&ldwAttrValueOffsetByNodeBase, sizeof(ldwAttrValueOffsetByNodeBase));
		lpAttributeInfoPtr += sizeof(ldwAttrValueOffsetByNodeBase);

		return m_pCurrentAddress;
	}

	char * KBinary_Sign_Node_Writer::SetAttibuteByIndex(DWORD adwIndex, const char *apszAttrName, DWORD adwAttrValueOffset)
	{
		ASSERT_RETURN(3 == m_byStep, FALSE);
		ASSERT_RETURN(NULL != apszAttrName, FALSE);
		ASSERT_RETURN(NULL != m_pCurrentAddress, FALSE);
		ASSERT_RETURN(FALSE == IsOverflow(m_pCurrentAddress), FALSE);
		DWORD ldwAttrNameOffsetByNodeBase = (DWORD)(m_pCurrentAddress - m_pBaseAddress + m_dwOffsetToBase);
		memcpy(m_pCurrentAddress, (VOID*)apszAttrName, strlen(apszAttrName));
		m_pCurrentAddress += strlen(apszAttrName);
		ASSERT_RETURN(FALSE == IsOverflow(m_pCurrentAddress), FALSE);
		*m_pCurrentAddress = '\0';
		m_pCurrentAddress++;

		DWORD Binary_Sign_Node_Length = sizeof(KSign_File_Count) + sizeof(KSign_File_Count) + sizeof(DWORD) + 
			adwIndex * sizeof(KBinary_Sign_File_Attribute_Node);
		char *lpAttributeInfoPtr = m_pBaseAddress + Binary_Sign_Node_Length;
		ASSERT_RETURN(FALSE == IsOverflow(lpAttributeInfoPtr), FALSE);
		memcpy(lpAttributeInfoPtr, (VOID*)&ldwAttrNameOffsetByNodeBase, sizeof(ldwAttrNameOffsetByNodeBase));
		lpAttributeInfoPtr += sizeof(ldwAttrNameOffsetByNodeBase);
		ASSERT_RETURN(FALSE == IsOverflow(lpAttributeInfoPtr), FALSE);
		memcpy(lpAttributeInfoPtr, (VOID*)&adwAttrValueOffset, sizeof(adwAttrValueOffset));
		lpAttributeInfoPtr += sizeof(adwAttrValueOffset);

		return m_pCurrentAddress;
	}

	char * KBinary_Sign_Node_Writer::SetAttibuteByIndex(DWORD adwIndex, DWORD adwAttrNameOffset, DWORD adwAttrValueOffset)
	{
		ASSERT_RETURN(3 == m_byStep, FALSE);
		ASSERT_RETURN(NULL != m_pCurrentAddress, FALSE);

		DWORD Binary_Sign_Node_Length = sizeof(KSign_File_Count) + sizeof(KSign_File_Count) + sizeof(DWORD) + 
			adwIndex * sizeof(KBinary_Sign_File_Attribute_Node);
		char *lpAttributeInfoPtr = m_pBaseAddress + Binary_Sign_Node_Length;
		ASSERT_RETURN(FALSE == IsOverflow(lpAttributeInfoPtr), FALSE);
		memcpy(lpAttributeInfoPtr, (VOID*)&adwAttrNameOffset, sizeof(adwAttrNameOffset));
		lpAttributeInfoPtr += sizeof(adwAttrNameOffset);
		ASSERT_RETURN(FALSE == IsOverflow(lpAttributeInfoPtr), FALSE);
		memcpy(lpAttributeInfoPtr, (VOID*)&adwAttrValueOffset, sizeof(adwAttrValueOffset));
		lpAttributeInfoPtr += sizeof(adwAttrValueOffset);

		return m_pCurrentAddress;
	}

	char * KBinary_Sign_Node_Writer::SetChildrenByIndex(DWORD adwIndex, const char *apszOffsetPtr)
	{
		ASSERT_RETURN(3 == m_byStep, FALSE);
		ASSERT_RETURN(NULL != m_pCurrentAddress, FALSE);
		ASSERT_RETURN(NULL != m_pBaseAddress, FALSE);
		ASSERT_RETURN(NULL != apszOffsetPtr, FALSE);
		ASSERT_RETURN(FALSE == IsOverflow(m_pCurrentAddress), FALSE);
		DWORD ldwChildNodeOffset = (DWORD)(apszOffsetPtr - m_pBaseAddress + m_dwOffsetToBase);
		DWORD Binary_Sign_Node_Length = sizeof(KSign_File_Count) + sizeof(KSign_File_Count) + sizeof(DWORD) + 
			m_AttributeCount * sizeof(KBinary_Sign_File_Attribute_Node) + adwIndex * sizeof(DWORD);
		char *lpAttributeInfoPtr = m_pBaseAddress + Binary_Sign_Node_Length;
		ASSERT_RETURN(FALSE == IsOverflow(lpAttributeInfoPtr), FALSE);
		memcpy(lpAttributeInfoPtr, (VOID*)&ldwChildNodeOffset, sizeof(ldwChildNodeOffset));
		lpAttributeInfoPtr += sizeof(ldwChildNodeOffset);
		return m_pCurrentAddress;
	}

	/****************** KBinary_Sign_File_Writer ******************/
	KBinary_Sign_File_Writer::KBinary_Sign_File_Writer()
	{
		Init();
	}

	KBinary_Sign_File_Writer::~KBinary_Sign_File_Writer()
	{
		Destroy();
	}

	VOID KBinary_Sign_File_Writer::Init()
	{
		m_pszBuff = NULL;
		m_pszCurrentPtr = NULL;
		mSize = 0;
	}

	VOID KBinary_Sign_File_Writer::Destroy()
	{
		if(m_pszBuff)
		{
			delete [] m_pszBuff;
			m_pszBuff = NULL;
		}
		m_pszCurrentPtr = NULL;
		mSize = 0;
	}

	BOOL KBinary_Sign_File_Writer::LoadXmlFile(const char *apszXmlFile)
	{
		ASSERT_RETURN(NULL != apszXmlFile, FALSE);
		InputFileStream lXmlFile;
		if( !lXmlFile.Open(apszXmlFile) ) return FALSE;

		XMLDomParser lXmlParser;
		if( !lXmlParser.Initialize("GB2312") ) return FALSE;

		XMLElementNode lstruManagerRoot;
		if( !lXmlParser.Parse(lXmlFile, lstruManagerRoot) )
		{
			printf_k("%s\n", lXmlParser.GetErrorMsg());
			return FALSE;
		}

		if( NULL != m_pszBuff ) Destroy();

		mSize = KFileUtil::GetFileSize(apszXmlFile) * 2;
		m_pszBuff = new char[mSize];
		memset(m_pszBuff, 0, sizeof(char) * mSize);
		m_pszCurrentPtr = m_pszBuff;

		if( !ParseXmlFileToBinary(lstruManagerRoot) ) return FALSE;

		return TRUE;
	}

	BOOL KBinary_Sign_File_Writer::LoadXmlDirectory(const char *apszXmlPath, const char *apszFilterText)
	{
		ASSERT_RETURN(NULL != apszXmlPath, FALSE);
		KDirectory lDirectory;
		if( !lDirectory.Open(apszXmlPath) ) return FALSE;

		KDirectory::FileList lFileList;
		KFileSuffixMultiFilter lFileFilter(apszFilterText);
		if( !lDirectory.ListFile(lFileList, &lFileFilter) ) return FALSE;

		if( lFileList.size() <= 0 ) return FALSE;

		InputFileStream lXmlFile;
		XMLDomParser lXmlParser;
		if( !lXmlParser.Initialize("GB2312") ) return FALSE;

		XMLElementNode *lpstruManagerRoots = new XMLElementNode[lFileList.size()];

		Destroy();

		KFileData lFileData;
		KString<MAX_PATH> lstrFullFileName("");
		for(INT i = 0; i < lFileList.size(); ++i)
		{
			lFileData = lFileList[i];

			lstrFullFileName.append(lFileData.m_dirName);
			lstrFullFileName.append("/");
			lstrFullFileName.append(lFileData.m_shortName);

			if( !lXmlFile.Open(lstrFullFileName.c_str()) ) return FALSE;
			if( !lXmlParser.Parse( lXmlFile, lpstruManagerRoots[i] ) )
			{
				printf_k("%s\n", lXmlParser.GetErrorMsg());
				lstrFullFileName.clear();
				continue;
			}

			mSize += KFileUtil::GetFileSize(lstrFullFileName.c_str());
			lstrFullFileName.clear();
		}

		// 合并所有读取出的xml为一个Root
		XMLElementNode lstruRoot = lpstruManagerRoots[0];
		for(INT i = 1; i < lFileList.size(); ++i)
		{
			XMLDomNode *lpXmlDomNode = NULL;
			XMLElementNode *lpXmlElementNode = NULL;
			for(INT j = 0; j < lpstruManagerRoots[i].m_children.Size(); ++j)
			{
				lpXmlDomNode = lpstruManagerRoots[i].m_children.GetNode(j);
				if( NULL == lpXmlDomNode ) continue;
				lpXmlElementNode = lpXmlDomNode->ToElementNode();
				if( NULL == lpXmlElementNode ) continue;

				XMLElementNode *lpSameNode = lstruRoot.GetChild( lpXmlElementNode->m_tagName.c_str() );
				if( lpSameNode )
				{
					XMLDomNode *lpXmlChildDomNode = NULL;
					for(INT l = 0; l < lpXmlElementNode->m_children.Size(); ++l)
					{
						lpXmlChildDomNode = lpXmlElementNode->GetChild(l);
						if( NULL == lpXmlChildDomNode ) continue;
						lpSameNode->AddChild( lpXmlChildDomNode );
					}
				}
				//else
				//{
				//	lstruRoot.AddChild( lpXmlElementNode );
				//}
			}
		}

		mSize = mSize * 2;
		m_pszBuff = new char[mSize];
		memset(m_pszBuff, 0, sizeof(char) * mSize);
		m_pszCurrentPtr = m_pszBuff;

		if( !ParseXmlFileToBinary(lstruRoot) ) return FALSE;

		return TRUE;
	}

	BOOL KBinary_Sign_File_Writer::ParseXmlFileToBinary(XMLElementNode &astruRootNode)
	{
		if( NULL == m_pszBuff) return FALSE;
		char *lpszStartPtr = m_pszCurrentPtr;
		KBinary_Sign_Node_Writer lNodeWrite(lpszStartPtr, m_pszCurrentPtr, (DWORD)mSize, (DWORD)(lpszStartPtr - m_pszBuff));

		// attributes count
		m_pszCurrentPtr = lNodeWrite.SetAttributeCount(astruRootNode.m_attrs.Size());

		// childrens count
		m_pszCurrentPtr = lNodeWrite.SetChildrenCount(astruRootNode.m_children.Size());

		// tarName
		KSameWordMap::iterator litItem = m_SameWordMap.find(astruRootNode.m_tagName.c_str());
		if( m_SameWordMap.end() == litItem )
		{
			m_pszCurrentPtr = lNodeWrite.SetSignName(astruRootNode.m_tagName.c_str());
			char *lpszWordOffsetToBaseFile = m_pszCurrentPtr - (DWORD)(astruRootNode.m_tagName.size() + 1);
			//for(KSameWordMap::iterator litI = m_SameWordMap.begin(); litI != m_SameWordMap.end(); ++litI)
			//{
			//	ASSERT( litI->second != (DWORD)(lpszWordOffsetToBaseFile - m_pszBuff) );
			//}
			m_SameWordMap.insert(astruRootNode.m_tagName.c_str(), (DWORD)(lpszWordOffsetToBaseFile - m_pszBuff));
		}
		else
		{
			m_pszCurrentPtr = lNodeWrite.SetSignName(litItem->second);
		}

		// attributes
		for(INT i = 0; i < astruRootNode.m_attrs.Size(); ++i)
		{
			const XMLAttribute *lpAttr = astruRootNode.m_attrs.GetAttribute(i);
			if( !lpAttr ) continue;

			KSameWordMap::iterator litAttrNameItem = litItem = m_SameWordMap.find(lpAttr->m_name.c_str());
			KSameWordMap::iterator litAttrValueItem = litItem = m_SameWordMap.find(lpAttr->m_value.c_str());
			if( m_SameWordMap.end() != litAttrNameItem && m_SameWordMap.end() != litAttrValueItem )
			{
				m_pszCurrentPtr = lNodeWrite.SetAttibuteByIndex(i, litAttrNameItem->second, litAttrValueItem->second);
			}
			else if( m_SameWordMap.end() != litAttrNameItem && m_SameWordMap.end() == litAttrValueItem )
			{
				m_pszCurrentPtr = lNodeWrite.SetAttibuteByIndex(i, litAttrNameItem->second, lpAttr->m_value.c_str());
				char *lpszValueWordOffsetToBaseFile = m_pszCurrentPtr - (DWORD)(lpAttr->m_value.Size() + 1);
				//for(KSameWordMap::iterator litI = m_SameWordMap.begin(); litI != m_SameWordMap.end(); ++litI)
				//{
				//	ASSERT( litI->second != (DWORD)(lpszValueWordOffsetToBaseFile - m_pszBuff) );
				//}
				m_SameWordMap.insert(lpAttr->m_value.c_str(), (DWORD)(lpszValueWordOffsetToBaseFile - m_pszBuff));
			}
			else if( m_SameWordMap.end() == litAttrNameItem && m_SameWordMap.end() != litAttrValueItem )
			{
				m_pszCurrentPtr = lNodeWrite.SetAttibuteByIndex(i, lpAttr->m_name.c_str(), litAttrValueItem->second);
				char *lpszNameWordOffsetToBaseFile = m_pszCurrentPtr - (DWORD)(lpAttr->m_name.size() + 1);
				//for(KSameWordMap::iterator litI = m_SameWordMap.begin(); litI != m_SameWordMap.end(); ++litI)
				//{
				//	ASSERT( litI->second != (DWORD)(lpszNameWordOffsetToBaseFile - m_pszBuff) );
				//}
				m_SameWordMap.insert(lpAttr->m_name.c_str(), (DWORD)(lpszNameWordOffsetToBaseFile - m_pszBuff));
			}
			else
			{
				m_pszCurrentPtr = lNodeWrite.SetAttibuteByIndex(i, lpAttr->m_name.c_str(), lpAttr->m_value.c_str());
				char *lpszValueWordOffsetToBaseFile = m_pszCurrentPtr - (DWORD)(lpAttr->m_value.Size() + 1);
				//for(KSameWordMap::iterator litI = m_SameWordMap.begin(); litI != m_SameWordMap.end(); ++litI)
				//{
				//	ASSERT( litI->second != (DWORD)(lpszValueWordOffsetToBaseFile - m_pszBuff) );
				//}
				m_SameWordMap.insert(lpAttr->m_value.c_str(), (DWORD)(lpszValueWordOffsetToBaseFile - m_pszBuff));
				char *lpszNameWordOffsetToBaseFile = lpszValueWordOffsetToBaseFile - (DWORD)(lpAttr->m_name.size() + 1);
				//for(KSameWordMap::iterator litI = m_SameWordMap.begin(); litI != m_SameWordMap.end(); ++litI)
				//{
				//	ASSERT( litI->second != (DWORD)(lpszNameWordOffsetToBaseFile - m_pszBuff) );
				//}
				m_SameWordMap.insert(lpAttr->m_name.c_str(), (DWORD)(lpszNameWordOffsetToBaseFile - m_pszBuff));
			}
		}

		// childrens
		for(INT i = 0; i < astruRootNode.m_children.Size(); ++i)
		{
			XMLDomNode *lpDomNode = astruRootNode.m_children.GetNode(i);
			if( !lpDomNode ) continue;
			XMLElementNode *lpElementNode = lpDomNode->ToElementNode();
			if( !lpElementNode ) continue;
			char *lpszChildNodeStartPtr = m_pszCurrentPtr;
			if( !ParseXmlFileToBinary( *lpElementNode ) ) ASSERT(FALSE);
			lNodeWrite.SetChildrenByIndex(i, lpszChildNodeStartPtr);
		}

		return TRUE;
	}

	BOOL KBinary_Sign_File_Writer::SaveToBinaryFile(const char *apszFile)
	{
		ASSERT_RETURN(NULL != apszFile, FALSE);
		FILE *lpFile = JG_F::KFileUtil::OpenFile(apszFile, "wb");
		if( NULL == lpFile ) return FALSE;
		size_t lSize = fwrite(m_pszBuff, sizeof(char), m_pszCurrentPtr - m_pszBuff, lpFile);
		ASSERT(lSize <= mSize);
		fclose(lpFile);
		return TRUE;
	}
};