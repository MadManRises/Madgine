
set(ANDROID_SDK C:\\android-sdk)

if (NOT EXISTS ${ANDROID_SDK})
	MESSAGE(SEND_ERROR "Please set location of ANDROID_SDK")
endif()

set(ANDROID_NO_UNDEFINED report-all)
set(ANDROID_PLATFORM android-21) # API level
#specify ANDROID_ABI in arguments


set(ANDROID_NDK ${ANDROID_SDK}/ndk-bundle)

set(ANDROID_TOOLCHAIN clang)

include (${ANDROID_NDK}/build/cmake/android.toolchain.cmake)
