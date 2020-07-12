#include <string>
#include "10zstl_map.h"

int main() {
	mySTL::map<std::string, int> simap;
	simap[std::string("fashionNineci")] = 1;
	simap[std::string("zhang")] = 2;
	simap[std::string("hello")] = 3;
	simap[std::string("beijing")] = 4;

	mySTL::map<std::string, int>::iterator it = simap.begin();
	std::cout << "simap : ";
	for (; it != simap.end(); ++it) {				//( beijing, 4 ),  ( fashionNineci, 1 ),  ( hello, 3 ),  ( zhang, 2 ),
		std::cout << " ( " << it->first << ", " << it->second << " ), ";
	}
	std::cout << std::endl;

	mySTL::pair<std::string, int> value(std::string("comming"), 5);
	simap.insert(value);

	it = simap.begin();
	std::cout << "simap : ";
	for (; it != simap.end(); ++it) {				//( beijing, 4 ),  ( comming, 5 ),  ( fashionNineci, 1 ),  ( hello, 3 ),  ( zhang, 2 ),
		std::cout << " ( " << it->first << ", " << it->second << " ), ";
	}
	std::cout << std::endl;

	int num = simap[std::string("fashionNineci")];
	std::cout << "value of fashionNineci : " << num << std::endl; //value of fashionNineci : 1

	it = simap.find(std::string("zhangerror"));
	if (it == simap.end()) {
		std::cout << "zhangerror not found" << std::endl;
	}
	it = simap.find(std::string("zhang"));
	if (it != simap.end()) {
		std::cout << "zhang found" << std::endl;
	}

	it->second = 88;
	num = simap[std::string("zhang")];
	std::cout << "value of zhang : " << num << std::endl;		//value of zhang : 88

	std::cout << " ( zhang" << ", " << simap["zhang"] << " )" << std::endl; // ( zhang, 88 )
	simap["nothing"];
	it = simap.begin();
	std::cout << "simap : ";
	for (; it != simap.end(); ++it) {					//( beijing, 4 ),  ( comming, 5 ),  ( fashionNineci, 1 ),  ( hello, 3 ),  ( nothing, 0 ),  ( zhang, 88 ),
		std::cout << " ( " << it->first << ", " << it->second << " ), ";
	}
	std::cout << std::endl;

	return 0;
}