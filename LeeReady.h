//
// Created by Mire on 21.04.2024.
//

#ifndef TCUPB_LEEREADY_H
#define TCUPB_LEEREADY_H

#include "Utils.h"


class LeeReady {
public:
    static KTC_Result classify(const KTC_Data &trades, const KTC_Data &ask, const KTC_Data &bid, BOOL_TYPE interpolate);

};


#endif //TCUPB_LEEREADY_H
