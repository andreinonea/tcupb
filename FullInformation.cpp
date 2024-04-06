#include "FullInformation.h"
#include "Utils.h"

#include <algorithm>

void
KTC_FullInformation::classify(
	const KTC_Algorithm version,
	const KTC_Data &trades,
	const KTC_Data &ask,
	const KTC_Data &bid)
{
	// TODO: only needed if KTC_Algorithm is FI_ALGO
	KTC_FullInformation::extract_variables(version, trades, ask, bid);

	switch (version)
	{
	case KTC_Algorithm::FULL_INFORMATION_DS_1:
		KTC_FullInformation::sign_trades_ds1();
		break;
	case KTC_Algorithm::FULL_INFORMATION_DS_2:
		KTC_FullInformation::sign_trades_ds2();
		break;
	case KTC_Algorithm::FULL_INFORMATION_DS_3:
		KTC_FullInformation::sign_trades_ds3();
		break;
	default:
		return;
	}

	// TODO: Plus some other things.
}

void
KTC_FullInformation::extract_variables(
	const KTC_Algorithm version,
	const KTC_Data &trades,
	const KTC_Data &ask,
	const KTC_Data &bid)
{
	fp_vector t_time_unique{};
	std::copy(trades.time.begin(), trades.time.end(), std::back_inserter(t_time_unique));
	std::sort(t_time_unique.begin(), t_time_unique.end());
	auto fin = std::unique(t_time_unique.begin(), t_time_unique.end());
	t_time_unique.resize(std::distance(t_time_unique.begin(), fin));

	quote_index(ask.time, t_time_unique);
	quote_index(bid.time, t_time_unique);
}

void
KTC_FullInformation::sign_trades_ds1()
{
}

void
KTC_FullInformation::sign_trades_ds2()
{
}

void
KTC_FullInformation::sign_trades_ds3()
{
}
