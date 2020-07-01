#ifndef _ZSTL_HEAP_
#include "00zstl_iterator.h"

__MYSTL_NAMESPACE_BEGIN_
	//以下这组 push_back() 不允许指定 "大小比较标准"
	template <class RandomAccessIterator, class Distance, class T>
	void __push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value) {
		Distance parent = (holeIndex - 1) / 2;		//找出父节点
		while (holeIndex > topIndex && *(first + parent) < value) {
			//当尚未到达顶端，且父节点小于新值(不符合 heap 的次序特性)
			//由于以上使用operator<，可知 STL heap 是一种 max-heap
			*(first + holeIndex) = *(first + parent);	//令洞值为父值
			holeIndex = parent;		//precolate up：调整洞号，向上提升至父节点
			parent = (holeIndex - 1) / 2;		//新洞的父节点
		}		//持续至顶端，或满足 heap 的次序特性为止
		*(first + holeIndex) = value;		//令洞值为新值，完成插入操作
	}

	template<class RandomAccessIterator, class Distance, class T>
	inline void __push_heap_aux(RandomAccessIterator first,
		RandomAccessIterator last, Distance*, T*) {
		__push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)));
		//以上根据 implicit representation heap 的结构特性：新值必置于底部
		//容器的最尾端，此即第一个洞号： (last - first) - 1
	}

	template<class RandomAccessIterator>
	inline void push_heap(RandomAccessIterator first,
							RandomAccessIterator last) {
		//注意，此函数被调用时，新元素应已置于底部容器的最尾端
		__push_heap_aux(first, last, distance_type(first), value_type(first));
	}

	//以下这组 __adjust_heap() 不允许指定大小比较标准
	template <class RandomAccessIterator, class Distance, class T>
	void __adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value) {
		Distance topIndex = holeIndex;
		Distance secondChild = 2 * holeIndex + 2;	//洞节点之右节点
		while (secondChild < len) {
			//比较洞节点之左右两个子值，然后以 secondChild 代表较大子节点
			if (*(first + secondChild) < *(first + (secondChild - 1))) {
				--secondChild;
			}
			// Percolate down：令较大子值为洞值，再令洞号下移至较大子节点处
			*(first + holeIndex) = *(first + secondChild);
			holeIndex = secondChild;
			//找出新洞节点的右子节点
			secondChild = 2 * (secondChild + 1);
		}
		if (secondChild == len) {	//没有右子节点，只有左子节点
			// Percolate down：令左子值为洞值，再令洞号下移至左子节点处
			*(first + holeIndex) = *(first + (secondChild - 1));
			holeIndex = secondChild - 1;
		}
		// 将欲调整值填入目前的洞号内，注意，此时肯定满足次序特性
		__push_heap(first, holeIndex, topIndex, value);
		//*(first + holeIndex) = value;
	}

	//以下这组 __pop_heap() 不允许指定大小比较标准
	template <class RandomAccessIterator, class T, class Distance>
	inline void __pop_heap(RandomAccessIterator first,
		RandomAccessIterator last,
		RandomAccessIterator result,
		T value, Distance*) {
		*result = *first;		//设定尾值为首值，于是尾值即为欲求结果，可由客户端稍后再以底层容器的 pop_back() 取出尾值

		__adjust_heap(first, Distance(0), Distance(last - first), value);
		//以上欲重新调整 heap，洞号为 0 (亦即树根处)，欲调整值为 value (原尾值)
	}

	template <class RandomAccessIterator, class T>
	inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*) {
		__pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first));
		//以上，根据 implicit representation heap 的次序特性，pop操作的结果应为底部容器的第一个元素
		//因此，首先设定欲调整值为尾置，然后将首值调至尾节点(所以以上将迭代器 result 设为 last-1)
		//然后重整 [first, last-1)，使之重新成一个合格的heap
	}

	template <class RandomAccessIterator> 
	inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
		__pop_heap_aux(first, last, value_type(first));
	}

	template <class RandomAccessIterator>
	void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
		//以下，每执行一次 pop_heap()，极大值被放在尾端
		//扣除尾端再执行一次 pop_heap()，次极大值又被放在新尾端
		//循环得排序结果
		while (last - first > 1) {
			pop_heap(first, last--);	//每执行一次，操作范围退缩一格
		}
	}

	//以下这组 make_heap() 不允许指定大小比较标准
	template <class RandomAccessIterator, class T, class Distance>
	void __make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*) {
		if (last - first < 2) return;	//如果长度为 0 或 1，不必重新排列
		Distance len = last - first;
		//找出第一个需要重排的子树头部，以 holeIndex 标示出(任何节点都不需要执行 perlocate down )
		Distance holeIndex = (len - 2) / 2;
		while (true) {
			//重排以 holeIndex 为首的子树，len是为了让 __adjust_heap() 判断操作范围
			__adjust_heap(first, holeIndex, len, T(*(first + holeIndex)));
			if (holeIndex == 0) return;		//走完根节点就结束
			--holeIndex;					//(即将重排的子树的)头部向前一个节点
		}
	}

	template <class RandomAccessIterator>
	inline void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
		__make_heap(first, last, value_type(first), distance_type(first));
	}

__MYSTL_NAMESPACE_END_

#endif // !_ZSTL_HEAP_
