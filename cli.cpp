#include <iostream>
#include <vector>

#include "Utils.h"
// #include <tcupb/Utils.h>
// #include <DataFrame/Utils/DateTime.h>

#define debug_vector(v) \
do { \
	std::cout << #v " ["; \
	for (auto el: v) \
		std::cout << el << ", "; \
	std::cout << "]\n"; \
} while (0)

int
main()
{
	std::vector<FP_TYPE> q_t {2, 2, 2};
	std::vector<FP_TYPE> tr_t {1, 3, 1};
	debug_vector(q_t);
	debug_vector(tr_t);

	auto inter = get_ind(q_t, tr_t);
	std::vector<INT_TYPE> &inter_left = inter.first;
	std::vector<INT_TYPE> &inter_right = inter.second;
	debug_vector(inter_left);
	debug_vector(inter_right);

	auto ind = quote_index(q_t, tr_t);
	std::vector<INT_TYPE> &left = ind.first;
	std::vector<INT_TYPE> &right = ind.second;
	debug_vector(left);
	debug_vector(right);

	return 0;
}
