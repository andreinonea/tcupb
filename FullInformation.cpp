#include "FullInformation.h"
#include "Utils.h"

#include <algorithm>
#include <iostream>

void
KTC_FiAlgo::classify(
	const KTC_FiAlgoVersion version,
	const KTC_Data &trades,
	const KTC_Data &ask,
	const KTC_Data &bid,
	FP_TYPE bar)
{
	auto vars = KTC_FiAlgo::extract_variables(version, trades, ask, bid);

	switch (version)
	{
	case KTC_FiAlgoVersion::DS_1:
		KTC_FiAlgo::sign_trades_ds1(vars, bar);
		break;
	case KTC_FiAlgoVersion::DS_2:
		KTC_FiAlgo::sign_trades_ds2(vars, bar);
		break;
	default:
	case KTC_FiAlgoVersion::DS_3:
		KTC_FiAlgo::sign_trades_ds3(vars, bar);
		break;
	}

	// TODO: Plus some other things.
}

int_vector
KTC_FiAlgo::delta_vol(const int_vector &price, const int_vector &volume, bool is_ask)
{
	int_vector vdiff = vec_diff(volume);
	vdiff.push_back(0);
	vdiff *= -1;

	auto p_second_last = std::prev(price.end());
	auto p_second = std::next(price.begin());

	int_vector up = vec_less(price.begin(), p_second_last, p_second, price.end());
	up.push_back(0);

	int_vector down = vec_greater(price.begin(), p_second_last, p_second, price.end());
	down.push_back(0);

	if (is_ask)
	{
		for (int i = 0; i < vdiff.size(); ++i)
		{
			if (up[i] == 1)
				vdiff[i] = volume[i];

			if (down[i] == 1)
				vdiff[i] = -1;
		}
	}
	else
	{
		for (int i = 0; i < vdiff.size(); ++i)
		{
			if (down[i] == 1)
				vdiff[i] = volume[i];

			if (up[i] == 1)
				vdiff[i] = -1;
		}
	}

	return vdiff;
}

KTC_FiAlgoVariables
KTC_FiAlgo::extract_variables(
	const KTC_FiAlgoVersion version,
	const KTC_Data &trades,
	const KTC_Data &ask,
	const KTC_Data &bid)
{
	fp_vector t_time_sorted{};
	fp_vector t_time_unique{};
	std::copy(trades.time.begin(), trades.time.end(), std::back_inserter(t_time_unique));
	std::sort(t_time_unique.begin(), t_time_unique.end());
	std::copy(t_time_unique.begin(), t_time_unique.end(), std::back_inserter(t_time_sorted));
	auto fin = std::unique(t_time_unique.begin(), t_time_unique.end());
	t_time_unique.resize(std::distance(t_time_unique.begin(), fin));

	KTC_Pair a = quote_index(ask.time, t_time_unique);
	KTC_Pair b = quote_index(bid.time, t_time_unique);

	fp_vector askit = interpolate_time(ask.time);
	fp_vector bidit = interpolate_time(bid.time);
	debug_vector(askit);
	debug_vector(bidit);

	int_vector askp(ask.price.begin(), ask.price.end());
	int_vector bidp(bid.price.begin(), bid.price.end());
	debug_vector(askp);
	debug_vector(bidp);

	int_vector askv(ask.volume.begin(), ask.volume.end());
	int_vector bidv(bid.volume.begin(), bid.volume.end());
	if (version != KTC_FiAlgoVersion::DS_3)
	{
		askv = KTC_FiAlgo::delta_vol(askp, askv);
		bidv = KTC_FiAlgo::delta_vol(bidp, bidv, false);
	}
	debug_vector(askv);
	debug_vector(bidv);

	int avsum = 0;
	for (auto& el : askv)
		avsum += el;
	std::cout << "avsum=" << avsum << '\n';

	int bvsum = 0;
	for (auto& el : bidv)
		bvsum += el;
	std::cout << "bvsum=" << bvsum << '\n';

	int_vector tr_n(t_time_unique.size(), 0);
	FP_TYPE last_seen = -1.0;
	INT_TYPE last_seen_idx = -1;
	for (auto &el : t_time_sorted)
	{
		if (el == last_seen)
			++tr_n[last_seen_idx];
		else
		{
			++tr_n[++last_seen_idx];
			last_seen = el;
		}
	}
	debug_vector_n(tr_n, 5);
	int tr_nsum = 0;
	for (auto& el : tr_n)
		tr_nsum += el;
	std::cout << "tr_nsize=" << tr_n.size() << '\n';
	std::cout << "tr_nsum=" << tr_nsum << '\n';

	int_vector p(trades.price.begin(), trades.price.end());
	int_vector v(trades.volume.begin(), trades.volume.end());

	return KTC_FiAlgoVariables {
		.P = p,
		.V = v,
		.Al = a.left,
		.Ar = a.right,
		.Bl = b.left,
		.Br = b.right,
		.runlength = tr_n,
		.askp = askp,
		.bidp = bidp,
		.avdiff = askv,
		.bvdiff = bidv,
		.atime = askit,
		.btime = bidit,
	};
}

KTC_Result
KTC_FiAlgo::sign_trades_ds1(const KTC_FiAlgoVariables &vars, FP_TYPE bar)
{
	return KTC_Result {};
}

KTC_Result
KTC_FiAlgo::sign_trades_ds2(const KTC_FiAlgoVariables &vars, FP_TYPE bar)
{
	return KTC_Result {};
}

KTC_Result
KTC_FiAlgo::sign_trades_ds3(const KTC_FiAlgoVariables &vars, FP_TYPE bar)
{
	return KTC_Result {};
}
