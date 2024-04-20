#ifndef UPB_TC_BULK_VOLUME_H_
#define UPB_TC_BULK_VOLUME_H_

#include "Utils.h"

enum class KTC_BvcWindowType : UINT_TYPE
{
	TIME = 0,
	VOLUME,
	PER_TRADE
};

enum class KTC_BvcDofType : UINT_TYPE
{
	NORMAL = 0,
	ESTIMATE,
};

struct KTC_BvcDegreesOfFreedom
{
	BOOL_TYPE is_fp;
	union
	{
		KTC_BvcDofType s;
		double f;
	} dof;
};

class KTC_Bvc
{
public:
	static KTC_Result classify(const KTC_Data &trades, FP_TYPE window = 1.0, KTC_BvcWindowType window_type = KTC_BvcWindowType::TIME, KTC_BvcDegreesOfFreedom dof = KTC_BvcDegreesOfFreedom {false, KTC_BvcDofType::ESTIMATE}, FP_TYPE start = 0.0);

private:

};

#endif // UPB_TC_BULK_VOLUME_H_
