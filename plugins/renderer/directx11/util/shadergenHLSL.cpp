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
    std::cerr << "Usage: Shadergen <source-file> <input-file> <output-folder> [-g]\n";
    return -1;
}

static std::map<std::string, uint32_t> sSemanticLocationMappings {
    { "POSITION0", 0 },
    { "POSITION1", 1 },
    { "POSITION2", 2 },
    { "NORMAL", 3 },
    { "COLOR", 4 },
    { "TEXCOORD", 5 },
    { "BONEINDICES", 6 },
    { "WEIGHTS", 7 },
    { "INSTANCEDATA", 8 },
    { "INSTANCEDATA1", std::numeric_limits<uint32_t>::max() }
};

int transpileHLSL(std::string &out, const std::vector<unsigned char> &code, const std::string &filename)
{
    try {
        spirv_cross::CompilerHLSL hlsl { (uint32_t *)code.data(), code.size() / 4 };

        hlsl.set_hlsl_options(spirv_cross::CompilerHLSL::Options { .shader_model = 50, .flatten_matrix_vertex_input_semantics = true });
        hlsl.set_common_options(spirv_cross::CompilerGLSL::Options { .relax_nan_checks = true });

        for (const spirv_cross::VariableID &id : hlsl.get_active_interface_variables()) {
            if (hlsl.get_storage_class(id) == spv::StorageClassInput && hlsl.get_execution_model() == spv::ExecutionModelVertex) {
                std::string name = hlsl.get_name(id);
                std::string semantic = name.substr(name.rfind('.') + 1);
                auto it = sSemanticLocationMappings.find(semantic);
                if (it != sSemanticLocationMappings.end()) {
                    uint32_t location = it->second;
                    if (location != std::numeric_limits<uint32_t>::max())
                        hlsl.set_decoration(id, spv::DecorationLocation, location);
                } else {
                    std::cerr << filename << "(1,1): warning : Unsupported semantic " << semantic << " used for " << name << std::endl;
                }
            }
        }

        //hlsl.build_dummy_sampler_for_combined_images();

        //hlsl.build_combined_image_samplers();

        out = hlsl.compile();

    } catch (spirv_cross::CompilerError &error) {
        std::cerr << filename << "(1,1): error: " << error.what()
                  << "\n";
        return -1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 4 && argc != 5) {
        return usage();
    }

    std::filesystem::path sourceFile = argv[1];
    std::filesystem::path inputFile = argv[2];
    std::filesystem::path outputFolder = argv[3];

    bool debug = false;
    if (argc == 5) {
        std::string debugFlag = argv[4];
        if (debugFlag != "-g")
            return usage();
        debug = true;
    }

    if (!inputFile.has_filename()) {
        std::cerr << "Error: input path must be a file!\n";
        return -1;
    }

    std::string extension = sourceFile.extension().string();

    std::string shaderCode;

    if (debug || extension == ".GS_hlsl") {
        std::string reason;
        if (debug) {
            reason = "for Debugging";
        } else {
            reason = "due to SpirV - Cross limitation";
        }
        std::cout << "Skipping HLSL, " << reason
                  << ", only preprocessing... ";

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
        hr = library->CreateBlobFromFile(sourceFile.c_str(), nullptr, &pSource);
        CHECK_HR(CreateBlobFromFile);

        std::vector<LPCWSTR> arguments;

        arguments.push_back(sourceFile.c_str());

        arguments.push_back(L"-P");

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
            return -1;
        }

        ReleasePtr<IDxcBlobEncoding> pPrecompiled;
        hr = pCompileResult->GetOutput(DXC_OUT_HLSL, IID_PPV_ARGS(&pPrecompiled), nullptr);

        shaderCode = { static_cast<char *>(pPrecompiled->GetBufferPointer()), pPrecompiled->GetBufferSize() };

    } else {

        std::cout << "HLSL..." << std::endl;

        std::ifstream ifs { inputFile, std::ios_base::binary };
        ifs >> std::noskipws;

        std::vector<unsigned char> code {
            std::istream_iterator<char> { ifs }, std::istream_iterator<char> {}
        };

        int result = transpileHLSL(shaderCode, code, inputFile.string());
        if (result != 0)
            return result;
    }

    std::filesystem::path outputFile = outputFolder / (inputFile.stem().string() + extension);

    std::ofstream of { outputFile };

    of << shaderCode << std::endl;

    std::cout << "Success!" << std::endl;

    return 0;
}