#ifndef __KTFACTORYMETHOD_H__
#define __KTFACTORYMETHOD_H__
#include "../KMacro.h"
#include "../KType.h"
#include "System/Memory/_heap_alloc.h"
#include "../Collections/KMapByVector.h"
/********************************************************************
created:    2011/6/7
author:		张昊  （Augustine）
purpose:	工厂方法的基础，目前只支持构造函数无参数传递的类的构件，还要要支持1 2 3 4个参数。今后移到system中
*********************************************************************/

namespace System {			namespace Patterns 
{
	template<typename BaseObjType,typename FactoryType = int>
	class KITBaseObjectFactory
	{
	public:
	public:
		KITBaseObjectFactory(FactoryType  iType):m_iType(iType){}
		virtual ~KITBaseObjectFactory(){}
		virtual BaseObjType* createObject() = 0; 
		virtual bool destroyObject(BaseObjType* pObj) = 0;
		FactoryType  getType(){return m_iType;}
	protected:
		FactoryType  m_iType;
	};

	template<typename ObjType,typename BaseObjType,typename FactoryType = int,typename Allocator = ::System::Memory::HeapAllocator>
	class KTBaseObjectFactory : public KITBaseObjectFactory<BaseObjType,FactoryType>
	{
	public:
		typedef KITBaseObjectFactory<BaseObjType,FactoryType> BASETYPE;

		KTBaseObjectFactory(FactoryType  iType, Allocator* pAlloc = NULL):BASETYPE(iType)
		{
			if(NULL == pAlloc)
			{
				m_pAllocator = new Allocator;
				m_ownAlloc = true;
			}
			else
			{
				m_pAllocator = pAlloc;
				m_ownAlloc = false;
			}
		}
		virtual ~KTBaseObjectFactory()
		{
			if(m_ownAlloc)
			{
				delete m_pAllocator;
				m_pAllocator = NULL;
			}
		}
		virtual BaseObjType* createObject()
		{
			//BaseObjType* pObject = new ObjType();
			void* pObject = m_pAllocator->alloc(sizeof(ObjType));
			ASSERT(pObject);
			new (pObject) ObjType();
			return (BaseObjType*)pObject;
		}
		virtual bool destroyObject(BaseObjType* pObj)
		{
			pObj->~BaseObjType();
			return m_pAllocator->free(pObj);
		}
	protected:
		Allocator* m_pAllocator;
		bool m_ownAlloc;
	};

	template<typename BaseObjType,typename FactoryType = int> 
	class KTBaseObjectFactoryManager
	{
	public:
		typedef KITBaseObjectFactory<BaseObjType,FactoryType> KIBaseFactory;
	protected:
		//typedef std::map<FactoryType, KIBaseFactory*>								KFactoryMap;
		//typedef typename KFactoryMap::iterator			KFactoryMapIter;
		typedef ::System::Collections::KMapByVector<FactoryType, KIBaseFactory*>		KFactoryMap;
		typedef typename KFactoryMap::iterator			KFactoryMapIter;
	public:
		KTBaseObjectFactoryManager(){}
		virtual ~KTBaseObjectFactoryManager()
		{
			KFactoryMapIter Iter;
			for(Iter = m_FactoryMap.begin() ; Iter != m_FactoryMap.end() ; ++Iter)
			{
				delete (Iter->second);
			}
			m_FactoryMap.clear();
		}
		template<typename ObjType,typename Allocator>
		bool registerClass( FactoryType iType,Allocator* pAlloc = NULL )
		{
			KIBaseFactory* pFactory = new KTBaseObjectFactory<ObjType,BaseObjType,FactoryType,Allocator>(iType);
			if(NULL == pFactory)
			{
				return false;
			}
			return registerClassImp(pFactory);
		}
		template<typename ObjType>
		bool registerClass( FactoryType iType)
		{
			KIBaseFactory* pFactory = new KTBaseObjectFactory<ObjType,BaseObjType,FactoryType,::System::Memory::HeapAllocator>(iType);
			if(NULL == pFactory)
			{
				return false;
			}
			return registerClassImp(pFactory);
		}
		void unRegisterClass(FactoryType iType)
		{
			KFactoryMapIter Iter = m_FactoryMap.find(iType);
			if(Iter != m_FactoryMap.end())
			{
				delete (Iter->second);
				m_FactoryMap.erase(Iter);
			}
		}

		KIBaseFactory* findClass( FactoryType iType)
		{
			KFactoryMapIter Iter = m_FactoryMap.find(iType);
			if(Iter != m_FactoryMap.end())
			{
				return (Iter->second);
			}
			else
			{
				return NULL;
			}
		}
		BaseObjType* createObject(FactoryType iType)
		{
			KIBaseFactory* pFactory = findClass(iType);
			if(NULL != pFactory)
			{
				return pFactory->createObject();
			}
			else
			{
				return NULL;
			}
		}
		bool destroyObject(BaseObjType* &pObj,FactoryType iType)
		{
			KIBaseFactory* pFactory = findClass(iType);
			if(NULL != pFactory)
			{
				bool bRet = pFactory->destroyObject(pObj);
				ASSERT(bRet);
				pObj = NULL;
				return true;
			}
			else
			{
				ASSERT(false);
				return false;
			}
			
		}
	protected: 
		bool registerClassImp(KIBaseFactory* pFactory)
		{
			KFactoryMapIter Iter = m_FactoryMap.find(pFactory->getType());
			if(Iter == m_FactoryMap.end())
			{
				m_FactoryMap[pFactory->getType()] = pFactory;
				return true;
			}
			else
			{
				delete pFactory;
				return false;
			}
			return true;
		}

	protected:
		KFactoryMap								m_FactoryMap;
	};



	//构造带一个参数的工厂
	template<typename BaseObjType,typename ParamType,typename FactoryType = int>
	class KITBaseObjectParamX1Factory
	{
	public:
	public:
		KITBaseObjectParamX1Factory(FactoryType  iType):m_iType(iType){}
		virtual ~KITBaseObjectParamX1Factory(){}
		virtual BaseObjType* createObject(ParamType param) = 0; 
		virtual bool destroyObject(BaseObjType* pObj) = 0;
		FactoryType  getType(){return m_iType;}
	protected:
		FactoryType  m_iType;
	};

	template<typename ObjType,typename BaseObjType,typename ParamType,typename FactoryType = int,typename Allocator = ::System::Memory::HeapAllocator>
	class KTBaseObjectParamX1Factory : public KITBaseObjectParamX1Factory<BaseObjType,ParamType,FactoryType>
	{
	public:
		typedef KITBaseObjectParamX1Factory<BaseObjType,ParamType,FactoryType> BASETYPE;
		KTBaseObjectParamX1Factory(FactoryType  iType, Allocator* pAlloc = NULL):BASETYPE(iType)
		{
			if(NULL == pAlloc)
			{
				m_pAllocator = new Allocator;
				m_ownAlloc = true;
			}
			else
			{
				m_pAllocator = pAlloc;
				m_ownAlloc = false;
			}
		}
		virtual ~KTBaseObjectParamX1Factory()
		{
			if(m_ownAlloc)
			{
				delete m_pAllocator;
				m_pAllocator = NULL;
			}
		}
		virtual BaseObjType* createObject(ParamType param)
		{
			void* pObject = m_pAllocator->alloc(sizeof(ObjType));
			ASSERT(pObject);
			new (pObject) ObjType(param);
			return (BaseObjType*)pObject;
		}
		virtual bool destroyObject(BaseObjType* pObj)
		{
			pObj->~BaseObjType();
			return m_pAllocator->free(pObj);
		}
		
	protected:
			Allocator* m_pAllocator;
			bool m_ownAlloc;
	};

	template<typename BaseObjType,typename ParamType,typename FactoryType = int> 
	class KTBaseObjectParamX1FactoryManager
	{
	public:
		typedef KITBaseObjectParamX1Factory<BaseObjType,ParamType,FactoryType> KIBaseFactory;
	protected:
		//typedef std::map<FactoryType, KIBaseFactory*>								KFactoryMap;
		//typedef typename std::map<FactoryType, KIBaseFactory*>::iterator			KFactoryMapIter;
		
		typedef ::System::Collections::KMapByVector<FactoryType, KIBaseFactory*>		KFactoryMap;
		typedef typename KFactoryMap::iterator			KFactoryMapIter;

	public:
		KTBaseObjectParamX1FactoryManager(){}
		virtual ~KTBaseObjectParamX1FactoryManager()
		{
			KFactoryMapIter Iter;
			for(Iter = m_FactoryMap.begin() ; Iter != m_FactoryMap.end() ; ++Iter)
			{
				delete (Iter->second);
			}
			m_FactoryMap.clear();
		}
		template<typename ObjType,typename Allocator>
		bool registerClass( FactoryType iType)
		{
			KIBaseFactory* pFactory = new KTBaseObjectParamX1Factory<ObjType,BaseObjType,ParamType,FactoryType,Allocator>(iType);
			if(NULL == pFactory)
			{
				return false;
			}
			return registerClassImp(pFactory);
		}
	
		template<typename ObjType>
		bool registerClass( FactoryType iType)
		{
			KIBaseFactory* pFactory = new KTBaseObjectParamX1Factory<ObjType,BaseObjType,ParamType,FactoryType,::System::Memory::HeapAllocator>(iType);
			if(NULL == pFactory)
			{
				return false;
			}
			return registerClassImp(pFactory);
		}

		void unRegisterClass(FactoryType iType)
		{
			KFactoryMapIter Iter = m_FactoryMap.find(iType);
			if(Iter != m_FactoryMap.end())
			{
				delete (Iter->second);
				m_FactoryMap.erase(Iter);
			}
		}

		KIBaseFactory* findClass( FactoryType iType)
		{
			KFactoryMapIter Iter = m_FactoryMap.find(iType);
			if(Iter != m_FactoryMap.end())
			{
				return (Iter->second);
			}
			else
			{
				return NULL;
			}
		}
		BaseObjType* createObject(FactoryType iType,ParamType param)
		{
			KIBaseFactory* pFactory = findClass(iType);
			if(NULL != pFactory)
			{
				return pFactory->createObject(param);
			}
			else
			{
				return NULL;
			}
		}
		bool destroyObject(BaseObjType* &pObj,FactoryType iType)
		{
			KIBaseFactory* pFactory = findClass(iType);
			if(NULL != pFactory)
			{
				bool bRet = pFactory->destroyObject(pObj);
				ASSERT(bRet);
				pObj = NULL;
				return true;
			}
			else
			{
				ASSERT(false);
				return false;
			}

		}
	protected: 
		bool registerClassImp(KIBaseFactory* pFactory)
		{
			KFactoryMapIter Iter = m_FactoryMap.find(pFactory->getType());
			if(Iter == m_FactoryMap.end())
			{
				m_FactoryMap[pFactory->getType()] = pFactory;
				return true;
			}
			else
			{
				delete pFactory;
				return false;
			}
			return true;
		}

	protected:
		KFactoryMap								m_FactoryMap;
	};


} /* end Patterns */		} /* System */

#endif