#include <iostream>
#include <vector>

#include "Utils.h"
#include <DataFrame/Utils/DateTime.h>
#include "FullInformation.h"

#include "csv.h"

int
main()
{
	KTC_Data trades{};
	{
		io::CSVReader<3, io::trim_chars<' ', '\t'>, io::double_quote_escape<',','\"'>> t_data("AAPL_trades_2021-10-26.csv");
		t_data.read_header(io::ignore_extra_column, "timestamp", "price", "size");
		const char *timestamp;
		FP_TYPE price, size;

		int line = 2;
		while (t_data.read_row(timestamp, price, size))
		{
			hmdf::DateTime datetime(timestamp, hmdf::DT_DATE_STYLE::ISO_STYLE, hmdf::DT_TIME_ZONE::GMT);
			double t = datetime.hour() * 3600.0 + datetime.minute() * 60.0 + datetime.sec() + datetime.microsec() * 0.000001;
			trades.time.push_back(t);
			trades.price.push_back(price);
			trades.volume.push_back(size);
		}
	}

	std::cout.precision(3);
	std::cout << std::fixed;

	KTC_Data ask{};
	KTC_Data bid{};
	{
		io::CSVReader<5, io::trim_chars<' ', '\t'>, io::double_quote_escape<',','\"'>> q_data("AAPL_quotes_2021-10-26.csv");
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

	KTC_Result res = KTC_FiAlgo::classify(KTC_FiAlgoVersion::DS_3, trades, ask, bid, 0.6);

	std::cout << "\n\n==== FINAL ====\n";

	debug_vector_n(res.initiator, 5);
	debug_vector_n(res.step, 5);
	std::cout << "isize=" << res.initiator.size() << '\n';
	std::cout << "ssize=" << res.step.size() << '\n';

	int isum = 0;
	for (auto& el : res.initiator)
		isum += el;
	std::cout << "isum=" << isum << '\n';
	int ssum = 0;
	for (auto& el : res.step)
		ssum += el;
	std::cout << "ssum=" << ssum << '\n';

	return 0;
}
