#pragma once
#ifndef _ZSTL_CONFIG_H_
#define _ZSTL_CONFIG_H_

#define __MYSTL_NAMESPACE_BEGIN_	namespace mySTL {
#define __MYSTL_NAMESPACE_END_						}

#define __STL_TEMPLATE_NULL template<>		//显式特化
//#define __STL_NULL_TMPL_ARGS		<>

__MYSTL_NAMESPACE_BEGIN_
	

	static const int __stl_nun_primes = 28;		//用于维护 hash table 大小的质数个数
	/* 质数数组（两相邻数之间成两倍关系） */
	static const unsigned long __stl_prime_list[__stl_nun_primes] = {
		53,         97,           193,         389,       769,
		1543,       3079,         6151,        12289,     24593,
		49157,      98317,        196613,      393241,    786433,
		1572869,    3145739,      6291469,     12582917,  25165843,
		50331653,   100663319,    201326611,   402653189, 805306457,
		1610612741, 3221225473ul, 4294967291ul
	};
__MYSTL_NAMESPACE_END_

#endif // !_ZSTL_CONFIG_H_
