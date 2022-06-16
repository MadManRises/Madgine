#include "directx12lib.h"

#include "directx12vertexshaderloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "directx12shadercodegen.h"

#include "Interfaces/filesystem/api.h"

#include "codegen/resolveincludes.h"

#include "Madgine/render/shadinglanguage/slloader.h"

#include "directx12rendercontext.h"

#include "directx12vertexshadersourceloader.h"

UNIQUECOMPONENT(Engine::Render::DirectX12VertexShaderLoader);

METATABLE_BEGIN(Engine::Render::DirectX12VertexShaderLoader)
METATABLE_END(Engine::Render::DirectX12VertexShaderLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX12VertexShaderLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::DirectX12VertexShaderLoader::ResourceType)

namespace Engine {
namespace Render {

    Threading::TaskFuture<bool> DirectX12VertexShaderLoader::HandleType::load(std::string_view name, VertexFormat format)
    {
        char buffer[256];
#if WINDOWS
        sprintf_s(buffer, "%s_%hu", name.data(), format);
#else
        sprintf(buffer, "%s_%hu", name.data(), format);
#endif

        *this = loadManual(buffer);
        ResourceDataInfo *i = info();
        if (!i)
            return false;
        return i->loadingTask();
    }

    static constexpr const wchar_t *vMacros[] = {
        L"HAS_POS_3D",
        L"HAS_POS_4D",
        L"HAS_POS2",
        L"HAS_NORMAL",
        L"HAS_COLOR",
        L"HAS_UV",
        L"HAS_BONE_INDICES",
        L"HAS_BONE_WEIGHTS"
    };

    std::wstring GetLatestVertexProfile()
    {
        return L"vs_6_0";
    }

    DirectX12VertexShaderLoader::DirectX12VertexShaderLoader()
        : ResourceLoader(std::vector<std::string> {})
    {
        HRESULT hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&mLibrary));
        //if(FAILED(hr)) Handle error...

        hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&mCompiler));
        //if(FAILED(hr)) Handle error
    }

    /* void DirectX12PixelShaderLoader::HandleType::create(const std::string &name, const CodeGen::ShaderFile &file, DirectX12PixelShaderLoader *loader)
    {
        *this = DirectX12PixelShaderLoader::loadManual(
            name, {}, [=, &file](DirectX12PixelShaderLoader *loader, DirectX12PixelShader &shader, const DirectX12PixelShaderLoader::ResourceDataInfo &info) { return loader->create(shader, info.resource(), file); }, loader);
    }*/

    Threading::Task<bool> DirectX12VertexShaderLoader::loadImpl(ReleasePtr<IDxcBlob> &shader, ResourceDataInfo &info)
    {
        if (info.resource()->path().empty()) {
            std::string_view fullName = info.resource()->name();
            auto pos = fullName.rfind('_');
            std::string_view name = fullName.substr(0, pos);
            VertexFormat format;
            std::from_chars(fullName.data() + pos + 1, fullName.data() + fullName.size(), reinterpret_cast<uint16_t &>(format));

            DirectX12VertexShaderSourceLoader::HandleType source;
            if (!co_await source.load(name))
                co_return false;

            co_return loadFromSource(shader, name, *source, format);
        } else {
            throw 0;
        }
    }

    void DirectX12VertexShaderLoader::unloadImpl(ReleasePtr<IDxcBlob> &shader)
    {
        shader.reset();
    }

    /* bool DirectX12PixelShaderLoader::create(DirectX12PixelShader &shader, ResourceType *res, const CodeGen::ShaderFile &file)
    {
        if (res->path().empty()) {
            Filesystem::Path dir = Filesystem::appDataPath() / "generated/shader/directx12";

            Filesystem::createDirectories(dir);

            res->setPath(dir / (std::string { res->name() } + ".PS_hlsl"));
        }

        std::ostringstream ss;
        DirectX12ShaderCodeGen::generate(ss, file, 1);

        {
            std::ofstream f { res->path() };
            f << ss.str();
        }

        return loadFromSource(shader, res->name(), ss.str());
    }*/

    bool DirectX12VertexShaderLoader::loadFromSource(ReleasePtr<IDxcBlob> &shader, std::string_view name, std::string source, VertexFormat format)
    {
        std::wstring profile = L"latest";
        if (profile == L"latest")
            profile = GetLatestVertexProfile();

        std::map<std::string, size_t> files;

        CodeGen::resolveIncludes(
            source, [](const Filesystem::Path &path, size_t line, std::string_view filename) {
                Resources::ResourceBase *res = SlLoader::get(path.stem());
                return "#line 1 \"" + path.filename().str() + "\"\n" + res->readAsText() + "\n#line " + std::to_string(line + 1) + " \"" + std::string { filename } + "\"";
            },
            name, files);

        DxcBuffer sourceBuffer;
        sourceBuffer.Ptr = source.c_str();
        sourceBuffer.Size = source.size();
        sourceBuffer.Encoding = CP_UTF8;

        std::vector<LPCWSTR> arguments;
        arguments.push_back(L"-E");
        arguments.push_back(L"main");

        arguments.push_back(L"-T");
        arguments.push_back(profile.c_str());

        for (size_t i = 0; i < VertexElements::size; ++i) {
            if (format.has(i)) {
                arguments.push_back(L"-D");
                arguments.push_back(vMacros[i]);
            }
        }

        ReleasePtr<IDxcResult> result;
        HRESULT hr = mCompiler->Compile(
            &sourceBuffer, // pSource
            arguments.data(), // pSourceName
            arguments.size(), // pEntryPoint
            nullptr,
            IID_PPV_ARGS(&result)); // ppResult
        if (SUCCEEDED(hr))
            result->GetStatus(&hr);
        if (FAILED(hr)) {
            LOG_ERROR("Loading of Shader '" << name << "' failed:");

            if (result) {
                ReleasePtr<IDxcBlobUtf8> pErrorBlob;
                hr = result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrorBlob), nullptr);
                if (SUCCEEDED(hr) && pErrorBlob) {
                    LOG_ERROR((char *)pErrorBlob->GetBufferPointer());
                }
            }
            return false;
        }
        result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shader), nullptr);

        return true;
    }

    Threading::TaskQueue *DirectX12VertexShaderLoader::loadingTaskQueue() const
    {
        return DirectX12RenderContext::renderQueue();
    }
}
}
