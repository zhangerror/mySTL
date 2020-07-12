/*�ռ�������

��һ�����������ڶ��������������������ڴ��

*/

#ifndef _ZSTL_ALLOC_H_
#define _ZSTL_ALLOC_H_

#include "00zstl_config.h"
//#include <stdlib.h>  //for malloc(),realloc()
//#include <stddef.h>  //for size_t
//#include <memory.h>  //for memcpy()

__MYSTL_NAMESPACE_BEGIN_
/*	SGI��װ�� alloc �������ӿ�	*/
	template<typename T, class Alloc>
	class simple_alloc {
	public:
		static T* allocate(size_t n) {
			return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T));
		}
		static T* allocate(void) {
			return (T*)Alloc::allocate(sizeof(T));
		}
		static void deallocate(T* p, size_t n) {
			if (0 != n)
				Alloc::deallocate(p, n * sizeof(T));
		}
		static void deallocate(T* p) {
			Alloc::deallocate(p, sizeof(T));
		}
	};		//end of class simple_alloc


#if 0
	#include <new>
	#define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
	#include <iostream>
	#define __THROW_BAD_ALLOC std::cerr << "out of memory\n"; exit(1);
#endif


/*	��һ�������� __malloc_alloc_template	*/
/*	��malloc() realloc() free() ��C����ִ��ʵ�ʵ��ڴ��������ͷŲ���		*/
	//ע�⣬��template�ͱ���������ڷ��ͱ����inst������ȫû�������ó�
	template<int inst>
	class __malloc_alloc_template {
	private:
		/* ���º��������������ڴ治������ */
		static void *oom_malloc(size_t);
		static void *oom_realloc(void*, size_t);
		static void(*__malloc_alloc_oom_handler)();

	public:
		static void* allocate(size_t n) {
			void* result = malloc(n);		//��һ��������ֱ���� malloc��Ҳ���� operator new
			if (0 == result)
				result = oom_malloc(n);		//�޷���������ʱ������ oom_malloc 
			return result;
		}

		static void deallocate(void* p, size_t n) {
			free(p);		//��һ��������ֱ���� free��Ҳ���� operator delete
		}

		static void* reallocate(void* p, size_t old_sz, size_t new_sz) {
			void* result = realloc(p, new_sz);		//��һ��������ֱ����realloc
			if (0 == result)
				result = oom_realloc(p, new_sz);
			return result;
		}

		/*���·���C++��set_new_handler()�������ͨ����ָ�����Լ��� out-of-memory-handler*/
		static void(*set_malloc_handler(void(*f)()))() {
			void(*old)() = __malloc_alloc_oom_handler;
			__malloc_alloc_oom_handler = f;
			return (old);
		}
	};	//end of class __malloc_alloc_template

	//�趨malloc_alloc out-of-memory handling��ֵΪ0���д��Ͷ��趨
	template<int inst>
	void(*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

	template<int inst>
	void* __malloc_alloc_template<inst>::oom_malloc(size_t n) {
		void (*my_malloc_handler)();
		void* result;
		while (1) {
			my_malloc_handler = __malloc_alloc_oom_handler;
			if (0 == my_malloc_handler)		//���δ���崦������
				__THROW_BAD_ALLOC;
			(*my_malloc_handler)();		//���ô������̣���ͼ�ͷ��ڴ�
			result = malloc(n);			//�ٴγ��������ڴ�
			if (result)
				return (result);
		}
	}

	template<int inst>
	void* __malloc_alloc_template<inst>::oom_realloc(void* p, size_t n) {
		void (*my_malloc_handler)();
		void* result;
		while (1) {
			my_malloc_handler = __malloc_alloc_oom_handler;
			if (0 == my_malloc_handler)		//���δ���崦������
				__THROW_BAD_ALLOC;
			(*my_malloc_handler)();		//���ô������̣���ͼ�ͷ��ڴ�
			result = realloc(p, n);		//�ٴγ��������ڴ�
			if (result)
				return result;
		}
	}
	//����ֱ�ӽ�����instָ��Ϊ0
	typedef __malloc_alloc_template<0> malloc_alloc;


/*	�ڶ��������� __default_alloc_template	*/
/*	ע�⣬��template�ͱ��������һ�������ڶ��̻߳��������ͱ����inst��ȫû�������ó� */
	enum { __ALIGN = 8 };		// С��������ϵ��߽�
	enum { __MAX_BYTES = 128 };		// С�����������
	enum { __NFREELISTS = __MAX_BYTES / __ALIGN };		// ��������ĸ���

	template<bool threads, int inst>
	class __default_alloc_template {
	private:
		//ROUND_UP()��byte�ϵ���8�ı���
		static size_t ROUND_UP(size_t bytes) {
			return ((bytes + __ALIGN - 1) & ~(__ALIGN - 1));
		}
	private:
		//��������ڵ�ṹ
		union obj {
			union obj* free_list_link;
			char client_data[1];		//The client see this
		};

		// 16��free list����ʼֵ����Ϊ0
		static obj* volatile free_list[__NFREELISTS];

		//���º������������С����ʹ�õ� n ��free-list��n ��0����
		static size_t FREELIST_INDEX(size_t bytes) {
			return ((bytes + __ALIGN - 1) / __ALIGN - 1);
		}

	private:
		//����һ����СΪn�Ķ��󣬲����ܼ����СΪn���������鵽 free-list
		static void* refill(size_t n);
		//����һ���ռ䣬������ nobjs ����СΪ size ������
		//������ nobjs �����鲻�㣬nobjs ���ܻή��
		static char* chunk_alloc(size_t size, int &nobjs);

		// Chunk allocation state
		static char*  start_free;		//�ڴ����ʼλ�ã�ֻ��chunk_alloc()�б仯
		static char*  end_free;			//�ڴ�ؽ���λ�ã�ֻ��chunk_alloc()�б仯
		static size_t heap_size;

	public:
		/* �ռ����ú��� */
		static void* allocate(size_t n);
		/* �ռ��ͷź��� */
		static void deallocate(void* p, size_t n);
		/* �ռ��������ú��� */
		static void* reallocate(void* p, size_t old_sz, size_t new_sz);

	};	// end of __default_alloc_template

	/* ������static data member �Ķ������ֵ�趨 */
	template<bool threads, int inst>
	char* __default_alloc_template<threads, inst>::start_free = 0;
	template<bool threads, int inst>
	char* __default_alloc_template<threads, inst>::end_free = 0;
	template<bool threads, int inst>
	size_t __default_alloc_template<threads, inst>::heap_size = 0;
	template<bool threads, int inst>
	typename __default_alloc_template<threads, inst>::obj * volatile
	__default_alloc_template<threads, inst>::free_list[__NFREELISTS] =
	{ 0, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 };

	/* �ռ����ú��� */
	template<bool threads, int inst>
	void* __default_alloc_template<threads, inst>::allocate(size_t total_byte) {
		obj* volatile* my_free_list;
		obj* result;

		//����128�͵��õ�һ��������
		if (total_byte > (size_t)__MAX_BYTES)
			return malloc_alloc::allocate(total_byte);

		//����Ѱ��16��free lis�����ʵ���һ��
		my_free_list = free_list + FREELIST_INDEX(total_byte);
		result = *my_free_list;
		if (result == 0) {		//û�ҵ����õ�free list���� n �ϵ��� 8 �ı�����׼���������free list
			void* r = refill(ROUND_UP(total_byte));
			return r;
		}
		//����free list��ȡ��һ������
		*my_free_list = result->free_list_link;
		return result;
	}

	/* �ռ��ͷź���, p����Ϊ0 */
	template<bool threads, int inst>
	void __default_alloc_template<threads, inst>::deallocate(void* p, size_t n) {
		obj* volatile* my_free_list;
		obj* q = (obj *)p;

		//����128�͵��õ�һ��������
		if (n > (size_t)__MAX_BYTES) {
			malloc_alloc::deallocate(p, n);
			return;
		}

		//����Ѱ�Ҷ�Ӧ��free list��������黹
		my_free_list = free_list + FREELIST_INDEX(n);
		q->free_list_link = *my_free_list;
		*my_free_list = q;
	}

	/* �ռ��������ú��� */
	template<bool threads, int inst>
	static void* __default_alloc_template<threads, inst>::reallocate(void* p, size_t old_sz, size_t new_sz) {
		obj* volatile* my_free_list;

		obj* result = malloc_alloc::reallocate(p, old_sz, new_sz);

		*my_free_list = result->free_list_link;
		return result;
	}

/* �������free list������һ����СΪ size ���ڴ����Ͷ�ʹ�ã����������������� */
/* size �Ѿ������� 8 �ı���������õ���ֹһ���ڴ���������ڴ�����ӵ��� free list �� */
	template<bool threads, int inst>
	void* __default_alloc_template<threads, inst>::refill(size_t size) {
		int nobjs = 10;			//һ�λ�ȡ 10 ����СΪ size ���ڴ��
		obj *volatile *my_free_list;
		obj *result, *next_obj;

		//����chunk_alloc()���Ի�� nonjs ��������Ϊfree-list���½ڵ�
		//ע����� nobjs �������÷�ʽ���룬������chunk_alloc���޸� nobjs Ϊʵ�ʵõ����������
		char* chunk = chunk_alloc(size, nobjs);

		//���ֻ���һ������������������������ߣ�free list���������
		if (1 == nobjs)
			return (chunk);
		//׼������free list�������½ڵ�
		my_free_list = free_list + FREELIST_INDEX(size);

		//������chunk�ռ��ڽ���free list
		result = (obj *)chunk;		//��һ�鷵�ظ��Ͷ�

		//���½����ڴ���������õĿռ�����free-list
		*my_free_list = next_obj = (obj*)(chunk + size);	//�ӵڶ������鿪ʼ
		
		for (int i = 1; ; ++i) {		//��1��ʼ����Ϊ0�Ѿ����ظ��Ͷ�
			obj* current_obj = next_obj;
			next_obj = (obj *)((char *)next_obj + size);	//ָ����һ������
			if (nobjs - 1 == i) {
				current_obj->free_list_link = 0;		//�������
				break;
			}
			else
				current_obj->free_list_link = next_obj;
		}
		return (result);
	}

/* ���ڴ����ȡ�ڴ����free list��size��ʾһ������Ĵ�С���Ѿ�������8�ı��� */
/* nobjs��ʾϣ����õ���������������÷�ʽ����Ϊ���ں����н����޸�Ϊʵ�ʵõ���������� */
	template<bool threads, int inst>
	char* __default_alloc_template<threads, inst>::chunk_alloc(size_t size, int& nobjs) {
		char* result;
		size_t total_bytes = size * nobjs;			//����ȡ�õ��ܿռ�
		size_t bytes_left = end_free - start_free;	//�ڴ��ʣ��ռ�

		if (bytes_left >= total_bytes) {		//�ڴ�ؿռ���ȫ����������
			result = start_free;
			start_free += total_bytes;
			return (result);
		}
		else if (bytes_left >= size) {		//�ڴ��ʣ��ռ䲻����ȫ�������󣬵�������������һ������
			nobjs = (int)(bytes_left / size);	//��nobjs�޸�Ϊʵ�ʵõ����ڴ�����
			total_bytes = size * nobjs;			//����ʵ�ʻ�õ��ܿռ�
			result = start_free;
			start_free += total_bytes;
			return (result);
		}
		else {		//�ڴ�ʣ������һ�����鶼�޷�����
			size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
			//�����������ڴ����ʣ�����ͷ�������ü�ֵ
			if (bytes_left > 0) {
				//Ѱ���ʵ���free list��Ȼ���䱸��free list
				obj* volatile* my_free_list = free_list + FREELIST_INDEX(bytes_left);

				//����free list�����ڴ����ʣ��Ŀռ�����
				((obj *)start_free)->free_list_link = *my_free_list;
				*my_free_list = (obj *)start_free;
			}

			/* ����heap�Ŀռ䣬���������ڴ�� */
			start_free = (char *)malloc(bytes_to_get);
			if (0 == start_free) {		//heap�ռ䲻�㣬mallocʧ��
				obj * volatile * my_free_list, *p;
				//���������ʵ���free list����������δ�����飬�������㹻�󡱵�free list
				for (int i = __MAX_BYTES; i > 0; i -= __ALIGN) {
					my_free_list = free_list + FREELIST_INDEX(i);
					p = *my_free_list;
					if (p != 0) {		//free list������δ������
						//����free list���ͷ�δ������
						*my_free_list = p->free_list_link;
						start_free = (char *)p;
						end_free = start_free + i;

						//�ݹ�����Լ���Ϊ������nobjs
						return (chunk_alloc(size, nobjs));
						//�κβ�����ͷ����������free list�б���
					}
				}
				end_free = 0;		//���������û���ڴ������
				//���õ�һ��������������out of memory�����ܷ���
				//����׳��쳣�����ڴ治��������ø���
				start_free = (char *)malloc_alloc::allocate(bytes_to_get);
			}
			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;
			//�ݹ�����Լ���Ϊ������nobjs
			return (chunk_alloc(size, nobjs));
		}
	}
	#define __NODE_ALLOCATOR_THREADS false		//���̻߳���

	//���õڶ���������ΪĬ��������
	typedef __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0> alloc;
__MYSTL_NAMESPACE_END_

#endif // !_ZSTL_ALLOC_H_
