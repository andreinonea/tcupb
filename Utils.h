#ifndef UPB_TC_UTILS_H_
#define UPB_TC_UTILS_H_

#include <utility>
#include <vector>

typedef bool BOOL_TYPE;
typedef int INT_TYPE;
typedef unsigned int UINT_TYPE;
typedef long double FP_TYPE;

std::pair<std::vector<INT_TYPE>, std::vector<INT_TYPE>> get_ind (const std::vector<FP_TYPE> &vfp_arr, const std::vector<FP_TYPE> &vfp_time);
#endif // "UPB_TC_UTILS_H_"
