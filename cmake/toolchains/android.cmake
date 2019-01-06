
set(ANDROID_SDK C:\\android-sdk)

if (NOT EXISTS ${ANDROID_SDK})
	MESSAGE(SEND_ERROR "Please set location of ANDROID_SDK")
endif()

set(ANDROID_NO_UNDEFINED report-all)
set(ANDROID_PLATFORM android-28) # API level
set(ANDROID_ABI x86_64)

set(ANDROID_NDK ${ANDROID_SDK}/ndk-bundle)

include (${ANDROID_NDK}/build/cmake/android.toolchain.cmake)
