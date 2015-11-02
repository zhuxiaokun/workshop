#include "KInteger.h"
#include <sstream>
#include <algorithm>
#include <math.h>
#include <float.h>
#include "../Memory/KStepObjectPool.h"

// 定义个使用池的INTEGER数字大小。小于等于0或未定义该宏则不使用池
#define SMALL_INTEGER_DIGIT_SIZE 4

#if defined(SMALL_INTEGER_DIGIT_SIZE) && SMALL_INTEGER_DIGIT_SIZE > 0
// 最小分配SMALL_INTEGER_DIGIT_SIZE单位大小
//static ::System::Memory::KPortableStepPool<KInteger::digit[SMALL_INTEGER_DIGIT_SIZE]> s_memPool;
static JG_M::_shrinkable_memory_pool<sizeof(KInteger::digit[SMALL_INTEGER_DIGIT_SIZE]),16> s_memPool;
#endif

typedef KInteger::digit digit;
typedef KInteger::twodigits twodigits;
typedef KInteger::sdigit sdigit;
typedef KInteger::stwodigits stwodigits;

#define ABS(x) ((x) < 0 ? -(x) : (x))

#define PyLong_SHIFT	30
#define PyLong_BASE	((digit)1 << PyLong_SHIFT)
#define PyLong_MASK	((digit)(PyLong_BASE - 1))

#define NEGATE(x) ((x)->m_size = -(x)->m_size)
#define Py_SAFE_DOWNCAST(VALUE, WIDE, NARROW) (NARROW)(VALUE)
/* For long multiplication, use the O(N**2) school algorithm unless
 * both operands contain more than KARATSUBA_CUTOFF digits (this
 * being an internal Python long digit, in base BASE).
 */
#define KARATSUBA_CUTOFF 70
#define KARATSUBA_SQUARE_CUTOFF (2 * KARATSUBA_CUTOFF)

/* For exponentiation, use the binary left-to-right algorithm
 * unless the exponent contains more than FIVEARY_CUTOFF digits.
 * In that case, do 5 bits at a time.  The potential drawback is that
 * a table of 2**5 intermediate results is computed.
 */
#define FIVEARY_CUTOFF 8

#undef MIN
#undef MAX
#define MAX(x, y) ((x) < (y) ? (y) : (x))
#define MIN(x, y) ((x) > (y) ? (y) : (x))

// 用最大的int当上限好了
#define PY_SSIZE_T_MAX ((int)(0xffffffff >> 1))

/* Argument must be a char or an int in [-128, 127] or [0, 255]. */
#define Py_CHARMASK(c) ((unsigned char)((c) & 0xff))

#ifdef WIN32
#	define Py_IS_INFINITY(X) (!_finite(X) && !_isnan(X))
#else
#	define Py_IS_INFINITY(X) (!finite(X) && !isnan(X))
#endif // WIN32

#define _PyLong_DECIMAL_SHIFT	9 /* max(e such that 10**e fits in a digit) */
#define _PyLong_DECIMAL_BASE	((digit)1000000000) /* 10 ** DECIMAL_SHIFT */


KInteger::KInteger()
{
	_init();
}

KInteger::KInteger( const KInteger& o )
{
	_init();
	int usize = ABS(o.m_size);
	_Alloc(usize);
	m_size = o.m_size;
	memcpy(ob_digit, o.ob_digit, sizeof(digit) * usize);
}

KInteger& KInteger::operator=( const KInteger& o )
{
	int usize = ABS(o.m_size);
	_Alloc(usize);
	m_size = o.m_size;
	memcpy(ob_digit, o.ob_digit, sizeof(digit) * usize);
	return *this;
}

void KInteger::_Alloc( int size )
{
// 	ASSERT_RETURN_VOID(size > 0);
	// 给的比size小才重新new
	if (m_digitSize < size)
	{
		digit* p = NULL;
		int sz = size;
#if defined(SMALL_INTEGER_DIGIT_SIZE) && SMALL_INTEGER_DIGIT_SIZE > 0
		if (size <= SMALL_INTEGER_DIGIT_SIZE)
		{
			p = (digit*)s_memPool.Alloc();
			sz = SMALL_INTEGER_DIGIT_SIZE;
		}
		else
#endif
		{
			p = new digit[size];
		}
		ASSERT_RETURN_VOID(p);
		_Free();
		m_digitSize = sz;
		ob_digit = p;
	}
	memset(ob_digit, 0, sizeof(digit) * size);
	m_size = size;
}

void KInteger::_Realloc( int size )
{
// 	ASSERT_RETURN_VOID(size > 0);
	// 给的比size小才重新new
	if (m_digitSize < size)
	{
		int size_a = ABS(m_size);
		digit* p = NULL;
		int sz = size;
#if defined(SMALL_INTEGER_DIGIT_SIZE) && SMALL_INTEGER_DIGIT_SIZE > 0
		if (size <= SMALL_INTEGER_DIGIT_SIZE)
		{
			p = (digit*)s_memPool.Alloc();
			sz = SMALL_INTEGER_DIGIT_SIZE;
		}
		else
#endif
		{
			p = new digit[size];
		}

		ASSERT_RETURN_VOID(p);

		for (int i = 0; i < size_a; i++)
		{
			p[i] = ob_digit[i];
		}
		for (int i = size_a; i < size; i++)
		{
			p[i] = 0;
		}
		int oldSize = m_size;
		_Free();
		m_digitSize = sz;
		ob_digit = p;
		m_size = oldSize;
	}
}

void KInteger::_Free()
{
	if (m_digitSize)
	{
#if defined(SMALL_INTEGER_DIGIT_SIZE) && SMALL_INTEGER_DIGIT_SIZE > 0
		if (m_digitSize == SMALL_INTEGER_DIGIT_SIZE)	// 从池里来的
		{
			s_memPool.Free(ob_digit);
		}
		else
#endif
		{
			delete [] ob_digit;
		}
		ob_digit = NULL;
		m_digitSize = 0;
	}
	m_size = 0;
}

static void
long_normalize(register KInteger* v)
{
	int j = ABS(v->m_size);
	int i = j;

	while (i > 0 && v->ob_digit[i-1] == 0)
		--i;
	if (i != j)
		v->m_size = (v->m_size < 0) ? -(i) : i;
}

static void
x_add(const KInteger* a, const KInteger* b, KInteger& iz)
{
	KInteger* z = &iz;

	int size_a = ABS(a->m_size), size_b = ABS(b->m_size);
	int i;
	digit carry = 0;

	/* Ensure a is the larger of the two: */
	if (size_a < size_b) {
		{ const KInteger *temp = a; a = b; b = temp; }
		{ int size_temp = size_a;
		size_a = size_b;
		size_b = size_temp; }
	}
// 	z = _PyLong_New(size_a+1);
	// 不管怎样，多分配一位以防止不够长
	iz._Alloc(size_a + 1);
// 	if (z == NULL)
// 		return NULL;
	for (i = 0; i < size_b; ++i) {
		carry += a->ob_digit[i] + b->ob_digit[i];
		z->ob_digit[i] = carry & PyLong_MASK;
		carry >>= PyLong_SHIFT;
	}
	for (; i < size_a; ++i) {
		carry += a->ob_digit[i];
		z->ob_digit[i] = carry & PyLong_MASK;
		carry >>= PyLong_SHIFT;
	}
	z->ob_digit[i] = carry;
	long_normalize(z);
}

static void
x_sub(const KInteger* a, const KInteger* b, KInteger& iz)
{
	KInteger* z = &iz;

	int size_a = ABS(a->m_size), size_b = ABS(b->m_size);
	int i;
	int sign = 1;
	digit borrow = 0;

	/* Ensure a is the larger of the two: */
	if (size_a < size_b) {
		sign = -1;
		{ const KInteger *temp = a; a = b; b = temp; }
		{ int size_temp = size_a;
			size_a = size_b;
			size_b = size_temp; }
	}
	else if (size_a == size_b) {
		/* Find highest digit where a and b differ: */
		i = size_a;
		while (--i >= 0 && a->ob_digit[i] == b->ob_digit[i])
			;
		if (i < 0)
		{
// 			return (KInteger *)PyLong_FromLong(0);
			z->ToZero();
			return;
		}
		if (a->ob_digit[i] < b->ob_digit[i]) {
			sign = -1;
			{ const KInteger *temp = a; a = b; b = temp; }
		}
		size_a = size_b = i+1;
	}
// 	z = _PyLong_New(size_a);
	z->_Alloc(size_a);
// 	if (z == NULL)
// 		return NULL;
	for (i = 0; i < size_b; ++i) {
		/* The following assumes unsigned arithmetic
		   works module 2**N for some N>PyLong_SHIFT. */
		borrow = a->ob_digit[i] - b->ob_digit[i] - borrow;
		z->ob_digit[i] = borrow & PyLong_MASK;
		borrow >>= PyLong_SHIFT;
		borrow &= 1; /* Keep only one sign bit */
	}
	for (; i < size_a; ++i) {
		borrow = a->ob_digit[i] - borrow;
		z->ob_digit[i] = borrow & PyLong_MASK;
		borrow >>= PyLong_SHIFT;
		borrow &= 1; /* Keep only one sign bit */
	}
	assert(borrow == 0);
	if (sign < 0)
		NEGATE(z);
	long_normalize(z);
}

KInteger KInteger::operator+( const KInteger& o ) const
{
	KInteger z;
	const KInteger* a = this;
	const KInteger* b = &o;
	if (a->m_size < 0) {
		if (b->m_size < 0) {
			x_add(a, b, z);
			if (z.m_size != 0) z.m_size = -z.m_size;
		}
		else x_sub(b, a, z);
	}
	else {
		if (b->m_size < 0) x_sub(a, b, z);
		else x_add(a, b, z);
	}
	return z;
}

KInteger KInteger::operator-( const KInteger& o ) const
{
	KInteger z;
	const KInteger* a = this;
	const KInteger* b = &o;
	if (a->m_size < 0) {
		if (b->m_size < 0)
			x_sub(a, b, z);
		else
			x_add(a, b, z);
		if (z.m_size != 0)
			z.m_size = -z.m_size;
	}
	else {
		if (b->m_size < 0)
			x_add(a, b, z);
		else
			x_sub(a, b, z);
	}
	return z;
}

/* Grade school multiplication, ignoring the signs.
 * Returns the absolute value of the product, or NULL if error.
 */
static void
x_mul(const KInteger* a, const KInteger* b, KInteger& iz)
{
	KInteger* z = &iz;
	int size_a = ABS(a->m_size);
	int size_b = ABS(b->m_size);
	int i;

// 	z = _PyLong_New(size_a + size_b);
// 	if (z == NULL)
// 		return NULL;
// 	memset(z->ob_digit, 0, Py_SIZE(z) * sizeof(digit));
	z->_Alloc(size_a + size_b);

	if (a == b) {
		/* Efficient squaring per HAC, Algorithm 14.16:
		 * http://www.cacr.math.uwaterloo.ca/hac/about/chap14.pdf
		 * Gives slightly less than a 2x speedup when a == b,
		 * via exploiting that each entry in the multiplication
		 * pyramid appears twice (except for the size_a squares).
		 */
		for (i = 0; i < size_a; ++i) {
			twodigits carry;
			twodigits f = a->ob_digit[i];
			digit *pz = z->ob_digit + (i << 1);
			digit *pa = a->ob_digit + i + 1;
			digit *paend = a->ob_digit + size_a;

// 			SIGCHECK({
// 				Py_DECREF(z);
// 				return NULL;
// 			});

			carry = *pz + f * f;
			*pz++ = (digit)(carry & PyLong_MASK);
			carry >>= PyLong_SHIFT;
			assert(carry <= PyLong_MASK);

			/* Now f is added in twice in each column of the
			* pyramid it appears.  Same as adding f<<1 once.
			*/
			f <<= 1;
			while (pa < paend) {
				carry += *pz + *pa++ * f;
				*pz++ = (digit)(carry & PyLong_MASK);
				carry >>= PyLong_SHIFT;
				assert(carry <= (PyLong_MASK << 1));
			}
			if (carry) {
				carry += *pz;
				*pz++ = (digit)(carry & PyLong_MASK);
				carry >>= PyLong_SHIFT;
			}
			if (carry)
				*pz += (digit)(carry & PyLong_MASK);
			assert((carry >> PyLong_SHIFT) == 0);
		}
	}
	else {      /* a is not the same as b -- gradeschool long mult */
		for (i = 0; i < size_a; ++i) {
			twodigits carry = 0;
			twodigits f = a->ob_digit[i];
			digit *pz = z->ob_digit + i;
			digit *pb = b->ob_digit;
			digit *pbend = b->ob_digit + size_b;

// 			SIGCHECK({
// 				Py_DECREF(z);
// 				return NULL;
// 			});

			while (pb < pbend) {
				carry += *pz + *pb++ * f;
				*pz++ = (digit)(carry & PyLong_MASK);
				carry >>= PyLong_SHIFT;
				assert(carry <= PyLong_MASK);
			}
			if (carry)
				*pz += (digit)(carry & PyLong_MASK);
			assert((carry >> PyLong_SHIFT) == 0);
		}
	}
	long_normalize(z);
}

/**
 * k_lopsided_mul会用到k_mul，所以提前声明一下
 */
static void
k_mul(const KInteger* a, const KInteger* b, KInteger& iz);

/* x[0:m] and y[0:n] are digit vectors, LSD first, m >= n required.  x[0:n]
 * is modified in place, by adding y to it.  Carries are propagated as far as
 * x[m-1], and the remaining carry (0 or 1) is returned.
 */
static digit
v_iadd(digit *x, int m, digit *y, int n)
{
	int i;
	digit carry = 0;

	assert(m >= n);
	for (i = 0; i < n; ++i) {
		carry += x[i] + y[i];
		x[i] = carry & PyLong_MASK;
		carry >>= PyLong_SHIFT;
		assert((carry & 1) == carry);
	}
	for (; carry && i < m; ++i) {
		carry += x[i];
		x[i] = carry & PyLong_MASK;
		carry >>= PyLong_SHIFT;
		assert((carry & 1) == carry);
	}
	return carry;
}

/* x[0:m] and y[0:n] are digit vectors, LSD first, m >= n required.  x[0:n]
 * is modified in place, by subtracting y from it.  Borrows are propagated as
 * far as x[m-1], and the remaining borrow (0 or 1) is returned.
 */
static digit
v_isub(digit *x, int m, digit *y, int n)
{
    int i;
    digit borrow = 0;

    assert(m >= n);
    for (i = 0; i < n; ++i) {
        borrow = x[i] - y[i] - borrow;
        x[i] = borrow & PyLong_MASK;
        borrow >>= PyLong_SHIFT;
        borrow &= 1;            /* keep only 1 sign bit */
    }
    for (; borrow && i < m; ++i) {
        borrow = x[i] - borrow;
        x[i] = borrow & PyLong_MASK;
        borrow >>= PyLong_SHIFT;
        borrow &= 1;
    }
    return borrow;
}

static void
k_lopsided_mul(const KInteger* a, const KInteger* b, KInteger& iz)
{
	const int asize = ABS(a->m_size);
	int bsize = ABS(b->m_size);
	int nbdone;          /* # of b digits already multiplied */
	KInteger* ret = &iz;
	KInteger islice;
	KInteger* bslice = &islice;
	KInteger tempProduct;

	assert(asize > KARATSUBA_CUTOFF);
	assert(2 * asize <= bsize);

	/* Allocate result space, and zero it out. */
// 	ret = _PyLong_New(asize + bsize);
// 	if (ret == NULL)
// 		return NULL;
// 	memset(ret->ob_digit, 0, Py_SIZE(ret) * sizeof(digit));
	ret->_Alloc(asize + bsize);

	/* Successive slices of b are copied into bslice. */
// 	bslice = _PyLong_New(asize);
// 	if (bslice == NULL)
// 		goto fail;
	bslice->_Alloc(asize);

	nbdone = 0;
	while (bsize > 0) {
		KInteger* product;
		const int nbtouse = MIN(bsize, asize);

		/* Multiply the next slice of b by a. */
		memcpy(bslice->ob_digit, b->ob_digit + nbdone,
			nbtouse * sizeof(digit));
		bslice->m_size = nbtouse;
// 		product = k_mul(a, bslice);
// 		if (product == NULL)
// 			goto fail;
		product = &tempProduct;
		k_mul(a, bslice, *product);

		/* Add into result. */
		(void)v_iadd(ret->ob_digit + nbdone, ret->m_size - nbdone,
			product->ob_digit, product->m_size);
// 		Py_DECREF(product);

		bsize -= nbtouse;
		nbdone += nbtouse;
	}

// 	Py_DECREF(bslice);
	long_normalize(ret);

// fail:
// 	Py_DECREF(ret);
// 	Py_XDECREF(bslice);
// 	return NULL;
}

/* A helper for Karatsuba multiplication (k_mul).
   Takes a long "n" and an integer "size" representing the place to
   split, and sets low and high such that abs(n) == (high << size) + low,
   viewing the shift as being by digits.  The sign bit is ignored, and
   the return values are >= 0.
*/
static void
kmul_split(const KInteger *n,
           int size,
           KInteger& high,
           KInteger& low)
{
	int size_lo, size_hi;
	const int size_n = ABS(n->m_size);

	size_lo = MIN(size_n, size);
	size_hi = size_n - size_lo;

// 	if ((hi = _PyLong_New(size_hi)) == NULL)
// 		return -1;
// 	if ((lo = _PyLong_New(size_lo)) == NULL) {
// 		Py_DECREF(hi);
// 		return -1;
// 	}
	high._Alloc(size_hi);
	low._Alloc(size_lo);

	memcpy(low.ob_digit, n->ob_digit, size_lo * sizeof(digit));
	memcpy(high.ob_digit, n->ob_digit + size_lo, size_hi * sizeof(digit));

	long_normalize(&high);
	long_normalize(&low);
}

static void
k_mul(const KInteger* a, const KInteger* b, KInteger& iz)
{
	int asize = ABS(a->m_size);
	int bsize = ABS(b->m_size);
	KInteger iah, ial, ibh, ibl;
	KInteger *ah = &iah;
	KInteger *al = &ial;
	KInteger *bh = &ibh;
	KInteger *bl = &ibl;
	KInteger *ret = &iz;
	KInteger it1, it2, it3;
	KInteger *t1 = &it1, *t2 = &it2, *t3 = &it3;
	int shift;           /* the number of digits we split off */
	int i;

	/* (ah*X+al)(bh*X+bl) = ah*bh*X*X + (ah*bl + al*bh)*X + al*bl
	* Let k = (ah+al)*(bh+bl) = ah*bl + al*bh  + ah*bh + al*bl
	* Then the original product is
	*     ah*bh*X*X + (k - ah*bh - al*bl)*X + al*bl
	* By picking X to be a power of 2, "*X" is just shifting, and it's
	* been reduced to 3 multiplies on numbers half the size.
	*/

	/* We want to split based on the larger number; fiddle so that b
	* is largest.
	*/
	if (asize > bsize) {
		const KInteger* t1 = a;
		a = b;
		b = t1;

		i = asize;
		asize = bsize;
		bsize = i;
	}

	/* Use gradeschool math when either number is too small. */
	i = a == b ? KARATSUBA_SQUARE_CUTOFF : KARATSUBA_CUTOFF;
	if (asize <= i) {
		if (asize == 0)
		{
// 			return (PyLongObject *)PyLong_FromLong(0);
			iz.ToZero();
			return;
		}
		else
		{
			x_mul(a, b, iz);
			return;
		}
	}

	/* If a is small compared to b, splitting on b gives a degenerate
	* case with ah==0, and Karatsuba may be (even much) less efficient
	* than "grade school" then.  However, we can still win, by viewing
	* b as a string of "big digits", each of width a->ob_size.  That
	* leads to a sequence of balanced calls to k_mul.
	*/
	if (2 * asize <= bsize)
	{
		k_lopsided_mul(a, b, iz);
		return;
	}

	/* Split a & b into hi & lo pieces. */
	shift = bsize >> 1;
// 	if (kmul_split(a, shift, &ah, &al) < 0) goto fail;
	kmul_split(a, shift, *ah, *bh);
	assert(ah->m_size > 0);            /* the split isn't degenerate */

	if (a == b) {
		bh = ah;
		bl = al;
// 		Py_INCREF(bh);
// 		Py_INCREF(bl);
	}
// 	else if (kmul_split(b, shift, &bh, &bl) < 0) goto fail;
	else kmul_split(b, shift, *bh, *bl);

	/* The plan:
	* 1. Allocate result space (asize + bsize digits:  that's always
	*    enough).
	* 2. Compute ah*bh, and copy into result at 2*shift.
	* 3. Compute al*bl, and copy into result at 0.  Note that this
	*    can't overlap with #2.
	* 4. Subtract al*bl from the result, starting at shift.  This may
	*    underflow (borrow out of the high digit), but we don't care:
	*    we're effectively doing unsigned arithmetic mod
	*    BASE**(sizea + sizeb), and so long as the *final* result fits,
	*    borrows and carries out of the high digit can be ignored.
	* 5. Subtract ah*bh from the result, starting at shift.
	* 6. Compute (ah+al)*(bh+bl), and add it into the result starting
	*    at shift.
	*/

	/* 1. Allocate result space. */
// 	ret = _PyLong_New(asize + bsize);
// 	if (ret == NULL) goto fail;
	ret->_Alloc(asize + bsize);
// #ifdef Py_DEBUG
// 	/* Fill with trash, to catch reference to uninitialized digits. */
// 	memset(ret->ob_digit, 0xDF, Py_SIZE(ret) * sizeof(digit));
// #endif

	/* 2. t1 <- ah*bh, and copy into high digits of result. */
// 	if ((t1 = k_mul(ah, bh)) == NULL) goto fail;
	k_mul(ah, bh, *t1);
	assert(t1->m_size >= 0);
	assert(2*shift + t1->m_size <= ret->m_size);
	memcpy(ret->ob_digit + 2*shift, t1->ob_digit,
		t1->m_size * sizeof(digit));

	/* Zero-out the digits higher than the ah*bh copy. */
	i = ret->m_size - 2*shift - t1->m_size;
	if (i)
		memset(ret->ob_digit + 2*shift + t1->m_size, 0,
		i * sizeof(digit));

	/* 3. t2 <- al*bl, and copy into the low digits. */
// 	if ((t2 = k_mul(al, bl)) == NULL) {
// 		Py_DECREF(t1);
// 		goto fail;
// 	}
	k_mul(al, bl, *t2);
	assert(t2->m_size >= 0);
	assert(t2->m_size <= 2*shift); /* no overlap with high digits */
	memcpy(ret->ob_digit, t2->ob_digit, t2->m_size * sizeof(digit));

	/* Zero out remaining digits. */
	i = 2*shift - t2->m_size;          /* number of uninitialized digits */
	if (i)
		memset(ret->ob_digit + t2->m_size, 0, i * sizeof(digit));

	/* 4 & 5. Subtract ah*bh (t1) and al*bl (t2).  We do al*bl first
	* because it's fresher in cache.
	*/
	i = ret->m_size - shift;  /* # digits after shift */
	(void)v_isub(ret->ob_digit + shift, i, t2->ob_digit, t2->m_size);
// 	Py_DECREF(t2);

	(void)v_isub(ret->ob_digit + shift, i, t1->ob_digit, t1->m_size);
// 	Py_DECREF(t1);

	/* 6. t3 <- (ah+al)(bh+bl), and add into result. */
// 	if ((t1 = x_add(ah, al)) == NULL) goto fail;
	x_add(ah, al, *t1);
// 	Py_DECREF(ah);
// 	Py_DECREF(al);
	ah = al = NULL;

	if (a == b) {
		t2 = t1;
// 		Py_INCREF(t2);
	}
// 	else if ((t2 = x_add(bh, bl)) == NULL) {
// 		Py_DECREF(t1);
// 		goto fail;
// 	}
	else
	{
		x_add(bh, bl, *t2);
		return;
	}
// 	Py_DECREF(bh);
// 	Py_DECREF(bl);
	bh = bl = NULL;

// 	t3 = k_mul(t1, t2);
	k_mul(t1, t2, *t3);
// 	Py_DECREF(t1);
// 	Py_DECREF(t2);
// 	if (t3 == NULL) goto fail;
	assert(t3->m_size >= 0);

	/* Add t3.  It's not obvious why we can't run out of room here.
	* See the (*) comment after this function.
	*/
	(void)v_iadd(ret->ob_digit + shift, i, t3->ob_digit, t3->m_size);
// 	Py_DECREF(t3);

	long_normalize(ret);

// fail:
// 	Py_XDECREF(ret);
// 	Py_XDECREF(ah);
// 	Py_XDECREF(al);
// 	Py_XDECREF(bh);
// 	Py_XDECREF(bl);
// 	return NULL;
}

KInteger KInteger::operator*( const KInteger& o ) const
{
	KInteger z;
	const KInteger* a = this;
	const KInteger* b = &o;

	k_mul(a, b, z);
	/* Negate if exactly one of the inputs is negative. */
	if ((a->m_size ^ b->m_size) < 0)
		NEGATE(&z);
	return z;
}

/* Divide long pin, w/ size digits, by non-zero digit n, storing quotient
   in pout, and returning the remainder.  pin and pout point at the LSD.
   It's OK for pin == pout on entry, which saves oodles of mallocs/frees in
   _PyLong_Format, but that should be done with great care since longs are
   immutable. */

static digit
inplace_divrem1(digit *pout, digit *pin, int size, digit n)
{
	twodigits rem = 0;

	assert(n > 0 && n <= PyLong_MASK);
	pin += size;
	pout += size;
	while (--size >= 0) {
		digit hi;
		rem = (rem << PyLong_SHIFT) | *--pin;
		*--pout = hi = (digit)(rem / n);
		rem -= (twodigits)hi * n;
	}
	return (digit)rem;
}

/* Divide a long integer by a digit, returning both the quotient
   (as function result) and the remainder (through *prem).
   The sign of a is ignored; n should not be zero. */

static void
divrem1(const KInteger* a, digit n, KInteger& resdiv, digit *prem)
{
	const int size = ABS(a->m_size);

	assert(n > 0 && n <= PyLong_MASK);
// 	z = _PyLong_New(size);
// 	if (z == NULL)
// 		return NULL;
	resdiv._Alloc(size);
	*prem = inplace_divrem1(resdiv.ob_digit, a->ob_digit, size, n);
	long_normalize(&resdiv);
}


/* bits_in_digit(d) returns the unique integer k such that 2**(k-1) <= d <
   2**k if d is nonzero, else 0. */

static const unsigned char BitLengthTable[32] = {
	0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
};

static int
bits_in_digit(digit d)
{
	int d_bits = 0;
	while (d >= 32) {
		d_bits += 6;
		d >>= 6;
	}
	d_bits += (int)BitLengthTable[d];
	return d_bits;
}

/* Shift digit vector a[0:m] d bits left, with 0 <= d < PyLong_SHIFT.  Put
 * result in z[0:m], and return the d bits shifted out of the top.
 */
static digit
v_lshift(digit *z, digit *a, int m, int d)
{
    int i;
    digit carry = 0;

    assert(0 <= d && d < PyLong_SHIFT);
    for (i=0; i < m; i++) {
        twodigits acc = (twodigits)a[i] << d | carry;
        z[i] = (digit)acc & PyLong_MASK;
        carry = (digit)(acc >> PyLong_SHIFT);
    }
    return carry;
}

/* Shift digit vector a[0:m] d bits right, with 0 <= d < PyLong_SHIFT.  Put
 * result in z[0:m], and return the d bits shifted out of the bottom.
 */
static digit
v_rshift(digit *z, digit *a, int m, int d)
{
    int i;
    digit carry = 0;
    digit mask = ((digit)1 << d) - 1U;

    assert(0 <= d && d < PyLong_SHIFT);
    for (i=m; i-- > 0;) {
        twodigits acc = (twodigits)carry << PyLong_SHIFT | a[i];
        carry = (digit)acc & mask;
        z[i] = (digit)(acc >> d);
    }
    return carry;
}


/* Unsigned long division with remainder -- the algorithm.  The arguments v1
   and w1 should satisfy 2 <= ABS(Py_SIZE(w1)) <= ABS(Py_SIZE(v1)). */

static void
x_divrem(const KInteger* v1, const KInteger* w1, KInteger& resdiv, KInteger& resrem)
{
	KInteger *v, *w, *a;
	KInteger temp;
	v = &temp;
	w = &resrem;
	a = &resdiv;
	int i, k, size_v, size_w;
	int d;
	digit wm1, wm2, carry, q, r, vtop, *v0, *vk, *w0, *ak;
	twodigits vv;
	sdigit zhi;
	stwodigits z;

	/* We follow Knuth [The Art of Computer Programming, Vol. 2 (3rd
	   edn.), section 4.3.1, Algorithm D], except that we don't explicitly
	   handle the special case when the initial estimate q for a quotient
	   digit is >= PyLong_BASE: the max value for q is PyLong_BASE+1, and
	   that won't overflow a digit. */

	/* allocate space; w will also be used to hold the final remainder */
	size_v = ABS(v1->m_size);
	size_w = ABS(w1->m_size);
	assert(size_v >= size_w && size_w >= 2); /* Assert checks by div() */
// 	v = _PyLong_New(size_v+1);
// 	if (v == NULL) {
// 		*prem = NULL;
// 		return NULL;
// 	}
	v->_Alloc(size_v + 1);
// 	w = _PyLong_New(size_w);
// 	if (w == NULL) {
// 		Py_DECREF(v);
// 		*prem = NULL;
// 		return NULL;
// 	}
	w->_Alloc(size_w);

	/* normalize: shift w1 left so that its top digit is >= PyLong_BASE/2.
	   shift v1 left by the same amount.  Results go into w and v. */
	d = PyLong_SHIFT - bits_in_digit(w1->ob_digit[size_w-1]);
	carry = v_lshift(w->ob_digit, w1->ob_digit, size_w, d);
	assert(carry == 0);
	carry = v_lshift(v->ob_digit, v1->ob_digit, size_v, d);
	if (carry != 0 || v->ob_digit[size_v-1] >= w->ob_digit[size_w-1]) {
		v->ob_digit[size_v] = carry;
		size_v++;
	}

	/* Now v->ob_digit[size_v-1] < w->ob_digit[size_w-1], so quotient has
	   at most (and usually exactly) k = size_v - size_w digits. */
	k = size_v - size_w;
	assert(k >= 0);
// 	a = _PyLong_New(k);
// 	if (a == NULL) {
// 		Py_DECREF(w);
// 		Py_DECREF(v);
// 		*prem = NULL;
// 		return NULL;
// 	}
	a->_Alloc(k);
	v0 = v->ob_digit;
	w0 = w->ob_digit;
	wm1 = w0[size_w-1];
	wm2 = w0[size_w-2];
	for (vk = v0+k, ak = a->ob_digit + k; vk-- > v0;) {
		/* inner loop: divide vk[0:size_w+1] by w0[0:size_w], giving
		   single-digit quotient q, remainder in vk[0:size_w]. */

// 		SIGCHECK({
// 			Py_DECREF(a);
// 			Py_DECREF(w);
// 			Py_DECREF(v);
// 			*prem = NULL;
// 			return NULL;
// 		});

		/* estimate quotient digit q; may overestimate by 1 (rare) */
		vtop = vk[size_w];
		assert(vtop <= wm1);
		vv = ((twodigits)vtop << PyLong_SHIFT) | vk[size_w-1];
		q = (digit)(vv / wm1);
		r = (digit)(vv - (twodigits)wm1 * q); /* r = vv % wm1 */
		while ((twodigits)wm2 * q > (((twodigits)r << PyLong_SHIFT)
			| vk[size_w-2])) {
				--q;
				r += wm1;
				if (r >= PyLong_BASE)
					break;
		}
		assert(q <= PyLong_BASE);

		/* subtract q*w0[0:size_w] from vk[0:size_w+1] */
		zhi = 0;
		for (i = 0; i < size_w; ++i) {
			/* invariants: -PyLong_BASE <= -q <= zhi <= 0;
			-PyLong_BASE * q <= z < PyLong_BASE */
			z = (sdigit)vk[i] + zhi -
				(stwodigits)q * (stwodigits)w0[i];
			vk[i] = (digit)z & PyLong_MASK;
// 			zhi = (sdigit)Py_ARITHMETIC_RIGHT_SHIFT(stwodigits,
// 				z, PyLong_SHIFT);
			zhi =  (sdigit)(z >> PyLong_SHIFT);
		}

		/* add w back if q was too large (this branch taken rarely) */
		assert((sdigit)vtop + zhi == -1 || (sdigit)vtop + zhi == 0);
		if ((sdigit)vtop + zhi < 0) {
			carry = 0;
			for (i = 0; i < size_w; ++i) {
				carry += vk[i] + w0[i];
				vk[i] = carry & PyLong_MASK;
				carry >>= PyLong_SHIFT;
			}
			--q;
		}

		/* store quotient digit */
		assert(q < PyLong_BASE);
		*--ak = q;
	}

	/* unshift remainder; we reuse w to store the result */
	carry = v_rshift(w0, v0, size_w, d);
	assert(carry==0);
// 	Py_DECREF(v);

	long_normalize(w);
	long_normalize(a);
}

/* Long division with remainder, top-level routine */

static void
long_divrem(const KInteger* a, const KInteger* b,
			KInteger& resdiv, KInteger& resrem)
{
	int size_a = ABS(a->m_size), size_b = ABS(b->m_size);
// 	PyLongObject *z;

	if (size_b == 0) {
// 		PyErr_SetString(PyExc_ZeroDivisionError,
// 			"integer division or modulo by zero");
// 		return -1;
		ASSERT_I(false);
		return;
	}
	if (size_a < size_b ||
		(size_a == size_b &&
		a->ob_digit[size_a-1] < b->ob_digit[size_b-1])) {
			/* |a| < |b|. */
// 			*pdiv = (PyLongObject*)PyLong_FromLong(0);
// 			if (*pdiv == NULL)
// 				return -1;
// 			Py_INCREF(a);
// 			*prem = (PyLongObject *) a;
// 			return 0;
			resdiv.ToZero();
			resrem = *a;
			return;
	}
	if (size_b == 1) {
		digit rem = 0;
// 		z = divrem1(a, b->ob_digit[0], &rem);
// 		if (z == NULL)
// 			return -1;
		divrem1(a, b->ob_digit[0], resdiv, &rem);
// 		*prem = (PyLongObject *) PyLong_FromLong((long)rem);
// 		if (*prem == NULL) {
// 			Py_DECREF(z);
// 			return -1;
// 		}
		resrem.fromInt((int)rem);
	}
	else {
// 		z = x_divrem(a, b, prem);
// 		if (z == NULL)
// 			return -1;
		x_divrem(a, b, resdiv, resrem);
	}
	/* Set the signs.
	The quotient z has the sign of a*b;
	the remainder r has the sign of a,
	so a = b*z + r. */
	if ((a->m_size < 0) != (b->m_size < 0))
		NEGATE(&resdiv);
	if (a->m_size < 0 && resrem.m_size != 0)
		NEGATE(&resrem);
// 	*pdiv = maybe_small_long(z);
// 	return 0;
}


/* The / and % operators are now defined in terms of divmod().
   The expression a mod b has the value a - b*floor(a/b).
   The long_divrem function gives the remainder after division of
   |a| by |b|, with the sign of a.  This is also expressed
   as a - b*trunc(a/b), if trunc truncates towards zero.
   Some examples:
     a           b      a rem b         a mod b
     13          10      3               3
    -13          10     -3               7
     13         -10      3              -7
    -13         -10     -3              -3
   So, to get from rem to mod, we have to add b if a and b
   have different signs.  We then subtract one from the 'div'
   part of the outcome to keep the invariant intact. */

/* Compute
 *     *pdiv, *pmod = divmod(v, w)
 * NULL can be passed for pdiv or pmod, in which case that part of
 * the result is simply thrown away.  The caller owns a reference to
 * each of these it requests (does not pass NULL for).
 */
static void
l_divmod(const KInteger* v, const KInteger* w,
		 KInteger& resdiv, KInteger& resmod)
{
	long_divrem(v, w, resdiv, resmod);
	if ((resmod.m_size < 0 && w->m_size > 0) ||
		(resmod.m_size > 0 && w->m_size < 0)) {
			KInteger one;
			one.ToOne();
			resmod = resmod + *w;
// 			if (mod == NULL) {
// 				Py_DECREF(div);
// 				return -1;
// 			}
// 			one = (PyLongObject *) PyLong_FromLong(1L);
// 			if (one == NULL ||
// 				(temp = (PyLongObject *) long_sub(div, one)) == NULL) {
// 					Py_DECREF(mod);
// 					Py_DECREF(div);
// 					Py_XDECREF(one);
// 					return -1;
// 			}
// 			Py_DECREF(one);
// 			Py_DECREF(div);
			resdiv = resdiv - one;
	}
// 	if (pdiv != NULL)
// 		*pdiv = div;
// 	else
// 		Py_DECREF(div);
// 
// 	if (pmod != NULL)
// 		*pmod = mod;
// 	else
// 		Py_DECREF(mod);

// 	return 0;
}

KInteger KInteger::operator%( const KInteger& o ) const
{
	KInteger div;
	KInteger mod;
	const KInteger* a = this;
	const KInteger* b = &o;

	l_divmod(a, b, div, mod);
	return mod;
}

KInteger KInteger::operator/( const KInteger& o ) const
{
	KInteger div;
	KInteger mod;
	const KInteger* a = this;
	const KInteger* b = &o;

	l_divmod(a, b, div, mod);
	return div;
}

void KInteger::Divmod( const KInteger& o, KInteger& div, KInteger& mod ) const
{
	l_divmod(this, &o, div, mod);
}

KInteger KInteger::operator-() const
{
	KInteger result;
	if (m_size == 0)
	{
		result.ToZero();
	}
	else
	{
		result = *this;
		result.m_size = -result.m_size;
	}
	return result;
}

KInteger KInteger::operator+() const
{
	return *this;
}

KInteger::operator bool() const
{
	return m_size != 0;
}
/*
std::string KInteger::toString() const
{
	// 一个简单的方法，每次除出来一个int能放下的10进制数，再把各个位输出
	std::string result = "0";
	if (!m_size) return result;
	std::stringstream ss;
	KInteger d1 = *this;
	KInteger div;
	div._Alloc(ABS(m_size));	// 预分配好足够用的内存
	// 用两个指针做交换除，省的做拷贝
	KInteger* p1 = &d1;
	KInteger* p2 = &div;
	KInteger* temp;
	const static digit TO_DIV = _PyLong_DECIMAL_BASE;	// 给一个digit能放下的十的幂，越大越好
	digit rem;
	while(p1->m_size)
	{
		divrem1(p1, TO_DIV, *p2, &rem);
		if (p2->m_size)	// 还有商
		{
			for (int i = 0; i < 9; i++)	// 宽度要和TO_DIV里面的0的个数一致
			{
				ss.put(rem % 10 + '0');
				rem /= 10;
			}
		}
		else	// 只剩余数了，要去掉多余的0
		{
			while (rem)
			{
				ss.put(rem % 10 + '0');
				rem /= 10;
			}
		}
		// 交换p1/p2
		temp = p1;
		p1 = p2;
		p2 = temp;
	}

	if (m_size < 0) ss.put('-');
	result = ss.str();
	reverse(result.begin(), result.end()); 
	return result;
}
*/
std::string KInteger::toString() const
{
	KInteger scratch;
	const KInteger* a = this;
	int size, strlen, size_a, i, j;
	digit *pout, *pin, rem, tenpow;
	int negative;

	size_a = ABS(a->m_size);
	negative = a->m_size < 0;

	/* quick and dirty upper bound for the number of digits
	   required to express a in base _PyLong_DECIMAL_BASE:

	   #digits = 1 + floor(log2(a) / log2(_PyLong_DECIMAL_BASE))

	   But log2(a) < size_a * PyLong_SHIFT, and
	   log2(_PyLong_DECIMAL_BASE) = log2(10) * _PyLong_DECIMAL_SHIFT
	   > 3 * _PyLong_DECIMAL_SHIFT
	*/
// 	if (size_a > PY_SSIZE_T_MAX / PyLong_SHIFT) {
// 		PyErr_SetString(PyExc_OverflowError,
// 			"long is too large to format");
// 		return -1;
// 	}
	/* the expression size_a * PyLong_SHIFT is now safe from overflow */
	size = 1 + size_a * PyLong_SHIFT / (3 * _PyLong_DECIMAL_SHIFT);
// 	scratch = _PyLong_New(size);
// 	if (scratch == NULL)
// 		return -1;
	scratch._Alloc(size);

	/* convert array of base _PyLong_BASE digits in pin to an array of
	   base _PyLong_DECIMAL_BASE digits in pout, following Knuth (TAOCP,
	   Volume 2 (3rd edn), section 4.4, Method 1b). */
	pin = a->ob_digit;
	pout = scratch.ob_digit;
	size = 0;
	for (i = size_a; --i >= 0; ) {
		digit hi = pin[i];
		for (j = 0; j < size; j++) {
			twodigits z = (twodigits)pout[j] << PyLong_SHIFT | hi;
			hi = (digit)(z / _PyLong_DECIMAL_BASE);
			pout[j] = (digit)(z - (twodigits)hi *
				_PyLong_DECIMAL_BASE);
		}
		while (hi) {
			pout[size++] = hi % _PyLong_DECIMAL_BASE;
			hi /= _PyLong_DECIMAL_BASE;
		}
		/* check for keyboard interrupt */
// 		SIGCHECK({
// 			Py_DECREF(scratch);
// 			return -1;
// 		});
	}
	/* pout should have at least one digit, so that the case when a = 0
	   works correctly */
	if (size == 0)
		pout[size++] = 0;

	/* calculate exact length of output string, and allocate */
	strlen = negative + 1 + (size - 1) * _PyLong_DECIMAL_SHIFT;
	tenpow = 10;
	rem = pout[size-1];
	while (rem >= tenpow) {
		tenpow *= 10;
		strlen++;
	}
	char* str = new char[strlen + 1];
	str[strlen] = 0;

	char* p = str + strlen;
	/* pout[0] through pout[size-2] contribute exactly
	   _PyLong_DECIMAL_SHIFT digits each */
	for (i=0; i < size - 1; i++) {
		rem = pout[i];
		for (j = 0; j < _PyLong_DECIMAL_SHIFT; j++) {
			*--p = '0' + rem % 10;
			rem /= 10;
		}
	}
	/* pout[size-1]: always produce at least one decimal digit */
	rem = pout[i];
	do {
		*--p = '0' + rem % 10;
		rem /= 10;
	} while (rem != 0);

	/* and sign */
	if (negative)
		*--p = '-';

	/* check we've counted correctly */
	assert(p == str);

	std::string result = str;
	delete [] str;
	return result;
}

std::string KInteger::toString( int base, int alternate /*= 0*/ ) const
{
	if (base == 10) return toString();
	const KInteger* a = this;
	char* v = NULL;
	int sz;
	int size_a;
	int negative;
	int bits;

	assert(base == 2 || base == 8 || base == 16);
// 	if (a == NULL || !PyLong_Check(a)) {
// 		PyErr_BadInternalCall();
// 		return -1;
// 	}
	size_a = ABS(a->m_size);
	negative = a->m_size < 0;

	/* Compute a rough upper bound for the length of the string */
	switch (base) {
	case 16:
		bits = 4;
		break;
	case 8:
		bits = 3;
		break;
	case 2:
		bits = 1;
		break;
	default:
		assert(0); /* shouldn't ever get here */
		bits = 0; /* to silence gcc warning */
	}

	/* Compute exact length 'sz' of output string. */
	if (size_a == 0) {
		sz = 1;
	}
	else {
		int size_a_in_bits;
		/* Ensure overflow doesn't occur during computation of sz. */
		if (size_a > (PY_SSIZE_T_MAX - 3) / PyLong_SHIFT) {
// 			PyErr_SetString(PyExc_OverflowError,
// 				"int is too large to format");
			ASSERT_I(false);
			return "";
		}
		size_a_in_bits = (size_a - 1) * PyLong_SHIFT +
			bits_in_digit(a->ob_digit[size_a - 1]);
		/* Allow 1 character for a '-' sign. */
		sz = negative + (size_a_in_bits + (bits - 1)) / bits;
	}
	if (alternate) {
		/* 2 characters for prefix  */
		sz += 2;
	}

	v = new char[sz + 1];
	v[sz] = 0;

	char* p = v + sz;
	if (size_a == 0) {
		*--p = '0';
	}
	else {
		/* JRH: special case for power-of-2 bases */
		twodigits accum = 0;
		int accumbits = 0;   /* # of bits in accum */
		int i;
		for (i = 0; i < size_a; ++i) {
			accum |= (twodigits)a->ob_digit[i] << accumbits;
			accumbits += PyLong_SHIFT;
			assert(accumbits >= bits);
			do {
				char cdigit;
				cdigit = (char)(accum & (base - 1));
				cdigit += (cdigit < 10) ? '0' : 'a'-10;
				*--p = cdigit;
				accumbits -= bits;
				accum >>= bits;
			} while (i < size_a-1 ? accumbits >= bits : accum > 0);
		}
	}
	if (alternate) {
		if (base == 16)
			*--p = 'x';
		else if (base == 8)
			*--p = 'o';
		else /* (base == 2) */
			*--p = 'b';
		*--p = '0';
	}
	if (negative)
		*--p = '-';
	ASSERT_I(p == v);

	std::string str(v);
	delete [] v;
	return str;
}

static int
long_compare(const KInteger* a, const KInteger* b)
{
	int sign;

	if (a->m_size != b->m_size) {
		sign = a->m_size - b->m_size;
	}
	else {
		int i = ABS(a->m_size);
		while (--i >= 0 && a->ob_digit[i] == b->ob_digit[i])
			;
		if (i < 0)
			sign = 0;
		else {
			sign = (sdigit)a->ob_digit[i] - (sdigit)b->ob_digit[i];
			if (a->m_size < 0)
				sign = -sign;
		}
	}
	return sign < 0 ? -1 : sign > 0 ? 1 : 0;
}


bool KInteger::operator<( const KInteger& o ) const
{
	int result = long_compare(this, &o);
	return result < 0;
}

bool KInteger::operator<=( const KInteger& o ) const
{
	int result = long_compare(this, &o);
	return result <= 0;
}

bool KInteger::operator==( const KInteger& o ) const
{
	int result = long_compare(this, &o);
	return result == 0;
}

bool KInteger::operator>=( const KInteger& o ) const
{
	int result = long_compare(this, &o);
	return result >= 0;
}

bool KInteger::operator>( const KInteger& o ) const
{
	int result = long_compare(this, &o);
	return result > 0;
}

bool KInteger::operator!=( const KInteger& o ) const
{
	int result = long_compare(this, &o);
	return result != 0;
}

static void
long_lshift(const KInteger* a, int shiftby, KInteger& result)
{
	/* This version due to Tim Peters */
	KInteger* z = &result;
	int oldsize, newsize, wordshift, remshift, i, j;
	twodigits accum;

// 	shiftby = PyLong_AsSsize_t((PyObject *)b);
// 	if (shiftby == -1L && PyErr_Occurred())
// 		goto lshift_error;
// 	if (shiftby < 0) {
// 		PyErr_SetString(PyExc_ValueError, "negative shift count");
// 		goto lshift_error;
// 	}
	if (shiftby < 0)
	{
		ASSERT_I(false);
		return;
	}
	/* wordshift, remshift = divmod(shiftby, PyLong_SHIFT) */
	wordshift = shiftby / PyLong_SHIFT;
	remshift  = shiftby - wordshift * PyLong_SHIFT;

	oldsize = ABS(a->m_size);
	newsize = oldsize + wordshift;
	if (remshift)
		++newsize;
// 	z = _PyLong_New(newsize);
// 	if (z == NULL)
// 		goto lshift_error;
	z->_Alloc(newsize);
	if (a->m_size < 0)
		NEGATE(z);
	for (i = 0; i < wordshift; i++)
		z->ob_digit[i] = 0;
	accum = 0;
	for (i = wordshift, j = 0; j < oldsize; i++, j++) {
		accum |= (twodigits)a->ob_digit[j] << remshift;
		z->ob_digit[i] = (digit)(accum & PyLong_MASK);
		accum >>= PyLong_SHIFT;
	}
	if (remshift)
		z->ob_digit[newsize-1] = (digit)accum;
	else
		assert(!accum);
	long_normalize(z);
}

KInteger KInteger::operator<<( int v ) const
{
	KInteger result;
	long_lshift(this, v, result);
	return result;
}


/**
 * 计算反码
 */
static void
long_invert(const KInteger* v, KInteger& result)
{
	/* Implement ~x as -(x+1) */
// 	PyLongObject *x;
// 	PyLongObject *w;
// 	if (ABS(Py_SIZE(v)) <=1)
// 		return PyLong_FromLong(-(MEDIUM_VALUE(v)+1));
// 	w = (PyLongObject *)PyLong_FromLong(1L);
// 	if (w == NULL)
// 		return NULL;
// 	x = long_add(v, w);
// 	Py_DECREF(w);
// 	if (x == NULL)
// 		return NULL;
// 	Py_SIZE(x) = -(Py_SIZE(x));
// 	return (PyObject *)maybe_small_long(x);
	KInteger one(1);
	result = *v + one;
	result.m_size = -result.m_size;
}

static void
long_rshift(const KInteger* a, int shiftby, KInteger& result)
{
	KInteger* z = &result;
	int newsize, wordshift, loshift, hishift, i, j;
	digit lomask, himask;

	if (a->m_size < 0) {
		/* Right shifting negative numbers is harder */
// 		PyLongObject *a1, *a2;
		KInteger a1, a2;
// 		a1 = (PyLongObject *) long_invert(a);
// 		if (a1 == NULL)
// 			goto rshift_error;
		long_invert(a, a1);
// 		a2 = (PyLongObject *) long_rshift(a1, b);
// 		Py_DECREF(a1);
// 		if (a2 == NULL)
// 			goto rshift_error;
		long_rshift(&a1, shiftby, a2);
// 		z = (PyLongObject *) long_invert(a2);
// 		Py_DECREF(a2);
		long_invert(&a2, result);
	}
	else {
// 		shiftby = PyLong_AsSsize_t((PyObject *)b);
// 		if (shiftby == -1L && PyErr_Occurred())
// 			goto rshift_error;
		if (shiftby < 0) {
// 			PyErr_SetString(PyExc_ValueError,
// 				"negative shift count");
// 			goto rshift_error;
			return;
		}
		wordshift = shiftby / PyLong_SHIFT;
		newsize = ABS(a->m_size) - wordshift;
		if (newsize <= 0)
		{
// 			return PyLong_FromLong(0);
			result.ToZero();
			return;
		}
		loshift = shiftby % PyLong_SHIFT;
		hishift = PyLong_SHIFT - loshift;
		lomask = ((digit)1 << hishift) - 1;
		himask = PyLong_MASK ^ lomask;
// 		z = _PyLong_New(newsize);
// 		if (z == NULL)
// 			goto rshift_error;
		z->_Alloc(newsize);
		if (a->m_size < 0)
			z->m_size = -z->m_size;
		for (i = 0, j = wordshift; i < newsize; i++, j++) {
			z->ob_digit[i] = (a->ob_digit[j] >> loshift) & lomask;
			if (i+1 < newsize)
				z->ob_digit[i] |= (a->ob_digit[j+1] << hishift) & himask;
		}
		long_normalize(z);
	}
}

KInteger KInteger::operator>>( int v ) const
{
	KInteger result;
	long_rshift(this, v, result);
	return result;
}



/* Compute two's complement of digit vector a[0:m], writing result to
   z[0:m].  The digit vector a need not be normalized, but should not
   be entirely zero.  a and z may point to the same digit vector. */

static void
v_complement(digit *z, digit *a, int m)
{
	int i;
	digit carry = 1;
	for (i = 0; i < m; ++i) {
		carry += a[i] ^ PyLong_MASK;
		z[i] = carry & PyLong_MASK;
		carry >>= PyLong_SHIFT;
	}
	assert(carry == 0);
}


/* Bitwise and/xor/or operations */

static void
long_bitwise(const KInteger* a,
			 char op,  /* '&', '|', '^' */
			 const KInteger* b,
			 KInteger& result
			 )
{
	int nega, negb, negz;
	int size_a, size_b, size_z, i;
	KInteger* z = &result;
	// 处理负数用的两个变量
	KInteger handleNegA;
	KInteger handleNegB;

	/* Bitwise operations for negative numbers operate as though
	   on a two's complement representation.  So convert arguments
	   from sign-magnitude to two's complement, and convert the
	   result back to sign-magnitude at the end. */

	/* If a is negative, replace it by its two's complement. */
	size_a = ABS(a->m_size);
	nega = a->m_size < 0;
	if (nega) {
// 		z = _PyLong_New(size_a);
// 		if (z == NULL)
// 			return NULL;
// 		z->_Alloc(size_a);
// 		v_complement(z->ob_digit, a->ob_digit, size_a);
// 		a = z;
		handleNegA._Alloc(size_a);
		v_complement(handleNegA.ob_digit, a->ob_digit, size_a);
		a = &handleNegA;
	}
// 	else
// 		/* Keep reference count consistent. */
// 		Py_INCREF(a);

	/* Same for b. */
	size_b = ABS(b->m_size);
	negb = b->m_size < 0;
	if (negb) {
// 		z = _PyLong_New(size_b);
// 		if (z == NULL) {
// 			Py_DECREF(a);
// 			return NULL;
// 		}
// 		z->_Alloc(size_b);
// 		v_complement(z->ob_digit, b->ob_digit, size_b);
// 		b = z;
		handleNegB._Alloc(size_b);
		v_complement(handleNegB.ob_digit, b->ob_digit, size_b);
		b = &handleNegB;
	}
// 	else
// 		Py_INCREF(b);

	/* Swap a and b if necessary to ensure size_a >= size_b. */
	if (size_a < size_b) {
		const KInteger* z = a; a = b; b = z;
		size_z = size_a; size_a = size_b; size_b = size_z;
		negz = nega; nega = negb; negb = negz;
	}

	/* JRH: The original logic here was to allocate the result value (z)
	   as the longer of the two operands.  However, there are some cases
	   where the result is guaranteed to be shorter than that: AND of two
	   positives, OR of two negatives: use the shorter number.  AND with
	   mixed signs: use the positive number.  OR with mixed signs: use the
	   negative number.
	*/
	switch (op) {
	case '^':
		negz = nega ^ negb;
		size_z = size_a;
		break;
	case '&':
		negz = nega & negb;
		size_z = negb ? size_a : size_b;
		break;
	case '|':
		negz = nega | negb;
		size_z = negb ? size_b : size_a;
		break;
	default:
// 		PyErr_BadArgument();
		ASSERT_I(false);
		return;
	}

	/* We allow an extra digit if z is negative, to make sure that
	   the final two's complement of z doesn't overflow. */
// 	z = _PyLong_New(size_z + negz);
// 	if (z == NULL) {
// 		Py_DECREF(a);
// 		Py_DECREF(b);
// 		return NULL;
// 	}
	z->_Alloc(size_z + negz);

	/* Compute digits for overlap of a and b. */
	switch(op) {
	case '&':
		for (i = 0; i < size_b; ++i)
			z->ob_digit[i] = a->ob_digit[i] & b->ob_digit[i];
		break;
	case '|':
		for (i = 0; i < size_b; ++i)
			z->ob_digit[i] = a->ob_digit[i] | b->ob_digit[i];
		break;
	case '^':
		for (i = 0; i < size_b; ++i)
			z->ob_digit[i] = a->ob_digit[i] ^ b->ob_digit[i];
		break;
	default:
// 		PyErr_BadArgument();
		ASSERT_I(false);
		return;
	}

	/* Copy any remaining digits of a, inverting if necessary. */
	if (op == '^' && negb)
		for (; i < size_z; ++i)
			z->ob_digit[i] = a->ob_digit[i] ^ PyLong_MASK;
	else if (i < size_z)
		memcpy(&z->ob_digit[i], &a->ob_digit[i],
		(size_z-i)*sizeof(digit));

	/* Complement result if negative. */
	if (negz) {
		z->m_size = -z->m_size;
		z->ob_digit[size_z] = PyLong_MASK;
		v_complement(z->ob_digit, z->ob_digit, size_z+1);
	}

// 	Py_DECREF(a);
// 	Py_DECREF(b);
// 	return (PyObject *)maybe_small_long(long_normalize(z));
	long_normalize(z);
}

KInteger KInteger::operator&( const KInteger& o ) const
{
	KInteger result;
	long_bitwise(this, '&', &o, result);
	return result;
}

KInteger KInteger::operator|( const KInteger& o ) const
{
	KInteger result;
	long_bitwise(this, '|', &o, result);
	return result;
}

KInteger KInteger::operator^( const KInteger& o ) const
{
	KInteger result;
	long_bitwise(this, '^', &o, result);
	return result;
}

KInteger KInteger::operator~() const
{
	KInteger result;
	long_invert(this, result);
	return result;
}

void
KInteger::_FromByteArray(const unsigned char* bytes, size_t n,
					  int little_endian, int is_signed)
{
	const unsigned char* pstartbyte;    /* LSB of bytes */
	int incr;                           /* direction to move pstartbyte */
	const unsigned char* pendbyte;      /* MSB of bytes */
	size_t numsignificantbytes;         /* number of bytes that matter */
	int ndigits;                        /* number of Python long digits */
	KInteger* v = this;                 /* result */
	int idigit = 0;                     /* next free index in v->ob_digit */

// 	if (n == 0)
// 		return PyLong_FromLong(0L);
	if (n == 0)
	{
		v->ToZero();
		return;
	}

	if (little_endian) {
		pstartbyte = bytes;
		pendbyte = bytes + n - 1;
		incr = 1;
	}
	else {
		pstartbyte = bytes + n - 1;
		pendbyte = bytes;
		incr = -1;
	}

	if (is_signed)
		is_signed = *pendbyte >= 0x80;

	/* Compute numsignificantbytes.  This consists of finding the most
	   significant byte.  Leading 0 bytes are insignificant if the number
	   is positive, and leading 0xff bytes if negative. */
	{
		size_t i;
		const unsigned char* p = pendbyte;
		const int pincr = -incr;  /* search MSB to LSB */
		const unsigned char insignficant = is_signed ? 0xff : 0x00;

		for (i = 0; i < n; ++i, p += pincr) {
			if (*p != insignficant)
				break;
		}
		numsignificantbytes = n - i;
		/* 2's-comp is a bit tricky here, e.g. 0xff00 == -0x0100, so
		   actually has 2 significant bytes.  OTOH, 0xff0001 ==
		   -0x00ffff, so we wouldn't *need* to bump it there; but we
		   do for 0xffff = -0x0001.  To be safe without bothering to
		   check every case, bump it regardless. */
		if (is_signed && numsignificantbytes < n)
			++numsignificantbytes;
	}

	/* How many Python long digits do we need?  We have
	   8*numsignificantbytes bits, and each Python long digit has
	   PyLong_SHIFT bits, so it's the ceiling of the quotient. */
	   /* catch overflow before it happens */
	if (numsignificantbytes > (PY_SSIZE_T_MAX - PyLong_SHIFT) / 8) {
// 		PyErr_SetString(PyExc_OverflowError,
// 			"byte array too long to convert to int");
// 		return NULL;
		ASSERT_I(false);
		v->ToZero();
		return;
	}
	ndigits = ((int)numsignificantbytes * 8 + PyLong_SHIFT - 1) / PyLong_SHIFT;
// 	v = _PyLong_New(ndigits);
// 	if (v == NULL)
// 		return NULL;
	v->_Alloc(ndigits);

	/* Copy the bits over.  The tricky parts are computing 2's-comp on
	   the fly for signed numbers, and dealing with the mismatch between
	   8-bit bytes and (probably) 15-bit Python digits.*/
	{
		size_t i;
		twodigits carry = 1;                    /* for 2's-comp calculation */
		twodigits accum = 0;                    /* sliding register */
		unsigned int accumbits = 0;             /* number of bits in accum */
		const unsigned char* p = pstartbyte;

		for (i = 0; i < numsignificantbytes; ++i, p += incr) {
			twodigits thisbyte = *p;
			/* Compute correction for 2's comp, if needed. */
			if (is_signed) {
				thisbyte = (0xff ^ thisbyte) + carry;
				carry = thisbyte >> 8;
				thisbyte &= 0xff;
			}
			/* Because we're going LSB to MSB, thisbyte is
			   more significant than what's already in accum,
			   so needs to be prepended to accum. */
			accum |= (twodigits)thisbyte << accumbits;
			accumbits += 8;
			if (accumbits >= PyLong_SHIFT) {
				/* There's enough to fill a Python digit. */
				assert(idigit < ndigits);
				v->ob_digit[idigit] = (digit)(accum & PyLong_MASK);
				++idigit;
				accum >>= PyLong_SHIFT;
				accumbits -= PyLong_SHIFT;
				assert(accumbits < PyLong_SHIFT);
			}
		}
		assert(accumbits < PyLong_SHIFT);
		if (accumbits) {
			assert(idigit < ndigits);
			v->ob_digit[idigit] = (digit)accum;
			++idigit;
		}
	}

	v->m_size = is_signed ? -idigit : idigit;
// 	return (PyObject *)long_normalize(v);
	long_normalize(v);
}

int
KInteger::_AsByteArray(unsigned char* bytes, size_t n,
					int little_endian, int is_signed) const
{
	int i;                      /* index into v->ob_digit */
	int ndigits;                /* |v->ob_size| */
	twodigits accum;            /* sliding register */
	unsigned int accumbits;     /* # bits in accum */
	int do_twos_comp;           /* store 2's-comp?  is_signed and v < 0 */
	digit carry;                /* for computing 2's-comp */
	size_t j;                   /* # bytes filled */
	unsigned char* p;           /* pointer to next byte in bytes */
	int pincr;                  /* direction to move p */
	const KInteger* v = this;

// 	assert(v != NULL && PyLong_Check(v));

	if (v->m_size < 0) {
		ndigits = -(v->m_size);
		if (!is_signed) {
// 			PyErr_SetString(PyExc_OverflowError,
// 				"can't convert negative int to unsigned");
			return -1;
		}
		do_twos_comp = 1;
	}
	else {
		ndigits = v->m_size;
		do_twos_comp = 0;
	}

	if (little_endian) {
		p = bytes;
		pincr = 1;
	}
	else {
		p = bytes + n - 1;
		pincr = -1;
	}

	/* Copy over all the Python digits.
	   It's crucial that every Python digit except for the MSD contribute
	   exactly PyLong_SHIFT bits to the total, so first assert that the long is
	   normalized. */
	assert(ndigits == 0 || v->ob_digit[ndigits - 1] != 0);
	j = 0;
	accum = 0;
	accumbits = 0;
	carry = do_twos_comp ? 1 : 0;
	for (i = 0; i < ndigits; ++i) {
		digit thisdigit = v->ob_digit[i];
		if (do_twos_comp) {
			thisdigit = (thisdigit ^ PyLong_MASK) + carry;
			carry = thisdigit >> PyLong_SHIFT;
			thisdigit &= PyLong_MASK;
		}
		/* Because we're going LSB to MSB, thisdigit is more
		   significant than what's already in accum, so needs to be
		   prepended to accum. */
		accum |= (twodigits)thisdigit << accumbits;

		/* The most-significant digit may be (probably is) at least
		   partly empty. */
		if (i == ndigits - 1) {
			/* Count # of sign bits -- they needn't be stored,
			 * although for signed conversion we need later to
			 * make sure at least one sign bit gets stored. */
			digit s = do_twos_comp ? thisdigit ^ PyLong_MASK : thisdigit;
			while (s != 0) {
				s >>= 1;
				accumbits++;
			}
		}
		else
			accumbits += PyLong_SHIFT;

		/* Store as many bytes as possible. */
		while (accumbits >= 8) {
			if (j >= n)
				goto Overflow;
			++j;
			*p = (unsigned char)(accum & 0xff);
			p += pincr;
			accumbits -= 8;
			accum >>= 8;
		}
	}

	/* Store the straggler (if any). */
	assert(accumbits < 8);
	assert(carry == 0);  /* else do_twos_comp and *every* digit was 0 */
	if (accumbits > 0) {
		if (j >= n)
			goto Overflow;
		++j;
		if (do_twos_comp) {
			/* Fill leading bits of the byte with sign bits
			   (appropriately pretending that the long had an
			   infinite supply of sign bits). */
			accum |= (~(twodigits)0) << accumbits;
		}
		*p = (unsigned char)(accum & 0xff);
		p += pincr;
	}
	else if (j == n && n > 0 && is_signed) {
		/* The main loop filled the byte array exactly, so the code
		   just above didn't get to ensure there's a sign bit, and the
		   loop below wouldn't add one either.  Make sure a sign bit
		   exists. */
		unsigned char msb = *(p - pincr);
		int sign_bit_set = msb >= 0x80;
		assert(accumbits == 0);
		if (sign_bit_set == do_twos_comp)
			return 0;
		else
			goto Overflow;
	}

	/* Fill remaining bytes with copies of the sign bit. */
	{
		unsigned char signbyte = do_twos_comp ? 0xffU : 0U;
		for ( ; j < n; ++j, p += pincr)
			*p = signbyte;
	}

	return 0;

Overflow:
// 	PyErr_SetString(PyExc_OverflowError, "int too big to convert");
	return -1;

}

/* Table of digit values for 8-bit string -> integer conversion.
 * '0' maps to 0, ..., '9' maps to 9.
 * 'a' and 'A' map to 10, ..., 'z' and 'Z' map to 35.
 * All other indices map to 37.
 * Note that when converting a base B string, a char c is a legitimate
 * base B digit iff _PyLong_DigitValue[Py_CHARPyLong_MASK(c)] < B.
 */
static unsigned char _PyLong_DigitValue[256] = {
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  37, 37, 37, 37, 37, 37,
    37, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 37, 37, 37, 37, 37,
    37, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
};

/* *str points to the first digit in a string of base `base` digits.  base
 * is a power of 2 (2, 4, 8, 16, or 32).  *str is set to point to the first
 * non-digit (which may be *str!).  A normalized long is returned.
 * The point to this routine is that it takes time linear in the number of
 * string characters.
 */
static void
long_from_binary_base(char **str, int base, KInteger& result)
{
	char *p = *str;
	char *start = p;
	int bits_per_char;
	int n;
	KInteger* z = &result;
	twodigits accum;
	int bits_in_accum;
	digit *pdigit;

	assert(base >= 2 && base <= 32 && (base & (base - 1)) == 0);
	n = base;
	for (bits_per_char = -1; n; ++bits_per_char)
		n >>= 1;
	/* n <- total # of bits needed, while setting p to end-of-string */
	while (_PyLong_DigitValue[Py_CHARMASK(*p)] < base)
		++p;
	*str = p;
	/* n <- # of Python digits needed, = ceiling(n/PyLong_SHIFT). */
	n = (int)((p - start) * bits_per_char + PyLong_SHIFT - 1);
	if (n / bits_per_char < p - start) {
// 		PyErr_SetString(PyExc_ValueError,
// 			"int string too large to convert");
// 		return NULL;
		ASSERT_I(false);
		return;
	}
	n = n / PyLong_SHIFT;
// 	z = _PyLong_New(n);
// 	if (z == NULL)
// 		return NULL;
	z->_Alloc(n);
	/* Read string from right, and fill in long from left; i.e.,
	 * from least to most significant in both.
	 */
	accum = 0;
	bits_in_accum = 0;
	pdigit = z->ob_digit;
	while (--p >= start) {
		int k = (int)_PyLong_DigitValue[Py_CHARMASK(*p)];
		assert(k >= 0 && k < base);
		accum |= (twodigits)k << bits_in_accum;
		bits_in_accum += bits_per_char;
		if (bits_in_accum >= PyLong_SHIFT) {
			*pdigit++ = (digit)(accum & PyLong_MASK);
			assert(pdigit - z->ob_digit <= n);
			accum >>= PyLong_SHIFT;
			bits_in_accum -= PyLong_SHIFT;
			assert(bits_in_accum < PyLong_SHIFT);
		}
	}
	if (bits_in_accum) {
		assert(bits_in_accum <= PyLong_SHIFT);
		*pdigit++ = (digit)accum;
		assert(pdigit - z->ob_digit <= n);
	}
	while (pdigit - z->ob_digit < n)
		*pdigit++ = 0;
	long_normalize(z);
}

static void
PyLong_FromString(char *str, char **pend, int base, KInteger& result)
{
	int sign = 1, error_if_nonzero = 0;
	char *start, *orig_str = str;
	KInteger* z = &result;
// 	PyObject *strobj;
// 	int slen;

	if ((base != 0 && base < 2) || base > 36) {
// 		PyErr_SetString(PyExc_ValueError,
// 			"int() arg 2 must be >= 2 and <= 36");
// 		return NULL;
		ASSERT_I(false);
		return;
	}
	while (*str != '\0' && isspace(Py_CHARMASK(*str)))
		str++;
	if (*str == '+')
		++str;
	else if (*str == '-') {
		++str;
		sign = -1;
	}
	if (base == 0) {
		if (str[0] != '0')
			base = 10;
		else if (str[1] == 'x' || str[1] == 'X')
			base = 16;
		else if (str[1] == 'o' || str[1] == 'O')
			base = 8;
		else if (str[1] == 'b' || str[1] == 'B')
			base = 2;
		else {
			/* "old" (C-style) octal literal, now invalid.
			it might still be zero though */
			error_if_nonzero = 1;
			base = 10;
		}
	}
	if (str[0] == '0' &&
		((base == 16 && (str[1] == 'x' || str[1] == 'X')) ||
		(base == 8  && (str[1] == 'o' || str[1] == 'O')) ||
		(base == 2  && (str[1] == 'b' || str[1] == 'B'))))
		str += 2;

	start = str;
	if ((base & (base - 1)) == 0)
		long_from_binary_base(&str, base, *z);
	else {
/***
Binary bases can be converted in time linear in the number of digits, because
Python's representation base is binary.  Other bases (including decimal!) use
the simple quadratic-time algorithm below, complicated by some speed tricks.

First some math:  the largest integer that can be expressed in N base-B digits
is B**N-1.  Consequently, if we have an N-digit input in base B, the worst-
case number of Python digits needed to hold it is the smallest integer n s.t.

    BASE**n-1 >= B**N-1  [or, adding 1 to both sides]
    BASE**n >= B**N      [taking logs to base BASE]
    n >= log(B**N)/log(BASE) = N * log(B)/log(BASE)

The static array log_base_BASE[base] == log(base)/log(BASE) so we can compute
this quickly.  A Python long with that much space is reserved near the start,
and the result is computed into it.

The input string is actually treated as being in base base**i (i.e., i digits
are processed at a time), where two more static arrays hold:

    convwidth_base[base] = the largest integer i such that base**i <= BASE
    convmultmax_base[base] = base ** convwidth_base[base]

The first of these is the largest i such that i consecutive input digits
must fit in a single Python digit.  The second is effectively the input
base we're really using.

Viewing the input as a sequence <c0, c1, ..., c_n-1> of digits in base
convmultmax_base[base], the result is "simply"

   (((c0*B + c1)*B + c2)*B + c3)*B + ... ))) + c_n-1

where B = convmultmax_base[base].

Error analysis:  as above, the number of Python digits `n` needed is worst-
case

    n >= N * log(B)/log(BASE)

where `N` is the number of input digits in base `B`.  This is computed via

    size_z = (Py_ssize_t)((scan - str) * log_base_BASE[base]) + 1;

below.  Two numeric concerns are how much space this can waste, and whether
the computed result can be too small.  To be concrete, assume BASE = 2**15,
which is the default (and it's unlikely anyone changes that).

Waste isn't a problem:  provided the first input digit isn't 0, the difference
between the worst-case input with N digits and the smallest input with N
digits is about a factor of B, but B is small compared to BASE so at most
one allocated Python digit can remain unused on that count.  If
N*log(B)/log(BASE) is mathematically an exact integer, then truncating that
and adding 1 returns a result 1 larger than necessary.  However, that can't
happen:  whenever B is a power of 2, long_from_binary_base() is called
instead, and it's impossible for B**i to be an integer power of 2**15 when
B is not a power of 2 (i.e., it's impossible for N*log(B)/log(BASE) to be
an exact integer when B is not a power of 2, since B**i has a prime factor
other than 2 in that case, but (2**15)**j's only prime factor is 2).

The computed result can be too small if the true value of N*log(B)/log(BASE)
is a little bit larger than an exact integer, but due to roundoff errors (in
computing log(B), log(BASE), their quotient, and/or multiplying that by N)
yields a numeric result a little less than that integer.  Unfortunately, "how
close can a transcendental function get to an integer over some range?"
questions are generally theoretically intractable.  Computer analysis via
continued fractions is practical:  expand log(B)/log(BASE) via continued
fractions, giving a sequence i/j of "the best" rational approximations.  Then
j*log(B)/log(BASE) is approximately equal to (the integer) i.  This shows that
we can get very close to being in trouble, but very rarely.  For example,
76573 is a denominator in one of the continued-fraction approximations to
log(10)/log(2**15), and indeed:

    >>> log(10)/log(2**15)*76573
    16958.000000654003

is very close to an integer.  If we were working with IEEE single-precision,
rounding errors could kill us.  Finding worst cases in IEEE double-precision
requires better-than-double-precision log() functions, and Tim didn't bother.
Instead the code checks to see whether the allocated space is enough as each
new Python digit is added, and copies the whole thing to a larger long if not.
This should happen extremely rarely, and in fact I don't have a test case
that triggers it(!).  Instead the code was tested by artificially allocating
just 1 digit at the start, so that the copying code was exercised for every
digit beyond the first.
***/
		register twodigits c;           /* current input character */
		int size_z;
		int i;
		int convwidth;
		twodigits convmultmax, convmult;
		digit *pz, *pzstop;
		char* scan;

		static double log_base_BASE[37] = {0.0e0,};
		static int convwidth_base[37] = {0,};
		static twodigits convmultmax_base[37] = {0,};

		if (log_base_BASE[base] == 0.0) {
			twodigits convmax = base;
			int i = 1;

			log_base_BASE[base] = (log((double)base) /
				log((double)PyLong_BASE));
			for (;;) {
				twodigits next = convmax * base;
				if (next > PyLong_BASE)
					break;
				convmax = next;
				++i;
			}
			convmultmax_base[base] = convmax;
			assert(i > 0);
			convwidth_base[base] = i;
		}

		/* Find length of the string of numeric characters. */
		scan = str;
		while (_PyLong_DigitValue[Py_CHARMASK(*scan)] < base)
			++scan;

		/* Create a long object that can contain the largest possible
		* integer with this base and length.  Note that there's no
		* need to initialize z->ob_digit -- no slot is read up before
		* being stored into.
		*/
		size_z = (int)((scan - str) * log_base_BASE[base]) + 1;
		/* Uncomment next line to test exceedingly rare copy code */
		/* size_z = 1; */
		assert(size_z > 0);
// 		z = _PyLong_New(size_z);
// 		if (z == NULL)
// 			return NULL;
// 		Py_SIZE(z) = 0;
		z->_Alloc(size_z);
		z->m_size = 0;

		/* `convwidth` consecutive input digits are treated as a single
		* digit in base `convmultmax`.
		*/
		convwidth = convwidth_base[base];
		convmultmax = convmultmax_base[base];

		/* Work ;-) */
		while (str < scan) {
			/* grab up to convwidth digits from the input string */
			c = (digit)_PyLong_DigitValue[Py_CHARMASK(*str++)];
			for (i = 1; i < convwidth && str != scan; ++i, ++str) {
				c = (twodigits)(c *  base +
					(int)_PyLong_DigitValue[Py_CHARMASK(*str)]);
				assert(c < PyLong_BASE);
			}

			convmult = convmultmax;
			/* Calculate the shift only if we couldn't get
			* convwidth digits.
			*/
			if (i != convwidth) {
				convmult = base;
				for ( ; i > 1; --i)
					convmult *= base;
			}

			/* Multiply z by convmult, and add c. */
			pz = z->ob_digit;
			pzstop = pz + z->m_size;
			for (; pz < pzstop; ++pz) {
				c += (twodigits)*pz * convmult;
				*pz = (digit)(c & PyLong_MASK);
				c >>= PyLong_SHIFT;
			}
			/* carry off the current end? */
			if (c) {
				assert(c < PyLong_BASE);
				if (z->m_size < size_z) {
					*pz = (digit)c;
					++z->m_size;
				}
				else {
					KInteger tmp;
					/* Extremely rare.  Get more space. */
					assert(z->m_size == size_z);
// 					tmp = _PyLong_New(size_z + 1);
// 					if (tmp == NULL) {
// 						Py_DECREF(z);
// 						return NULL;
// 					}
// 					memcpy(tmp->ob_digit,
// 						z->ob_digit,
// 						sizeof(digit) * size_z);
// 					Py_DECREF(z);
// 					z = tmp;
// 					z->ob_digit[size_z] = (digit)c;
					tmp = *z;
					z->_Alloc(size_z + 1);
					*z = tmp;
					z->ob_digit[size_z] = (digit)c;

					++size_z;
				}
			}
		}
	}
// 	if (z == NULL)
// 		return NULL;
	if (error_if_nonzero) {
		/* reset the base to 0, else the exception message
		doesn't make too much sense */
		base = 0;
		if (z->m_size != 0)
			goto onError;
		/* there might still be other problems, therefore base
		remains zero here for the same reason */
	}
	if (str == start)
		goto onError;
	if (sign < 0)
		z->m_size = -z->m_size;
	while (*str && isspace(Py_CHARMASK(*str)))
		str++;
	if (*str != '\0')
		goto onError;
	if (pend)
		*pend = str;
	long_normalize(z);
// 	return (PyObject *) maybe_small_long(z);
	return;

onError:
	ASSERT_I(false);
// 	Py_XDECREF(z);
// 	slen = strlen(orig_str) < 200 ? strlen(orig_str) : 200;
// 	strobj = PyUnicode_FromStringAndSize(orig_str, slen);
// 	if (strobj == NULL)
// 		return NULL;
// 	PyErr_Format(PyExc_ValueError,
// 		"invalid literal for int() with base %d: %R",
// 		base, strobj);
// 	Py_DECREF(strobj);
// 	return NULL;
}

void KInteger::fromString( const char* str )
{
	// 空串认为0
	if (!str[0]) ToZero();
	else PyLong_FromString((char*)str, NULL, 0, *this);
}

/* Create a new long int object from a C double */

static void
PyLong_FromDouble(double dval, KInteger& result)
{
	KInteger* v = &result;
	double frac;
	int i, ndig, expo, neg;
	neg = 0;
#if !defined(__clang__)
	if (Py_IS_INFINITY(dval)) {
// 		PyErr_SetString(PyExc_OverflowError,
// 			"cannot convert float infinity to integer");
// 		return NULL;
		ASSERT_I(false);
		result.ToZero();
		return;
	}
#endif
// 	if (Py_IS_NAN(dval)) {
// 		PyErr_SetString(PyExc_ValueError,
// 			"cannot convert float NaN to integer");
// 		return NULL;
// 	}
	if (dval < 0.0) {
		neg = 1;
		dval = -dval;
	}
	frac = frexp(dval, &expo); /* dval = frac*2**expo; 0.0 <= frac < 1.0 */
// 	if (expo <= 0)
// 		return PyLong_FromLong(0L);
	if (expo <= 0)
	{
		v->ToZero();
		return;
	}
	ndig = (expo-1) / PyLong_SHIFT + 1; /* Number of 'digits' in result */
// 	v = _PyLong_New(ndig);
// 	if (v == NULL)
// 		return NULL;
	v->_Alloc(ndig);
	frac = ldexp(frac, (expo-1) % PyLong_SHIFT + 1);
	for (i = ndig; --i >= 0; ) {
		digit bits = (digit)frac;
		v->ob_digit[i] = bits;
		frac = frac - (double)bits;
		frac = ldexp(frac, PyLong_SHIFT);
	}
	if (neg)
		v->m_size = -v->m_size;
}
void KInteger::fromDouble( double v )
{
	PyLong_FromDouble(v, *this);
}


/* For a nonzero PyLong a, express a in the form x * 2**e, with 0.5 <=
   abs(x) < 1.0 and e >= 0; return x and put e in *e.  Here x is
   rounded to DBL_MANT_DIG significant bits using round-half-to-even.
   If a == 0, return 0.0 and set *e = 0.  If the resulting exponent
   e is larger than PY_SSIZE_T_MAX, raise OverflowError and return
   -1.0. */

/* attempt to define 2.0**DBL_MANT_DIG as a compile-time constant */
#if DBL_MANT_DIG == 53
#define EXP2_DBL_MANT_DIG 9007199254740992.0
#else
#define EXP2_DBL_MANT_DIG (ldexp(1.0, DBL_MANT_DIG))
#endif

static double
_PyLong_Frexp(const KInteger* a, int* e)
{
	int a_size, a_bits, shift_digits, shift_bits, x_size;
	/* See below for why x_digits is always large enough. */
	digit rem, x_digits[2 + (DBL_MANT_DIG + 1) / PyLong_SHIFT];
	double dx;
	/* Correction term for round-half-to-even rounding.  For a digit x,
	   "x + half_even_correction[x & 7]" gives x rounded to the nearest
	   multiple of 4, rounding ties to a multiple of 8. */
	static const int half_even_correction[8] = {0, -1, -2, 1, 0, -1, 2, 1};

	a_size = ABS(a->m_size);
	if (a_size == 0) {
		/* Special case for 0: significand 0.0, exponent 0. */
		*e = 0;
		return 0.0;
	}
	a_bits = bits_in_digit(a->ob_digit[a_size-1]);
	/* The following is an overflow-free version of the check
	   "if ((a_size - 1) * PyLong_SHIFT + a_bits > PY_SSIZE_T_MAX) ..." */
	if (a_size >= (PY_SSIZE_T_MAX - 1) / PyLong_SHIFT + 1 &&
		(a_size > (PY_SSIZE_T_MAX - 1) / PyLong_SHIFT + 1 ||
		a_bits > (PY_SSIZE_T_MAX - 1) % PyLong_SHIFT + 1))
		goto overflow;
	a_bits = (a_size - 1) * PyLong_SHIFT + a_bits;

	/* Shift the first DBL_MANT_DIG + 2 bits of a into x_digits[0:x_size]
	   (shifting left if a_bits <= DBL_MANT_DIG + 2).

	   Number of digits needed for result: write // for floor division.
	   Then if shifting left, we end up using

	   1 + a_size + (DBL_MANT_DIG + 2 - a_bits) // PyLong_SHIFT

	   digits.  If shifting right, we use

	   a_size - (a_bits - DBL_MANT_DIG - 2) // PyLong_SHIFT

	   digits.  Using a_size = 1 + (a_bits - 1) // PyLong_SHIFT along with
	   the inequalities

	   m // PyLong_SHIFT + n // PyLong_SHIFT <= (m + n) // PyLong_SHIFT
	   m // PyLong_SHIFT - n // PyLong_SHIFT <=
	   1 + (m - n - 1) // PyLong_SHIFT,

	   valid for any integers m and n, we find that x_size satisfies

	   x_size <= 2 + (DBL_MANT_DIG + 1) // PyLong_SHIFT

	   in both cases.
	*/
	if (a_bits <= DBL_MANT_DIG + 2) {
		shift_digits = (DBL_MANT_DIG + 2 - a_bits) / PyLong_SHIFT;
		shift_bits = (DBL_MANT_DIG + 2 - a_bits) % PyLong_SHIFT;
		x_size = 0;
		while (x_size < shift_digits)
			x_digits[x_size++] = 0;
		rem = v_lshift(x_digits + x_size, a->ob_digit, a_size,
			(int)shift_bits);
		x_size += a_size;
		x_digits[x_size++] = rem;
	}
	else {
		shift_digits = (a_bits - DBL_MANT_DIG - 2) / PyLong_SHIFT;
		shift_bits = (a_bits - DBL_MANT_DIG - 2) % PyLong_SHIFT;
		rem = v_rshift(x_digits, a->ob_digit + shift_digits,
			a_size - shift_digits, (int)shift_bits);
		x_size = a_size - shift_digits;
		/* For correct rounding below, we need the least significant
		bit of x to be 'sticky' for this shift: if any of the bits
		shifted out was nonzero, we set the least significant bit
		of x. */
		if (rem)
			x_digits[0] |= 1;
		else
			while (shift_digits > 0)
				if (a->ob_digit[--shift_digits]) {
					x_digits[0] |= 1;
					break;
				}
	}
	assert(1 <= x_size && x_size <= (int)(sizeof(x_digits) / sizeof(x_digits[0])));

	/* Round, and convert to double. */
	x_digits[0] += half_even_correction[x_digits[0] & 7];
	dx = x_digits[--x_size];
	while (x_size > 0)
		dx = dx * PyLong_BASE + x_digits[--x_size];

	/* Rescale;  make correction if result is 1.0. */
	dx /= 4.0 * EXP2_DBL_MANT_DIG;
	if (dx == 1.0) {
		if (a_bits == PY_SSIZE_T_MAX)
			goto overflow;
		dx = 0.5;
		a_bits += 1;
	}

	*e = a_bits;
	return a->m_size < 0 ? -dx : dx;

overflow:
	/* exponent > PY_SSIZE_T_MAX */
// 	PyErr_SetString(PyExc_OverflowError,
// 		"huge integer: number of bits overflows a Py_ssize_t");
// 	*e = 0;
	*e = DBL_MAX_EXP + 1;
	return 0.0;
}

/* Get a C double from a long int object.  Rounds to the nearest double,
   using the round-half-to-even rule in the case of a tie. */

static double
PyLong_AsDouble(const KInteger* v)
{
	int exponent;
	double x;

// 	if (v == NULL) {
// 		PyErr_BadInternalCall();
// 		return -1.0;
// 	}
	if (!v)
	{
		ASSERT_I(false);
		return 0.0f;
	}
// 	if (!PyLong_Check(v)) {
// 		PyErr_SetString(PyExc_TypeError, "an integer is required");
// 		return -1.0;
// 	}
	x = _PyLong_Frexp(v, &exponent);
// 	if ((x == -1.0 && PyErr_Occurred()) || exponent > DBL_MAX_EXP) {
// 		PyErr_SetString(PyExc_OverflowError,
// 			"long int too large to convert to float");
// 		return -1.0;
// 	}
	if (exponent > DBL_MAX_EXP)
	{
		ASSERT_I(false);
		return 0.0;
	}
	return ldexp(x, (int)exponent);
}

KInteger::operator double() const
{
	return PyLong_AsDouble(this);
}

/**
 * 数字绝对值部分自加1
 */
static void
raw_add1(KInteger& bi)
{
	int size_a = ABS(bi.m_size);
	int i;
	digit carry = 1;

	// 检查每一位，找到加上后不进位的位置
	for (i = 0; i < size_a; i++)
	{
		carry = bi.ob_digit[i] + 1;
		carry >>= PyLong_SHIFT;
		if (carry)	// 需要进位，则本位清零
		{
			bi.ob_digit[i] = 0;
		}
		else
		{
			++bi.ob_digit[i];
			break;
		}
	}
	if (carry)	// 还有一位没有进位，需要括出来一位
	{
		++size_a;
		bi._Realloc(size_a);
		bi.ob_digit[size_a - 1] = 1;
		if (bi.m_size < 0) bi.m_size = -size_a;
		else bi.m_size = size_a;
	}
	long_normalize(&bi);
}

/**
 * 数字绝对值部分自减1
 */
static void
raw_sub1(KInteger& bi)
{
	int size_a = ABS(bi.m_size);
	int i;
	digit borrow = 0;

	if (!size_a)	// 如果是0，减之后变成负数
	{
		bi._Alloc(1);
		bi.ob_digit[0] = 1;
		bi.m_size = -1;
		return;
	}
	// 检查每一位，找到减去后不借位的位置
	for (i = 0; i < size_a; i++)
	{
		if (!bi.ob_digit[i])	// 该位是0，需要借位
		{
			bi.ob_digit[i] = PyLong_MASK;
		}
		else
		{
			--bi.ob_digit[i];
			break;
		}
	}
	long_normalize(&bi);
}


KInteger& KInteger::operator++()
{
	if (m_size < 0) raw_sub1(*this);
	else raw_add1(*this);
	return *this;
}

KInteger KInteger::operator++(int)
{
	KInteger old = *this;
	++*this;
	return old;
}

KInteger& KInteger::operator--()
{
	if (m_size < 0) raw_add1(*this);
	else raw_sub1(*this);
	return *this;
}

KInteger KInteger::operator--(int)
{
	KInteger old = *this;
	--*this;
	return old;
}

KInteger& KInteger::operator+=( const KInteger& o )
{
	return *this = *this + o;
}

KInteger& KInteger::operator-=( const KInteger& o )
{
	return *this = *this - o;
}

KInteger& KInteger::operator*=( const KInteger& o )
{
	return *this = *this * o;
}

KInteger& KInteger::operator/=( const KInteger& o )
{
	return *this = *this / o;
}

KInteger& KInteger::operator%=( const KInteger& o )
{
	return *this = *this % o;
}

KInteger& KInteger::operator&=( const KInteger& o )
{
	return *this = *this & o;
}

KInteger& KInteger::operator|=( const KInteger& o )
{
	return *this = *this | o;
}

KInteger& KInteger::operator^=( const KInteger& o )
{
	return *this = *this ^ o;
}

KInteger& KInteger::operator<<=( int val )
{
	return *this = *this << val;
}

KInteger& KInteger::operator>>=( int val )
{
	return *this = *this >> val;
}