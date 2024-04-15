#include "Utils.h"

#include <iostream>
#include <utility>
#include <vector>

int_vector
vec_nonzero(const fp_vector &v)
{
	int_vector ind{};

	for (int i = 0; i < v.size(); ++i)
		if (v[i] != 0.0)
			ind.push_back(i);

	return ind;
}

std::pair<int_vector, int_vector>
get_ind(const fp_vector &arr, const fp_vector &time)
{
	INT_TYPE s = arr.size();
	INT_TYPE n = time.size();

	std::cout << "s=" << s << '\n';
	std::cout << "n=" << n << '\n';

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

		if (j > (n - 3))
		{
			std::cout << "it " << j << ": tmp: ";
			for (int z = 0; z < k; ++z)
				std::cout << tmp[z] << ' ';
			std::cout << '\n';
			std::cout << '\n';
		}

		for (i = 0; i < k; ++i)
		{
			v = tmp[i] - time[j];

			if ((!found) && (v >= 0.0))
			{
				found = true;
				begin = ind + i;
			}

			if (found)
			{
				if (v > 0.0)
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
	std::cout << "orig\n";
	debug_vector(q_t);
	debug_vector(tr_t);

	auto ind = get_ind(q_t, tr_t);
	int_vector &left = ind.first;
	int_vector &right = ind.second;

	std::cout << "get_ind\n";
	debug_vector(ind.first);
	debug_vector(ind.second);

	for (int i = 0; i < left.size(); ++i)
	{
		if (left[i] < right[i])
			right[i] -= 1;

		left[i] -= 1;

		if (left[i] < 0)
			left[i] = 0;
	}

	std::cout << "quote_index\n";
	debug_vector(left);
	debug_vector(right);

	return std::make_pair(left, right);
}

fp_vector
concat_runs(const int_vector &x, bool hj_version)
{
	int len = 0;
	for (auto& el : x)
		len += el;

	fp_vector interp(len, 0.0);
	INT_TYPE count = 0;

	if (hj_version)
	{
		for (int i = 0; i < x.size(); ++i)
		{
			FP_TYPE k = x[i] + 1.0;

			for (int j = 1; j < x[i] + 1; ++j)
				interp[count++] = ((2 * j) - 1) / (2 * k);
		}
	}
	else
	{
		for (int i = 0; i < x.size(); ++i)
		{
			FP_TYPE k = x[i] + 1.0;

			for (int j = 1; j < x[i] + 1; ++j)
				interp[count++] = j / k;
		}
	}

	return interp;
}

fp_vector
interpolate_time(const fp_vector &time, bool hj_version)
{
	fp_vector run = vec_diff(time);
	run.insert(run.begin(), 0.0);

	int_vector ind = vec_nonzero(run);
	ind.push_back(run.size());
	ind.insert(ind.begin(), 0);
	int_vector runlength = vec_diff(ind);

	fp_vector intertime = concat_runs(runlength, hj_version);
	debug_vector(intertime);

	return time + intertime;
}
