#include "11zstl_hashtable.h"

using namespace mySTL;

int main() {
	hashtable<int, int, hash<int>, identity<int>, equal_to<int>, alloc>
		iht(50, hash<int>(), equal_to<int>());

	std::cout << iht.size() << std::endl;				//0
	std::cout << iht.bucket_count() << std::endl;		//53
	std::cout << iht.max_bucket_count() << std::endl;	//4294967291

	iht.insert_unique(59);
	iht.insert_unique(63);
	iht.insert_unique(108);
	iht.insert_unique(2);
	iht.insert_unique(53);
	iht.insert_unique(55);
	std::cout << iht.size() << std::endl;		//6

	hashtable<int, int, hash<int>, identity<int>, equal_to<int>, alloc>::iterator it = iht.begin();
	for (int i = 0; i < iht.size(); ++i, ++it) {
		std::cout << *it << " ";
	}	//53 55 2 108 59 63
	std::cout << std::endl;

	for (int i = 0; i < iht.bucket_count(); ++i) {
		int n = iht.elems_in_bucket(i);
		if (n) {
			std::cout << "bucket[" << i << "] has " << n << " elems." << std::endl;
		}
	}
	//bucket[0] has 1 elems.
	//bucket[2] has 3 elems.
	//bucket[6] has 1 elems.
	//bucket[10] has 1 elems.

	for (int i = 0; i <= 47; ++i) {
		iht.insert_equal(i);
	}
	std::cout << iht.size() << std::endl;			//54
	std::cout << iht.bucket_count() << std::endl;	//97

	for (int i = 0; i < iht.bucket_count(); ++i) {
		int n = iht.elems_in_bucket(i);
		if (n) {
			std::cout << "bucket[" << i << "] has " << n << " elems." << std::endl;
		}
	}

	it = iht.begin();
	for (int i = 0; i < iht.size(); ++i, ++it) {
		std::cout << *it << " ";
	}	//0 1 2 2 3 4 5 6 7 8 9 10 11 108 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 53 55 59 63
	std::cout << std::endl;

	it = iht.find(2);
	if (it != iht.end()) {
		std::cout << *it << std::endl;			//2
	}
	it = iht.find(100);
	if (it != iht.end()) {
		std::cout << *it << std::endl;
	}
	std::cout << iht.count(2) << std::endl;		//2
	
	return 0;
}