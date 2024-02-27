#include "Interfaces/interfaceslib.h"

#include "playservices.h"

#include "Interfaces/helpers/android_jni.h"

void VoidReceiver_setValue(JNIEnv *env, jclass, Engine::Execution::VirtualReceiverBase<std::string> *rec)
{
    rec->set_value();
}

void VoidReceiver_setError(JNIEnv *env, jclass, Engine::Execution::VirtualReceiverBase<std::string> *rec, jthrowable ex)
{ 
    rec->set_error(Engine::JNI::getExceptionMessage(ex, env));
}

namespace PlayServices {

void setup()
{
    Engine::JNI::callStaticFunction("com/Madgine/PlayServices", "Setup", Engine::JNI::activity());

    static const JNINativeMethod methods[] = {
        { "setValue", "(J)V", reinterpret_cast<void *>(VoidReceiver_setValue) },
        { "setError", "(JLjava/lang/Exception;)V", reinterpret_cast<void *>(VoidReceiver_setError) },
    };

    Engine::JNI::registerNatives("com/Madgine/VoidReceiver", methods);
}

namespace Achievements {

    void unlockImpl(Engine::Execution::VirtualReceiverBase<std::string> &rec, std::string_view id)
    {
        Engine::JNI::callStaticFunction2("com/Madgine/PlayServices", "Achievements_Unlock", id, reinterpret_cast<jlong>(&rec));
    }

}

}
