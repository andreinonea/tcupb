#include "BulkVolume.h"
#include "Utils.h"

#include <boost/math/distributions/normal.hpp>
#include <boost/math/distributions/students_t.hpp>
#include <cmath>

// TODO: remove
#include <iostream>


std::optional<KTC_Bvc::Result>
KTC_Bvc::classify(
	const KTC_Data &trades,
	INT_TYPE window,
	INT_TYPE *start,
	KTC_BvcWindowType window_type,
	std::variant<double, KTC_BvcDofType> dof)
{
	if (window_type == KTC_BvcWindowType::PER_TRADE)
	{
		int_vector group;
		group.reserve(trades.price.size() - 1);
		for (int i = 0; i < trades.price.size() - 1; ++i)
			group.push_back(i);

		fp_vector buy_frac;
		{
			std::optional<fp_vector> opt = fraction_buy(trades.price, dof);
			if (opt.has_value()) buy_frac = opt.value();
			else return {};
		}

		fp_vector vol(std::next(trades.volume.begin()), trades.volume.end());
		fp_vector buy_vol(group.size(), 0.0);

		for (int i = 0; i < group.size(); ++i)
			if (buy_frac[i] > 0.5) buy_vol[i] = vol[i];
			else if (buy_frac[i] == 0.5) buy_vol[i] = -1.0;

		debug_vector(group);
		debug_vector_extra(group);
		debug_vector(buy_frac);
		debug_vector_extra(buy_frac);
		debug_vector(vol);
		debug_vector_extra(vol);
		debug_vector(buy_vol);
		debug_vector_extra(buy_vol);

		return KTC_Bvc::Result {
			.group = group,
			.buy_frac = buy_frac,
			.vol = vol,
			.buy_vol = buy_vol,
		};
	}
	else
	{
		int_vector group;

		if (window_type == KTC_BvcWindowType::TIME)
		{
			group = int_vector(std::next(trades.time.begin()), trades.time.end());

			if (start != 0)
			{
				for (int i = 0; i < group.size(); ++i)
				{
					group[i] -= *start;
					group[i] /= window;
				}
			}
			else
			{
				INT_TYPE g0 = group[0];

				for (int i = 0; i < group.size(); ++i)
				{
					group[i] -= g0;
					group[i] /= window;
				}
			}
		}
		else // KTC_BvcWindowType::VOLUME
		{
			group = int_vector(std::next(trades.volume.begin()), trades.volume.end());

			// vol_bin
			INT_TYPE vol = 0, g = 0;
			for (int i = 0; i < group.size(); ++i)
			{
				vol += group[i];
				group[i] = g;
				if (vol >= window)
				{
					++g;
					vol = 0;
				}
			}
		}

		debug_vector(group);
		debug_vector_extra(group);

		int_vector group_unique = vec_unique(group);

		debug_vector(group_unique);
		debug_vector_extra(group_unique);

		int_vector ind = vec_searchsorted(group, group_unique, Side::RIGHT);
		ind -= 1;

		debug_vector(ind);
		debug_vector_extra(ind);

		fp_vector p;
		p.reserve(1 + ind.size());
		p.push_back(trades.price[0]);
		for (int i = 0; i < ind.size(); ++i)
			p.push_back(trades.price[1 + ind[i]]);

		debug_vector(p);
		debug_vector_extra(p);

		fp_vector buy_frac;
		{
			std::optional<fp_vector> opt = fraction_buy(p, dof);
			if (opt.has_value()) buy_frac = opt.value();
			else return {};
		}

		fp_vector vol(group_unique.size(), 0.0);
		fp_vector buy_vol(group_unique.size(), 0.0);

		int_vector group_unique_sorted(group_unique);
		std::sort(group_unique_sorted.begin(), group_unique_sorted.end());

		// Join on unique group, summing volumes
		for (int i = 0; i < group.size(); ++i)
		{
			// Binary search
			auto found = std::lower_bound(group_unique_sorted.begin(), group_unique_sorted.end(), group[i]);
			if (found != group_unique_sorted.end() && !(group[i] < *found))
			{
				INT_TYPE idx = std::distance(group_unique_sorted.begin(), found);
				vol[idx] += trades.volume[1 + i];
				buy_vol[idx] = vol[idx] * buy_frac[idx];
			}
		}
		debug_vector(group_unique);
		debug_vector_extra(group_unique);
		debug_vector(buy_frac);
		debug_vector_extra(buy_frac);
		debug_vector(vol);
		debug_vector_extra(vol);
		debug_vector(buy_vol);
		debug_vector_extra(buy_vol);

		return KTC_Bvc::Result {
			.group = group_unique,
			.buy_frac = buy_frac,
			.vol = vol,
			.buy_vol = buy_vol,
		};
	}

	return {};
}

std::optional<fp_vector>
KTC_Bvc::fraction_buy(const fp_vector &p, std::variant<double, KTC_BvcDofType> dof)
{
	// TODO: remove
	std::cout.precision(8);

	fp_vector dp(p.size() - 1, 0.0);
	{
		FP_TYPE cached_log = std::log(p[0]);
		for (int i = 0; i < dp.size(); ++i)
		{
			FP_TYPE next_log = std::log(p[i+1]);
			dp[i] = next_log - cached_log;
			cached_log = next_log;
		}
	}
	debug_vector(dp);
	debug_vector_extra(dp);

	fp_vector dp_masked{};
	for (const auto &el : dp)
		if (!(std::isinf(el) || std::isnan(el)))
			dp_masked.push_back(el);

	debug_vector(dp_masked);
	debug_vector_extra(dp_masked);

	if (dp_masked.size() < 2)
		return {}; // fp_vector(dp.size(), std::nanl(""));

	FP_TYPE sigma = vec_std(dp_masked, 1);
	if (sigma == 0.0)
		return {}; // fp_vector(dp.size(), std::nanl(""));

	std::cout << "sigma=" << sigma << '\n';

	dp /= sigma;
	debug_vector(dp);
	debug_vector_extra(dp);

	FP_TYPE fp_dof;
	if (std::holds_alternative<double>(dof))
		fp_dof = std::get<double>(dof);
	else if (std::get<KTC_BvcDofType>(dof) == KTC_BvcDofType::ESTIMATE)
	{
		dp_masked.clear();

		for (const auto &el : dp)
			if (!(std::isinf(el) || std::isnan(el)))
				dp_masked.push_back(el);

		std::optional<fp_vector> opt = KTC_Bvc::t_fit(dp_masked, 0, 1);
		if (opt.has_value()) fp_dof = opt.value()[0];
		else return {};
		debug_val(fp_dof);
	}
	else return KTC_Bvc::norm_cdf(dp);
	return KTC_Bvc::t_cdf(dp, fp_dof);
}

std::optional<fp_vector>
KTC_Bvc::t_fit(const fp_vector &x, INT_TYPE loc, INT_TYPE scale)
{
	fp_vector vals = KTC_Bvc::minimize_neldermead(x, loc, scale);

	if (!(vals.front() > 0 && vals.back() > 0))
		return {};

	return vals;
}

fp_vector
KTC_Bvc::norm_cdf(const fp_vector &x)
{
	fp_vector out(x.size(), 0.0);
	FP_TYPE loc = 0.0, scale = 1.0;

	for (int i = 0; i < x.size(); ++i)
	{
		FP_TYPE xi = (x[i] - loc) / scale;

		if (std::isnan(xi))
			out[i] = std::nanl("");
		else if (xi > 0 && std::isinf(xi))
			out[i] = 1.0;
		else if (std::isfinite(xi))
		{
			out[i] = boost::math::cdf(boost::math::normal_distribution<FP_TYPE>(), xi);
		}
	}

	debug_vector(out);
	debug_vector_extra(out);
	return out;
}

fp_vector
KTC_Bvc::t_cdf(const fp_vector &x, FP_TYPE dof)
{
	fp_vector out(x.size(), 0.0);
	FP_TYPE loc = 0.0, scale = 1.0;
	BOOL_TYPE dof_ispos = dof > 0.0;

	for (int i = 0; i < x.size(); ++i)
	{
		FP_TYPE xi = (x[i] - loc) / scale;

		if (!dof_ispos && std::isnan(xi))
			out[i] = std::nanl("");
		else if (dof_ispos && xi > 0 && std::isinf(xi))
			out[i] = 1.0;
		else if (dof_ispos && std::isfinite(xi))
		{
			out[i] = boost::math::cdf(boost::math::students_t_distribution<FP_TYPE>(dof), xi);
		}
	}

	debug_vector(out);
	debug_vector_extra(out);
	return out;
}

fp_vector
KTC_Bvc::minimize_neldermead(const fp_vector &x, INT_TYPE loc, INT_TYPE scale)
{
	const FP_TYPE rho = 1.0, chi = 2.0, psi = 0.5, sigma = 0.5;
	const FP_TYPE nonzdelt = 0.05, zdelt = 0.00025;
	const FP_TYPE xatol = 0.0001, fatol = 0.0001;

	fp_vector x0 {1.0, static_cast<FP_TYPE>(loc), static_cast<FP_TYPE>(scale)};
	const INT_TYPE n = x0.size();

	fp_matrix sim(n + 1, fp_vector(n));
	sim[0] = x0;
	for (int k = 0; k < n; ++k)
	{
		fp_vector y = x0;
		if (y[k] != 0.0) y[k] *= 1.0 + nonzdelt;
		else y[k] = zdelt;
		sim[k + 1] = y;
	}

	INT_TYPE maxiter = n * 200, maxfun = maxiter;
	INT_TYPE fcalls = 0;

	const auto func = [&fcalls, &maxfun, &x](const fp_vector &theta) -> std::optional<FP_TYPE> {
		if (fcalls > maxfun)
			return {};

		++fcalls;
		return KTC_Bvc::penalized_nnlf(theta, x);
	};

	fp_vector fsim(n + 1, fp_inf());
	for (int k = 0; k < n + 1; ++k)
	{
		std::optional<FP_TYPE> fx = func(sim[k]);
		if (fx.has_value()) fsim[k] = fx.value();
		else break;
	}

	{
		const auto ind = vec_argsort(fsim);
		vec_reorder(sim, ind);
		vec_reorder(fsim, ind);
	}

	INT_TYPE iterations = 1;
	while (fcalls < maxfun && iterations < maxiter)
	{
		BOOL_TYPE should_break = true;
		{
			FP_TYPE curmax = std::numeric_limits<FP_TYPE>::min();
			for (int i = 1; i < sim.size() && should_break; ++i)
				for (int j = 0; j < sim[0].size() && should_break; ++j)
				{
					FP_TYPE absdiff = std::abs(sim[i][j] - sim[0][j]);
					if (absdiff > curmax)
					{
						curmax = absdiff;
						if (curmax > xatol) should_break = false;
					}
				}

			if (should_break)
			{
				curmax = std::numeric_limits<FP_TYPE>::min();
				for (int i = 1; i < fsim.size() && should_break; ++i)
				{
					FP_TYPE absdiff = std::abs(fsim[0] - fsim[i]);
					if (absdiff > curmax)
					{
						curmax = absdiff;
						if (curmax > fatol) should_break = false;
					}
				}
			}
		}
		if (should_break) break;

		fp_vector xbar(n), xr(n);
		for (int j = 0; j < n; ++j)
		{
			for (int i = 0; i < sim.size() - 1; ++i)
				xbar[j] += sim[i][j];
			xbar[j] /= n;
			xr[j] = (1.0 + rho) * xbar[j] - rho * sim[n][j];
		}

		BOOL_TYPE doshrink = false;
		FP_TYPE fxr;
		{
			std::optional<FP_TYPE> opt = func(xr);
			if (opt.has_value()) fxr = opt.value();
			else break;
		}
		if (fxr < fsim[0])
		{
			fp_vector xe(n);
			for (int j = 0; j < n; ++j)
				xe[j] = (1.0 + rho * chi) * xbar[j] - rho * chi * sim[n][j];

			FP_TYPE fxe;
			{
				std::optional<FP_TYPE> opt = func(xe);
				if (opt.has_value()) fxe = opt.value();
				else break;
			}

			if (fxe < fxr)
				sim[n] = xe, fsim[n] = fxe;
			else
				sim[n] = xr, fsim[n] = fxr;
		}
		else if (fxr < fsim[n - 1])
			sim[n] = xr, fsim[n] = fxr;
		else
		{
			if (fxr < fsim[n])
			{
				fp_vector xc(n);
				for (int j = 0; j < n; ++j)
					xc[j] = (1.0 + psi * rho) * xbar[j] - psi * rho * sim[n][j];

				FP_TYPE fxc;
				{
					std::optional<FP_TYPE> opt = func(xc);
					if (opt.has_value()) fxc = opt.value();
					else break;
				}

				if (fxc <= fxr)
					sim[n] = xc, fsim[n] = fxc;
				else
					doshrink = true;
			}
			else
			{
				fp_vector xcc(n);
				for (int j = 0; j < n; ++j)
					xcc[j] = (1.0 - psi) * xbar[j] + psi * sim[n][j];

				FP_TYPE fxcc;
				{
					std::optional<FP_TYPE> opt = func(xcc);
					if (opt.has_value()) fxcc = opt.value();
					else break;
				}

				if (fxcc < fsim[n])
					sim[n] = xcc, fsim[n] = fxcc;
				else
					doshrink = true;
			}

			if (doshrink)
				for (int i = 1; i <= n && !should_break; ++i)
				{
					for (int j = 0; j < n; ++j)
						sim[i][j] = sim[0][j] + sigma * (sim[i][j] - sim[0][j]);

					std::optional<FP_TYPE> fx = func(sim[i]);
					if (fx.has_value()) fsim[i] = fx.value();
					else should_break = true;
				}
			if (should_break) break;
		}
		++iterations;
		{
			const auto ind = vec_argsort(fsim);
			vec_reorder(sim, ind);
			vec_reorder(fsim, ind);
		}
	}
	debug_matrix(sim);
	debug_matrix_extra(sim);
	debug_vector(fsim);
	debug_vector_extra(fsim);
	debug_val(iterations);

	return sim[0];
}

FP_TYPE
KTC_Bvc::penalized_nnlf(const fp_vector &theta, const fp_vector &x)
{
	auto scale = theta[2];
	auto loc = theta[1];
	auto arg = theta[0];

	if (arg <= 0.0 or scale <= 0.0)
		return fp_inf();

	auto n_log_scale = static_cast<FP_TYPE>(x.size()) * std::log(scale);
	FP_TYPE total = 0.0, n_bad = 0.0;

	if (std::isinf(arg))
	{
		const FP_TYPE f = std::log(std::sqrt(2.0 * M_PI));

		for (auto &el : x)
		{
			FP_TYPE val = (el - loc) / scale;
			val = -(std::pow(val, 2)) / 2.0 - f;

			if (std::isfinite(val)) total += val;
			else ++n_bad;
		}
	}
	else if (arg >= 200.0)
	{
		const FP_TYPE f = -0.5 * (1.0 + std::log(2.0 * M_PI))
			+ arg / 2.0 * std::log(1.0 / (arg + 1.0))
			+ 1.0 / 6.0 * std::pow((arg + 1), -1.0)
			- 1.0 / 45.0 * std::pow((arg + 1), -3.0)
			- 1.0 / 6.0 * std::pow(arg, -1.0)
			+ 1.0 / 45.0 * std::pow(arg, -3.0);

		for (auto &el : x)
		{
			FP_TYPE val = (el - loc) / scale;
			val = f - (arg + 1.0) / 2.0 * std::log(1 + (val * val / arg));

			if (std::isfinite(val)) total += val;
			else ++n_bad;
		}
	}
	else
	{
		const FP_TYPE f = std::lgamma((arg + 1.0) / 2.0)
			- std::lgamma(arg / 2.0)
			- (0.5 * std::log(arg * M_PI));

		for (auto &el : x)
		{
			FP_TYPE val = (el - loc) / scale;
			val = f - (arg + 1.0) / 2.0 * std::log(1 + (val * val / arg));

			if (std::isfinite(val)) total += val;
			else ++n_bad;
		}
	}

	FP_TYPE penalty = -total + n_bad * std::log(std::numeric_limits<FP_TYPE>::max()) * 100.0;
	return penalty + n_log_scale;
}
