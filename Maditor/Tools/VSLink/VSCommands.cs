#if !__cplusplus    
namespace VSCommands
{
    public
#endif

// shared enum for both C, C++ and C#
enum VSCommand
    { OpenSolution, PIDRequest, PIDAnswer, OpenSourceFile, VSStarted };

#if !__cplusplus
}
#endif