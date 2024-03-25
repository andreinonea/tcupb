#include "Trades.h"
#include <iostream>
using std::string;

Trades::Trades(std::string symbol, std::string timestamp, std::string exchange, double price, double size, double id) :
	symbol(symbol), timestamp(timestamp), exchange(exchange), price(price), size(size), id(id){

}
