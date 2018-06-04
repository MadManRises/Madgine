#pragma once

#ifdef GAME_RELEASE
#define %2 
#elif defined(CLIENT_%1_EXPORTS) || defined(SERVER_%1_EXPORTS)
#define %2 __declspec(dllexport)
#else
#define %2 __declspec(dllimport)
#endif