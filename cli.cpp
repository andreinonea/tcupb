#include <iostream>
#include <vector>

#include "csv.h"
#include "cli.h"
#include "Utils.h"
#include "Trades.h"
#include "Quotes.h"


int main(int argc, char* argv[])
{
    auto paths = parse_cli_arguments(argc, argv);

    std::cout << "Trades path: " << paths.first << "\n";
    std::cout << "Quotes path: " << paths.second << "\n";

    parse_quotes(paths.second);
    parse_trades(paths.first);

    return 0;
}

std::pair<std::string, std::string> parse_cli_arguments(int argc, char *argv[]) {
    std::string trades_path;
    std::string quotes_path;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-tr") {
            if (i + 1 < argc) {
                trades_path = argv[++i];
            } else {
                std::cerr << "-tr requires one argument." << std::endl;
                exit(1);
            }
        } else if (arg == "-qt") {
            if (i + 1 < argc) {
                quotes_path = argv[++i];
            } else {
                std::cerr << "-qt requires one argument." << std::endl;
                exit(1);
            }
        }
    }

    if(trades_path.empty()) {
        std::cerr << "No trades file path provided. Use -tr <filepath>." << std::endl;
        exit(1);
    }

    return std::make_pair(trades_path, quotes_path);
}

//process files
void parse_quotes(std::string quotes_path){
    std::vector<Quotes> quotes;

    io::CSVReader<8> in(quotes_path);
    in.read_header(io::ignore_extra_column, "symbol", "timestamp", "ask_exchange",
                   "ask_price", "ask_size", "bid_exchange", "bid_price", "bid_size");

    std::string symbol;
    std::string timestamp;
    std::string ask_exchange;
    double ask_price;
    double ask_size;
    std::string bid_exchange;
    double bid_price;
    double bid_size;

    while (in.read_row(symbol, timestamp, ask_exchange, ask_price, ask_size, bid_exchange, bid_price, bid_size)) {
        quotes.push_back(Quotes(symbol, timestamp, ask_exchange, ask_price, ask_size, bid_exchange, bid_price, bid_size));
    }

    std::cout << "From csv file has been read: " << quotes.size() << "\n";
}

void parse_trades(std::string trades_path){
    std::vector<Trades> trades;
    io::CSVReader<6> in_trades(trades_path);
    in_trades.read_header(io::ignore_extra_column,  "symbol", "timestamp", "exchange", "price", "size", "id");

    std::string id;
    std::string exchange;
    std::string timestamp;
    std::string symbol;
    double price;
    double size;

    while (in_trades.read_row(symbol, timestamp, exchange, price, size, id)) {
        trades.push_back(Trades(id, symbol, timestamp, exchange, price, size));
    }

    std::cout << "From csv file has been read: " << trades.size() << "\n";
}