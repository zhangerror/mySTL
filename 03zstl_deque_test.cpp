#include <iostream>
#include "03zstl_deque.h"

int main() {
	mySTL::deque<int, mySTL::alloc, 8> ideq(20, 9);
	std::cout << "size = " << ideq.size() << std::endl;
	//��ʱ��������һ�� deque ����20�� int Ԫ�أ���ֵΪ 9����������СΪ32bytes

	//Ϊÿһ��Ԫ���趨��ֵ
	for (int i = 0; i < ideq.size(); ++i) {
		ideq[i] = i;
	}

	for (int i = 0; i < ideq.size(); ++i) {
		std::cout << ideq[i] << ' ';
	}
	std::cout << std::endl;

	//��β������1��Ԫ�أ���ֵΪ3
	ideq.push_back(3);
	
	std::cout << ideq[ideq.size()-1] << std::endl;
	std::cout << "size = " << ideq.size() << std::endl;

	//����ǰ������1��Ԫ�أ���ֵΪ99
	ideq.push_front(99);
	
	std::cout << ideq[0] << std::endl;
	std::cout << "size = " << ideq.size() << std::endl;

	//����ǰ������2��Ԫ�أ���ֵ�ֱ�Ϊ98�� 97
	ideq.push_front(98);
	ideq.push_front(97);
	for (int i = 0; i < ideq.size(); ++i) {
		std::cout << ideq[i] << ' ';
	}
	std::cout << std::endl;
	std::cout << "size = " << ideq.size() << std::endl;

	//��Ѱ��ֵΪ 99 ��Ԫ�أ�����ӡ����
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