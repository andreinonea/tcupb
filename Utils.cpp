#include "Utils.h"

#include <iostream>
#include <queue>
#include <utility>
#include <vector>
#include <numeric>
#include <algorithm>


int_vector
vec_nonzero(const fp_vector &v) {
    int_vector ind{};

    for (int i = 0; i < v.size(); ++i)
        if (v[i] != 0.0)
            ind.push_back(i);

    return ind;
}

KTC_Pair
quote_index(const fp_vector &quote_times, const fp_vector &trade_times) {
    INT_TYPE s = quote_times.size();
    INT_TYPE n = trade_times.size();

    int_vector left(n, 0);
    int_vector right(n, 0);

    BOOL_TYPE found;
    INT_TYPE begin, end, k;
    INT_TYPE j, i;
    UINT_TYPE ind = 0;

    fp_vector tmp;
    FP_TYPE v;

    for (j = 0; j < n; ++j) {
        begin = s;
        end = 0;
        found = false;
        tmp = fp_vector(quote_times.begin() + ind, quote_times.end());
        k = tmp.size();

        for (i = 0; i < k; ++i) {
            v = tmp[i] - trade_times[j];

            if ((!found) && (v >= 0.0)) {
                found = true;
                begin = ind + i;
            }

            if (found) {
                if (v > 0.0) {
                    ind += i;
                    end = ind;
                    break;
                } else if (i == (k - 1)) {
                    end = s;
                    break;
                }
            }
        }

        left[j] = begin;
        right[j] = end;
    }

    for (int i = 0; i < left.size(); ++i) {
        if (left[i] < right[i])
            right[i] -= 1;

        left[i] -= 1;

        if (left[i] < 0)
            left[i] = 0;
    }

    return KTC_Pair{left, right};
}

fp_vector
concat_runs(const int_vector &x, bool hj_version) {
    int len = 0;
    for (auto &el: x)
        len += el;

    fp_vector interp(len, 0.0);
    INT_TYPE count = 0;

    if (hj_version) {
        for (int i = 0; i < x.size(); ++i) {
            FP_TYPE k = x[i] + 1.0;

            for (int j = 1; j < x[i] + 1; ++j)
                interp[count++] = ((2 * j) - 1) / (2 * k);
        }
    } else {
        for (int i = 0; i < x.size(); ++i) {
            FP_TYPE k = x[i] + 1.0;

            for (int j = 1; j < x[i] + 1; ++j)
                interp[count++] = j / k;
        }
    }

    return interp;
}

fp_vector
interpolate_time(const fp_vector &time, bool hj_version) {
    fp_vector run = vec_diff(time);
    run.insert(run.begin(), 0.0);

    int_vector ind = vec_nonzero(run);
    ind.push_back(run.size());
    ind.insert(ind.begin(), 0);
    int_vector runlength = vec_diff(ind);

    fp_vector intertime = concat_runs(runlength, hj_version);

    return time + intertime;
}

void
apply_tick(KTC_Result &res, const fp_vector &trade_prices) {
    int_vector indexes{}, prices{};

    for (int i = 0; i < res.step.size(); ++i)
        if (res.step[i] == 0) {
            indexes.emplace_back(i);
            prices.push_back((INT_TYPE) trade_prices[i]);
        }

    std::queue<INT_TYPE> s{};

    for (int j = 0; j < prices.size(); ++j) {
        INT_TYPE count = 0;
        INT_TYPE p = prices[j];
        INT_TYPE t = indexes[j];

        int_vector lstp(trade_prices.begin(), std::next(trade_prices.begin(), t));
        INT_TYPE sz = lstp.size();

        s.push(0);

        while (sz > count++) {
            INT_TYPE lp = lstp[sz - count];
            if (p > lp) {
                s.back() = 1;
                break;
            }
            if (p < lp) {
                s.back() = -1;
                break;
            }
        }
    }

    for (int i = 0; i < res.step.size(); ++i)
        if (res.step[i] == 0) {
            res.initiator[i] = s.front();
            s.pop();
        }
}

KTC_Interpolation
tcol_interpolation(const fp_vector &ask_time, const fp_vector &bid_time) {
    fp_vector askit = interpolate_time(ask_time);
    fp_vector bidit = interpolate_time(bid_time);

    double askit_min = *std::min_element(askit.begin(), askit.end());
    double askit_max = *std::max_element(askit.begin(), askit.end());
    double askit_sum = std::accumulate(askit.begin(), askit.end(), 0.0);
    size_t askit_len = askit.size();

    double bidit_min = *std::min_element(bidit.begin(), bidit.end());
    double bidit_max = *std::max_element(bidit.begin(), bidit.end());
    double bidit_sum = std::accumulate(bidit.begin(), bidit.end(), 0.0);
    size_t bidit_len = bidit.size();

// Printing the results:
    std::cout << "For askit:\n";
    std::cout << "Min: " << askit_min << "\n";
    std::cout << "Max: " << askit_max << "\n";
    std::cout << "Sum: " << askit_sum << "\n";
    std::cout << "Length: " << askit_len << "\n";

    std::cout << "\nFor bidit:\n";
    std::cout << "Min: " << bidit_min << "\n";
    std::cout << "Max: " << bidit_max << "\n";
    std::cout << "Sum: " << bidit_sum << "\n";
    std::cout << "Length: " << bidit_len << "\n";

    return KTC_Interpolation{askit, bidit};
}

fp_vector
get_midpoint(KTC_Interpolation interpolation,
             const fp_vector &ask_price,
             const fp_vector &bid_price,
             const fp_vector &trades_time) {
    fp_vector ask = get_lastquote(interpolation.ask_time, ask_price, trades_time);
    fp_vector bid = get_lastquote(interpolation.bid_time, bid_price, trades_time);

    fp_vector midpoint(ask.size());

    for (std::size_t i = 0; i < ask.size(); ++i) {
        midpoint[i] = (ask[i] + bid[i]) / 2.0L;
    }
    double NaN = std::numeric_limits<double>::quiet_NaN();

    for (std::size_t i = 0; i < ask.size(); ++i)
    {
        if (ask[i] < bid[i])
        {
            midpoint[i] = NaN;
        }
    }
    return midpoint;
}

fp_vector
get_lastquote(const fp_vector &quotes_time,
              const fp_vector &quote_price,
              const fp_vector &as_of) {
    std::vector<FP_TYPE> sorted_quotes_time = quotes_time;
    std::vector<FP_TYPE> sorted_as_of = as_of;

// Sort the copied vectors
    std::sort(sorted_quotes_time.begin(), sorted_quotes_time.end());
    std::sort(sorted_as_of.begin(), sorted_as_of.end());
    std::vector<int> indices;

    for (const auto& value: sorted_as_of) {
        auto it = std::lower_bound(sorted_quotes_time.begin(), sorted_quotes_time.end(), value);
        int ind = std::distance(sorted_quotes_time.begin(), it);
        // Mimic Python's behavior when value is less than any element in quotes_time.
        if (it == sorted_quotes_time.begin() && *it != value) ind--;
        indices.push_back(ind);
    }

    fp_vector last_quote(as_of.size(), 0.0);

    bool_vector mask;
    mask.reserve(indices.size());
    for (const auto &index: indices) {
        mask.push_back(index >= 0);
    }

    for (size_t i = 0; i < indices.size(); i++) {
        if (mask[i] && indices[i] > 0) {
            last_quote[i] = quote_price[indices[i] - 1];
        }
    }

    double NaN = std::numeric_limits<double>::quiet_NaN();

    for (int i = 0; i < mask.size(); i++)
    {
        if (!mask[i]) // If mask[i] is false, update the corresponding value in last_quote
        {
            last_quote[i] = NaN;
        }
    }
    return last_quote;
}
