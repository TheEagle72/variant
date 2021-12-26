#include "variant.h"
#include <variant>
#include <string>
#include <iostream>

int main()
{
	variant<int, double> v;
	variant<int, double> v2(5.0);
	variant<int, double> w(10.0);

	v = w;

	std::cout << (v < w) << std::endl;

	std::cout << v.valueless_by_exception() << std::endl;
	v = 5.25;
	std::cout << v.valueless_by_exception() << std::endl;
	std::cout << v.index() << std::endl;

	std::cout << get<double>(w);
	std::cout << get<1>(w);
	


	return 0;
}
