#include <iostream>
#include <vector>

#include "Utils.h"
// #include <tcupb/Utils.h>
#include <DataFrame/Utils/DateTime.h>
#include "FullInformation.h"

#include "csv.h"

int
main()
{
	// fp_vector q_t {2, 2, 2};
	// fp_vector tr_t {1, 3, 1};
	// debug_vector(q_t);
	// debug_vector(tr_t);

	// auto inter = get_ind(q_t, tr_t);
	// int_vector &inter_left = inter.first;
	// int_vector &inter_right = inter.second;
	// debug_vector(inter_left);
	// debug_vector(inter_right);

	// auto ind = quote_index(q_t, tr_t);
	// int_vector &left = ind.first;
	// int_vector &right = ind.second;
	// debug_vector(left);
	// debug_vector(right);


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
		io::CSVReader<5, io::trim_chars<' ', '\t'>, io::double_quote_escape<',','\"'>> qt_data("AAPL_quotes_2021-10-26.csv");
		qt_data.read_header(io::ignore_extra_column, "timestamp", "ask_price", "ask_size", "bid_price", "bid_size");
		const char *timestamp;
		FP_TYPE ask_price, ask_size;
		FP_TYPE bid_price, bid_size;

		while (qt_data.read_row(timestamp, ask_price, ask_size, bid_price, bid_size))
		{
			hmdf::DateTime datetime(timestamp, hmdf::DT_DATE_STYLE::ISO_STYLE, hmdf::DT_TIME_ZONE::GMT);
			// double h = datetime.hour() * 3600.0;
			// double m = datetime.minute() * 60.0;
			// double s = datetime.sec();
			// double us = datetime.microsec() * 0.000001;
			// double t = h + m + s + us;
			// double faraus = h + m + s;
			// std::cout << "h = " << h << '\n';
			// std::cout << "m = " << m << '\n';
			// std::cout << "s = " << s << '\n';
			// std::cout << "us = " << us << '\n';
			// std::cout << "faraus = " << faraus << '\n';
			// std::cout << "t = " << t << '\n';
			// std::cout << "h + m + s + us = " << h + m + s + us << '\n';
			// std::cout << "-----\n";
			double t = datetime.hour() * 3600.0 + datetime.minute() * 60.0 + datetime.sec() + datetime.microsec() * 0.000001;
			ask.time.push_back(t);
			ask.price.push_back(ask_price);
			ask.volume.push_back(ask_size);

			bid.time.push_back(t);
			bid.price.push_back(bid_price);
			bid.volume.push_back(bid_size);
		}
	}

	std::cout << "len(df_tr.time)=" << trades.time.size() << '\n';
	KTC_FullInformation::classify(KTC_Algorithm::FULL_INFORMATION_DS_1, trades, ask, bid);

	return 0;
}
