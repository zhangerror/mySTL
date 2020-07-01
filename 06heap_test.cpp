#include <iostream>
#include "01zstl_vector.h"
#include "06zstl_heap.h"

int main() {
	mySTL::vector<int> ivec = { 0, 1, 2, 3, 4, 8, 9, 3, 5 };

	mySTL::make_heap(ivec.begin(), ivec.end());
	for (int i = 0; i < ivec.size(); ++i) {
		std::cout << ivec[i] << " ";
	}
	std::cout << std::endl;

	ivec.push_back(7);
	mySTL::push_heap(ivec.begin(), ivec.end());
	for (int i = 0; i < ivec.size(); ++i) {
		std::cout << ivec[i] << " ";
	}
	std::cout << std::endl;

	mySTL::pop_heap(ivec.begin(), ivec.end());
	std::cout << ivec.back() << std::endl;		//return but no remove
	ivec.pop_back();							//remove last elem and no return

	for (int i = 0; i < ivec.size(); ++i) {
		std::cout << ivec[i] << " ";
	}
	std::cout << std::endl;

	mySTL::sort_heap(ivec.begin(), ivec.end());
	for (int i = 0; i < ivec.size(); ++i) {
		std::cout << ivec[i] << " ";
	}
	std::cout << std::endl;

	system("pause");
	return 0;
}