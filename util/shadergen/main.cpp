#define NOMINMAX
#ifdef _WIN32
#    include <Windows.h>
#endif
#include "dxc/dxcapi.h"

#include <vector>

#include <memory>

#include <assert.h>

#include <iostream>

#include <map>

#include <fstream>

#include <codecvt>

struct ReleaseDeleter {
    template <typename T>
    void operator()(T *ptr)
    {
        ptr->Release();
    }
};

template <typename T>
struct ReleasePtr : std::unique_ptr<T, ReleaseDeleter> {
    using std::unique_ptr<T, ReleaseDeleter>::unique_ptr;

    T **operator&()
    {
        assert(!*this);
        return reinterpret_cast<T **>(this);
    }

    T *const *operator&() const
    {
        return reinterpret_cast<T *const *>(this);
    }

    operator T *() const
    {
        return this->get();
    }
};

ReleasePtr<IDxcLibrary> library;
ReleasePtr<IDxcCompiler3> compiler;
ReleasePtr<IDxcIncludeHandler> includeHandler;

#define CHECK_HR(Operation)                                          \
    if (FAILED(hr)) {                                                \
        std::cerr << "Error in " #Operation ": " << hr << std::endl; \
        return -1;                                                   \
    }

int usage()
{
    std::cerr << "Usage: ShaderGen <source-file> <output-file> <data-folder> [-g] Targets...\n";
    return -1;
}

int transpileGLSL(const std::string &fileName, const std::string &outFolder, IDxcResult *result);
int transpileGLSLES(const std::string &fileName, const std::string &outFolder, IDxcResult *result);
int transpileHLSL(const std::string &fileName, const std::string &outFolder, IDxcResult *result, bool debug);
int transpileSPIRV(const std::string &fileName, const std::string &outFolder, std::vector<LPCWSTR> arguments, IDxcBlobEncoding *pSource);

int main(int argc, char **argv)
{
    if (argc < 4) {
        return usage();
    }

    std::string sourceFile = argv[1];
    std::string outputFile = argv[2];
    std::string dataFolder = argv[3];

    bool debug = false;
    for (int i = 4; i < argc; ++i) {
        if (strcmp(argv[i], "-g") == 0) {
            debug = true;
        }
    }

    std::string extension = sourceFile.substr(sourceFile.rfind('.'));
    std::wstring profile;
    if (extension == ".PS_hlsl") {
        profile = L"ps_6_2";
    } else if (extension == ".VS_hlsl") {
        profile = L"vs_6_2";
    } else if (extension == ".GS_hlsl") {
        profile = L"gs_6_2";
    }

    HRESULT hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
    CHECK_HR(DxcCreateInstance / Library);

    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
    CHECK_HR(DxcCreateInstance / Compiler);

    hr = library->CreateIncludeHandler(&includeHandler);
    CHECK_HR(CreateIncludeHandler);

    std::wstring wSource = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> {}.from_bytes(sourceFile);

    ReleasePtr<IDxcBlobEncoding> pSource;
    hr = library->CreateBlobFromFile(wSource.c_str(), nullptr, &pSource);
    CHECK_HR(CreateBlobFromFile);

    std::vector<LPCWSTR> arguments;

    arguments.push_back(wSource.c_str());

    arguments.push_back(L"-spirv");

    arguments.push_back(L"-T");
    arguments.push_back(profile.c_str());

    arguments.push_back(L"-E");
    arguments.push_back(L"main");

    arguments.push_back(L"-Zpc");

    DxcBuffer sourceBuffer;
    sourceBuffer.Ptr = pSource->GetBufferPointer();
    sourceBuffer.Size = pSource->GetBufferSize();
    sourceBuffer.Encoding = 0;

    ReleasePtr<IDxcResult> pCompileResult;
    hr = compiler->Compile(&sourceBuffer, arguments.data(), arguments.size(), includeHandler, IID_PPV_ARGS(&pCompileResult));
    CHECK_HR(Compile);

    //Error Handling
    ReleasePtr<IDxcBlobUtf8> pErrors;
    hr = pCompileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), nullptr);
    CHECK_HR(GetOutput / Errors);
    if (pErrors && pErrors->GetStringLength() > 0) {
        std::cerr << (char *)pErrors->GetBufferPointer() << std::endl;
        return -1;
    }

    ReleasePtr<IDxcBlob> pSpirv;
    hr = pCompileResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pSpirv), nullptr);
    CHECK_HR(GetOutput / Spirv)

    std::ofstream of { outputFile, std::ios::binary };
    of.write(static_cast<char *>(pSpirv->GetBufferPointer()), pSpirv->GetBufferSize());

    int result = 0;

    for (int i = 4; i < argc; ++i) {
        if (strcmp(argv[i], "-HLSL") == 0) {
            int hlsl_result = transpileHLSL(sourceFile, dataFolder, pCompileResult, debug);
            if (hlsl_result != 0)
                result = hlsl_result;
        } else if (strcmp(argv[i], "-GLSL") == 0) {
            int glsl_result = transpileGLSL(sourceFile, dataFolder, pCompileResult);
            if (glsl_result != 0)
                result = glsl_result;
        } else if (strcmp(argv[i], "-GLSLES") == 0) {
            int glsles_result = transpileGLSLES(sourceFile, dataFolder, pCompileResult);
            if (glsles_result != 0)
                result = glsles_result;
        } else if (strcmp(argv[i], "-SPIRV") == 0) {
            int spirv_result = transpileSPIRV(sourceFile, dataFolder, std::move(arguments), pSource);
            if (spirv_result != 0)
                result = spirv_result;
        } else if (strcmp(argv[i], "-g") != 0) {
            std::cerr << "Unknown target language: " << argv[i] << std::endl;
            result = -1;
        }
    }

    return result;
}