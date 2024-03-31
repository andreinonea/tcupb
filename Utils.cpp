#include "Utils.h"

#include <utility>
#include <vector>

std::pair<int_vector, int_vector>
get_ind(const fp_vector &arr, const fp_vector &time)
{
	INT_TYPE n = arr.size();
	INT_TYPE s = time.size();

	int_vector left(n, 0);
	int_vector right(n, 0);

	BOOL_TYPE found;
	INT_TYPE begin, end, k;
	INT_TYPE j, i;
	UINT_TYPE ind = 0;

	fp_vector tmp;
	FP_TYPE v;

	for (j = 0; j < n; ++j)
	{
		begin = s;
		end = 0;
		found = false;
		tmp = fp_vector(arr.begin() + ind, arr.end());
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

std::pair<int_vector, int_vector>
quote_index(const fp_vector &q_t, const fp_vector &tr_t)
{
	auto ind = get_ind(q_t, tr_t);
	int_vector &left = ind.first;
	int_vector &right = ind.second;

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
