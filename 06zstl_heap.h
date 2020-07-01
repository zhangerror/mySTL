#ifndef _ZSTL_HEAP_
#include "00zstl_iterator.h"

__MYSTL_NAMESPACE_BEGIN_
	//�������� push_back() ������ָ�� "��С�Ƚϱ�׼"
	template <class RandomAccessIterator, class Distance, class T>
	void __push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value) {
		Distance parent = (holeIndex - 1) / 2;		//�ҳ����ڵ�
		while (holeIndex > topIndex && *(first + parent) < value) {
			//����δ���ﶥ�ˣ��Ҹ��ڵ�С����ֵ(������ heap �Ĵ�������)
			//��������ʹ��operator<����֪ STL heap ��һ�� max-heap
			*(first + holeIndex) = *(first + parent);	//�ֵΪ��ֵ
			holeIndex = parent;		//precolate up���������ţ��������������ڵ�
			parent = (holeIndex - 1) / 2;		//�¶��ĸ��ڵ�
		}		//���������ˣ������� heap �Ĵ�������Ϊֹ
		*(first + holeIndex) = value;		//�ֵΪ��ֵ����ɲ������
	}

	template<class RandomAccessIterator, class Distance, class T>
	inline void __push_heap_aux(RandomAccessIterator first,
		RandomAccessIterator last, Distance*, T*) {
		__push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)));
		//���ϸ��� implicit representation heap �Ľṹ���ԣ���ֵ�����ڵײ�
		//��������β�ˣ��˼���һ�����ţ� (last - first) - 1
	}

	template<class RandomAccessIterator>
	inline void push_heap(RandomAccessIterator first,
							RandomAccessIterator last) {
		//ע�⣬�˺���������ʱ����Ԫ��Ӧ�����ڵײ���������β��
		__push_heap_aux(first, last, distance_type(first), value_type(first));
	}

	//�������� __adjust_heap() ������ָ����С�Ƚϱ�׼
	template <class RandomAccessIterator, class Distance, class T>
	void __adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value) {
		Distance topIndex = holeIndex;
		Distance secondChild = 2 * holeIndex + 2;	//���ڵ�֮�ҽڵ�
		while (secondChild < len) {
			//�Ƚ϶��ڵ�֮����������ֵ��Ȼ���� secondChild ����ϴ��ӽڵ�
			if (*(first + secondChild) < *(first + (secondChild - 1))) {
				--secondChild;
			}
			// Percolate down����ϴ���ֵΪ��ֵ��������������ϴ��ӽڵ㴦
			*(first + holeIndex) = *(first + secondChild);
			holeIndex = secondChild;
			//�ҳ��¶��ڵ�����ӽڵ�
			secondChild = 2 * (secondChild + 1);
		}
		if (secondChild == len) {	//û�����ӽڵ㣬ֻ�����ӽڵ�
			// Percolate down��������ֵΪ��ֵ����������������ӽڵ㴦
			*(first + holeIndex) = *(first + (secondChild - 1));
			holeIndex = secondChild - 1;
		}
		// ��������ֵ����Ŀǰ�Ķ����ڣ�ע�⣬��ʱ�϶������������
		__push_heap(first, holeIndex, topIndex, value);
		//*(first + holeIndex) = value;
	}

	//�������� __pop_heap() ������ָ����С�Ƚϱ�׼
	template <class RandomAccessIterator, class T, class Distance>
	inline void __pop_heap(RandomAccessIterator first,
		RandomAccessIterator last,
		RandomAccessIterator result,
		T value, Distance*) {
		*result = *first;		//�趨βֵΪ��ֵ������βֵ��Ϊ�����������ɿͻ����Ժ����Եײ������� pop_back() ȡ��βֵ

		__adjust_heap(first, Distance(0), Distance(last - first), value);
		//���������µ��� heap������Ϊ 0 (�༴������)��������ֵΪ value (ԭβֵ)
	}

	template <class RandomAccessIterator, class T>
	inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*) {
		__pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first));
		//���ϣ����� implicit representation heap �Ĵ������ԣ�pop�����Ľ��ӦΪ�ײ������ĵ�һ��Ԫ��
		//��ˣ������趨������ֵΪβ�ã�Ȼ����ֵ����β�ڵ�(�������Ͻ������� result ��Ϊ last-1)
		//Ȼ������ [first, last-1)��ʹ֮���³�һ���ϸ��heap
	}

	template <class RandomAccessIterator> 
	inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
		__pop_heap_aux(first, last, value_type(first));
	}

	template <class RandomAccessIterator>
	void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
		//���£�ÿִ��һ�� pop_heap()������ֵ������β��
		//�۳�β����ִ��һ�� pop_heap()���μ���ֵ�ֱ�������β��
		//ѭ����������
		while (last - first > 1) {
			pop_heap(first, last--);	//ÿִ��һ�Σ�������Χ����һ��
		}
	}

	//�������� make_heap() ������ָ����С�Ƚϱ�׼
	template <class RandomAccessIterator, class T, class Distance>
	void __make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*) {
		if (last - first < 2) return;	//�������Ϊ 0 �� 1��������������
		Distance len = last - first;
		//�ҳ���һ����Ҫ���ŵ�����ͷ������ holeIndex ��ʾ��(�κνڵ㶼����Ҫִ�� perlocate down )
		Distance holeIndex = (len - 2) / 2;
		while (true) {
			//������ holeIndex Ϊ�׵�������len��Ϊ���� __adjust_heap() �жϲ�����Χ
			__adjust_heap(first, holeIndex, len, T(*(first + holeIndex)));
			if (holeIndex == 0) return;		//������ڵ�ͽ���
			--holeIndex;					//(�������ŵ�������)ͷ����ǰһ���ڵ�
		}
	}

	template <class RandomAccessIterator>
	inline void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
		__make_heap(first, last, value_type(first), distance_type(first));
	}

__MYSTL_NAMESPACE_END_

#endif // !_ZSTL_HEAP_
