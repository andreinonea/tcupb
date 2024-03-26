#include "Quotes.h"
#include <iostream>
using std::string;
using hmdf::DateTime;

Quotes::Quotes(string symbol, DateTime timestamp, string ask_exchange, double ask_price, double ask_size, string bid_exchange, double bid_price, double bid_size) :
	symbol(symbol), timestamp(timestamp), ask_exchange(ask_exchange), ask_size(ask_size), bid_exchange(big_exchange), bid_price(bid_price), bid_size(bid_size){

}
