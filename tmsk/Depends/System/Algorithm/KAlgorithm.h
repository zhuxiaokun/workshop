#include "../Misc/rand.h"

// 交换数据
template <typename _Tp>
inline void KSwap( _Tp& a, _Tp& b )
{
	_Tp temp = a;
	a = b;
	b = temp;
}

// 随机打乱容器c内[begin, begin+num)范围的数据,c为随机可访问容器
template <typename _Collec>
inline void KRandom_Shuffle( _Collec& c , int begin, int num )
{
	if ( begin < 0 || num < 0 )
		return;

	for ( int i = begin + 1; i < begin+num; ++i  )
		KSwap(c[i], c[g_Random.GetRandom(begin, i+1)]);
}

// 随机打乱[first,last)范围内的数据
template <typename Iter>
inline void KRandom_Shuffle( Iter first, Iter last )
{
	if ( first == last )
		return;
	for ( Iter i = first; i != last; ++i )
		KSwap( i, first + g_Random.GetRandom((i - first) + 1) );
}