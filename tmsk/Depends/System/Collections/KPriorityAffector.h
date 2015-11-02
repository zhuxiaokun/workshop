#ifndef __KTPRIORITYAFFECTOR_H__
#define __KTPRIORITYAFFECTOR_H__

#include "../KType.h"
#include "../KMacro.h"
#include "../Memory/KMemory.h"

namespace System { namespace Collections {
	

	template <typename T>
	class KPriorityAffectCallback
	{
	public:
		KPriorityAffectCallback(){}
		virtual ~KPriorityAffectCallback(){}
		virtual void OnCurChanged(DWORD iOldPriority,T* pOldObj,DWORD iNewPriority,T* pNewObj) = 0;
		virtual void OnPriorityObjChanged(DWORD iPriority,T* pOldObj,T* pNewObj) = 0;		
		virtual void OnUnHoldobj(DWORD iPriority,T* pOldObj) = 0;
	};

	template <typename T,int initCapacity=32, int inflateStep=16> 
	class KPriorityAffector
	{
	public:	
		typedef KPair<DWORD,T*> PriorityNode;	
		class PriorityNodeLess
		{
		
		public:
			BOOL operator () (const PriorityNode& a, const PriorityNode& b) const
			{
				return a.first < b.first;
			}
		};

		
		KPriorityAffector():m_pCallback(NULL){ }
		~KPriorityAffector(){ }
		void SetCallBack(KPriorityAffectCallback<T>* pCallback){m_pCallback = pCallback;}
		bool SetObj(T* pObj,DWORD iPriority)
		{
		
			T* pOldObj = NULL;

			PriorityNode oldTop = GetTop();

			PriorityNode node(iPriority,pObj);
			BOOL found;
			int pos = m_Array.bsearch(node, found);
			if(found)
			{
				pOldObj	= m_Array[pos].second;
				m_Array[pos].second = pObj;	
			}
			else
			{
				pos = m_Array.insert(node);
			}
			if((pos+1) == m_Array.size())
			{
				onTopChanged(oldTop);
			}

			if(NULL != pOldObj && NULL != m_pCallback)
			{
				m_pCallback->OnPriorityObjChanged(iPriority,pOldObj,pObj);
				m_pCallback->OnUnHoldobj(iPriority,pOldObj);
			}

			return true;
		}
		
		bool RemoveByObj(T* pObj)
		{
			bool bRet = false;
			int iSize = m_Array.size();
			for(int i = 0;i<iSize;++i)
			{
				if( m_Array[i].second == pObj)
				{
					PriorityNode theNode = m_Array[i];
					if((i+1) == iSize)
					{
						PriorityNode oldTop = GetTop();
						m_Array.erase_by_index(i);
						onTopChanged(oldTop);
					}
					else
					{
						m_Array.erase_by_index(i);
					}
					
					onUnHold(theNode);
					bRet = true;
					break;
				}
			}
			return bRet;
		}

		bool RemoveByPriority(DWORD iPriority)
		{
			bool bRet = false;
			PriorityNode node(iPriority,NULL);
			BOOL found;
			int pos = m_Array.bsearch(node, found);
			if(found)
			{
				PriorityNode theNode = m_Array[pos];
				if((pos+1) == m_Array.size())
				{
					PriorityNode oldTop = GetTop();
					m_Array.erase_by_index(pos);
					onTopChanged(oldTop);
				}
				else
				{
					m_Array.erase_by_index(pos);
				}
				onUnHold(theNode);
				bRet = true;
				
			}
			return true;
		}
		void RemoveAll()
		{
			PriorityNode oldTop = GetTop();
			onTopChanged(oldTop);
			for (size_t i = 0; i<m_Array.size(); i++)
			{
				onUnHold(m_Array[i]);
			}
			m_Array.clear();

		}

		T* GetByPriority(DWORD iPriority)
		{
			T* pRet = NULL;
			PriorityNode node(iPriority,NULL);
			BOOL found;
			int pos = m_Array.bsearch(node, found);
			if(found)
			{
				pRet = m_Array[pos];
			}
			return pRet;
		}
		PriorityNode GetTop()
		{
			PriorityNode node(0,NULL);
			int iSize = m_Array.size();
			if(iSize>0)
			{
				node.first = m_Array[iSize-1].first;
				node.second = m_Array[iSize-1].second;
			}
			return node;
		}
	protected:
		void onTopChanged(PriorityNode OldNode)
		{
			if(NULL != m_pCallback)
			{
				PriorityNode NewNode = GetTop();
				m_pCallback->OnCurChanged(OldNode.first, OldNode.second, NewNode.first, NewNode.second);
			}
		}
		
		void onUnHold(PriorityNode theNode)
		{
			if(NULL != m_pCallback)
			{
				m_pCallback->OnUnHoldobj(theNode.first,theNode.second);
			}
		}

	protected:
		DynSortedArray<PriorityNode, PriorityNodeLess,initCapacity,inflateStep> m_Array;
		KPriorityAffectCallback<T>* m_pCallback;  
	};

} }

#endif