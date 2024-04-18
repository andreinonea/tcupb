#ifndef UPB_TC_FULL_INFORMATION_H_
#define UPB_TC_FULL_INFORMATION_H_

#include "Utils.h"

enum class KTC_FiAlgoVersion : UCHAR_TYPE
{
	DS_1 = 1,
	DS_2,
	DS_3,
};

struct KTC_FiAlgoVariables
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
	int_vector askv;
	int_vector bidv;
	fp_vector atime;
	fp_vector btime;
};

class KTC_FiAlgo
{
public:
	static KTC_Result classify(const KTC_FiAlgoVersion version, const KTC_Data &trades, const KTC_Data &ask, const KTC_Data &bid, FP_TYPE bar = 0.3);

private:
	static int_vector delta_vol(const int_vector &price, const int_vector &volume, bool is_ask = true);
	static KTC_FiAlgoVariables extract_variables(const KTC_FiAlgoVersion version, const KTC_Data &trades, const KTC_Data &ask, const KTC_Data &bid);
	static KTC_Result sign_trades_ds1(const KTC_FiAlgoVariables &vars, FP_TYPE bar);
	static KTC_Result sign_trades_ds2(KTC_FiAlgoVariables &vars, FP_TYPE bar);
	static KTC_Result sign_trades_ds3(KTC_FiAlgoVariables &vars, FP_TYPE bar);
};

#endif // UPB_TC_FULL_INFORMATION_H_
