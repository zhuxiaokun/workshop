#pragma once
#include "../interface/PathFinderDefine.h"
#include "../interface/IFinderPlugin.h"
#include "AlgorithmBase.h"
#include <System/Collections/DynArray.h>

namespace KPathFinderAbout
{
	template < typename C >
	class IAlgorithmDynArray : public IAlgorithmBase
	{
	public:
		virtual void ReleaseNodes()
		{
			clear_open();
			clear_close();
		}
		virtual void clear_open()
		{
			m_openTab_id.clear();
			m_openTab.clear();
		}
		virtual bool add_to_open(const _Node * pNode)
		{
			ASSERT_RETURN(pNode, false);
			return m_openTab_id.insert_unique(pNode) > -1 && m_openTab.insert_unique(pNode) > -1;
		}
		virtual bool erase_from_open(const _Node * pNode)
		{
			ASSERT_RETURN(pNode, false);
			m_openTab_id.erase(pNode);
			m_openTab.erase(pNode);
			return true;
		}
		virtual _Node * find_from_open(int_r nID)
		{
			_Node lNode;
			lNode.id = nID;
			int_r pos = m_openTab_id.find(&lNode);
			return -1 == pos ? NULL : const_cast<_Node *>(m_openTab_id[pos]);
		}
		virtual _Node * get_from_open(int_r nIdx)
		{
			return (nIdx >= 0 && nIdx < m_openTab.size()) ? const_cast<_Node *>(m_openTab[nIdx]) : NULL;
		}
		virtual void clear_close()
		{
			m_closeTab_id.clear();
			m_closeTab.clear();
		}
		virtual bool add_to_close(const _Node * pNode)
		{
			ASSERT_RETURN(pNode, false);
			return m_closeTab_id.insert_unique(pNode) > -1 && m_closeTab.insert_unique(pNode) > -1;
		}
		virtual bool erase_from_close(const _Node * pNode)
		{
			ASSERT_RETURN(pNode, false);
			m_closeTab_id.erase(pNode);
			m_closeTab.erase(pNode);
			return true;
		}
		virtual _Node * find_from_close(int_r nID)
		{
			_Node lNode;
			lNode.id = nID;
			int_r pos = m_closeTab_id.find(&lNode);
			return -1 == pos ? NULL : const_cast<_Node *>(m_closeTab_id[pos]);
		}
		virtual _Node * get_from_close(int_r nIdx)
		{
			return (nIdx >= 0 && nIdx < m_closeTab.size()) ? const_cast<_Node *>(m_closeTab[nIdx]) : NULL;
		}

	protected:
		typedef JG_C::KDynSortedArray<const _Node *, _Node_Lite::PtIDCmp> KNodeTable_ID;
		typedef JG_C::KDynSortedArray<const _Node *, C> KNodeTable_F;
		KNodeTable_ID m_openTab_id;
		KNodeTable_F m_openTab;
		KNodeTable_ID m_closeTab_id;
		KNodeTable_F m_closeTab;
	};
};