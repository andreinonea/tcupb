#ifndef UPB_TC_FULL_INFORMATION_H_
#define UPB_TC_FULL_INFORMATION_H_

#include "Utils.h"

class KTC_FullInformation
{
public:
	static void classify(const KTC_Algorithm version, const KTC_Data &trades, const KTC_Data &ask, const KTC_Data &bid);

private:
	static void extract_variables(const KTC_Algorithm version, const KTC_Data &trades, const KTC_Data &ask, const KTC_Data &bid);
	static void sign_trades_ds1();
	static void sign_trades_ds2();
	static void sign_trades_ds3();
};

#endif // UPB_TC_FULL_INFORMATION_H_
