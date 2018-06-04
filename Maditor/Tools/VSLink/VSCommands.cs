#if __cplusplus    
#pragma once
#endif
namespace VSCommands
{
#if !__cplusplus    
    public
#endif

// shared enum for both C, C++ and C#
enum VSCommand
    { OpenSolution, PIDRequest, PIDAnswer, OpenSourceFile, VSStarted };


}
