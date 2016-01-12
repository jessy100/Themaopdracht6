#include "bmptk.h"

// not user level include files, hence not included by bmptk.h
// #include "bmptk_win_graphics.hx"
#include "windows.h"
#include <iostream>
#include <iomanip>

bmptk::time bmptk::now(){
   return GetTickCount() * 1000ULL;
}

void bmptk::wait( bmptk::time d ){
   bmptk::time end = bmptk::now() + d;
   while( bmptk::now() < end ){
	   MSG Msg; 
	   BOOL r = PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE );
      if( r == -1 ){
         exit( 0 );
      } else if( r > 0) {
		   TranslateMessage(&Msg); /* Translate key codes to chars if present */
		   DispatchMessage(&Msg); /* Send it to WndProc */
	   }   
   }
}

/*
void bmptk::fatal_error_detected( const char *msg  ){
   std::cout << "fatal error : " << msg << "\n" << std::flush;
   for(;;);
}
*/


