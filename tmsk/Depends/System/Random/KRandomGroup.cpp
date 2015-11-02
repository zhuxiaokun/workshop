#include "../Log/log.h"
#include "KRandomGroup.h"
#include "KRandom.h"

namespace System
{
	namespace Random
	{
		const DWORD KRandomGroup::MAX_RANGE = 10000000;

		static unsigned int randbignum(unsigned int n)
		{
			static ::System::Random::KRandom r;
			return r.GetRandom(0, n);
		}

		KRandomGroup::ELEM_DATA::ELEM_DATA()
			: m_dwRate(0)
			, m_type(enumT_Number)
		{
			memset(&m_range, 0, sizeof(m_range));
		}

		bool KRandomGroup::ELEM_DATA::operator<( const ELEM_DATA& other ) const
		{
			return m_dwRate < other.m_dwRate;
		}

		int KRandomGroup::ELEM_DATA::GetValue() const
		{
			switch (m_type)
			{
			case enumT_Number:
				return m_num;
			case enumT_Range:
				return (int)randbignum((unsigned int)(m_range.m_max - m_range.m_min + 1)) + m_range.m_min;
			default:
				ASSERT_I(false);
				return 0;
			}
		}
		void KRandomGroup::InsertData(DWORD rate, int data)
		{
			ASSERT_RETURN_VOID(rate != 0);
			KRandomGroup::ELEM_DATA::TYPE type = KRandomGroup::ELEM_DATA::enumT_Number;
			KRandomGroup::ELEM_DATA ed;
			DWORD dwBaseRate = 0;
			if (!m_vecData.empty())
			{
				dwBaseRate = m_vecData[m_vecData.size() - 1].m_dwRate;
			}
			ASSERT_RETURN_VOID(dwBaseRate + rate <= KRandomGroup::MAX_RANGE);
			ed.m_dwRate = dwBaseRate + rate;
			ed.m_type = type;
			ed.m_num = data;
			m_vecData.insert(ed);
		}

		void KRandomGroup::InsertRange(DWORD rate, int rangeMin, int rangeMax)
		{
			ASSERT_RETURN_VOID(rate != 0);
			ASSERT_RETURN_VOID(rangeMin <= rangeMax);
			KRandomGroup::ELEM_DATA ed;
			DWORD dwBaseRate = 0;
			if (!m_vecData.empty())
			{
				dwBaseRate = m_vecData[m_vecData.size() - 1].m_dwRate;
			}
			ASSERT_RETURN_VOID(dwBaseRate + rate <= KRandomGroup::MAX_RANGE);
			ed.m_dwRate = dwBaseRate + rate;
			ed.m_type = KRandomGroup::ELEM_DATA::enumT_Range;
			ed.m_range.m_min = rangeMin;
			ed.m_range.m_max = rangeMax;
			m_vecData.insert(ed);
		}

		const KRandomGroup::ELEM_DATA* KRandomGroup::GetRandomElement() const
		{
			if (m_vecData.empty())
			{
				ASSERT_I(false);
				return NULL;
			}
			DWORD maxRate = m_vecData[m_vecData.size() - 1].m_dwRate;
			DWORD key = randbignum(maxRate) + 1;
			BOOL bFound = FALSE;
			int pos = m_vecData.find(key, bFound);
			return &m_vecData[pos];
		}

		void KRandomGroup::Clear()
		{
			m_vecData.clear();
		}

		int KRandomGroup::GetRandomVal() const
		{
			const ELEM_DATA* pED = GetRandomElement();
			if (pED)
			{
				return pED->GetValue();
			}
			return 0;
		}

		bool KRandomGroup::IsEmpty() const
		{
			return m_vecData.empty() != FALSE;
		}

		int KRandomGroup::TakeRandomVal()
		{
			DWORD result = 0;
			if (m_vecData.empty())
			{
				ASSERT_I(false);
				return 0;
			}
			DWORD maxRate = m_vecData[m_vecData.size() - 1].m_dwRate;
			DWORD key = randbignum(maxRate) + 1;
			BOOL bFound = FALSE;
			int pos = m_vecData.find(key, bFound);
			result = m_vecData[pos].GetValue();
			DWORD rateToDecrease = 0;
			if (pos == 0)
			{
				rateToDecrease = m_vecData[pos].m_dwRate;
			}
			else
			{
				rateToDecrease = m_vecData[pos].m_dwRate - m_vecData[pos - 1].m_dwRate;
			}
			m_vecData.erase(pos);
			for (int i = pos; i < m_vecData.size(); i++)
			{
				m_vecData[i].m_dwRate -= rateToDecrease;
			}
			return result;
		}
	}
}