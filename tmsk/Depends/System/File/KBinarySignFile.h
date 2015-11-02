#ifndef _BINARY_SIGN_FILE_H_
#define _BINARY_SIGN_FILE_H_

#include "../KType.h"
#include "../Log/log.h"
#include "../KMacro.h"
#include "KFile.h"
#include "../Collections/KMapByVector.h"
#include "../Collections/KVector.h"
#include "../Collections/KString.h"

#include "sys/stat.h"

#include "../Patterns/KSingleton.h"
#include "../tinyxml/xmldom.h"

namespace KBinary_Sign_File_About
{
	typedef USHORT KSign_File_Count;

	struct KBinary_Sign_File_Attribute_Node
	{
	public:
		KBinary_Sign_File_Attribute_Node() {Clear();}
		VOID Clear() {memset(this, 0, sizeof(KBinary_Sign_File_Attribute_Node));}
		DWORD m_dwAttributeNameOffset;
		DWORD m_dwAttributeValueOffset;
	};

	struct KBinary_Sign_File_Node
	{
	public:
		KBinary_Sign_File_Node() {memset(this, 0, sizeof(KBinary_Sign_File_Node));}
		KSign_File_Count	m_ttributeCount;
		KSign_File_Count	m_hChildrenCount;
		DWORD	m_dwSignName;
		KBinary_Sign_File_Attribute_Node	*m_pAttributes;
		DWORD								*m_pChildrens;
	};

	struct KBinary_Sign_Node_InterFace;
	class KBinary_Sign_Node;
	class KBinary_Sign_Node_Revolution;
	typedef ::System::Collections::KVector<KBinary_Sign_Node> KBinary_Sign_Nodes;

	struct KBinary_Sign_Node_InterFace
	{
	public:
		enum {Invalid, Normal, Revolution,};
	public:
		virtual BYTE GetType() const = 0;
		virtual BOOL Equal(const KBinary_Sign_Node_InterFace *apszNode) = 0;
		virtual BOOL IsNull() const = 0;
		virtual VOID Clear() = 0;

		virtual const char* GetText() const = 0;

		virtual size_t GetAttributeCount() const = 0;
		virtual size_t GetChildCount() const = 0;

		virtual const char* GetAttributeNameByIndex(int attrIdx) const = 0;
		virtual const char* GetAttributeValueByIndex(int attrIdx) const = 0;
		virtual const char* GetAttributeValueByName(const char* name) const = 0;

		virtual BYTE GetChildType(int childIdx) const = 0;
		virtual BOOL GetChild(int childIdx, KBinary_Sign_Node_InterFace *apNode) const = 0;
		virtual BYTE GetChildType(const char* tagName) const = 0;
		virtual BOOL GetChild(const char* tagName, KBinary_Sign_Node_InterFace *apNode) const = 0;
		virtual BOOL Query(const char* path, KBinary_Sign_Node_InterFace *apNode) const = 0;
		virtual const char* QueryAtt(const char* path) const = 0;
	};

	class KBinary_Sign_Node : public KBinary_Sign_Node_InterFace
	{
	public:
		KBinary_Sign_Node();
		KBinary_Sign_Node(const KBinary_Sign_Node_Revolution &aoData);
		~KBinary_Sign_Node();
		KBinary_Sign_Node & operator = (const KBinary_Sign_Node_Revolution &aoData);

		DWORD GetOffsetToBase() {return m_dwOffsetToBase;}
		VOID SetOffsetToBase(DWORD adwOffsetToBase) {m_dwOffsetToBase = adwOffsetToBase;}
		char * GetPtr() {return m_pPtr;}
		VOID SetPtr(char * a_pPtr) {m_pPtr = a_pPtr;}

		virtual BYTE GetType() const { return Normal; }
		virtual BOOL Equal(const KBinary_Sign_Node_InterFace *apszNode);
		virtual BOOL IsNull() const { return NULL == m_pPtr; }
		virtual VOID Clear() { m_dwOffsetToBase = 0;m_pPtr = NULL; }

		virtual const char* GetText() const;

		virtual size_t GetAttributeCount() const;
		virtual size_t GetChildCount() const;

		virtual const char* GetAttributeNameByIndex(int attrIdx) const;
		virtual const char* GetAttributeValueByIndex(int attrIdx) const;
		virtual const char* GetAttributeValueByName(const char* name) const;

		virtual BYTE GetChildType(int childIdx) const;
		virtual BOOL GetChild(int childIdx, KBinary_Sign_Node_InterFace *apNode) const;
		virtual BYTE GetChildType(const char* tagName) const;
		virtual BOOL GetChild(const char* tagName, KBinary_Sign_Node_InterFace *apNode) const;
		virtual BOOL Query(const char* path, KBinary_Sign_Node_InterFace *apNode) const;
		virtual const char* QueryAtt(const char* path) const;

		DWORD GetChildren(KBinary_Sign_Nodes &aNodes) const;
		DWORD GetChildren(const char* tagName, KBinary_Sign_Nodes &aNodes);
		DWORD QueryChildren(const char* path, KBinary_Sign_Nodes &aNodes);

		KBinary_Sign_Node_Revolution GetChild_Revolution(int childIdx) const;
		KBinary_Sign_Node_Revolution Query_Revolution(const char* path) const;
		KBinary_Sign_Node_Revolution GetChild_Revolution(const char* tagName) const;

	private:
		DWORD m_dwOffsetToBase;
		char *m_pPtr;
	};

	class KBinary_Sign_Node_Revolution : public KBinary_Sign_Node_InterFace
	{
	public:
		typedef ::System::Collections::KVector< KBinary_Sign_Node_Revolution > KChildrenVector;

	public:
		KBinary_Sign_Node_Revolution();
		KBinary_Sign_Node_Revolution(const KBinary_Sign_Node &aoData);
		~KBinary_Sign_Node_Revolution();
		KBinary_Sign_Node_Revolution & operator = (const KBinary_Sign_Node &aoData);

		virtual BYTE GetType() const { return Revolution; }
		virtual BOOL Equal(const KBinary_Sign_Node_InterFace *apszNode);
		virtual BOOL IsNull() const;
		virtual VOID Clear();
		virtual const char* GetText() const;
		virtual size_t GetAttributeCount() const;
		virtual size_t GetChildCount() const;

		virtual const char* GetAttributeNameByIndex(int attrIdx) const;
		virtual const char* GetAttributeValueByIndex(int attrIdx) const;
		virtual const char* GetAttributeValueByName(const char* name) const;

		virtual BYTE GetChildType(int childIdx) const;
		virtual BOOL GetChild(int childIdx, KBinary_Sign_Node_InterFace *apNode) const;
		virtual BYTE GetChildType(const char* tagName) const;
		virtual BOOL GetChild(const char* tagName, KBinary_Sign_Node_InterFace *apNode) const;
		virtual BOOL Query(const char* path, KBinary_Sign_Node_InterFace *apNode) const;
		virtual const char* QueryAtt(const char* path) const;

		VOID SetText(const char *apszText);
		INT InsertAttribute(const char* name, const char *apszValue);
		VOID SetAttributeValueByName(const char* name, const char *apszValue);
		BOOL DeleteAttributeByIndex(INT anIndex);
		INT InsertAChild(KBinary_Sign_Node_Revolution &aNode);
		VOID SetAChildByIndex(INT anIndex, KBinary_Sign_Node_Revolution &aNode);
		BOOL DeleteAChildByIndex(INT anIndex);

	private:
		::System::Collections::KString<MAX_PATH> m_Title;
		::System::Collections::KVector< ::System::Collections::KString<MAX_PATH> > m_AttributeName;
		::System::Collections::KVector< ::System::Collections::KString<MAX_PATH> > m_AttributeValue;
		KChildrenVector m_ChildrenVector;
	};

	class KBinary_Sign_File_Reader : public ::System::Patterns::KSingleton<KBinary_Sign_File_Reader>
	{
	public:
		KBinary_Sign_File_Reader();
		~KBinary_Sign_File_Reader();

		VOID Init();
		VOID Init(char *apszBuff, size_t aBuffSize);
		VOID Destroy();

		BOOL LoadBinarySignFile(const char *apszFile);
		BOOL SaveToXmlFile(const char *apszXmlFile);
		KBinary_Sign_Node GetRoot();

	private:
		BOOL SaveToXml(KBinary_Sign_Node &aNode, XMLElementNode &aXmlNode);

	private:
		char *m_pszBuff;
		char *m_pszCurrentPtr;
		size_t mSize;
	};

	class KBinary_Sign_Node_Writer
	{
	public:
		KBinary_Sign_Node_Writer();
		KBinary_Sign_Node_Writer(char *apBaseAddress, char *apCurrentAddress, DWORD adwLength, DWORD adwOffsetToBase);
		~KBinary_Sign_Node_Writer();

		BOOL IsOverflow(const char *apszPtr);

		// 务必按顺序调用
		char * SetAttributeCount(KSign_File_Count anCount);
		char * SetChildrenCount(KSign_File_Count anCount);
		char * SetSignName(const char *apszSignName);
		char * SetSignName(DWORD adwSignNameOffset);
		char * SetAttibuteByIndex(DWORD adwIndex, const char *apszAttrName, const char *apszAttrValue);
		char * SetAttibuteByIndex(DWORD adwIndex, DWORD adwAttrNameOffset, const char *apszAttrValue);
		char * SetAttibuteByIndex(DWORD adwIndex, const char *apszAttrName, DWORD adwAttrValueOffset);
		char * SetAttibuteByIndex(DWORD adwIndex, DWORD adwAttrNameOffset, DWORD adwAttrValueOffset);
		char * SetChildrenByIndex(DWORD adwIndex, const char *apszOffsetPtr);

	private:
		DWORD	m_dwOffsetToBase;
		DWORD	m_dwBuffLength;

		char*	m_pBaseAddress;
		char*	m_pCurrentAddress;
		BYTE	m_byStep;

		KSign_File_Count	m_AttributeCount;
		KSign_File_Count	m_ChildrenCount;
	};

	class KBinary_Sign_File_Writer : public ::System::Patterns::KSingleton<KBinary_Sign_File_Writer>
	{
	public:
		KBinary_Sign_File_Writer();
		~KBinary_Sign_File_Writer();

		VOID Init();
		VOID Destroy();
		BOOL LoadXmlFile(const char *apszXmlFile);
		BOOL LoadXmlDirectory(const char *apszXmlPath, const char *apszFilterText);
		BOOL ParseXmlFileToBinary(XMLElementNode &astruRootNode);
		BOOL SaveToBinaryFile(const char *apszFile);

	private:
		template <int size> class KBinary_Sign_File_Writer_TextLess
		{
		public:
			typedef ::System::Collections::KString<size> String;
			BOOL operator () (const String& a, const String& b) const
			{
				return _stricmp(a.c_str(), b.c_str()) < 0;
			}
		};
		typedef ::System::Collections::KMapByVector< ::System::Collections::KString<MAX_PATH>, DWORD, KBinary_Sign_File_Writer_TextLess<MAX_PATH> > KSameWordMap;
		KSameWordMap m_SameWordMap;;

		char *m_pszBuff;
		char *m_pszCurrentPtr;
		size_t mSize;
	};
};

DECLARE_SIMPLE_TYPE(KBinary_Sign_File_About::KBinary_Sign_Node)

#endif // _BINARY_SIGN_FILE_H_