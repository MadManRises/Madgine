#pragma once

/// @cond

#include "Madgine/meshloaderlib.h"
#include "Madgine/pipelineloaderlib.h"
#include "Madgine/textureloaderlib.h"
#include "Madgine/clientlib.h"

#if defined(Vulkan_EXPORTS)
#    define MADGINE_VULKAN_EXPORT DLL_EXPORT
#else
#    define MADGINE_VULKAN_EXPORT DLL_IMPORT
#endif

#include "vulkanforward.h"


#define NOMINMAX
#if !ANDROID && !EMSCRIPTEN && !IOS
#    include "../glad/vulkan.h"
#elif IOS
#    include <OpenGLES/ES3/gl.h>
#elif EMSCRIPTEN
#    include <GLES3/gl3.h>
#else
#    include <GLES3/gl31.h>
#endif

#include "util/vulkanptr.h"

inline void vkCheck(VkResult result)
{
    if (result != VK_SUCCESS) {
        LOG("Vulkan-Error: "
            << "?");
        std::terminate();
    }
}

#define VK_CHECK(...) vkCheck(__VA_ARGS__)

//#define VK_LOG(x) LOG("VK: " << x)
#define VK_LOG(x)

/// @endcond
