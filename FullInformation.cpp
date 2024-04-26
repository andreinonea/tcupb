#include "FullInformation.h"
#include "Utils.h"

#include <algorithm>

KTC_Result
KTC_FiAlgo::classify(
	const KTC_FiAlgoVersion version,
	const KTC_Data &trades,
	const KTC_Data &ask,
	const KTC_Data &bid,
	FP_TYPE bar)
{
	KTC_Result res;
	auto vars = KTC_FiAlgo::extract_variables(version, trades, ask, bid);

	switch (version)
	{
	case KTC_FiAlgoVersion::DS_1:
		res = KTC_FiAlgo::sign_trades_ds1(vars, bar);
		break;
	case KTC_FiAlgoVersion::DS_2:
		res = KTC_FiAlgo::sign_trades_ds2(vars, bar);
		break;
	default:
	case KTC_FiAlgoVersion::DS_3:
		res = KTC_FiAlgo::sign_trades_ds3(vars, bar);
		break;
	}

	apply_tick(res, trades.price);

	for (int i = 0; i < res.step.size(); ++i)
		if (res.step[i] == 0)
			res.step[i] = 4;

	return res;
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

	int_vector askp(ask.price.begin(), ask.price.end());
	int_vector bidp(bid.price.begin(), bid.price.end());

	int_vector askv(ask.volume.begin(), ask.volume.end());
	int_vector bidv(bid.volume.begin(), bid.volume.end());
	if (version != KTC_FiAlgoVersion::DS_3)
	{
		askv = KTC_FiAlgo::delta_vol(askp, askv);
		bidv = KTC_FiAlgo::delta_vol(bidp, bidv, false);
	}

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
		.askv = askv,
		.bidv = bidv,
		.atime = askit,
		.btime = bidit,
	};
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

KTC_Result
KTC_FiAlgo::sign_trades_ds1(const KTC_FiAlgoVariables &vars, FP_TYPE bar)
{
	INT_TYPE n = vars.Al.size();
	INT_TYPE anum = vars.askp.size();
	INT_TYPE bnum = vars.bidp.size();
	INT_TYPE trnum = vars.P.size();

	int_vector s(trnum, 0);
	int_vector c(trnum, 0);

	INT_TYPE i, j;
	INT_TYPE al, ar, bl, br, last_ask, last_bid, k, d_a, d_b, tmpind, p, v;

	int_vector discard_a(anum, 0);
	int_vector discard_b(bnum, 0);

	INT_TYPE ind = 0;
	BOOL_TYPE av_match, bv_match;
	FP_TYPE upper, lower;

	for (j = 0; j < n; ++j)
	{
		al = vars.Al[j];
		ar = vars.Ar[j];
		bl = vars.Bl[j];
		br = vars.Br[j];

		last_ask = vars.askp[al];
		last_bid = vars.bidp[bl];

		for (i = 0; i < vars.runlength[j]; ++i)
		{
			tmpind = ind + i;
			p = vars.P[tmpind];
			v = vars.V[tmpind];
			av_match = false;
			bv_match = false;

			for (k = 0; k < (ar - al); ++k)
				if ((discard_a[al + k] == 0)
					&& (vars.askp[al + k] == p)
					&& (vars.askv[al + k] == v))
				{
					av_match = true;
					d_a = k;
					break;
				}

			for (k = 0; k < (br - bl); ++k)
				if ((discard_b[bl + k] == 0)
					&& (vars.bidp[bl + k] == p)
					&& (vars.bidv[bl + k] == v))
				{
					bv_match = true;
					d_b = k;
					break;
				}

			if (av_match && bv_match)
			{
				if (vars.atime[al + d_a] < vars.btime[bl + d_b])
				{
					s[tmpind] = 1;
					c[tmpind] = 2;
					discard_a[al + d_a] = 1;
					last_ask = vars.askp[al + d_a];
				}
				else if (vars.atime[al + d_a] > vars.btime[bl + d_b])
				{
					s[tmpind] = -1;
					c[tmpind] = 2;
					discard_b[bl + d_b] = 1;
					last_bid = vars.bidp[bl + d_b];
				}
			}
			else if (av_match)
			{
				s[tmpind] = 1;
				c[tmpind] = 1;
				last_ask = vars.askp[al + d_a];
			}
			else if (bv_match)
			{
				s[tmpind] = -1;
				c[tmpind] = 1;
				last_bid = vars.bidp[bl + d_b];
			}
			else if (last_ask > last_bid)
			{
				upper = last_ask * (1 - bar) + last_bid * bar;
				lower = last_ask * bar + last_bid * (1 - bar);

				if (p > upper)
				{
					s[tmpind] = 1;
					c[tmpind] = 3;
				}
				else if (p < lower)
				{
					s[tmpind] = -1;
					c[tmpind] = 3;
				}
			}
		}
		ind += vars.runlength[j];
	}
	return KTC_Result { .step = c, .initiator = s };
}

KTC_Result
KTC_FiAlgo::sign_trades_ds2(KTC_FiAlgoVariables &vars, FP_TYPE bar)
{
	INT_TYPE n = vars.Al.size();
	INT_TYPE anum = vars.askp.size();
	INT_TYPE bnum = vars.bidp.size();
	INT_TYPE trnum = vars.P.size();

	int_vector s(trnum, 0);
	int_vector c(trnum, 0);

	INT_TYPE i, j;
	INT_TYPE al, ar, bl, br, last_ask, last_bid, k, d_a, d_b, tmpind, p, v;

	INT_TYPE ind = 0;
	BOOL_TYPE av_match, bv_match;
	FP_TYPE upper, lower;

	for (j = 0; j < n; ++j)
	{
		al = vars.Al[j];
		ar = vars.Ar[j];
		bl = vars.Bl[j];
		br = vars.Br[j];

		last_ask = vars.askp[al];
		last_bid = vars.bidp[bl];

		for (i = 0; i < vars.runlength[j]; ++i)
		{
			tmpind = ind + i;
			p = vars.P[tmpind];
			v = vars.V[tmpind];
			av_match = false;
			bv_match = false;

			for (k = 0; k < (ar - al); ++k)
				if ((vars.askp[al + k] == p)
					&& (vars.askv[al + k] >= v))
				{
					av_match = true;
					d_a = k;
					break;
				}

			for (k = 0; k < (br - bl); ++k)
				if ((vars.bidp[bl + k] == p)
					&& (vars.bidv[bl + k] >= v))
				{
					bv_match = true;
					d_b = k;
					break;
				}

			if (av_match && bv_match)
			{
				if (vars.atime[al + d_a] < vars.btime[bl + d_b])
				{
					s[tmpind] = 1;
					c[tmpind] = 2;
					vars.askv[al + d_a] -= v;
				}
				else if (vars.atime[al + d_a] > vars.btime[bl + d_b])
				{
					s[tmpind] = -1;
					c[tmpind] = 2;
					vars.bidv[bl + d_b] -= v;
				}
			}
			else if (av_match)
			{
				s[tmpind] = 1;
				c[tmpind] = 1;
			}
			else if (bv_match)
			{
				s[tmpind] = -1;
				c[tmpind] = 1;
			}
			else
			{
				for (k = 0; k < (ar - al); ++k)
					if ((al + k + 1) == anum)
						break;
					else if ((vars.askp[al + k] < p)
						&& (vars.askp[al + k + 1] == p))
					{
						av_match = true;
						d_a = k + 1;
						break;
					}

				for (k = 0; k < (br - bl); ++k)
					if ((bl + k + 1) == bnum)
						break;
					else if ((vars.bidp[bl + k] > p)
						&& (vars.bidp[bl + k + 1] == p))
					{
						bv_match = true;
						d_b = k + 1;
						break;
					}

				if (av_match && bv_match)
				{
					if (vars.atime[al + d_a] < vars.btime[bl + d_b])
					{
						s[tmpind] = 1;
						c[tmpind] = 6;
					}
					else if (vars.atime[al + d_a] > vars.btime[bl + d_b])
					{
						s[tmpind] = -1;
						c[tmpind] = 6;
					}
				}
				else if (av_match)
				{
					s[tmpind] = 1;
					c[tmpind] = 5;
				}
				else if (bv_match)
				{
					s[tmpind] = -1;
					c[tmpind] = 5;
				}
				else
				{
					for (k = 0; k < (ar - al); ++k)
						if ((al + k + 1) == anum)
							break;
						else if ((vars.askp[al + k] < p)
							&& (vars.askp[al + k + 1] > p))
						{
							av_match = true;
							d_a = k + 1;
							break;
						}

					for (k = 0; k < (br - bl); ++k)
						if ((bl + k + 1) == bnum)
							break;
						else if ((vars.bidp[bl + k] > p)
							&& (vars.bidp[bl + k + 1] < p))
						{
							bv_match = true;
							d_b = k + 1;
							break;
						}

					if (av_match && bv_match)
					{
						if (vars.atime[al + d_a] < vars.btime[bl + d_b])
						{
							s[tmpind] = 1;
							c[tmpind] = 8;
						}
						else if (vars.atime[al + d_a] > vars.btime[bl + d_b])
						{
							s[tmpind] = -1;
							c[tmpind] = 8;
						}
					}
					else if (av_match)
					{
						s[tmpind] = 1;
						c[tmpind] = 7;
					}
					else if (bv_match)
					{
						s[tmpind] = -1;
						c[tmpind] = 7;
					}
					else if (last_ask > last_bid)
					{
						upper = last_ask * (1 - bar) + last_bid * bar;
						lower = last_ask * bar + last_bid * (1 - bar);

						if (p > upper)
						{
							s[tmpind] = 1;
							c[tmpind] = 3;
						}
						else if (p < lower)
						{
							s[tmpind] = -1;
							c[tmpind] = 3;
						}
					}
				}
			}
		}
		ind += vars.runlength[j];
	}

	return KTC_Result { .step = c, .initiator = s };
}

KTC_Result
KTC_FiAlgo::sign_trades_ds3(KTC_FiAlgoVariables &vars, FP_TYPE bar)
{
	INT_TYPE n = vars.Al.size();
	INT_TYPE anum = vars.askp.size();
	INT_TYPE bnum = vars.bidp.size();
	INT_TYPE trnum = vars.P.size();

	int_vector s(trnum, 0);
	int_vector c(trnum, 0);

	INT_TYPE i, j;
	INT_TYPE al, ar, bl, br, last_ask, last_bid, k, d_a, d_b, tmpind, p, v;

	INT_TYPE ind = 0;
	BOOL_TYPE av_match, bv_match;
	FP_TYPE upper, lower;

	for (j = 0; j < n; ++j)
	{
		al = vars.Al[j];
		ar = vars.Ar[j];
		bl = vars.Bl[j];
		br = vars.Br[j];

		last_ask = vars.askp[al];
		last_bid = vars.bidp[bl];

		for (i = 0; i < vars.runlength[j]; ++i)
		{
			tmpind = ind + i;
			p = vars.P[tmpind];
			v = vars.V[tmpind];
			av_match = false;
			bv_match = false;

			for (k = 0; k < (ar - al); ++k)
				if ((vars.askp[al + k] == p)
					&& (vars.askv[al + k] >= v))
				{
					av_match = true;
					d_a = k;
					break;
				}

			for (k = 0; k < (br - bl); ++k)
				if ((vars.bidp[bl + k] == p)
					&& (vars.bidv[bl + k] >= v))
				{
					bv_match = true;
					d_b = k;
					break;
				}

			if (av_match && bv_match)
			{
				if (vars.atime[al + d_a] < vars.btime[bl + d_b])
				{
					s[tmpind] = 1;
					c[tmpind] = 2;
					vars.askv[al + d_a] -= v;
				}
				else if (vars.atime[al + d_a] > vars.btime[bl + d_b])
				{
					s[tmpind] = -1;
					c[tmpind] = 2;
					vars.bidv[bl + d_b] -= v;
				}
			}
			else if (av_match)
			{
				s[tmpind] = 1;
				c[tmpind] = 1;
			}
			else if (bv_match)
			{
				s[tmpind] = -1;
				c[tmpind] = 1;
			}
			else if (last_ask > last_bid)
			{
				upper = last_ask * (1 - bar) + last_bid * bar;
				lower = last_ask * bar + last_bid * (1 - bar);

				if (p > upper)
				{
					s[tmpind] = 1;
					c[tmpind] = 3;
				}
				else if (p < lower)
				{
					s[tmpind] = -1;
					c[tmpind] = 3;
				}
			}
		}
		ind += vars.runlength[j];
	}

	return KTC_Result { .step = c, .initiator = s };
}
