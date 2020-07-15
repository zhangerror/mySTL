#include "13zstl_hash_multiset.h"

using namespace mySTL;

int main() {
	hash_multiset<int> iset;

	iset.insert(3);
	iset.insert(196);
	iset.insert(1);
	iset.insert(1);
	iset.insert(389);
	iset.insert(194);
	iset.insert(387);

	hash_multiset<int>::iterator it = iset.begin();
	it = iset.find(3);
	if (it != iset.end()) {
		std::cout << *it << std::endl;
	}	//3

	it = iset.find(30);
	if (it == iset.end()) {
		std::cout << "30 not found" << std::endl;
	}	//30 not found

	it = iset.begin();
	for (; it != iset.end(); ++it) {
		std::cout << *it << " ";
	}	//387 194 1 1 389 196 3

	return 0;
}
