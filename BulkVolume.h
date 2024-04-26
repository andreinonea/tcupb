#ifndef UPB_TC_BULK_VOLUME_H_
#define UPB_TC_BULK_VOLUME_H_

#include "Utils.h"

#include <optional>
#include <variant>

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

class KTC_Bvc
{
public:
	struct Result
	{
		int_vector group;
		fp_vector buy_frac;
		fp_vector vol;
		fp_vector buy_vol;
	};
	static std::optional<Result> classify(const KTC_Data &trades, INT_TYPE window = 1, INT_TYPE *start = 0, KTC_BvcWindowType window_type = KTC_BvcWindowType::TIME, std::variant<double, KTC_BvcDofType> dof = {KTC_BvcDofType::ESTIMATE});

private:
	static std::optional<fp_vector> fraction_buy(const fp_vector &p, std::variant<double, KTC_BvcDofType> dof);
	static std::optional<fp_vector> t_fit(const fp_vector &x, INT_TYPE loc, INT_TYPE scale);
	static fp_vector norm_cdf(const fp_vector &x);
	static fp_vector t_cdf(const fp_vector &x, FP_TYPE dof);
	static fp_vector minimize_neldermead(const fp_vector &x, INT_TYPE loc, INT_TYPE scale);
	static FP_TYPE penalized_nnlf(const fp_vector &theta, const fp_vector &x);
};

#endif // UPB_TC_BULK_VOLUME_H_
