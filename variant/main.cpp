#include "variant.h"
#include <variant>
#include <string>
#include <iostream>

int main()
{
	variant<int, double> v(5);
	variant<int, double> w(10);

	//w = v;

	std::cout << (v < w) << std::endl;

	std::cout << v.valueless_by_exception() << std::endl;
	v = 5.25;
	std::cout << v.valueless_by_exception() << std::endl;
	std::cout << v.index() << std::endl;

	return 0;
}
