#pragma once
#include <string>

// Trades.h
class Trades {
    std::string id;
    std::string symbol;
    std::string timestamp;
    std::string exchange;
    double price;
    double size;

public:
    Trades(std::string id, std::string symbol, std::string timestamp, std::string exchange, double price, double size);
    // rest of your code...
};