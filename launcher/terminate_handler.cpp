#include "Interfaces/interfaceslib.h"
#include "Interfaces/debug/stacktrace.h"
#include "Generic/guard.h"

void madgine_terminate_handler()
{
    {
        Engine::Util::LogDummy cout { Engine::Util::MessageType::ERROR_TYPE };
        cout << "Terminate called! (Madgine-Handler)\n";
        cout << "Stack-Trace:\n";
        for (const Engine::Debug::TraceBack &trace : Engine::Debug::StackTrace<64>::getCurrent(1).calculateReadable())
            cout << trace.mFunction << " (" << trace.mFile << ": " << trace.mLineNr << ")\n";
    }
    abort();
}

static Engine::Guard global { []() { std::set_terminate(&madgine_terminate_handler); } };
