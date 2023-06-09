#include "vulkanlib.h"

#include "vulkanrendercontext.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Modules/uniquecomponent/uniquecomponent.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Modules/threading/workgroupstorage.h"

#include "Meta/math/vector4.h"

#include "vulkanrendertexture.h"
#include "vulkanrenderwindow.h"

#include "vulkanmeshloader.h"
#include "vulkanpipelineloader.h"
#include "vulkantextureloader.h"

#include "Madgine/render/constantvalues.h"

UNIQUECOMPONENT(Engine::Render::VulkanRenderContext)

METATABLE_BEGIN(Engine::Render::VulkanRenderContext)
METATABLE_END(Engine::Render::VulkanRenderContext)

namespace Engine {
namespace Render {

    Threading::WorkgroupLocal<VulkanPtr2<VkDevice, &vkDestroyDevice>> sDevice;

    Threading::WorkgroupLocal<VulkanRenderContext *> sSingleton = nullptr;

    VkDevice GetDevice()
    {
        return sDevice->get();
    }

    const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char *> extensions = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME
#if WINDOWS
        ,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif
    };

    const std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
        VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME
    };

    static constexpr uint32_t sMaxFramesInFlight = 3;

    bool checkValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char *layerName : validationLayers) {
            bool layerFound = false;

            for (const auto &layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData)
    {
        Util::MessageType lvl;
        switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            lvl = Util::MessageType::ERROR_TYPE;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            lvl = Util::MessageType::WARNING_TYPE;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            lvl = Util::MessageType::INFO_TYPE;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            lvl = Util::MessageType::DEBUG_TYPE;
            break;
        }

        std::stringstream cout;
        cout << "Debug message";
        if (pCallbackData->pMessageIdName) {
            cout << "(" << pCallbackData->pMessageIdName << ")";
        }
        cout << ": " << pCallbackData->pMessage;

        Util::LogDummy { lvl } << cout.str();

        if (lvl == Util::MessageType::ERROR_TYPE)
            Util::log_fatal(cout.str());

        return VK_FALSE;
    }

    VkDebugUtilsMessengerEXT sDebugMessenger;

    void destroyInstance(VkInstance instance, VkAllocationCallbacks *pAllocator)
    {
        vkDestroyDebugUtilsMessengerEXT(instance, sDebugMessenger, nullptr);

        vkDestroyInstance(instance, pAllocator);
    }

    VulkanPtr2<VkInstance, destroyInstance> createInstance()
    {
        assert(checkValidationLayerSupport());

        VkApplicationInfo appInfo {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "MadgineLauncher";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Madgine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 1, 0);

        VkInstanceCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        createInfo.enabledLayerCount = validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();

        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT createInfo2 {};
        createInfo2.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo2.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo2.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo2.pfnUserCallback = debugCallback;
        createInfo2.pUserData = nullptr; // Optional

        createInfo.pNext = &createInfo2;

        VulkanPtr2<VkInstance, destroyInstance> instance;
        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
        VK_CHECK(result);

#if !ANDROID
        gladLoaderLoadVulkan(instance, nullptr, nullptr);
#endif

        result = vkCreateDebugUtilsMessengerEXT(instance, &createInfo2, nullptr, &sDebugMessenger);
        VK_CHECK(result);

        return instance;
    }

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies { queueFamilyCount };
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto &queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

#if WINDOWS
            if (vkGetPhysicalDeviceWin32PresentationSupportKHR(device, i))
                indices.presentFamily = i;
#elif ANDROID
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.presentFamily = i;
            }
#else
#    error "Unsupported Platform!"
#endif

            i++;
        }

        return indices;
    }

    bool isDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices = findQueueFamilies(device);

        if (!indices.isComplete())
            return false;

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions { extensionCount };
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string_view> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto &extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        if (!requiredExtensions.empty())
            return false;

        return true;
    }

    VkPhysicalDevice selectPhysicalDevice(VkInstance instance)
    {

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        assert(deviceCount > 0);

        std::vector<VkPhysicalDevice> devices { deviceCount };
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (VkPhysicalDevice device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        assert(physicalDevice != VK_NULL_HANDLE);
        return physicalDevice;
    }

    struct Environment {
        VulkanPtr2<VkInstance, destroyInstance> mInstance;
        VkPhysicalDevice mDevice;
    };

    Environment createEnvironment()
    {
#if !ANDROID
        gladLoaderLoadVulkan(nullptr, nullptr, nullptr);
#endif
        auto instance = createInstance();
        auto physicalDevice = selectPhysicalDevice(instance);
#if !ANDROID
        gladLoaderLoadVulkan(instance, physicalDevice, nullptr);
#endif

        return { std::move(instance), std::move(physicalDevice) };
    }

    const Environment &getEnvironment()
    {
        static auto environment = createEnvironment();
        return environment;
    }

    VkInstance GetInstance()
    {
        return getEnvironment().mInstance;
    }

    VkPhysicalDevice GetPhysicalDevice()
    {
        return getEnvironment().mDevice;
    }

    VulkanRenderContext::VulkanRenderContext(Threading::TaskQueue *queue)
        : Component(queue)
    {

        assert(!sSingleton);

        sSingleton = this;

        VkPhysicalDevice physicalDevice = getEnvironment().mDevice;

        mIndices = findQueueFamilies(physicalDevice);

        float queuePriority = 1.0f;

        VkDeviceQueueCreateInfo queueCreateInfo[2] {};
        queueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo[0].queueFamilyIndex = mIndices.graphicsFamily.value();
        queueCreateInfo[0].queueCount = 1;
        queueCreateInfo[0].pQueuePriorities = &queuePriority;

        queueCreateInfo[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo[1].queueFamilyIndex = mIndices.presentFamily.value();
        queueCreateInfo[1].queueCount = 1;
        queueCreateInfo[1].pQueuePriorities = &queuePriority;

        VkPhysicalDeviceFeatures deviceFeatures {};

        VkPhysicalDeviceTimelineSemaphoreFeatures features {};
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;
        features.timelineSemaphore = true;

        VkPhysicalDeviceDescriptorIndexingFeatures features2 {};
        features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
        features2.pNext = &features;
        features2.descriptorBindingPartiallyBound = true;

        VkDeviceCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pNext = &features2;

        createInfo.pQueueCreateInfos = queueCreateInfo;
        createInfo.queueCreateInfoCount = 2;

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        createInfo.enabledExtensionCount = deviceExtensions.size();
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &*sDevice);
        VK_CHECK(result);

        vkGetDeviceQueue(*sDevice, mIndices.graphicsFamily.value(), 0, &mGraphicsQueue);

        vkGetDeviceQueue(*sDevice, mIndices.presentFamily.value(), 0, &mPresentQueue);

        VkDescriptorPoolSize poolSize {};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = 32;

        VkDescriptorPoolCreateInfo poolInfo {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;

        poolInfo.maxSets = 32;

        result = vkCreateDescriptorPool(*sDevice, &poolInfo, nullptr, &mDescriptorPool);
        VK_CHECK(result);

        VkCommandPoolCreateInfo poolInfo2 {};
        poolInfo2.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo2.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo2.queueFamilyIndex = mIndices.graphicsFamily.value();

        result = vkCreateCommandPool(*sDevice, &poolInfo2, nullptr, &mCommandPool);
        VK_CHECK(result);

        mConstantBufferHeap = { 16 * 1024 * 1024 };

        createPipelineLayout();

        ConstantValues values;
        mConstantBuffer.setData({ &values, sizeof(values) });
    }

    VulkanRenderContext::~VulkanRenderContext()
    {
        mConstantBuffer.reset();

        mPipelineLayout.reset();

        mDescriptorSetLayouts[0].reset();
        mDescriptorSetLayouts[1].reset();
        mDescriptorSetLayouts[2].reset();

        mSamplers[0].reset();
        mSamplers[1].reset();

        mCommandPool.reset();

        VkResult result = vkFreeDescriptorSets(GetDevice(), mDescriptorPool, 1, &mSamplerDescriptorSet);
        VK_CHECK(result);

        mConstantBufferHeap.reset();

        mCommandPool.reset();
        mDescriptorPool.reset();

        sDevice->reset();

        assert(sSingleton == this);
        sSingleton = nullptr;
    }

    std::unique_ptr<RenderTarget> VulkanRenderContext::createRenderTexture(const Vector2i &size, const RenderTextureConfig &config)
    {
        return std::make_unique<VulkanRenderTexture>(this, size, config);
    }

    bool VulkanRenderContext::beginFrame()
    {
        return RenderContext::beginFrame();
    }

    void VulkanRenderContext::endFrame()
    {
        RenderContext::endFrame();
    }

    VulkanRenderContext &VulkanRenderContext::getSingleton()
    {
        return *sSingleton;
    }

    std::unique_ptr<RenderTarget> VulkanRenderContext::createRenderWindow(Window::OSWindow *w, size_t samples)
    {
        return std::make_unique<VulkanRenderWindow>(this, w, samples);
    }

    void VulkanRenderContext::unloadAllResources()
    {
        RenderContext::unloadAllResources();

        for (std::pair<const std::string, VulkanPipelineLoader::Resource> &res : VulkanPipelineLoader::getSingleton()) {
            res.second.forceUnload();
        }

        for (std::pair<const std::string, VulkanTextureLoader::Resource> &res : VulkanTextureLoader::getSingleton()) {
            res.second.forceUnload();
        }

        for (std::pair<const std::string, VulkanMeshLoader::Resource> &res : VulkanMeshLoader::getSingleton()) {
            res.second.forceUnload();
        }
    }

    bool VulkanRenderContext::supportsMultisampling() const
    {
        return true;
    }

    void VulkanRenderContext::createPipelineLayout()
    {
        VkDescriptorBindingFlags flags[4] { 0, 0, 0, VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT };
        VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlags {};
        bindingFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
        bindingFlags.bindingCount = 4;
        bindingFlags.pBindingFlags = flags;

        VkDescriptorSetLayoutBinding uboLayoutBinding[4] {};
        for (size_t i = 0; i < 4; ++i) {
            uboLayoutBinding[i].binding = i;
            uboLayoutBinding[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            uboLayoutBinding[i].descriptorCount = 1;
            uboLayoutBinding[i].stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
            uboLayoutBinding[i].pImmutableSamplers = nullptr;
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 4;
        layoutInfo.pBindings = uboLayoutBinding;
        layoutInfo.pNext = &bindingFlags;

        VkResult result = vkCreateDescriptorSetLayout(*sDevice, &layoutInfo, nullptr, &mDescriptorSetLayouts[0]);
        VK_CHECK(result);

        VkDescriptorSetLayoutBinding imagePushLayoutBindings[5] {};
        for (size_t i = 0; i < 5; ++i) {
            imagePushLayoutBindings[i].binding = i;
            imagePushLayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            imagePushLayoutBindings[i].descriptorCount = 1;
            imagePushLayoutBindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            imagePushLayoutBindings[i].pImmutableSamplers = nullptr;
        }
        VkDescriptorBindingFlags flags2[5] { VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT, VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT, VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT, VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT, VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT };
        bindingFlags.bindingCount = 5;
        bindingFlags.pBindingFlags = flags2;

        layoutInfo.bindingCount = 5;
        layoutInfo.pBindings = imagePushLayoutBindings;
        layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;
        layoutInfo.pNext = &bindingFlags;

        result = vkCreateDescriptorSetLayout(*sDevice, &layoutInfo, nullptr, &mDescriptorSetLayouts[1]);
        VK_CHECK(result);

        layoutInfo.flags = 0;
        layoutInfo.pNext = nullptr;

        VkSamplerCreateInfo samplerInfo {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.maxAnisotropy = 1;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        samplerInfo.minLod = 0;
        samplerInfo.maxLod = std::numeric_limits<float>::max();

        vkCreateSampler(*sDevice, &samplerInfo, nullptr, &mSamplers[0]);

        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

        vkCreateSampler(*sDevice, &samplerInfo, nullptr, &mSamplers[1]);

        VkDescriptorSetLayoutBinding samplerLayoutBinding {};
        samplerLayoutBinding.binding = 0;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        samplerLayoutBinding.descriptorCount = 2;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        samplerLayoutBinding.pImmutableSamplers = &std::as_const(*mSamplers);

        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &samplerLayoutBinding;
        result = vkCreateDescriptorSetLayout(*sDevice, &layoutInfo, nullptr, &mDescriptorSetLayouts[2]);
        VK_CHECK(result);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 3; // Optional
        pipelineLayoutInfo.pSetLayouts = &std::as_const(*mDescriptorSetLayouts); // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        result = vkCreatePipelineLayout(*sDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout);
        VK_CHECK(result);

        VkDescriptorSetAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = mDescriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &std::as_const(mDescriptorSetLayouts[2]);
        result = vkAllocateDescriptorSets(GetDevice(), &allocInfo, &mSamplerDescriptorSet);
        VK_CHECK(result);
    }

    static constexpr VkFormat vFormats[] = {
        VK_FORMAT_R32G32B32_SFLOAT,
        VK_FORMAT_R32_SFLOAT,
        VK_FORMAT_R32G32_SFLOAT,
        VK_FORMAT_R32G32B32_SFLOAT,
        VK_FORMAT_R32G32B32A32_SFLOAT,
        VK_FORMAT_R32G32_SFLOAT,
        VK_FORMAT_R32G32B32A32_SINT,
        VK_FORMAT_R32G32B32A32_SFLOAT
    };

    std::pair<std::vector<VkVertexInputBindingDescription>, std::vector<VkVertexInputAttributeDescription>> VulkanRenderContext::createVertexLayout(VertexFormat format, size_t instanceDataSize)
    {
        std::pair<std::vector<VkVertexInputBindingDescription>, std::vector<VkVertexInputAttributeDescription>> vertexLayoutDesc;

        uint32_t stride = format.stride();

        vertexLayoutDesc.first.push_back({ 0, stride, VK_VERTEX_INPUT_RATE_VERTEX });
        vertexLayoutDesc.first.push_back({ 2, 0, VK_VERTEX_INPUT_RATE_VERTEX });

        uint32_t offset = 0;
        for (uint32_t i = 0; i < VertexElements::size; ++i) {
            if (format.has(i)) {
                vertexLayoutDesc.second.push_back({ i, 0, vFormats[i], offset });
                offset += sVertexElementSizes[i];
            } else {
                vertexLayoutDesc.second.push_back({ i, 2, vFormats[i], vConstantOffsets[i] });
            }
        }

        if (instanceDataSize > 0) {
            vertexLayoutDesc.first.push_back({ 1, static_cast<uint32_t>(instanceDataSize), VK_VERTEX_INPUT_RATE_INSTANCE });
            assert(instanceDataSize % 16 == 0);
            for (size_t i = 0; i < instanceDataSize / 16; ++i) {
                vertexLayoutDesc.second.push_back({ static_cast<uint32_t>(VertexElements::size + i), 1, VK_FORMAT_R32G32B32A32_SFLOAT, static_cast<uint32_t>(i * 16) });
            }
        }

        return vertexLayoutDesc;
    }

}
}
