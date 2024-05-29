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
	KTC_LeeReady::Result res {};

	fp_vector trades_time = trades.time;
	fp_vector ask_time = ask.time;
	fp_vector bid_time = bid.time;

	if (interpolate)
	{
		trades_time = interpolate_time(trades.time, true);
		ask_time = interpolate_time(ask.time, true);
		bid_time = interpolate_time(bid.time, true);
	}

	fp_vector trades_time_unique = vec_unique(trades_time);
	std::sort(ask_time.begin(), ask_time.end());
	std::sort(bid_time.begin(), bid_time.end());

	fp_vector midpoint = get_midpoint(ask_time, bid_time, ask.price, bid.price, trades_time_unique);
	midpoint = vec_merge_left(trades_time, midpoint, trades_time_unique);

	res.initiator = int_vector(trades.price.size(), 0);
	res.step = int_vector(trades.price.size(), 0);

	for (int i = 0; i < trades.price.size(); ++i)
	{
		if (trades.price[i] > midpoint[i])
			res.initiator[i] = 1;
		else if (trades.price[i] < midpoint[i])
			res.initiator[i] = -1;

		if (trades.price[i] != midpoint[i])
			res.step[i] = 1;
	}

	apply_tick(res, trades.price);

	for (int i = 0; i < trades.price.size(); ++i)
		if (res.step[i] == 0)
			res.step[i] = 2;

	return res;
}
