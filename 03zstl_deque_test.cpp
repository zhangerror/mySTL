#include <iostream>
#include "03zstl_deque.h"

int main() {
	mySTL::deque<int, mySTL::alloc, 8> ideq(20, 9);
	std::cout << "size = " << ideq.size() << std::endl;
	//此时，构造了一个 deque ，有20个 int 元素，初值为 9，缓冲区大小为32bytes

	//为每一个元素设定新值
	for (int i = 0; i < ideq.size(); ++i) {
		ideq[i] = i;
	}

	for (int i = 0; i < ideq.size(); ++i) {
		std::cout << ideq[i] << ' ';
	}
	std::cout << std::endl;

	//在尾端增加1个元素，其值为3
	ideq.push_back(3);
	
	std::cout << ideq[ideq.size()-1] << std::endl;
	std::cout << "size = " << ideq.size() << std::endl;

	//在最前端增加1个元素，其值为99
	ideq.push_front(99);
	
	std::cout << ideq[0] << std::endl;
	std::cout << "size = " << ideq.size() << std::endl;

	//在最前端增加2个元素，其值分别为98， 97
	ideq.push_front(98);
	ideq.push_front(97);
	for (int i = 0; i < ideq.size(); ++i) {
		std::cout << ideq[i] << ' ';
	}
	std::cout << std::endl;
	std::cout << "size = " << ideq.size() << std::endl;

	//搜寻数值为 99 的元素，并打印出来
	mySTL::deque<int, mySTL::alloc, 8>::iterator itr;
	itr = std::find(ideq.begin(), ideq.end(), 99);
	std::cout << *itr << std::endl;
	std::cout << *(itr.cur) << std::endl;

	ideq.erase(itr);
	std::cout << *itr << std::endl;
	ideq.insert(itr, 100);
	std::cout << *itr << std::endl;
	for (int i = 0; i < ideq.size(); ++i) {
		std::cout << ideq[i] << ' ';
	}
	std::cout << std::endl;
	std::cout << "size = " << ideq.size() << std::endl;

	return 0;
}