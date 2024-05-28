//
// Created by Mire on 21.04.2024.
//

#ifndef UPB_TC_LEE_READY_H_
#define UPB_TC_LEE_READY_H_

#include "Utils.h"

class KTC_LeeReady
{
public:
	using Result = KTC_Result;
	static Result classify(const KTC_Data &trades, const KTC_Data &ask, const KTC_Data &bid, BOOL_TYPE interpolate);
};

#endif // UPB_TC_LEE_READY_H_
