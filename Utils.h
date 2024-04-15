#ifndef UPB_TC_UTILS_H_
#define UPB_TC_UTILS_H_

#define debug_vector_n(v, n) \
do { \
	std::cout << #v " [ "; \
\
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
\
	std::cout << "]\n"; \
} while (0)

#define debug_vector_all(v) \
do { \
	std::cout << #v " ["; \
	for (auto el: v) \
		std::cout << el << ", "; \
	std::cout << "]\n"; \
} while (0)

#define debug_vector(v) debug_vector_n(v, 3)

#include <algorithm>
#include <cassert>
#include <functional>
#include <utility>
#include <vector>

typedef bool BOOL_TYPE;
typedef unsigned char UCHAR_TYPE;
typedef int INT_TYPE;
typedef unsigned int UINT_TYPE;
typedef long double FP_TYPE;

typedef std::vector<INT_TYPE> int_vector;
typedef std::vector<FP_TYPE> fp_vector;
typedef std::vector<std::vector<FP_TYPE>> fp_matrix;

// Vector utils
template <typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(result), std::plus<T>());
    return result;
}

fp_vector vec_diff(const fp_vector &v);
int_vector vec_diff(const int_vector &v);
int_vector vec_nonzero(const fp_vector &v);

std::pair<int_vector, int_vector> get_ind(const fp_vector &arr, const fp_vector &time);
std::pair<int_vector, int_vector> quote_index(const fp_vector &q_t, const fp_vector &tr_t);
fp_vector concat_runs(const int_vector &x, bool hj_version);
fp_vector interpolate_time(const fp_vector &time, bool hj_version = false);

struct KTC_Data
{
	fp_vector time;
	fp_vector price;
	fp_vector volume;
};

enum class KTC_Algorithm : UCHAR_TYPE
{
	FULL_INFORMATION_DS_1 = 1,
	FULL_INFORMATION_DS_2,
	FULL_INFORMATION_DS_3,
	LEE_READY,
	BULK_VOLUME,
	EMO,
	CLNV,
};

#endif // UPB_TC_UTILS_H_
