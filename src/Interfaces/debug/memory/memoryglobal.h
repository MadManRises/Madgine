#pragma once

#ifdef ENABLE_MEMTRACKING

#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>
#endif 


#endif