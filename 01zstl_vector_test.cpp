//#include <iostream>
//#include "01zstl_vector.h"
//
//int main() {
//	mySTL::vector<int> v0;
//
//	mySTL::vector<int> v1(3, 1);
//
//	mySTL::vector<int> v2(v1);
//
//	mySTL::vector<int> v3 = { 0, 1, 2, 3, 4, 5, 6 };
//
//	for (int i = 0; i < 3; ++i) {
//		std::cout << "v1[" << i << "] = " << v1[i] << "\t";
//		std::cout << "v2[" << i << "] = " << v1[i] << std::endl;
//	}
//
//	for (int i = 0; i < v3.size(); ++i) {
//		std::cout << "v3[" << i << "] = " << v3[i] << std::endl;
//	}
//	std::cout << std::endl;
//
//	std::cout << "v0.size() = " << v0.size() << "  v0.capacity() = " << v0.capacity() << std::endl;
//	v0.push_back(0);
//	std::cout << "v0.size() = " << v0.size() << "  v0.capacity() = " << v0.capacity() << std::endl;
//	v0.push_back(1);
//	std::cout << "v0.size() = " << v0.size() << "  v0.capacity() = " << v0.capacity() << std::endl;
//	v0.push_back(2);
//	std::cout << "v0.size() = " << v0.size() << "  v0.capacity() = " << v0.capacity() << std::endl;
//	v0.push_back(3);
//	std::cout << "v0.size() = " << v0.size() << "  v0.capacity() = " << v0.capacity() << std::endl;
//
//	for (int i = 0; i < v0.size(); ++i) {
//		std::cout << "v0[" << i << "] = " << v0[i] << std::endl;
//	}
//	std::cout << std::endl;
//
//	mySTL::vector<int>::iterator it = std::find(v0.begin(), v0.end(), 1);
//	if (it != v0.end()) {
//		v0.insert(it, 1, 10);
//	}
//	std::cout << "v0.size() = " << v0.size() << "  v0.capacity() = " << v0.capacity() << std::endl;
//	for (int i = 0; i < v0.size(); ++i) {
//		std::cout << "v0[" << i << "] = " << v0[i] << std::endl;
//	}
//	std::cout << std::endl;
//
//	v0.erase(it);
//	std::cout << "v0.size() = " << v0.size() << "  v0.capacity() = " << v0.capacity() << std::endl;
//	
//	for (int i = 0; i < v0.size(); ++i) {
//		std::cout << "v0[" << i << "] = " << v0[i] << std::endl;
//	}
//	std::cout << std::endl;
//
//	while (!v0.empty()) {
//		v0.pop_back();
//	}
//	std::cout << "v0.size() = " << v0.size() << "  v0.capacity() = " << v0.capacity() << std::endl;
//
//
//	return 0;
//}