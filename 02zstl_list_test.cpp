//#include <iostream>
//#include "02zstl_list.h"
//
//int main() {
//	mySTL::list<int> ilist;
//	std::cout << "size = " << ilist.size() << std::endl;
//
//	ilist.push_back(0);
//	ilist.push_back(1);
//	ilist.push_back(2);
//	ilist.push_back(3);
//	ilist.push_back(4);
//	std::cout << "ilist size=" << ilist.size() << std::endl;
//
//	mySTL::list<int>::iterator ite;
//	std::cout << "ilist : ";
//	for (ite = ilist.begin(); ite != ilist.end(); ++ite) {
//		std::cout << *ite << ' ';
//	}
//	std::cout << std::endl;
//
//	ite = std::find(ilist.begin(), ilist.end(), 3);
//	if (ite != 0) {
//		ilist.insert(ite, 99);
//	}
//
//	std::cout << "ilist size = " << ilist.size() << std::endl;
//	std::cout << *ite << std::endl;
//
//	std::cout << "ilist : ";
//	for (ite = ilist.begin(); ite != ilist.end(); ++ite) {
//		std::cout << *ite << ' ';
//	}
//	std::cout << std::endl;
//
//	ite = std::find(ilist.begin(), ilist.end(), 1);
//	if (ite != 0) {
//		std::cout << *(ilist.erase(ite)) << std::endl;
//	}
//
//	std::cout << "ilist : ";
//	for (ite = ilist.begin(); ite != ilist.end(); ++ite) {
//		std::cout << *ite << ' ';
//	}
//	std::cout << std::endl;
//
//	ilist.push_back(4);
//	ilist.push_back(4);
//	ilist.push_back(4);
//	ilist.push_back(3);
//	ilist.push_back(2);
//	ilist.remove(0);
//	std::cout << "ilist : ";
//	for (ite = ilist.begin(); ite != ilist.end(); ++ite) {
//		std::cout << *ite << ' ';
//	}
//	std::cout << std::endl;
//	
//	ilist.unique();
//	std::cout << "ilist : ";
//	for (ite = ilist.begin(); ite != ilist.end(); ++ite) {
//		std::cout << *ite << ' ';
//	}
//	std::cout << std::endl;
//
//	ilist.remove(2);
//	std::cout << "ilist : ";
//	for (ite = ilist.begin(); ite != ilist.end(); ++ite) {
//		std::cout << *ite << ' ';
//	}
//	std::cout << std::endl;
//
//	mySTL::list<int> tlist;
//	tlist.push_front(6);
//	tlist.push_front(5);
//	tlist.push_front(3);
//	tlist.push_front(7);
//	std::cout << "tlist : ";
//	for (ite = tlist.begin(); ite != tlist.end(); ++ite) {
//		std::cout << *ite << ' ';
//	}
//	std::cout << std::endl;
//
//	ilist.sort();
//	std::cout << "ilist : ";
//	for (ite = ilist.begin(); ite != ilist.end(); ++ite) {
//		std::cout << *ite << ' ';
//	}
//	std::cout << std::endl;
//	tlist.sort();
//	std::cout << "tlist : ";
//	for (ite = tlist.begin(); ite != tlist.end(); ++ite) {
//		std::cout << *ite << ' ';
//	}
//	std::cout << std::endl;
//
//	ilist.merge(tlist);
//	std::cout << "ilist : ";
//	for (ite = ilist.begin(); ite != ilist.end(); ++ite) {
//		std::cout << *ite << ' ';
//	}
//	std::cout << std::endl;
//
//	ilist.reverse();
//	std::cout << "ilist : ";
//	for (ite = ilist.begin(); ite != ilist.end(); ++ite) {
//		std::cout << *ite << ' ';
//	}
//	std::cout << std::endl;
//
//	ilist.pop_back();
//	ilist.pop_front();
//	std::cout << "ilist : ";
//	for (ite = ilist.begin(); ite != ilist.end(); ++ite) {
//		std::cout << *ite << ' ';
//	}
//	std::cout << std::endl;
//
//	ilist.clear();
//	std::cout << "ilist size=" << ilist.size() << std::endl;
//
//	return 0;
//}