#include "cortex.h"

namespace bmptk {

time now(){
   static bool init_done = false;
   if( ! init_done ) { 
      timing::init();
      init_done = true;
   }
   return timing::now().raw();
}

void wait( time d ){
   time end = now() + d;
   while( now() < end ){}
}   

};