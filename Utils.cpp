#include "Utils.h"

#include <utility>
#include <vector>

std::pair<std::vector<INT_TYPE>, std::vector<INT_TYPE>>
get_ind(const std::vector<FP_TYPE> &arr, const std::vector<FP_TYPE> &time)
{
	INT_TYPE n = arr.size();
	INT_TYPE s = time.size();

	std::vector<INT_TYPE> left(n, 0);
	std::vector<INT_TYPE> right(n, 0);

	BOOL_TYPE found;
	INT_TYPE begin, end, k;
	INT_TYPE j, i;
	UINT_TYPE ind = 0;

	std::vector<FP_TYPE> tmp;
	FP_TYPE v;

	for (j = 0; j < n; ++j)
	{
		begin = s;
		end = 0;
		found = false;
		tmp = std::vector<FP_TYPE>(arr.begin() + ind, arr.end());
		k = tmp.size();

		for (i = 0; i < k; ++i)
		{
			v = tmp[i] - time[j];

			if ((!found) && (v >= 0))
			{
				found = true;
				begin = ind + i;
			}

			if (found)
			{
				if (v > 0)
				{
					ind += i;
					end = ind;
					break;
				}
				else if (i == (k - 1))
				{
					end = s;
					break;
				}
			}
		}

		left[j] = begin;
		right[j] = end;
	}

	return std::make_pair(left, right);
}

std::pair<std::vector<INT_TYPE>, std::vector<INT_TYPE>>
quote_index(const std::vector<FP_TYPE> &q_t, const std::vector<FP_TYPE> &tr_t)
{
	auto ind = get_ind(q_t, tr_t);
	std::vector<INT_TYPE> &left = ind.first;
	std::vector<INT_TYPE> &right = ind.second;

	for (int i = 0; i < left.size(); ++i)
	{
		if (left[i] < right[i])
			right[i] -= 1;

		left[i] -= 1;

		if (left[i] < 0)
			left[i] = 0;
	}

	return std::make_pair(left, right);
}
