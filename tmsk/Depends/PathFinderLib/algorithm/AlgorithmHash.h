#pragma once
#include "../interface/PathFinderDefine.h"
#include "../interface/IFinderPlugin.h"
#include "AlgorithmBase.h"
#include "_Hashtable.h"

namespace KPathFinderAbout
{
	class IAlgorithmHash : public IAlgorithmBase
	{
	public:
		virtual void ReleaseNodes();
		virtual bool add_to_open(const _Node * pNode);
		virtual bool erase_from_open(const _Node * pNode);
		virtual _Node * find_from_open(int_r nID);
		virtual _Node * get_from_open(int_r nIdx);
		virtual bool add_to_close(const _Node * pNode);
		virtual bool erase_from_close(const _Node * pNode);
		virtual _Node * find_from_close(int_r nID);
		virtual _Node * get_from_close(int_r nIdx);

	private:
		typedef _Hashtable<int_r, const _Node *, KPointerCompare<int_r>, KPointerHasher<int_r>, JG_S::KNoneLock,256,10240> KNodeHashTable;
		KNodeHashTable m_openTab;
		KNodeHashTable m_closeTab;
	};
};