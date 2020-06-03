//#include "00zalloc.h"
//#include <vector>
//#include <iostream>
//
//using namespace std;
//
////class test {
////public:
////	int _a;
////	test() {}
////	test(int a) : _a(a) {}
////	~test() {
////		_a = 0;
////	};
////
////	friend ostream & operator<<(ostream& os, const test & t) {
////		cout << t._a;
////		return os;
////	}
////};
//
//int main() {
//	int ia[5] = { 0, 1, 2, 3, 4 };
//	unsigned int i;
//
//	std::vector< int, mySTL::allocator<int> > iv(ia, ia + 5);
//	for (i = 0; i < iv.size(); ++i) {
//		std::cout << iv[i] << ' ';
//	}
//	std::cout << std::endl;
//
//	system("pause");
//	return 0;
//}

//#include <iostream>
//
//using namespace std;
//
//class test {
//	int m_i;
//public:
//	test(): m_i(0) {}
//	test(const int& _i) : m_i(_i) {}
//};
//
//void test_func(test* p) {
//
//}
//
//int main() {
//	test* ptr = new test();
//	test_func(ptr);
//
//	return 0;
//}