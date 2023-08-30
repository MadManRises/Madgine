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

#include <sstream>

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

int transpileGLSL(const std::wstring &fileName, const std::wstring &outFolder, IDxcResult *result);
int transpileGLSLES(const std::wstring &fileName, const std::wstring &outFolder, IDxcResult *result);
int transpileHLSL(int apilevel, const std::wstring &fileName, const std::wstring &outFolder, IDxcResult *result, bool debug, const std::vector<LPCWSTR> &includes);
int transpileSPIRV(const std::wstring &fileName, const std::wstring &outFolder, std::vector<LPCWSTR> arguments, IDxcBlobEncoding *pSource);

int wmain(int argc, wchar_t **argv)
{
    if (argc < 4) {
        return usage();
    }

    std::wstring sourceFile = argv[1];
    std::wstring outputFile = argv[2];
    std::wstring dataFolder = argv[3];

    std::vector<LPCWSTR> includes;

    bool debug = false;
    for (int i = 4; i < argc; ++i) {
        if (wcscmp(argv[i], L"-g") == 0) {
            debug = true;
        } else if (wcscmp(argv[i], L"-I") == 0) {
            ++i;
            if (i == argc)
                return usage();
            includes.push_back(argv[i]);
        } else if (wcscmp(argv[i], L"-P") == 0) {
            int i;
            std::cin >> i;
        }
    }

    std::wstring extension = sourceFile.substr(sourceFile.rfind('.'));
    std::wstring profile;
    if (extension == L".PS_hlsl") {
        profile = L"ps_6_2";
    } else if (extension == L".VS_hlsl") {
        profile = L"vs_6_2";
    } else if (extension == L".GS_hlsl") {
        profile = L"gs_6_2";
    }

    HRESULT hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
    CHECK_HR(DxcCreateInstance / Library);

    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
    CHECK_HR(DxcCreateInstance / Compiler);

    hr = library->CreateIncludeHandler(&includeHandler);
    CHECK_HR(CreateIncludeHandler);

    ReleasePtr<IDxcBlobEncoding> pSource;
    hr = library->CreateBlobFromFile(sourceFile.c_str(), nullptr, &pSource);
    CHECK_HR(CreateBlobFromFile);

    std::vector<LPCWSTR> arguments;

    arguments.push_back(sourceFile.c_str());

    arguments.push_back(L"-spirv");

    arguments.push_back(L"-T");
    arguments.push_back(profile.c_str());

    arguments.push_back(L"-E");
    arguments.push_back(L"main");

    arguments.push_back(L"-Zpc");

    arguments.push_back(L"-HV");
    arguments.push_back(L"2021");

    for (LPCWSTR include : includes) {
        arguments.push_back(L"-I");
        arguments.push_back(include);
    }

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
        std::cerr << "Error compiling HLSL" << std::endl;
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
        if (wcscmp(argv[i], L"-HLSL11") == 0) {
            int hlsl_result = transpileHLSL(11, sourceFile, dataFolder, pCompileResult, debug, includes);
            if (hlsl_result != 0)
                result = hlsl_result;
        } else if (wcscmp(argv[i], L"-HLSL12") == 0) {
            int hlsl_result = transpileHLSL(12, sourceFile, dataFolder, pCompileResult, debug, includes);
            if (hlsl_result != 0)
                result = hlsl_result;
        } else if (wcscmp(argv[i], L"-GLSL") == 0) {
            int glsl_result = transpileGLSL(sourceFile, dataFolder, pCompileResult);
            if (glsl_result != 0)
                result = glsl_result;
        } else if (wcscmp(argv[i], L"-GLSLES") == 0) {
            int glsles_result = transpileGLSLES(sourceFile, dataFolder, pCompileResult);
            if (glsles_result != 0)
                result = glsles_result;
        } else if (wcscmp(argv[i], L"-SPIRV") == 0) {
            int spirv_result = transpileSPIRV(sourceFile, dataFolder, std::move(arguments), pSource);
            if (spirv_result != 0)
                result = spirv_result;
        } else if (wcscmp(argv[i], L"-I") == 0) {
            ++i;
        } else if (wcscmp(argv[i], L"-g") != 0 && wcscmp(argv[i], L"-P")) {
            std::wcerr << L"Unknown target language: " << argv[i] << std::endl;
            result = -1;
        }
    }

    return result;
}