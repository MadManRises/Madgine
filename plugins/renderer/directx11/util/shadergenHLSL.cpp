#define NOMINMAX
#include <Windows.h>
#include <dxcapi.h>
#include <spirv_hlsl.hpp>

#include <vector>

#include <memory>

#include <assert.h>

#include <iostream>

#include <map>

#include <filesystem>

#include <fstream>

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

#define CHECK_HR(Operation)                                          \
    if (FAILED(hr)) {                                                \
        std::cerr << "Error in " #Operation ": " << hr << std::endl; \
        return -1;                                                   \
    }

int usage()
{
    std::cerr << "Usage: Shadergen <input-file> <output-folder>\n";
    return -1;
}

/* int generateSpirV(ReleasePtr<IDxcBlob> &out, std::filesystem::path inputFile)
{
    std::wstring shaderType = inputFile.extension().wstring().substr(1, 2);
    std::ranges::transform(shaderType, shaderType.begin(), static_cast<int (*)(int)>(std::tolower));
    if (shaderType == L"ps") {

    } else if (shaderType == L"vs") {

    } else if (shaderType == L"gs") {

    } else {
        std::cerr << "Unable to detect shader type!";
        return -1;
    }

    wchar_t cwd[1024];
    if (GetCurrentDirectoryW(1024, cwd) == 0)
        return GetLastError();

    if (inputFile.has_parent_path()) {
        SetCurrentDirectoryW(inputFile.parent_path().c_str());
    }

    ReleasePtr<IDxcLibrary> library;
    ReleasePtr<IDxcCompiler3> compiler;

    HRESULT hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
    CHECK_HR(DxcCreateInstance / Library);

    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
    CHECK_HR(DxcCreateInstance / Compiler);

    ReleasePtr<IDxcIncludeHandler> pIncludeHandler;
    hr = library->CreateIncludeHandler(&pIncludeHandler);
    CHECK_HR(CreateIncludeHandler);

    ReleasePtr<IDxcBlobEncoding> pSource;
    hr = library->CreateBlobFromFile(inputFile.filename().c_str(), nullptr, &pSource);
    CHECK_HR(CreateBlobFromFile);

    std::vector<LPCWSTR> arguments;

    arguments.push_back(inputFile.c_str());

    //-E for the entry point (eg. PSMain)
    arguments.push_back(L"-E");
    arguments.push_back(L"main");

    //-T for the target profile (eg. ps_6_2)
    shaderType += L"_6_2";
    arguments.push_back(L"-T");
    arguments.push_back(shaderType.c_str());

    arguments.push_back(L"-spirv");

    //arguments.push_back(L"-fspv-print-all");

    DxcBuffer sourceBuffer;
    sourceBuffer.Ptr = pSource->GetBufferPointer();
    sourceBuffer.Size = pSource->GetBufferSize();
    sourceBuffer.Encoding = 0;

    ReleasePtr<IDxcResult> pCompileResult;
    hr = compiler->Compile(&sourceBuffer, arguments.data(), arguments.size(), pIncludeHandler, IID_PPV_ARGS(&pCompileResult));
    CHECK_HR(Compile);

    //Error Handling
    ReleasePtr<IDxcBlobUtf8> pErrors;
    hr = pCompileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), nullptr);
    CHECK_HR(GetOutput);
    if (pErrors && pErrors->GetStringLength() > 0) {
        std::cerr << (char *)pErrors->GetBufferPointer() << std::endl;
    }

    if (inputFile.has_parent_path()) {
        SetCurrentDirectoryW(cwd);
    }

    if (!pCompileResult->HasOutput(DXC_OUT_OBJECT))
        return -1;

    hr = pCompileResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&out), nullptr);
    CHECK_HR(GetOutput);

    if (out->GetBufferSize() == 0)
        return -1;

    return 0;
}
*/
int transpileHLSL(std::string &out, const std::vector<unsigned char> &code)
{
    spirv_cross::CompilerHLSL hlsl { (uint32_t *)code.data(), code.size() / 4 };

    hlsl.set_hlsl_options(spirv_cross::CompilerHLSL::Options { .shader_model = 50, .flatten_matrix_vertex_input_semantics = true });

    //hlsl.build_dummy_sampler_for_combined_images();

    //hlsl.build_combined_image_samplers();

    out = hlsl.compile();

    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        return usage();
    }

    std::filesystem::path inputFile = argv[1];
    std::filesystem::path outputFolder = argv[2];

    if (!inputFile.has_filename()) {
        std::cerr << "Error: input path must be a file!\n";
        return -1;
    }

    std::cout << "Transpiling " << inputFile.filename() << " to HLSL..." << std::endl;

    std::ifstream ifs { inputFile, std::ios_base::binary };
    ifs >> std::noskipws;

    std::vector<unsigned char> code {
        std::istream_iterator<char> { ifs }, std::istream_iterator<char> {}
    };

    std::string shaderCode;

    try {
        int result = transpileHLSL(shaderCode, code);
        if (result != 0)
            return result;
    } catch (spirv_cross::CompilerError &error) {
        std::cerr << inputFile.string() << "(1,1): error: " << error.what()
                  << "\n";
        return -1;
    }

    std::string extension = inputFile.extension().string().substr(0, 3) + "_hlsl";

    std::filesystem::path outputFile = outputFolder / (inputFile.stem().string() + extension);

    std::ofstream of { outputFile };

    of << shaderCode << std::endl;

    std::cout << "Success!" << std::endl;

    return 0;
}