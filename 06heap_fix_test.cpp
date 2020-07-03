#include <iostream>
#include "01zstl_vector.h"
#include "06zstl_heap_fix.h"

int main() {
	mySTL::vector<int> ivec = { 0, 1, 2, 3, 4, 8, 9, 3, 5 };
	mySTL::heap<int> tHeap(ivec);
	//mySTL::heap<int, mySTL::vector<int>, mySTL::greater<int> > tHeap(ivec);
	for (int i = 0; i < tHeap.m_c.size(); ++i) {	//9 5 8 3 4 0 2 3 1
		std::cout << tHeap.m_c[i] << " ";
	}
	std::cout << std::endl;

	tHeap.push(7);
	for (int i = 0; i < tHeap.m_c.size(); ++i) {	//9 7 8 3 5 0 2 3 1 4
		std::cout << tHeap.m_c[i] << " ";
	}
	std::cout << std::endl;

	tHeap.pop();
	for (int i = 0; i < tHeap.m_c.size(); ++i) {	//8 7 4 3 5 0 2 3 1
		std::cout << tHeap.m_c[i] << " ";
	}
	std::cout << std::endl;

	tHeap.sort();
	for (int i = 0; i < tHeap.m_c.size(); ++i) {	//0 1 2 3 3 4 5 7 8
		std::cout << tHeap.m_c[i] << " ";
	}
	std::cout << std::endl;

	system("pause");
	return 0;
}