#include "directx11lib.h"

#include "directx11geometryshaderloader.h"

#include "directx11rendercontext.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "directx11shadercodegen.h"

#include "Interfaces/filesystem/api.h"

#include "Madgine/render/shadinglanguage/slloader.h"

#include "Madgine/codegen/resolveincludes.h"

UNIQUECOMPONENT(Engine::Render::DirectX11GeometryShaderLoader);

namespace Engine {
namespace Render {

    std::string GetLatestGeometryProfile()
    {
        assert(sDevice);

        // Query the current feature level:
        D3D_FEATURE_LEVEL featureLevel = sDevice->GetFeatureLevel();
        switch (featureLevel) {
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
            return "gs_5_0";
            break;
        case D3D_FEATURE_LEVEL_10_1:
            return "gs_4_1";
            break;
        case D3D_FEATURE_LEVEL_10_0:
            return "gs_4_0";
            break;
        case D3D_FEATURE_LEVEL_9_3:
            return "gs_4_0_level_9_3";
            break;
        case D3D_FEATURE_LEVEL_9_2:
        case D3D_FEATURE_LEVEL_9_1:
            return "gs_4_0_level_9_1";
            break;
        default:
            return "";
        }
    }

    DirectX11GeometryShaderLoader::DirectX11GeometryShaderLoader()
        : ResourceLoader({ ".gs_hlsl" })
    {
    }

    Threading::TaskFuture<bool> DirectX11GeometryShaderLoader::Handle::create(const std::string &name, const CodeGen::ShaderFile &file, DirectX11GeometryShaderLoader *loader)
    {
        return Base::Handle::create(
            name, {}, [=, &file](DirectX11GeometryShaderLoader *loader, ReleasePtr<ID3D11GeometryShader> &shader, const DirectX11GeometryShaderLoader::ResourceDataInfo &info) { return loader->create(shader, info.resource(), file); }, loader);
    }

    bool DirectX11GeometryShaderLoader::loadImpl(ReleasePtr<ID3D11GeometryShader> &shader, ResourceDataInfo &info)
    {
        std::string source = info.resource()->readAsText();

        return loadFromSource(shader, info.resource()->path().stem(), source);
    }

    void DirectX11GeometryShaderLoader::unloadImpl(ReleasePtr<ID3D11GeometryShader> &shader)
    {
        shader.reset();
    }

    bool DirectX11GeometryShaderLoader::create(ReleasePtr<ID3D11GeometryShader> &shader, Resource *res, const CodeGen::ShaderFile &file)
    {
        if (res->path().empty()) {
            Filesystem::Path dir = Filesystem::appDataPath() / "generated/shader/directx11";

            Filesystem::createDirectories(dir);

            res->setPath(dir / (std::string { res->name() } + ".GS_hlsl"));
        }

        std::ostringstream ss;
        DirectX11ShaderCodeGen::generate(ss, file, 1);

        {
            std::ofstream f { res->path() };
            f << ss.str();
        }

        return loadFromSource(shader, res->name(), ss.str());
    }

    bool DirectX11GeometryShaderLoader::loadFromSource(ReleasePtr<ID3D11GeometryShader> &shader, std::string_view name, std::string source)
    {
        std::string profile = "latest";
        if (profile == "latest")
            profile = GetLatestGeometryProfile();

        std::map<std::string, size_t> files;

        CodeGen::resolveIncludes(
            source, [](const Filesystem::Path &path, size_t line, std::string_view filename) {
                Resources::ResourceBase *res = SlLoader::get(path.stem());
                return "#line 1 \"" + path.filename().str() + "\"\n" + res->readAsText() + "\n#line " + std::to_string(line + 1) + " \"" + std::string { filename } + "\"";
            },
            name, files);

        const char *cSource = source.data();

        ReleasePtr<ID3DBlob> pShaderBlob;
        ReleasePtr<ID3DBlob> pErrorBlob;

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
        flags |= D3DCOMPILE_DEBUG;
#endif

        HRESULT hr = D3DCompile(cSource, source.size(), name.data(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", profile.c_str(),
            flags, 0, &pShaderBlob, &pErrorBlob);

        if (FAILED(hr)) {
            LOG_ERROR("Loading of GeometryShader '" << name << "' failed:");
            if (pErrorBlob) {
                LOG_ERROR((char *)pErrorBlob->GetBufferPointer());
            }

            return false;
        }

        hr = sDevice->CreateGeometryShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &shader);
        DX11_CHECK(hr);

        return true;
    }

    Threading::TaskQueue *DirectX11GeometryShaderLoader::loadingTaskQueue() const
    {
        return DirectX11RenderContext::renderQueue();
    }

}
}

METATABLE_BEGIN(Engine::Render::DirectX11GeometryShaderLoader)
METATABLE_END(Engine::Render::DirectX11GeometryShaderLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX11GeometryShaderLoader::Resource, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::DirectX11GeometryShaderLoader::Resource)
