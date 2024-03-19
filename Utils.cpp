#include "Utils.h"

#include <utility>
#include <vector>

std::pair<std::vector<INT_TYPE>, std::vector<INT_TYPE>>
get_ind (const std::vector<FP_TYPE> &vfp_arr, const std::vector<FP_TYPE> &vfp_time)
{
	INT_TYPE int_n = vfp_arr.size();
	INT_TYPE int_s = vfp_time.size();

	std::vector<INT_TYPE> vint_left(int_n, 0);
	std::vector<INT_TYPE> vint_right(int_n, 0);

	BOOL_TYPE b_found;
	INT_TYPE int_begin, int_end, int_k;
	INT_TYPE int_j, int_i;
	UINT_TYPE uint_ind = 0;

	std::vector<FP_TYPE> vfp_tmp;
	FP_TYPE fp_v;

	for (int_j = 0; int_j < int_n; ++int_j)
	{
		int_begin = int_s;
		int_end = 0;
		b_found = false;
		vfp_tmp = std::vector<FP_TYPE>(vfp_arr.begin() + uint_ind, vfp_arr.end());
		int_k = vfp_tmp.size();

		for (int_i = 0; int_i < int_k; ++int_i)
		{
			fp_v = vfp_tmp[int_i] - vfp_time[int_j];

			if ((!b_found) && (fp_v >= 0))
			{
				b_found = true;
				int_begin = uint_ind + int_i;
			}

			if (b_found)
			{
				if (fp_v > 0)
				{
					uint_ind += int_i;
					int_end = uint_ind;
					break;
				}
				else if (int_i == (int_k - 1))
				{
					int_end = int_s;
					break;
				}
			}
		}

		vint_left[int_j] = int_begin;
		vint_right[int_j] = int_end;
	}

	return std::make_pair(vint_left, vint_right);
}
