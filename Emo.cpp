#include "Emo.h"
#include "Utils.h"

#include <vector>

KTC_Emo::Result KTC_Emo::classify(	const KTC_Data &trades,
                                      const KTC_Data &ask, const KTC_Data &bid, BOOL_TYPE interpolate) {
    KTC_Emo::Result res {};

    fp_vector ask_time = ask.time;
    fp_vector bid_time = bid.time;
    fp_vector trades_time = trades.time;

    if (interpolate)
    {
        trades_time = interpolate_time(trades.time, true);
        ask_time = interpolate_time(ask.time, true);
        bid_time = interpolate_time(bid.time, true);
    }

    fp_vector trades_time_unique = vec_unique(trades_time);

    fp_vector lastask = get_lastquote(ask_time, ask.price, trades_time_unique);
    fp_vector lastbid = get_lastquote(bid_time, bid.price, trades_time_unique);

    fp_vector
        ask_bid_join = outerJoin(lastask, lastbid);
    fp_vector ask_bid = vec_merge_left(trades_time, ask_bid_join, trades_time_unique);

    fp_vector mask(ask_bid);
    fp_vector ask_price = ask.price;
    fp_vector bid_price = bid.price;
    for(size_t i = 0; i < ask_price.size(); i++){
        mask[i] = ask_price[i] <= bid_price[i];
        if (mask[i]) {
            ask_price[i] = NAN;
            bid_price[i] = NAN;
        }
    }

    res.initiator = int_vector(trades.price.size(), 0);
    res.step = int_vector(trades.price.size(), 0);

    for(size_t i = 0; i < ask_price.size(); i++) {
        if(ask_price[i] == trades.price[i]) {
            res.initiator[i] = 1;
            res.step[i] = 1;
        }
        else if(bid_price[i] == trades.price[i]) {
            res.initiator[i] = -1;
            res.step[i] = -1;
        }
        else {
            res.initiator[i] = 0;
            res.step[i] = 0;
        }
    }

    apply_tick(res, trades.price);

    for (int i = 0; i < trades.price.size(); ++i)
        if (res.step[i] == 0)
            res.step[i] = 2;

    return res;
}
