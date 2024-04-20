#include <filesystem>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

#include <omp.h>
#include <DataFrame/Utils/DateTime.h>
#include <Python.h>

#include "csv.h"
#include "Utils.h"
#include "FullInformation.h"

using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;


void
load_trades_from_csv(KTC_Data &trades, const char *csvpath)
{
	io::CSVReader<3, io::trim_chars<' ', '\t'>, io::double_quote_escape<',','\"'>> t_data(csvpath);
	t_data.read_header(io::ignore_extra_column, "timestamp", "price", "size");
	const char *timestamp;
	FP_TYPE price, size;

	while (t_data.read_row(timestamp, price, size))
	{
		hmdf::DateTime datetime(timestamp, hmdf::DT_DATE_STYLE::ISO_STYLE, hmdf::DT_TIME_ZONE::GMT);
		double t = datetime.hour() * 3600.0 + datetime.minute() * 60.0 + datetime.sec() + datetime.microsec() * 0.000001;
		trades.time.push_back(t);
		trades.price.push_back(price);
		trades.volume.push_back(size);
	}
}

void
load_quotes_from_csv(KTC_Data &ask, KTC_Data &bid, const char *csvpath)
{
	io::CSVReader<5, io::trim_chars<' ', '\t'>, io::double_quote_escape<',','\"'>> q_data(csvpath);
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

int
main(int argc, char **argv)
{
	if (argc < 3)
	{
		std::cerr << "please specify path to root directory containing datasets and path to directory containing Python script 'test.py'\n";
		return 1;
	}

	const char *dataset_path = argv[1];
	const char *module_path = argv[2];

	std::cout.precision(3);
	std::cout << std::fixed;

	std::string last_kind = "";
	std::vector<std::pair<std::string, std::string>> maybe_trades{}, maybe_quotes{};
	std::vector<std::pair<std::string, std::string>> matches{};

	for (const auto &dirEntry : recursive_directory_iterator(dataset_path))
		if (dirEntry.path().extension() == ".csv")
		{
			const auto &csvpath = dirEntry.path();
			const char *kind = csvpath.parent_path().stem().c_str();

			if (strcmp(last_kind.c_str(), kind) != 0)
			{
				if (strcmp(kind, "trades") == 0)
				{
					for (const auto &t: maybe_trades)
					{
						std::stringstream tss(t.second);
						std::string tseg;
						std::vector<std::string> tsplit;

						while(std::getline(tss, tseg, '_'))
						{
							tsplit.push_back(tseg);
						}

						for (const auto &q: maybe_quotes)
						{
							std::stringstream qss(q.second);
							std::string qseg;
							std::vector<std::string> qsplit;

							while(std::getline(qss, qseg, '_'))
							{
								qsplit.push_back(qseg);
							}

							if (tsplit[0] == qsplit[0] && tsplit[2] == qsplit[2])
							{
								matches.push_back(std::make_pair(t.first, q.first));
								break;
							}
						}
					}

					std::cout << "--------- BORDER ---------\n";

					maybe_trades.clear();
					maybe_quotes.clear();
				}

				last_kind = kind;
			}

			if (strcmp(kind, "trades") == 0)
				maybe_trades.emplace_back(std::make_pair(csvpath.c_str(), csvpath.stem().c_str())), std::cout << "trade added: " << csvpath.c_str() << '\n';
			if (strcmp(kind, "quotes") == 0)
				maybe_quotes.emplace_back(std::make_pair(csvpath.c_str(), csvpath.stem().c_str())), std::cout << "quote added: " << csvpath.c_str() << '\n';
		}

		// Initialize Python
		PyObject *pModule = 0, *pFunc = 0, *pArgs = 0, *pValue = 0;
		Py_Initialize();

		// Add module path
		PyObject *sys_path = PySys_GetObject("path");
		PyList_Append(sys_path, PyUnicode_FromString(module_path));

		// Import module
		pModule = PyImport_ImportModule("test_tcupb");
		if (pModule == 0)
		{
			std::cerr << "pModule error\n";
			Py_Finalize();
			return 1;
		}

		// Import test function
		pFunc = PyObject_GetAttrString(pModule, "run_for");
		if (pFunc == 0 || !PyCallable_Check(pFunc))
		{
			std::cerr << "pFunc error\n";
			Py_DECREF(pModule);
			Py_Finalize();
			return 1;
		}

		int failed_tests = 0, skipped_tests = 0;

		for (const auto &m: matches)
		{
			std::cout << "\nProcessing\n";
			std::cout << "tr " << m.first << '\n';
			std::cout << "qt " << m.second << '\n';

			KTC_Data trades{};
			load_trades_from_csv(trades, m.first.c_str());

			KTC_Data ask{}, bid{};
			load_quotes_from_csv(ask, bid, m.second.c_str());

			// TODO: loop through each algorithm unless flag is given with specific?
			KTC_Result res = KTC_FiAlgo::classify(KTC_FiAlgoVersion::DS_3, trades, ask, bid, 0.6);

			INT_TYPE isize = res.initiator.size();
			INT_TYPE ssize = res.step.size();

			int isum = 0, ssum = 0;
			for (auto& el : res.initiator)
				isum += el;
			for (auto& el : res.step)
				ssum += el;

			std::stringstream cres;
			cres << isize << ' ' << ssize << ' ' << isum << ' ' << ssum;
			std::cout << cres.str() << '\n';

			pArgs = PyTuple_New(3);

			pValue = PyUnicode_FromString(m.first.c_str());
			PyTuple_SetItem(pArgs, 0, pValue);

			pValue = PyUnicode_FromString(m.second.c_str());
			PyTuple_SetItem(pArgs, 1, pValue);

			pValue = PyBool_FromLong(1);
			PyTuple_SetItem(pArgs, 2, pValue);

			pValue = PyObject_CallObject(pFunc, pArgs);
			const char *pyres = 0;

			if (pArgs != 0)
				Py_DECREF(pArgs);

			if (pValue != 0 && PyUnicode_Check(pValue))
			{
				PyObject *tmp_bytes = PyUnicode_AsEncodedString(pValue, "UTF-8", "strict");
				if (tmp_bytes != 0)
				{
					pyres = PyBytes_AS_STRING(tmp_bytes);
					std::cout << pyres << '\n';

					if (strcmp(cres.str().c_str(), pyres) != 0)
						++failed_tests, std::cout << "FAIL\n";
					else
						std::cout << "OK\n";

					Py_DECREF(tmp_bytes);
				}
				else
				{
					std::cout << "SKIP\n";
					std::cerr << "cannot decode result from Python call\n";
					++skipped_tests;
					continue;
				}
				Py_DECREF(pValue);
			}
			else
			{
				std::cout << "SKIP\n";
				std::cerr << "no result from Python call\n";
				++skipped_tests;
				continue;
			}
		}
		Py_Finalize();

		std::cout << "\n--\n";
		std::cout << "Total tests:\t" << matches.size() << '\n';
		std::cout << "Skipped tests:\t" << skipped_tests << '\n';
		std::cout << "Failed tests:\t" << failed_tests << '\n';
}
