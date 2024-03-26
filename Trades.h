#pragma once
#include <string>
#include <vector>


class Trades {
private:
	std::string symbol;
	std::string timestamp;
	std::string exchange;
	double price;
	double size;
	double id;

public:
	Trades(std::string symbol, std::string timestamp, std::string exchange, double price, double size, double id);

};