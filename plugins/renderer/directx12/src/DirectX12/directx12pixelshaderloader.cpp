#include "directx12lib.h"

#include "directx12pixelshaderloader.h"

#include "util/directx12pixelshader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "directx12shadercodegen.h"

#include "Interfaces/filesystem/api.h"

#include "codegen/resolveincludes.h"

#include "Madgine/render/shadinglanguage/slloader.h"

UNIQUECOMPONENT(Engine::Render::DirectX12PixelShaderLoader);

namespace Engine {
namespace Render {

    std::string GetLatestPixelProfile()
    {
        return "ps_5_1";
    }

    DirectX12PixelShaderLoader::DirectX12PixelShaderLoader()
        : ResourceLoader({ ".PS_hlsl" })
    {
    }

    void DirectX12PixelShaderLoader::HandleType::create(const std::string &name, const CodeGen::ShaderFile &file, DirectX12PixelShaderLoader *loader)
    {
        *this = DirectX12PixelShaderLoader::loadManual(
            name, {}, [=, &file](DirectX12PixelShaderLoader *loader, DirectX12PixelShader &shader, const DirectX12PixelShaderLoader::ResourceDataInfo &info) { return loader->create(shader, info.resource(), file); }, loader);
    }

    bool DirectX12PixelShaderLoader::loadImpl(DirectX12PixelShader &shader, ResourceDataInfo &info)
    {
        std::string source = info.resource()->readAsText();

        return loadFromSource(shader, info.resource()->path().stem(), source);
    }

    void DirectX12PixelShaderLoader::unloadImpl(DirectX12PixelShader &shader)
    {
        shader.reset();
    }

    bool DirectX12PixelShaderLoader::create(DirectX12PixelShader &shader, ResourceType *res, const CodeGen::ShaderFile &file)
    {
        if (res->path().empty()) {
            Filesystem::Path dir = Filesystem::appDataPath() / "generated/shader/directx12";

            Filesystem::createDirectories(dir);

            res->setPath(dir / (std::string { res->name() } + ".PS_hlsl"));
        }

        std::stringstream ss;
        DirectX12ShaderCodeGen::generate(ss, file, 1);

        {
            std::ofstream f { res->path() };
            f << ss.str();
        }

        return loadFromSource(shader, res->name(), ss.str());
    }

    bool DirectX12PixelShaderLoader::loadFromSource(DirectX12PixelShader &shader, std::string_view name, std::string source)
    {
        std::string profile = "latest";
        if (profile == "latest")
            profile = GetLatestPixelProfile();

        std::map<std::string, size_t> files;

        CodeGen::resolveIncludes(
            source, [](const Filesystem::Path &path, size_t line, std::string_view filename) {
                Resources::ResourceBase *res = SlLoader::get(path.stem());
                return "#line 1 \"" + path.filename().str() + "\"\n" + res->readAsText() + "\n#line " + std::to_string(line + 1) + " \"" + std::string { filename } + "\"";
            },
            name, files);

        const char *cSource = source.data();

        ID3DBlob *pShaderBlob = nullptr;
        ID3DBlob *pErrorBlob = nullptr;

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
        flags |= D3DCOMPILE_DEBUG;
#endif

        HRESULT hr = D3DCompile(cSource, source.size(), name.data(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", profile.c_str(),
            flags, 0, &pShaderBlob, &pErrorBlob);

        if (FAILED(hr)) {
            LOG_ERROR("Loading of Shader '" << name << "' failed:");
            if (pErrorBlob) {
                LOG_ERROR((char *)pErrorBlob->GetBufferPointer());

                if (pShaderBlob) {
                    pShaderBlob->Release();
                    pShaderBlob = nullptr;
                }
                if (pErrorBlob) {
                    pErrorBlob->Release();
                    pErrorBlob = nullptr;
                }
            }

            return false;
        }

        shader = { pShaderBlob };

        if (pErrorBlob) {
            pErrorBlob->Release();
            pErrorBlob = nullptr;
        }

        return true;
    }

}
}

METATABLE_BEGIN(Engine::Render::DirectX12PixelShaderLoader)
METATABLE_END(Engine::Render::DirectX12PixelShaderLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX12PixelShaderLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::DirectX12PixelShaderLoader::ResourceType)
