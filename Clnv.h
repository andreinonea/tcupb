//
// Created by Mire on 01.06.2024.
//

#ifndef TCUPB_CLNV_H
#define TCUPB_CLNV_H
#include "Utils.h"


class KTC_Clnv {
public:
    using Result = KTC_Result;
    static Result classify(	const KTC_Data &trades,
                               const KTC_Data &ask, const KTC_Data &bid, BOOL_TYPE interpolate = false);
};


#endif //TCUPB_CLNV_H
