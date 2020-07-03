#ifndef _ZSTL_HEAP_FIX_H_
#define _ZSTL_HEAP_FIX_H_

#include "00zstl_algobase.h"
#include "01zstl_vector.h"

__MYSTL_NAMESPACE_BEGIN_
	/* [default]bottom container : vector */
	template<class T, class Sequence = vector<T>,
			class Compare = less<typename Sequence::value_type> >
	class heap {
	public:
		typedef typename Sequence::iterator			iterator;
		typedef typename Sequence::size_type		size_type;
		typedef typename Sequence::value_type		value_type;
		typedef typename Sequence::difference_type	Distance;
		typedef typename Sequence::reference		reference;

	protected:
		/* percolate up */
		void percolate_up(iterator first, Distance topIndex, Distance holeIndex, value_type value);

		/* percolate down first, then up*/
		void adjust(iterator first, Distance holeIndex, Distance len, value_type value);
		
		/* return but no remove */
		void pop_heap(iterator first, iterator last) {
			value_type value = *(last - 1);
			*(last - 1) = *first;

			adjust(first, Distance(0), Distance(last - first - 1), value);

		}
	public:
		heap() :m_c() { }
		heap(const Sequence & c) :m_c(c) {
			make_heap();
		}

		bool operator == (const heap<T, Sequence>& x) {
			return (m_c == x.m_c);
		}

		bool operator < (const heap<T, Sequence>& x) {
			return (m_c < x.m_c);
		}

		bool empty() const { return m_c.empty(); }
		size_type size() const { return m_c.size(); }

		/* return the top of heap */
		reference top() { return m_c.front(); }

		void make_heap();

		void sort() {
			iterator first = m_c.begin();
			iterator end = m_c.end();
			while (end - first > 1) {
				pop_heap(first, end);
				--end;
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
		Compare  comp;
	};

	template<class T, class Sequence, class Compare>
	void heap<T, Sequence, Compare>::percolate_up(iterator first, Distance topIndex, Distance holeIndex, value_type value) {
		Distance parent = (holeIndex - 1) / 2;

		while ((holeIndex > topIndex) && (comp(*(first + parent), value))) {
			*(first + holeIndex) = *(first + parent);
			holeIndex = parent;
			parent = (holeIndex - 1) / 2;
		}

		*(first + holeIndex) = value;
	}

	template<class T, class Sequence, class Compare>
	void heap<T, Sequence, Compare>::adjust(iterator first, Distance holeIndex, Distance len, value_type value) {
		Distance topIndex = holeIndex;
		Distance secondChild = 2 * (holeIndex + 1);

		while (secondChild < len) {
			if (comp(*(first + secondChild), *(first + secondChild - 1))) {
				--secondChild;
			}
			
			*(first + holeIndex) = *(first + secondChild);
			holeIndex = secondChild;
			secondChild = 2 * (secondChild + 1);
		}
		if (secondChild == len) {
			*(first + holeIndex) = *(first + secondChild - 1);
			holeIndex = secondChild - 1;
		}

		percolate_up(first, topIndex, holeIndex, value);
	}

	template <class T, class Sequence, class Compare> 
	void heap<T, Sequence, Compare>::make_heap() {
		iterator first = m_c.begin();
		iterator last = m_c.end();
		Distance len = last - first;
		if (len < 2) return;

		Distance holeIndex = (len - 2) / 2;
		while (true) {
			adjust(first, holeIndex, len, *(first + holeIndex));
			if (holeIndex == 0) return;
			--holeIndex;
		}
	}

__MYSTL_NAMESPACE_END_

#endif // !_ZSTL_HEAP_FIX_H_
