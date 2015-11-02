/********************************************************************
created:	2008/02/20
filename: 	KTGroupNode.h
file path:	KTGroupNode.h
file base:	KTGroupNode, KTGroupNode
file ext:	h
author:		Augustine

purpose:	组合模式的基础框架，封装了组合的绑定管理，生存周期管理逻辑
*********************************************************************/

#ifndef __KBASEGROUPMETHOD_H__
#define __KBASEGROUPMETHOD_H__

#include "../KMacro.h"
#include "../KType.h"
#include "../Sync/KSync.h"
#include "../Collections/KCoupler.h"
#include "../Collections/DynArray.h"

// 今后移到client公共数据结构中

namespace System {			namespace Patterns {

	template < class T >
	class KTGroupNode;

	/********************************************************************
	created:	2008/02/20
	file base:	KTChildNode
	file ext:	h
	author:		Augustine

	purpose:	子结点的框架类
	*********************************************************************/

	template < class T >
	class KTChildNode : public T
	{
	public:
		friend class KTGroupNode<T>;
		/* 
			定义从一个子结点遍历所有父结点的基本回调函数。
			pNode:		父结点指针。
			pData:		自定义数据类型。
			返回:		true  遍历停止
						false 继续遍历
		*/
		typedef bool ParentNodeCallBack (KTGroupNode<T> * pNode, void *pData);
	public:
		KTChildNode()
		{
			// 父结点
			m_pParentNode = NULL;
		}
		virtual ~KTChildNode()
		{
			if (NULL != m_pParentNode)
			{
				m_pParentNode->RemoveChild(this);
				m_pParentNode = NULL;
			}
		}		

		/* 
			从一个子结点遍历所有父结点。
			ParentNodeCallBack:		每一个子结点的处理函数。
			pData:					自定义数据类型。
			返回:					KTGroupNode<T>* 满足条件的结点
		*/

		KTGroupNode<T>* ChildNodeForAllParentNodes( ParentNodeCallBack callback, void *pData )
		{	
			if( NULL == m_pParentNode)
			{
				return NULL;
			}
			else
			{
				if(callback(m_pParentNode,pData))
				{
					return m_pParentNode;	
				}
				return m_pParentNode->ChildNodeForAllParentNodes(callback,pData );
			}
		}

	protected:
		void  AttachParent( KTGroupNode<T>* ParentNode )
		{
			m_pParentNode = ParentNode;
		}
		void  DetachParent(void)
		{
			m_pParentNode = NULL;
		}

	protected:
		// 父结点
		KTGroupNode<T>* m_pParentNode;
	};

	/********************************************************************
	created:	2008/02/20
	file base:	父结点的框架类
	file ext:	h
	author:		Augustine

	purpose:	父结点的框架类
	*********************************************************************/
	
	// 生命周期的管理类型
	enum en_GroupNodeFlag
	{
		// 不管理子结点的生命周期
		NodeRemove_Flag = 0,
		// 管理子结点的生命周期
		NodeDelete_Flag,
	};
	template < class T >
	class KTGroupNode : public KTChildNode<T>
	{
	public:	
		typedef typename KTChildNode<T> ChildNodeType;
		typedef typename ::System::Collections::KCoupler<void*,en_GroupNodeFlag> ChildInforType;

	public:
		KTGroupNode(){;}
		virtual ~KTGroupNode()
		{
			ClearAll();
		}

		/* 
			添加一个子结点。
			pNode:				子结点。
			iFlag               生命周期的管理
		*/

		bool AddChild( KTChildNode<T>* pNode, en_GroupNodeFlag iFlag = NodeRemove_Flag)
		{
			if (NULL == pNode)
			{	
				return false;
			}
			if (NULL != FindChild(pNode))
			{
				return true;
			}
			if( NULL != pNode->m_pParentNode)
			{
				pNode->m_pParentNode->RemoveChild(pNode);		
			}

			ChildInforType NodeType;
			NodeType.clsItemA = pNode;
			NodeType.clsItemB = iFlag;
			m_ChildArray.push_back(NodeType);
			pNode->AttachParent(this);
			return true;
		}

		/* 
			查找一个子结点。
			pNode:				子结点。
		*/
		KTChildNode<T>* FindChild(KTChildNode<T>* pNode)
		{
			KTChildNode<T>* pRet = NULL;
			
			int iSize = m_ChildArray.size();
			for(int i = 0;i<iSize;++i)
			{
				if(m_ChildArray[i].clsItemA == pNode)
				{
					pRet = pNode;
					break;
				}
			}

			return pRet;
		}

		/* 
			移除一个子结点。
			pNode:				子结点。
		*/
		void RemoveChild(KTChildNode<T>* pNode )
		{
			int i = 0;
			while(i<m_ChildArray.size())
			{
				if(m_ChildArray[i].clsItemA == pNode)
				{
					((ChildNodeType*)m_ChildArray[i].clsItemA)->DetachParent();
					m_ChildArray.erase(i);
					break;
				}
				++i;
			}

		}
		/* 
			清除所有
		*/
		void ClearAll()
		{
			int i = 0;
			while(i<m_ChildArray.size())
			{
				((ChildNodeType*)m_ChildArray[i].clsItemA)->DetachParent();
				if(m_ChildArray[i].clsItemB == NodeDelete_Flag)
				{
					delete ((ChildNodeType*)m_ChildArray[i].clsItemA);
				}
				++i;
			}
			m_ChildArray.clear();
		}

		size_t GetSize(){return (size_t)m_ChildArray.size();}
		KTChildNode<T>* GetChild(size_t iIndex)
		{
			return (KTChildNode<T>*)(m_ChildArray[iIndex].clsItemA);
		}

	protected:
		::System::Collections::DynArray<ChildInforType> m_ChildArray;
	};
	
	template <> struct _is_simple_type_< ::System::Collections::KCoupler<void*,en_GroupNodeFlag> > 
	{ 
		static const bool value = true; 
	};

} /* end Patterns */		} /* System */
#endif