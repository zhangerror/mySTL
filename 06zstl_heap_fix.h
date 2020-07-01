#ifndef _ZSTL_HEAP_FIX_H_
#define _ZSTL_HEAP_FIX_H_

#include "00zstl_iterator.h"
#include "01zstl_vector.h"

__MYSTL_NAMESPACE_BEGIN_
	/* [default]bottom container : vector */
	template<class T, class Sequence = vector<T>>
	class heap {
	public:
		typedef typename Sequence::iterator			iterator;
		typedef typename Sequence::size_type		size_type;
		typedef typename Sequence::value_type		value_type;
		typedef typename Sequence::difference_type	Distance;

	protected:
		/* percolate up */
		void percolate_up(iterator first, Distance topIndex, Distance holeIndex, value_type value);

		/* percolate down */
		void percolate_down(iterator first, Distance holeIndex, Distance len, value_type value);

		/* return but no remove */
		void pop_heap(iterator first, iterator last) {
			value_type value = *(last - 1);
			*(last - 1) = *first;

			percolate_down(first, Distance(0), Distance(last - first - 1), value);
		}
	public:
		heap() :m_c() { }
		heap(const Sequence & c) :m_c(c) {
			make_heap();
		}

		void make_heap();

		void sort() {
			iterator first = m_c.begin();
			iterator end = m_c.end();
			while (end - first > 1) {
				pop_heap(first, end--);
			}
		}

		void push(const value_type & x) {
			m_c.push_back(x);
			percolate_up(m_c.begin(), Distance(0), Distance(m_c.size() - 1), m_c.back());
		}

		void pop() {
			pop_heap(m_c.begin(), m_c.end());
			m_c.pop_back();
		}

		Sequence m_c;		//bottom container
	};

	template<class T, class Sequence>
	void heap<T, Sequence>::percolate_up(iterator first, Distance topIndex, Distance holeIndex, value_type value) {
		Distance parent = (holeIndex - 1) / 2;

		while ((holeIndex > topIndex) && (value > *(first + parent))) {
			*(first + holeIndex) = *(first + parent);
			holeIndex = parent;
			parent = (holeIndex - 1) / 2;
		}

		*(first + holeIndex) = value;
	}

	template<class T, class Sequence>
	void heap<T, Sequence>::percolate_down(iterator first, Distance holeIndex, Distance len, value_type value) {
		Distance secondChild = 2 * holeIndex + 2;

		while (secondChild < len) {
			if (*(first + secondChild) < *(first + secondChild - 1)) {
				--secondChild;
			}
			
			*(first + holeIndex) = *(first + secondChild);
			holeIndex = secondChild;
			secondChild = 2 * (secondChild + 1);
		}
		if ((secondChild == len) && (*(first + secondChild - 1) > value)) {
			*(first + holeIndex) = *(first + (secondChild - 1));
			holeIndex = secondChild - 1;
		}
		*(first + holeIndex) = value;
	}

	template <class T, class Sequence> 
	void heap<T, Sequence>::make_heap() {
		iterator first = m_c.begin();
		iterator last = m_c.end();
		Distance len = last - first;
		if (len < 2) return;

		Distance holeIndex = (len - 2) / 2;
		while (true) {
			percolate_down(first, holeIndex, len, *(first + holeIndex));
			if (holeIndex == 0) return;
			--holeIndex;
		}
	}

__MYSTL_NAMESPACE_END_

#endif // !_ZSTL_HEAP_FIX_H_
