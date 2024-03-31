#include "FullInformation.h"
#include "Utils.h"

void
KTC_FullInformation::classify(const FullInformationVersion version, const fp_matrix &df_tr, const fp_matrix &ask, const fp_matrix &bid)
{
	KTC_FullInformation::extract_variables(version, df_tr, ask, bid);

	switch (version)
	{
	case FullInformationVersion::DS_1:
		KTC_FullInformation::sign_trades_ds1();
		break;
	case FullInformationVersion::DS_2:
		KTC_FullInformation::sign_trades_ds2();
		break;
	case FullInformationVersion::DS_3:
	default:
		KTC_FullInformation::sign_trades_ds3();
		break;
	}

	// Plus some other things.
}

void
KTC_FullInformation::extract_variables(const FullInformationVersion version, const fp_matrix &df_tr, const fp_matrix &ask, const fp_matrix &bid)
{
}

void
KTC_FullInformation::sign_trades_ds1()
{
}

void
KTC_FullInformation::sign_trades_ds2()
{
}

void
KTC_FullInformation::sign_trades_ds3()
{
}
