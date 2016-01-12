#ifndef _BMPTK_CORTEX_H_
#define _BMPTK_CORTEX_H_

#include "targets/lpc1114fn28.hpp"

namespace bmptk {

typedef hwcpp::lpc1114fn28< 48 * hwcpp::MHz > target;
typedef target::timing timing;

typedef long long int time;
const time us = timing::duration::ticks_per_us;
const time ms = 1000 * us;
const time s  = 1000 * ms;
time now();
void wait( time d );

} // namespace bmptk

#endif // #ifdef _BMPTK_CORTEX_H_ 