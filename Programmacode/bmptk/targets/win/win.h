#ifndef _BMPTK_WIN_H_
#define _BMPTK_WIN_H_

#include <iostream>

namespace bmptk {

typedef long long int time;
const time us = 1;
const time ms = 1000 * us;
const time s  = 1000 * ms;
time now();
void wait( time d );

} // namespace bmptk

namespace hwcpp {

namespace io = std;
   
}; // namespace hwcpp

#endif // #ifdef _BMPTK_WIN_H_ 
