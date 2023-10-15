#include "Interfaces/interfaceslib.h"
#include "Interfaces/debug/stacktrace.h"
#include "Generic/guard.h"

#ifndef NDEBUG
void madgine_terminate_handler()
{
    {
        std::stringstream cout;
        cout << "Terminate called! (Madgine-Handler)\n";
        cout << "Stack-Trace:\n";
        for (const Engine::Debug::TraceBack &trace : Engine::Debug::StackTrace<64>::getCurrent(1).calculateReadable())
            cout << trace.mFunction << " (" << trace.mFile << ": " << trace.mLineNr << ")\n";
        LOG_FATAL(cout.str());
    }
    abort();
}

static Engine::Guard global { []() { std::set_terminate(&madgine_terminate_handler); } };
#endif