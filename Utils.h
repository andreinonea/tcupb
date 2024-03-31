#ifndef UPB_TC_UTILS_H_
#define UPB_TC_UTILS_H_

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

std::pair<int_vector, int_vector> get_ind(const fp_vector &arr, const fp_vector &time);
std::pair<int_vector, int_vector> quote_index(const fp_vector &q_t, const fp_vector &tr_t);

#endif // UPB_TC_UTILS_H_
