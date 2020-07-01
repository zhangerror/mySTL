#include <iostream>
#include "01zstl_vector.h"
#include "06zstl_heap_fix.h"

int main() {
	mySTL::vector<int> ivec = { 0, 1, 2, 3, 4, 8, 9, 3, 5 };

	mySTL::heap<int> tHeap(ivec);
	for (int i = 0; i < tHeap.m_c.size(); ++i) {
		std::cout << tHeap.m_c[i] << " ";
	}
	std::cout << std::endl;

	tHeap.push(7);
	for (int i = 0; i < tHeap.m_c.size(); ++i) {
		std::cout << tHeap.m_c[i] << " ";
	}
	std::cout << std::endl;

	tHeap.pop();
	for (int i = 0; i < tHeap.m_c.size(); ++i) {
		std::cout << tHeap.m_c[i] << " ";
	}
	std::cout << std::endl;

	tHeap.sort();
	for (int i = 0; i < tHeap.m_c.size(); ++i) {
		std::cout << tHeap.m_c[i] << " ";
	}
	std::cout << std::endl;

	system("pause");
	return 0;
}