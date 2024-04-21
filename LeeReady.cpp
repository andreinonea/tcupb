//
// Created by Mire on 21.04.2024.
//

#include "LeeReady.h"
#include "Utils.h"
#include <numeric>
#include <iostream>
#include <cmath>
#include <vector>
#include <unordered_set>



KTC_Result
LeeReady::classify(
        const KTC_Data &trades,
        const KTC_Data &ask,
        const KTC_Data &bid,
        BOOL_TYPE interpolate) {
    KTC_Interpolation ktcInterpolation;
    if (interpolate) {
        ktcInterpolation = tcol_interpolation(ask.time, bid.time);
    } else {
        ktcInterpolation = KTC_Interpolation{ask.time, bid.time};
    }

    std::vector<FP_TYPE> unique_times;
    unique_times.reserve(trades.time.size());

    std::unordered_set<FP_TYPE> seen;

    for (auto & time : trades.time) {
        if (seen.insert(time).second) {
            unique_times.push_back(time);
        }
    }
    fp_vector midpoint = get_midpoint(ktcInterpolation, ask.price, bid.price, unique_times);

    double max_val = -std::numeric_limits<double>::infinity();
    double min_val = std::numeric_limits<double>::infinity();
    double sum_val = 0;

    for(auto& val : midpoint){
        if(!std::isnan(val)) {
            max_val = (val > max_val) ? val : max_val;
            min_val = (val < min_val) ? val : min_val;
            sum_val += val;
        }
    }

    std::cout << "Max: " << max_val << ", Min: " << min_val << ", Sum: " << sum_val << "\n";

    std::cout << "Max: " << max_val << ", Min: " << min_val << ", Sum: " << sum_val << "\n";
    return KTC_Result{};
}