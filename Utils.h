#ifndef UPB_TC_UTILS_H_
#define UPB_TC_UTILS_H_

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

// TODO: remove
#include <iostream>

#define debug_val(v) \
do { \
	std::cout << #v "=" << v << '\n'; \
} while (0)

#define debug_vector_n_priv(v, n, inmatrix) \
do { \
	if (!inmatrix) std::cout << #v " "; \
	std::cout << "[ "; \
\
	if (v.size() > 0) \
	{ \
		auto fwd_mark = std::next(v.begin(), n); \
		if (fwd_mark > v.end()) fwd_mark = v.end(); \
	\
		for (auto it = v.begin(); it != fwd_mark; ++it) \
			std::cout << *it << " "; \
	\
		auto bwd_mark = std::prev(v.end(), n); \
		if (bwd_mark <= fwd_mark) bwd_mark = fwd_mark; \
		else std::cout << "... "; \
	\
		for (auto it = bwd_mark; it != v.end(); ++it) \
			std::cout << *it << " "; \
	} \
\
	std::cout << "]"; \
	if (!inmatrix) std::cout << '\n'; \
} while (0)

#define debug_vector_n(v, n) debug_vector_n_priv(v, n, 0)

#define debug_vector_all(v) \
do { \
	std::cout << #v " ["; \
	for (auto el: v) \
		std::cout << el << ", "; \
	std::cout << "]\n"; \
} while (0)

#define debug_vector(v) debug_vector_n(v, 3)

#define debug_vector_extra(v) \
do { \
	auto sum = 0.0; \
	for (const auto& el : v) \
		sum += el; \
	std::cout << "sum(" #v ")=" << sum << '\n'; \
	std::cout << "size(" #v ")=" << v.size() << '\n'; \
} while (0)

#define debug_matrix_n(m, n) \
do { \
	std::cout << #m; \
	if (m.size() > 0) \
	{ \
		bool first = true; \
		for (const auto &v : m) \
		{ \
			if (first) \
			{ \
				first = false; \
				std::cout << "\n["; \
				debug_vector_n_priv(v, n, 1); \
			} \
			else \
			{ \
				std::cout << "\n "; \
				debug_vector_n_priv(v, n, 1); \
			} \
		} \
		std::cout << "]\n"; \
	} \
	else std::cout << " [[]]\n"; \
} while (0)

#define debug_matrix(m) debug_matrix_n(m, 3)

#define debug_matrix_extra(m) \
do { \
	auto sum = 0.0; \
	auto rows = m.size(); \
	auto columns = 0, cur_cols = 0; \
	if (rows > 0) \
	{ \
		columns = m[0].size(); \
		for (const auto& r : m) \
		{ \
			cur_cols = r.size(); \
			if (cur_cols != columns) columns = -1; \
			for (const auto& el : r) \
				sum += el; \
		} \
	} \
	std::cout << "sum(" #m ")=" << sum << '\n'; \
	std::cout << "size(" #m ")=" << '(' << rows << ',' << columns << ')' << '\n'; \
} while (0)

#include <algorithm>
#include <cassert>
#include <functional>
#include <utility>
#include <vector>
#include <map>
#include <cmath>

typedef bool BOOL_TYPE;
typedef unsigned char UCHAR_TYPE;
typedef int INT_TYPE;
typedef unsigned int UINT_TYPE;
typedef long double FP_TYPE;
typedef std::pair<FP_TYPE, FP_TYPE> fp_pair;
typedef std::vector<fp_pair> quotes_type;

typedef std::vector<INT_TYPE> int_vector;
typedef std::vector<FP_TYPE> fp_vector;
typedef std::vector<BOOL_TYPE> bool_vector;
typedef std::vector<fp_vector> fp_matrix;

#define fp_inf() std::numeric_limits<FP_TYPE>::infinity()

// Vector utils
template <typename T>
std::vector<T> operator+(const std::vector<T> &a, const std::vector<T> &b)
{
	assert(a.size() == b.size());
	std::vector<T> result;
	result.reserve(a.size());

	std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(result), std::plus<T>());
	return result;
}

template <typename T>
std::vector<T> operator-(const std::vector<T> &a, const std::vector<T> &b)
{
	assert(a.size() == b.size());
	std::vector<T> result;
	result.reserve(a.size());

	std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(result), std::minus<T>());
	return result;
}

template <typename T>
std::vector<T>& operator-=(std::vector<T> &vec, const T &val)
{
	for (auto &el : vec)
		el -= val;
	return vec;
}

template <typename T>
std::vector<T>& operator*=(std::vector<T> &vec, const T &val)
{
	for (auto &el : vec)
		el *= val;
	return vec;
}

template <typename T>
std::vector<T>& operator/=(std::vector<T> &vec, const T &val)
{
	for (auto &el : vec)
		el /= val;
	return vec;
}

int_vector vec_nonzero(const fp_vector &v);

template <typename T>
std::vector<T> vec_diff(const std::vector<T> &v)
{
	std::vector<T> d(v.size() - 1, (T)0);

	for (int i = 0; i < d.size(); ++i)
		d[i] = v[i+1] - v[i];

	return d;
}

template <typename T>
int_vector vec_less(const std::vector<T> &a, const std::vector<T> &b)
{
	assert(a.size() == b.size());
	int_vector d(a.size(), 0);

	for (int i = 0; i < d.size(); ++i)
		if (a[i] < b[i])
			d[i] = 1;

	return d;
}

template <typename Iter>
int_vector vec_less(const Iter a_begin, const Iter a_end, const Iter b_begin, const Iter b_end)
{
	auto a_range = std::distance(a_begin, a_end);
	auto b_range = std::distance(b_begin, b_end);
	assert(a_range == b_range);

	int_vector d(a_range, 0);
	auto d_it = d.begin();

	for (auto a_it = a_begin, b_it = b_begin; a_it < a_end && b_it < b_end && d_it < d.end(); ++a_it, ++b_it, ++d_it)
		if (*a_it < *b_it)
			*d_it = 1;

	return d;
}

template <typename T>
int_vector vec_greater(const std::vector<T> &a, const std::vector<T> &b)
{
	assert(a.size() == b.size());
	int_vector d(a.size(), 0);

	for (int i = 0; i < d.size(); ++i)
		if (a[i] > b[i])
			d[i] = 1;

	return d;
}

template <typename Iter>
int_vector vec_greater(const Iter a_begin, const Iter a_end, const Iter b_begin, const Iter b_end)
{
	auto a_range = std::distance(a_begin, a_end);
	auto b_range = std::distance(b_begin, b_end);
	assert(a_range == b_range);

	int_vector d(a_range, 0);
	auto d_it = d.begin();

	for (auto a_it = a_begin, b_it = b_begin; a_it < a_end && b_it < b_end && d_it < d.end(); ++a_it, ++b_it, ++d_it)
		if (*a_it > *b_it)
			*d_it = 1;

	return d;
}

template <typename T>
std::vector<T> vec_unique(const std::vector<T> &v)
{
	std::vector<T> v_unique{};
	std::copy(v.begin(), v.end(), std::back_inserter(v_unique));
	std::sort(v_unique.begin(), v_unique.end());
	auto fin = std::unique(v_unique.begin(), v_unique.end());
	v_unique.resize(std::distance(v_unique.begin(), fin));
	return v_unique;
}

// Thanks to https://stackoverflow.com/a/12399290/20826565
template <typename T>
std::vector<size_t> vec_argsort(const std::vector<T> &v)
{
	std::vector<size_t> idx(v.size());
	std::iota(idx.begin(), idx.end(), 0);

	std::stable_sort(idx.begin(), idx.end(),
		[&v](size_t i1, size_t i2) -> BOOL_TYPE {
			return v[i1] < v[i2];
		}
	);

	return idx;
}

// Thanks to https://stackoverflow.com/a/838789/20826565
template<typename T>
void vec_reorder(std::vector<T> &v, const std::vector<size_t> &order)
{
	assert(v.size() == order.size());
	std::vector<size_t> ord(order.begin(), order.end());

	for (size_t vv = 0; vv < v.size() - 1; ++vv)
	{
		if (ord[vv] == vv)
			continue;

		size_t oo;
		for (oo = vv + 1; oo < ord.size(); ++oo)
			if (ord[oo] == vv) break;

		std::swap(v[vv], v[ord[vv]]);
		std::swap(ord[vv], ord[oo]);
	}
}

enum class Side : UINT_TYPE
{
	LEFT = 0,
	RIGHT
};

// TODO: give credit where credit is due!
template<typename T>
INT_TYPE vec_searchsorted(const std::vector<T> &a, T v, Side side = Side::LEFT)
{
	switch (side)
	{
		case Side::LEFT:
		{
			return std::distance(a.begin(), std::lower_bound(a.begin(), a.end(), v));
		}
		case Side::RIGHT:
		{
			return std::distance(a.begin(), std::upper_bound(a.begin(), a.end(), v));
		}
	}

	// get rid of compiler warning
	return {};
}

template<typename T>
int_vector vec_searchsorted(const std::vector<T> &a, const std::vector<T> &v, Side side = Side::LEFT)
{
	int_vector indices(v.size(), 0);
	std::transform(v.begin(),
								 v.end(),
								 indices.begin(),
								 [&a, side](const auto& value) { return vec_searchsorted(a, value, side); });
	return indices;
}

template <typename T>
FP_TYPE vec_mean(const std::vector<T> &v)
{
	auto sum = std::accumulate(v.begin(), v.end(), 0.0);
	return sum /= static_cast<FP_TYPE>(v.size());
}

template <typename T>
FP_TYPE vec_std(const std::vector<T> &v, INT_TYPE ddof = 0)
{
	FP_TYPE mean = vec_mean(v);
	FP_TYPE sum = 0.0;

	for (const auto &el : v)
		sum += std::pow((static_cast<FP_TYPE>(el) - mean), 2);

	return std::sqrt(sum / (static_cast<FP_TYPE>(v.size()) - static_cast<FP_TYPE>(ddof)));
}

struct KTC_Data
{
	fp_vector time;
	fp_vector price;
	fp_vector volume;
};

struct KTC_Pair
{
	int_vector left;
	int_vector right;
};

struct KTC_Result
{
	int_vector step;
	int_vector initiator;
};

struct KTC_Interpolation
{
	fp_vector ask_time;
	fp_vector bid_time;
};

KTC_Pair quote_index(const fp_vector &quote_times, const fp_vector &trade_times);
fp_vector concat_runs(const int_vector &x, bool hj_version);
fp_vector interpolate_time(const fp_vector &time, bool hj_version = false);
void apply_tick(KTC_Result &res, const fp_vector &trade_prices);
KTC_Interpolation tcol_interpolation(const fp_vector &ask_time, const fp_vector &bid_time);
fp_vector get_midpoint(KTC_Interpolation interpolation, const fp_vector &ask_price, const fp_vector &bid_price, const fp_vector &trades_time);
fp_vector get_lastquote(const fp_vector &quotes_time, const fp_vector &quote_price, const fp_vector &as_of);

#endif // UPB_TC_UTILS_H_
