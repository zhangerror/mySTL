#include <iostream>
#include "04zstl_stack.h"

int main() {
	mySTL::stack<int> istack;
	istack.push(0);
	istack.push(1);
	istack.push(2);
	istack.push(3);

	std::cout << "size = " << istack.size() << std::endl;
	std::cout << "top = " << istack.top() << std::endl;

	istack.pop();
	std::cout << "top = " << istack.top() << std::endl;
	istack.pop();
	std::cout << "top = " << istack.top() << std::endl;
	istack.pop();
	std::cout << "top = " << istack.top() << std::endl;

	std::cout << "size = " << istack.size() << std::endl;

	return 0;
}