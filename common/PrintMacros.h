 /*********************************************************************************
*  Copyright (c) 2010-2011, Elliott Cooper-Balis
*                             Paul Rosenfeld
*                             Bruce Jacob
*                             University of Maryland 
*                             dramninjas [at] gmail [dot] com
*  All rights reserved.
*  
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*  
*     * Redistributions of source code must retain the above copyright notice,
*        this list of conditions and the following disclaimer.
*  
*     * Redistributions in binary form must reproduce the above copyright notice,
*        this list of conditions and the following disclaimer in the documentation
*        and/or other materials provided with the distribution.
*  
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
*  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
*  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************************************************/





#ifndef PRINT_MACROS_H
#define PRINT_MACROS_H

#include <iostream>

extern int DEBUG_SHOW_OUTPUT;

#define ERROR(str) std::cerr<<"[ERROR ("<<__FILE__<<":"<<__LINE__<<")]: "<<str<<std::endl;

using std::ostream;

#define DEBUG_BUILD
//#define DEBUG_TO_FILE

#ifdef DEBUG_BUILD
#ifdef DEBUG_TO_FILE
#include <fstream>
#define  DEBUG(str) {\
    std::ofstream ofs;\
    ofs.open("DebugInfo.log",std::ios_base::app|std::ios_base::out);\
    ofs<<str<<std::endl;\
    ofs.close();\
    }

#define DEBUGN(str) {\
    std::ofstream ofs;\
    ofs.open("DebugInfo.log",std::ios_base::app|std::ios_base::out);\
    ofs<<str;\
    ofs.close();\
    }
#else
    #define DEBUG(str)  std::cerr<< str //<<std::endl;
    #define DEBUGN(str) std::cerr<< str
#endif      //DEBUG_TO_FILE

#else
    #define DEBUG(str)
    #define DEBUGN(str)
#endif      //DEBUG_BUILD

#ifdef NO_OUTPUT
	#undef DEBUG
	#undef DEBUGN
	#define DEBUG(str) ;
	#define DEBUGN(str) ;
	#define PRINT(str) ;
	#define PRINTN(str) ;
#else
	#ifdef LOG_OUTPUT
        #define PRINT(str)  { usb3014_log <<str<<std::endl; }
        #define PRINTN(str) { usb3014_log <<str; }
	#else
        #define PRINT(str)  if(DEBUG_SHOW_OUTPUT) { std::cout <<str<<std::endl; }
        #define PRINTN(str) if(DEBUG_SHOW_OUTPUT) { std::cout <<str; }
	#endif
#endif

#endif /*PRINT_MACROS_H*/
