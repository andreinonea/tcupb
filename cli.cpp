#include <iostream>
#include <vector>

#include "tc.h"
#include "Utils.h"

int main()
{
	std::vector<FP_TYPE> dummy {0};
	auto ind = get_ind(dummy, dummy);
	std::cout << add (2, 2) << '\n';
	return 0;
}
