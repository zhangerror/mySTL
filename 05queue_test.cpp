#include <iostream>
#include "05zstl_queue.h"
//#include "02zstl_list.h"

int main() {
	mySTL::queue<int> iqueue;
	//mySTL::queue<int, mySTL::list<int>> iqueue;
	iqueue.push(1);
	iqueue.push(3);
	iqueue.push(5);
	iqueue.push(7);

	std::cout << "size = " << iqueue.size() << std::endl;
	std::cout << "front = " << iqueue.front() << std::endl;

	iqueue.pop();
	std::cout << "front = " << iqueue.front() << std::endl;
	iqueue.pop();
	std::cout << "front = " << iqueue.front() << std::endl;
	iqueue.pop();
	std::cout << "front = " << iqueue.front() << std::endl;
	std::cout << "size = " << iqueue.size() << std::endl;

	return 0;
}