#include <iostream>
#include <vector>

#include "csv.h"
#include "tc.h"
#include "Utils.h"
#include "Trades.h"
#include "Quotes.h"

int main()
{
	std::vector<FP_TYPE> dummy {0};
	auto ind = get_ind(dummy, dummy);

	//IN data for quotes and trades are stored in vectors
	std::vector<Quotes> quotes;
	std::vector<Trades> trades;

	// Initiate reading quotes from CSV file based on column name
	io::CSVReader<8> in("C:\\Users\\andra\\Source\\Repos\\tcupb\\ibm_quotes.csv");
	in.read_header(io::ignore_extra_column, "symbol", "timestamp", "ask_exchange", "ask_price", "ask_size", "bid_exchange", "bid_price", "bid_size");
	std::string symbol; 
	std::string timestamp ; 
	std::string ask_exchange; 
	double ask_price;
	double ask_size;
	std::string bid_exchange;
	double bid_price;
	double bid_size;
	bool has_column = in.has_column("symbol");
	std::cout << has_column;

	// Read each line and instantiate a new object
	while (in.read_row(symbol, timestamp, ask_exchange, ask_price, ask_size, bid_exchange, bid_price, bid_size)) {
		// std::cout << symbol << " - " << timestamp << " - " << ask_exchange << " - " << ask_price << " - " << ask_size << " - " << bid_exchange << " - "<< bid_price << " - "<< bid_size << "\n";
		quotes.push_back(Quotes(symbol, timestamp, ask_exchange, ask_price, ask_size, bid_exchange, bid_price, bid_size));
	}

	std::cout << "From csv file has been read: " << quotes.size() << "\n";


	return 0;
}