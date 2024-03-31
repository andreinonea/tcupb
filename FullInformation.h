#ifndef UPB_TC_FULL_INFORMATION_H_
#define UPB_TC_FULL_INFORMATION_H_

#include "Utils.h"

enum class FullInformationVersion : UCHAR_TYPE
{
	DS_1 = 1,
	DS_2,
	DS_3
};

class KTC_FullInformation
{
public:
	static void classify(const FullInformationVersion version, const fp_matrix &df_tr, const fp_matrix &ask, const fp_matrix &bid);

private:
	static void extract_variables(const FullInformationVersion version, const fp_matrix &df_tr, const fp_matrix &ask, const fp_matrix &bid);
	static void sign_trades_ds1();
	static void sign_trades_ds2();
	static void sign_trades_ds3();
};

#endif // UPB_TC_FULL_INFORMATION_H_
