#ifndef UPB_TC_FULL_INFORMATION_H_
#define UPB_TC_FULL_INFORMATION_H_

#include "Utils.h"

enum class KTC_FullInformationVersion : UCHAR_TYPE
{
	DS_1 = 1,
	DS_2,
	DS_3,
};

struct KTC_FullInformationVariables
{
	int_vector P;
	int_vector V;
	int_vector Al;
	int_vector Ar;
	int_vector Bl;
	int_vector Br;
	int_vector runlength;
	int_vector askp;
	int_vector bidp;
	int_vector avdiff;
	int_vector bvdiff;
	fp_vector atime;
	fp_vector btime;
};

class KTC_FullInformation
{
public:
	static void classify(const KTC_FullInformationVersion version, const KTC_Data &trades, const KTC_Data &ask, const KTC_Data &bid, FP_TYPE bar = 0.3);

private:
	static int_vector delta_vol(const int_vector &price, const int_vector &volume, bool is_ask = true);
	static KTC_FullInformationVariables extract_variables(const KTC_FullInformationVersion version, const KTC_Data &trades, const KTC_Data &ask, const KTC_Data &bid);
	static void sign_trades_ds1(const KTC_FullInformationVariables &vars, FP_TYPE bar);
	static void sign_trades_ds2(const KTC_FullInformationVariables &vars, FP_TYPE bar);
	static void sign_trades_ds3(const KTC_FullInformationVariables &vars, FP_TYPE bar);
};

#endif // UPB_TC_FULL_INFORMATION_H_
