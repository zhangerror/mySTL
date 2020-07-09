#include "00zstl_RBTree.h"

int main() {
	mySTL::rb_tree<int, int, mySTL::identity<int>, mySTL::less<int> > itree;
	std::cout << "size: " <<itree.size() << std::endl;	//0

	itree.insert_unique(10);
	itree.insert_unique(7);
	itree.insert_unique(8);
	itree.insert_unique(15);
	itree.insert_unique(5);
	itree.insert_unique(6);
	itree.insert_unique(11);
	itree.insert_unique(13);
	itree.insert_unique(12);

	std::cout << "size: " << itree.size() << std::endl;	//9
	mySTL::rb_tree<int, int, mySTL::identity<int> >::iterator it = itree.begin();
	std::cout << "RBTree: ";
	for (; it != itree.end(); ++it) {
		std::cout << *it << ' ';			//5 6 7 8 10 11 12 13 15
	}
	std::cout << std::endl;

	it = itree.find(8);
	if (it != itree.end()) {
		std::cout << "find: " << *it << ' ';	//8
	}
	std::cout << std::endl;

	std::cout << "erase: " << *it << std::endl;
	itree.erase(it);
	it = itree.begin();
	std::cout << "RBTree: ";
	for (; it != itree.end(); ++it) {
		std::cout << *it << ' ';			//5 6 7 10 11 12 13 15
	}
	std::cout << std::endl;

	itree.insert_equal(5);
	std::cout << "count(5) : " << itree.count(5) << std::endl;
	it = itree.begin();
	std::cout << "RBTree: ";
	for (; it != itree.end(); ++it) {
		std::cout << *it << ' ';			//5 6 7 10 11 12 13 15
	}
	std::cout << std::endl;

	return 0;
}