#include "AlgorithmHash.h"
#include "../PerfHitch/perf_PathFinderLib.h"

namespace KPathFinderAbout
{
	void IAlgorithmHash::ReleaseNodes()
	{	PERF_COUNTER(IAlgorithmHash_ReleaseNodes);

		m_openTab.clear();
		m_closeTab.clear();
	}

	bool IAlgorithmHash::add_to_open(const _Node * pNode)
	{	PERF_COUNTER(IAlgorithmHash_add_to_open);

		ASSERT_RETURN(pNode, false);
		return m_openTab.end() != m_openTab.insert_unique(pNode->id, pNode) ? true : false;
	}

	bool IAlgorithmHash::erase_from_open(const _Node * pNode)
	{	PERF_COUNTER(IAlgorithmHash_erase_from_open);

		ASSERT_RETURN(pNode, false);
		m_openTab.erase(pNode->id);
		return true;
	}

	_Node * IAlgorithmHash::find_from_open(int_r nID)
	{
		KNodeHashTable::iterator it = m_openTab.find(nID);
		return m_openTab.end() == it ? NULL : const_cast<_Node *>(it->second);
	}

	_Node * IAlgorithmHash::get_from_open(int_r nIdx)
	{
		return NULL;
	}

	bool IAlgorithmHash::add_to_close(const _Node * pNode)
	{	PERF_COUNTER(IAlgorithmHash_add_to_close);

		ASSERT_RETURN(pNode, false);
		return m_closeTab.end() != m_closeTab.insert_unique(pNode->id, pNode) ? true : false;
	}

	bool IAlgorithmHash::erase_from_close(const _Node * pNode)
	{	PERF_COUNTER(IAlgorithmHash_erase_from_close);

		ASSERT_RETURN(pNode, false);
		m_closeTab.erase(pNode->id);
		return true;
	}

	_Node * IAlgorithmHash::find_from_close(int_r nID)
	{
		KNodeHashTable::iterator it = m_closeTab.find(nID);
		return m_closeTab.end() == it ? NULL : const_cast<_Node *>(it->second);
	}

	_Node * IAlgorithmHash::get_from_close(int_r nIdx)
	{
		return NULL;
	}
}