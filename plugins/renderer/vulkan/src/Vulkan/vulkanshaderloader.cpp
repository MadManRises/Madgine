#include "vulkanlib.h"

#include "vulkanshaderloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

#include "Interfaces/filesystem/fsapi.h"

#include "vulkanshadercodegen.h"
#include "vulkanrendercontext.h"

UNIQUECOMPONENT(Engine::Render::VulkanShaderLoader);

METATABLE_BEGIN(Engine::Render::VulkanShaderLoader)
METATABLE_END(Engine::Render::VulkanShaderLoader)

METATABLE_BEGIN_BASE(Engine::Render::VulkanShaderLoader::Resource, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::VulkanShaderLoader::Resource)

namespace Engine {
namespace Render {

    std::wstring GetLatestPixelProfile()
    {
        return L"ps_6_0";
    }

    VulkanShaderLoader::VulkanShaderLoader()
        : ResourceLoader({ ".spirv" })
    {
    }

    Threading::TaskFuture<bool> VulkanShaderLoader::Handle::create(std::string_view name, const CodeGen::ShaderFile &file, ShaderType type, VulkanShaderLoader *loader)
    {
        std::string fullName { name };
        if (fullName != Resources::ResourceBase::sUnnamed) {
            switch (type) {
            case ShaderType::PixelShader:
                fullName += "_PS";
                break;
            case ShaderType::VertexShader:
                fullName += "_VS";
                break;
            default:
                throw 0;
            }
        }

        return Base::Handle::create(
            name, {}, [=, &file](VulkanShaderLoader *loader, VulkanPtr<VkShaderModule, &vkDestroyShaderModule> &shader, const VulkanShaderLoader::ResourceDataInfo &info) { return loader->create(shader, info.resource(), file, type); }, loader);
    }

    Threading::TaskFuture<bool> VulkanShaderLoader::Handle::load(std::string_view name, ShaderType type, VulkanShaderLoader *loader)
    {
        std::string actualName { name };
        switch (type) {
        case ShaderType::PixelShader:
            actualName += "_PS";
            break;
        case ShaderType::VertexShader:
            actualName += "_VS";
            break;
        case ShaderType::GeometryShader:
            actualName += "_GS";
            break;
        default:
            throw 0;
        }

        return Base::Handle::load(actualName, loader);
    }

    bool VulkanShaderLoader::loadImpl(VulkanPtr<VkShaderModule, &vkDestroyShaderModule> &shader, ResourceDataInfo &info)
    {
        std::string_view filename = info.resource()->name();

        ShaderType type;
        if (filename.ends_with("_VS"))
            type = ShaderType::VertexShader;
        else if (filename.ends_with("_PS"))
            type = ShaderType::PixelShader;
        else if (filename.ends_with("_GS"))
            type = ShaderType::GeometryShader;
        else
            throw 0;

        std::vector<unsigned char> source = info.resource()->readAsBlob();

        return loadFromSource(shader, filename, std::move(source), type, info.resource()->path().stem());
    }

    void VulkanShaderLoader::unloadImpl(VulkanPtr<VkShaderModule, &vkDestroyShaderModule> &shader)
    {
        shader.reset();
    }

    bool VulkanShaderLoader::create(VulkanPtr<VkShaderModule, &vkDestroyShaderModule> &shader, Resource *res, const CodeGen::ShaderFile &file, ShaderType type)
    {
        if (res->path().empty()) {
            Filesystem::Path dir = Filesystem::appDataPath() / "generated/shader/vulkan";

            Filesystem::createDirectories(dir);

            res->setPath(dir / (std::string { res->name() } + (type == VertexShader ? "_VS" : "_PS") + ".spirv"));
        }

        std::ostringstream ss;
        VulkanShaderCodeGen::generate(ss, file, 1);

        {
            std::ofstream f { res->path() };
            f << ss.str();
        }
        throw 0;
        //return loadFromSource(shader, res->name(), ss.str(), type);
    }

    bool VulkanShaderLoader::loadFromSource(VulkanPtr<VkShaderModule, &vkDestroyShaderModule> &shader, std::string_view name, std::vector<unsigned char> source, ShaderType type, const Filesystem::Path &path)
    {
       
        VkShaderModuleCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = source.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(source.data());
        VkResult result = vkCreateShaderModule(GetDevice(), &createInfo, nullptr, &shader);
        VK_CHECK(result);

        return true;
    }

    Threading::TaskQueue *VulkanShaderLoader::loadingTaskQueue() const
    {
        return VulkanRenderContext::renderQueue();
    }
}
}
