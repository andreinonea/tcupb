//
// Created by Mire on 21.04.2024.
//

#include "LeeReady.h"
#include "Utils.h"

#include <cmath>
#include <iostream>
#include <numeric>
#include <unordered_set>
#include <vector>

KTC_LeeReady::Result
KTC_LeeReady::classify(
	const KTC_Data &trades,
	const KTC_Data &ask,
	const KTC_Data &bid,
	BOOL_TYPE interpolate)
{
	KTC_Interpolation ktcInterpolation;
	if (interpolate)
			ktcInterpolation = tcol_interpolation(ask.time, bid.time);
	else
			ktcInterpolation = KTC_Interpolation { ask.time, bid.time };

	fp_vector unique_times;
	unique_times.reserve(trades.time.size());

	std::unordered_set<FP_TYPE> seen;

	for (auto &time : trades.time)
		if (seen.insert(time).second)
			unique_times.push_back(time);

	fp_vector midpoint = get_midpoint(ktcInterpolation, ask.price, bid.price, unique_times);

	FP_TYPE max_val = -fp_inf();
	FP_TYPE min_val = fp_inf();
	FP_TYPE sum_val = 0;

	for(auto &val : midpoint)
		if(!std::isnan(val))
		{
			max_val = (val > max_val) ? val : max_val;
			min_val = (val < min_val) ? val : min_val;
			sum_val += val;
		}

	std::cout << "Max: " << max_val << ", Min: " << min_val << ", Sum: " << sum_val << "\n";
	std::cout << "Max: " << max_val << ", Min: " << min_val << ", Sum: " << sum_val << "\n";
	return KTC_Result {};
}
