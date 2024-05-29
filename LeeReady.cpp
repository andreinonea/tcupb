//
// Created by Mire on 21.04.2024.
//

#include "LeeReady.h"
#include "Utils.h"

#include <cmath>
#include <iostream>
#include <numeric>
#include <vector>

KTC_LeeReady::Result
KTC_LeeReady::classify(
	const KTC_Data &trades,
	const KTC_Data &ask,
	const KTC_Data &bid,
	BOOL_TYPE interpolate)
{
	fp_vector trades_time = trades.time;
	fp_vector ask_time = ask.time;
	fp_vector bid_time = bid.time;

	if (interpolate)
	{
		trades_time = interpolate_time(trades.time, true);
		ask_time = interpolate_time(ask.time, true);
		bid_time = interpolate_time(bid.time, true);
	}

	trades_time = vec_unique(trades_time);
	std::sort(ask_time.begin(), ask_time.end());
	std::sort(bid_time.begin(), bid_time.end());

	fp_vector midpoint = get_midpoint(ask_time, bid_time, ask.price, bid.price, trades_time);

	// TODO: BELOW UNKNOWN

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
