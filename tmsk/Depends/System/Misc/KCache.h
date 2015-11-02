#ifndef _K_CACHE_H_
#define _K_CACHE_H_

#include "../KType.h"
#include "../KPlatform.h"
#include "../Collections/DynArray.h"
#include "../Memory/KStepObjectPool.h"

#define DEF_MAX_HIT_RATE	(0x7fffffff)
template<typename T, typename C=KLess<T>, uint_r maxCapacity=32, uint_r maxStatistics=9999, uint_r maxTry=3>
class KCache
{
protected:
	typedef JG_M::KPortableStepPool<T, maxCapacity> KValuePool;
	typedef KPair<T*, int_r> KHitRateKey;
	class KCacheCmp
	{
	public:
		typedef KHitRateKey vPtr;
		int operator () (const vPtr& a, const vPtr& b) const
		{
			return mCmp(a.first, b.first);
		}
	private:
		C mCmp;
	};
	typedef JG_C::DynSortedArray<KHitRateKey, KCacheCmp, maxCapacity, 2> KCacheList;
	class KCacheHitRateCmp
	{
	public:
		typedef KHitRateKey vPtr;
		int operator () (const vPtr& a, const vPtr& b) const
		{
			if(a.second == b.second)
			{
				return mCmp(a.first, b.first);
			}
			return a.second > b.second;
		}
	private:
		C mCmp;
	};
	typedef JG_C::DynSortedArray<KHitRateKey, KCacheHitRateCmp, maxCapacity, 2> KCacheHitRateList;

public:
	KCache():mnHitCount(0) {mCacheList.clear();mCacheHitRateList.clear();}
	virtual ~KCache() {clear();}
	virtual void clear()
	{
		mnHitCount = 0;
		int_r nCount = mCacheList.size();
		KHitRateKey * pKey(NULL);
		for(int_r i = 0; i < nCount; ++i)
		{
			pKey = &mCacheList[i];
			ASSERT(pKey);
			KValuePool::Free(pKey->first);
		}
		mCacheList.clear();
		mCacheHitRateList.clear();
	}
	inline void size() const {return mCacheList.size();}
	virtual bool get(T &o)
	{
		//__check();

		int_r pos = -1;
		if(mnHitCount >= maxStatistics)
		{
			int_r nCount = mCacheHitRateList.size();
			for(int_r i = 0; i < maxTry && i < nCount; ++i)
			{
				mTempKey = mCacheHitRateList[i];
				if(!mCmp(mTempKey.first, &o) && !mCmp(&o, mTempKey.first))
				{
					pos = mCacheList.find(mTempKey);
					ASSERT(-1 != pos);

					KHitRateKey &lTempKey = mCacheList[pos];
					_on_get(lTempKey);
					o = *lTempKey.first;
					return true;
				}
			}
		}

		new (&mTempKey) KHitRateKey(&o, 0);
		pos = mCacheList.find(mTempKey);
		if(-1 == pos)
			return false;

		KHitRateKey &lTempKey = mCacheList[pos];
		_on_get(lTempKey);
		o = *(lTempKey.first);
		
		if(mnHitCount < maxStatistics)
			mnHitCount++;

		//__check();
		return true;
	}
	virtual bool push(T &o)
	{
		//__check();
		new (&mTempKey) KHitRateKey(&o, 0);
		int_r pos = mCacheList.find(mTempKey);
		if(-1 != pos)
			return false;

		_on_push();

		T* pKey = KValuePool::Alloc();
		ASSERT(pKey);
		*pKey = o;
		new (&mTempKey) KHitRateKey(pKey, 0);
		pos = mCacheList.insert_unique(mTempKey);
		if(-1 == pos)
			return false;

		pos = mCacheList.find(mTempKey);
		ASSERT(-1 != pos);
		mTempKey = mCacheList[pos];
		pos = mCacheHitRateList.insert_unique(mTempKey);
		ASSERT(-1 != pos);
		ASSERT(mCacheHitRateList.size() == mCacheList.size());
		//__check();
		return -1 != pos;
	}

protected:
	virtual void _on_get(KHitRateKey &o)
	{
		//__check();
		int_r pos = mCacheHitRateList.find(o);
		ASSERT(-1 != pos);
		mTempKey = mCacheHitRateList[pos];
		mCacheHitRateList.erase(pos);
		mTempKey.second = kmin((int_r)DEF_MAX_HIT_RATE, mTempKey.second + 1);
		o.second = mTempKey.second;
		pos = mCacheHitRateList.insert_unique(mTempKey);
		//__check();
		ASSERT(-1 != pos);
	}
	virtual void _on_push()
	{
		if(mCacheHitRateList.size() >= maxCapacity)
		{
			//__check();
			ASSERT(mCacheList.size() == maxCapacity);
			int_r nSize = mCacheHitRateList.size();
			mTempKey = mCacheHitRateList[nSize - 1];
			int_r pos = mCacheList.find(mTempKey);
			ASSERT(-1 != pos);
			KValuePool::Free(mCacheList[pos].first);
			mCacheList.erase(pos);
			mCacheHitRateList.erase(nSize - 1);
			//__check();
		}
	}

private:
	void __check()
	{
		int_r pos(-1);
		int_r nCount = mCacheList.size();
		KHitRateKey * pKey(NULL);
		for(int_r i = 0; i < nCount; ++i)
		{
			pKey = &mCacheList[i];
			ASSERT(pKey);
			pos = mCacheHitRateList.find(*pKey);
			ASSERT(-1 != pos);
		}
	}

protected:
	KCacheList mCacheList;
	KCacheHitRateList mCacheHitRateList;
	uint_r mnHitCount;
	C mCmp;
	KHitRateKey mTempKey;
};

#endif // _K_CACHE_H_