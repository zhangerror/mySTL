#include <iostream>
#include "07zstl_priority_queue.h"

int main() {
	mySTL::vector<int> ivec = { 0,1,2,3,4,8,9,3,5 };
	mySTL::priority_queue<int, mySTL::heap<int, mySTL::vector<int>, mySTL::greater<int>> > ipq(ivec);
	std::cout << "size = " << ipq.size() << std::endl;		//size = 9

	for (int i = 0; i < ipq.size(); ++i) {
		std::cout << ipq.front() << ' ';		//9 9 9 9 9 9 9 9 9
	}
	std::cout << std::endl;

	while (!ipq.empty()) {
		std::cout << ipq.front() << ' ';		//9 8 5 4 3 3 2 1 0
		ipq.pop();
	}
	std::cout << std::endl;

	return 0;
}