#ifndef _ZALLOC_H_
#define _ZALLOC_H_

#include <new>		//for placement new
#include <cstddef>	//for ptrdiff_t, size_t(ptrdiff_t：与机器相关的数据类型，signed，保存两个指针减法操作的结果)
#include <cstdlib>	//for exit()
#include <climits>	//for UNIT_MAX(unsigned int的最大值)
#include <iostream>	//for cerr
#include "00zstl_config.h"

__MYSTL_NAMESPACE_BEGIN_
	template <class T>
	inline T* _allocate(ptrdiff_t size, T*) {
		std::set_new_handler(0);	//当::operator new无法分配足够内存时，new-handler机制调用指定函数
		T* tmp = (T*)(::operator new((size_t)(size * sizeof(T))));
		if (tmp == 0) {
			std::cerr << "out of memeory" << std::endl;
			exit(1);
		}
		return tmp;
	}

	template <class T>
	inline void _deallocate(T* buffer) {
		::operator delete(buffer);
	}

	template <class T1, class T2>
	inline void _construct(T1 *p, const T2& value) {
		new(p) T1(value);		//placement new. invoke ctor of T1
	}

	template <class T>
	inline void _destroy(T* ptr) {
		ptr->~T();
	}

	template <class T>
	class allocator {
	public:
		typedef T			value_type;
		typedef T*			pointer;
		typedef const T*	const_pointer;
		typedef T&			reference;
		typedef const T&	const_reference;
		typedef size_t		size_type;
		typedef ptrdiff_t	difference_type;

		//rebind allocator of type _Other
		template <class _Other>
		struct rebind {
			typedef allocator<_Other> other;
		};

#ifdef _MBCS
		allocator() {
			//std::cout << "allocator()" << std::endl;
			return;
		}
		template<class _Other>
		allocator ( const allocator<_Other>& c) {
			//std::cout << "allocator(c)" << std::endl;
		}
#endif

		//hint used for locality
		pointer allocate(size_type n, const void* hint = 0) {
			return _allocate((difference_type)n, (pointer)0);
		}

		void deallocate(pointer p, size_type n) { 
			_deallocate(p); 
			//destroy(p);
		}

		void construct(pointer p, const T& value) {
			_construct(p, value);
		}

		void destroy(pointer p) {
			//std::cout << "destroy" << std::endl;
			_destroy(p);
		}

		pointer address(reference x) {
			return (pointer)&x;
		}

		const_pointer const_address(const_reference x) {
			return (const_pointer)&x;
		}

		size_type max_size() const {
			return size_type(UINT_MAX / sizeof(T));
		}
	};
__MYSTL_NAMESPACE_END_

#endif	//_ZALLOC_