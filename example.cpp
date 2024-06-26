#include <iostream>
#include <optional>
#include <vector>

#include <DataFrame/Utils/DateTime.h>

#include "csv.h"
#include "Utils.h"
#include "BulkVolume.h"
#include "FullInformation.h"
#include "LeeReady.h"
#include "Emo.h"
#include "Clnv.h"


void
load_trades_from_csv(KTC_Data &trades, const char *csvpath)
{
	io::CSVReader<3, io::trim_chars<' ', '\t'>, io::double_quote_escape<',','\"'>> t_data(csvpath);
	t_data.read_header(io::ignore_extra_column, "timestamp", "price", "size");
	const char *timestamp;
	FP_TYPE price, size;

	while (t_data.read_row(timestamp, price, size))
	{
		hmdf::DateTime datetime(timestamp, hmdf::DT_DATE_STYLE::ISO_STYLE, hmdf::DT_TIME_ZONE::GMT);
		double t = datetime.hour() * 3600.0 + datetime.minute() * 60.0 + datetime.sec() + datetime.microsec() * 0.000001;
		trades.time.push_back(t);
		trades.price.push_back(price);
		trades.volume.push_back(size);
	}
}

void
load_quotes_from_csv(KTC_Data &ask, KTC_Data &bid, const char *csvpath)
{
	io::CSVReader<5, io::trim_chars<' ', '\t'>, io::double_quote_escape<',','\"'>> q_data(csvpath);
	q_data.read_header(io::ignore_extra_column, "timestamp", "ask_price", "ask_size", "bid_price", "bid_size");
	const char *timestamp;
	FP_TYPE ask_price, ask_size;
	FP_TYPE bid_price, bid_size;

	while (q_data.read_row(timestamp, ask_price, ask_size, bid_price, bid_size))
	{
		hmdf::DateTime datetime(timestamp, hmdf::DT_DATE_STYLE::ISO_STYLE, hmdf::DT_TIME_ZONE::GMT);
		double t = datetime.hour() * 3600.0 + datetime.minute() * 60.0 + datetime.sec() + datetime.microsec() * 0.000001;
		ask.time.push_back(t);
		ask.price.push_back(ask_price);
		ask.volume.push_back(ask_size);

		bid.time.push_back(t);
		bid.price.push_back(bid_price);
		bid.volume.push_back(bid_size);
	}
}

int
main()
{
	std::cout.precision(3);
	std::cout << std::fixed;

	KTC_Data trades{};
	load_trades_from_csv(trades, "trades.csv");

	KTC_Data ask{}, bid{};
	load_quotes_from_csv(ask, bid, "ibm_quotes.csv");

	// KTC_FiAlgo::Result res = KTC_FiAlgo::classify(KTC_FiAlgoVersion::DS_3, trades, ask, bid, 0.6);

	// INT_TYPE start = 3;
	// std::optional<KTC_Bvc::Result> res = KTC_Bvc::classify(
	// 	trades, 5, &start, KTC_BvcWindowType::PER_TRADE, KTC_BvcDofType::NORMAL);

	KTC_Clnv::Result res = KTC_Clnv::classify(trades, ask, bid, false);

	return 0;
}
