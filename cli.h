#ifndef CLI_H
#define CLI_H

#include <string>

std::pair<std::string, std::string> parse_cli_arguments(int argc, char *argv[]); // to parse command line arguments
void parse_quotes(std::string quotes_path); // to parse quotes from files
void parse_trades(std::string trades_path); // to parse trades from files

#endif