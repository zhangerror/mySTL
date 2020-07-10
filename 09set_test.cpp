#include "09zstl_set.h"

int main() {
	int ia[5] = { 0, 1, 2, 3, 4 };
	mySTL::set<int> iset(ia, ia + 5);

	std::cout << "size = " << iset.size() << std::endl;				//size = 5
	std::cout << "count(3) : " << iset.count(3) << std::endl;		//count(3) : 1
	mySTL::set<int>::iterator it = iset.begin();
	for (; it != iset.end(); ++it) {
		std::cout << *it << " ";									//0 1 2 3 4
	}
	std::cout << std::endl;

	iset.insert(3);
	std::cout << "size = " << iset.size() << std::endl;				//size = 5
	std::cout << "count(3) : " << iset.count(3) << std::endl;		//count(3) : 1
	iset.insert(5);
	std::cout << "size = " << iset.size() << std::endl;				//size = 6
	std::cout << "count(3) : " << iset.count(3) << std::endl;		//count(3) : 1
	it = iset.begin();
	for (; it != iset.end(); ++it) {
		std::cout << *it << " ";									//0 1 2 3 4 5
	}
	std::cout << std::endl;
	it = iset.find(3);
	iset.erase(it);
	it = iset.begin();
	for (; it != iset.end(); ++it) {
		std::cout << *it << " ";									//0 1 2 4 5
	}
	std::cout << std::endl;

	iset.erase(1);
	std::cout << "size = " << iset.size() << std::endl;				//size = 4
	std::cout << "count(3) : " << iset.count(3) << std::endl;		//count(3) : 0
	std::cout << "count(1) : " << iset.count(1) << std::endl;		//count(1) : 0

	it = iset.begin();
	for (; it != iset.end(); ++it) {
		std::cout << *it << " ";									//0 2 4 5
	}
	std::cout << std::endl;

	it = iset.find(3);												//3 not found
	if (it == iset.end()) {
		std::cout << "3 not found" << std::endl;
	}

	it = iset.find(1);												//1 not found
	if (it == iset.end()) {
		std::cout << "1 not found" << std::endl;
	}

	//*it = 9;		//error

	return 0;
}