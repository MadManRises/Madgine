#pragma once

#if ANDROID

#include <jni.h>

namespace Engine {
namespace JNI {

    INTERFACES_EXPORT void setVM(JavaVM *vm, JNIEnv *env, jobject activity);

    INTERFACES_EXPORT void initThread();
    INTERFACES_EXPORT void finalizeThread();

    INTERFACES_EXPORT jobject activity();
    
    INTERFACES_EXPORT void callStaticFunction(const char *className, const char *functionName, jobject object);
    INTERFACES_EXPORT void callStaticFunction2(const char *className, const char *functionName, std::string_view string, jlong v);

    INTERFACES_EXPORT void registerNatives(const char *className, std::span<const JNINativeMethod> methods);

    INTERFACES_EXPORT std::string getExceptionMessage(jthrowable ex, JNIEnv *env = nullptr);

}
}

#endif