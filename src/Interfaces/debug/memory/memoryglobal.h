#pragma once

#if ENABLE_MEMTRACKING

#if WINDOWS
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>
#endif 


#endif