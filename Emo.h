//
// Created by Mire on 01.06.2024.
//

#ifndef TCUPB_EMO_H
#define TCUPB_EMO_H

#include "Utils.h"


class KTC_Emo {
public:
    using Result = KTC_Result;
    static Result classify(	const KTC_Data &trades,
                               const KTC_Data &ask, const KTC_Data &bid, BOOL_TYPE interpolate = false);

};


#endif //TCUPB_EMO_H
