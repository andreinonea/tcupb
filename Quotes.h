#pragma once
#include <string>
#include <vector>

class Quotes {
private:
    std::string symbol;
    std::string timestamp;
    std::string ask_exchange;
    double ask_price;
    double ask_size;
    std::string bid_exchange;
    double bid_price;
    double bid_size;

public:
    // Constructor declaration
    Quotes(const std::string& symbol,
        const std::string& timestamp,
        const std::string& ask_exchange,
        double ask_price,
        double ask_size,
        const std::string& bid_exchange,
        double bid_price,
        double bid_size);
};

// Constructor definition
Quotes::Quotes(const std::string& symbol,
    const std::string& timestamp,
    const std::string& ask_exchange,
    double ask_price,
    double ask_size,
    const std::string& bid_exchange,
    double bid_price,
    double bid_size)
    : symbol(symbol), timestamp(timestamp), ask_exchange(ask_exchange), ask_price(ask_price),
    ask_size(ask_size), bid_exchange(bid_exchange), bid_price(bid_price), bid_size(bid_size) {}
