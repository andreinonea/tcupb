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

	// fp_vector diff_test {0.0, 1.0, 3.0, 6.0};
	// fp_vector diff_test_result = vec_diff(diff_test);
	// int_vector nonzero_test_result = vec_nonzero(diff_test);
	// debug_vector(diff_test);
	// debug_vector(diff_test_result);
	// debug_vector(nonzero_test_result);
	// return 0;


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

	std::cout << "len(df_tr.time)=" << trades.time.size() << '\n';
	KTC_Result res = KTC_FiAlgo::classify(KTC_FiAlgoVersion::DS_3, trades, ask, bid);

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
