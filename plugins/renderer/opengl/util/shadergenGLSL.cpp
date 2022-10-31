#define NOMINMAX
#include <Windows.h>
#include <dxcapi.h>
#include <spirv_glsl.hpp>

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

int transpileGLSL(std::string &out, const std::vector<unsigned char> &code, const std::string &filename)
{
    try {
        spirv_cross::CompilerGLSL glsl { (uint32_t *)code.data(), code.size() / 4 };

        glsl.set_common_options(spirv_cross::CompilerGLSL::Options { .relax_nan_checks = true });

        spirv_cross::ShaderResources resources = glsl.get_shader_resources();

        std::map<spirv_cross::ID, std::pair<std::string, uint32_t>> imageData;

        for (auto &resource : resources.separate_images) {
            imageData[resource.id] = { glsl.get_name(resource.id), glsl.get_decoration(resource.id, spv::DecorationBinding) };
        }

        glsl.build_dummy_sampler_for_combined_images();

        glsl.build_combined_image_samplers();

        auto &mappings = glsl.get_combined_image_samplers();
        std::map<spirv_cross::ID, spirv_cross::ID> map;
        for (auto &mapping : mappings) {
            map[mapping.combined_id] = mapping.image_id;
        }

        resources = glsl.get_shader_resources();

        for (auto &resource : resources.sampled_images) {
            auto &data = imageData[map[resource.id]];
            glsl.set_name(resource.id, data.first);
            glsl.set_decoration(resource.id, spv::DecorationBinding, data.second);
        }

        for (const spirv_cross::VariableID &id : glsl.get_active_interface_variables()) {
            if (glsl.get_storage_class(id) == spv::StorageClassInput && glsl.get_execution_model() == spv::ExecutionModelVertex) {
                std::string name = glsl.get_name(id);
                std::string semantic = name.substr(name.rfind('.') + 1);
                auto it = sSemanticLocationMappings.find(semantic);
                if (it != sSemanticLocationMappings.end()) {
                    uint32_t location = it->second;
                    if (location != std::numeric_limits<uint32_t>::max())
                        glsl.set_decoration(id, spv::DecorationLocation, location);
                } else {
                    std::cerr << filename << "(1,1): warning : Unsupported semantic " << semantic << " used for " << name << std::endl;
                }
            }
        }

        out = glsl.compile();
    } catch (spirv_cross::CompilerError &error) {
        std::cout << std::endl;
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

    if (!inputFile.has_filename()) {
        std::cerr << "Error: input path must be a file!\n";
        return -1;
    }

    bool debug = false;
    if (argc == 5) {
        std::string debugFlag = argv[4];
        if (debugFlag != "-g")
            return usage();
        debug = true;
    }

    std::cout << "GLSL... ";

    std::ifstream ifs { inputFile, std::ios_base::binary };
    ifs >> std::noskipws;

    std::vector<unsigned char> code {
        std::istream_iterator<char> { ifs }, std::istream_iterator<char> {}
    };

    std::string shaderCode;

    int result = transpileGLSL(shaderCode, code, inputFile.string());
    if (result != 0)
        return result;

    std::string extension = "_" + inputFile.extension().string().substr(1, 2) + ".glsl";

    std::filesystem::path outputFile = outputFolder / (inputFile.stem().string() + extension);

    std::ofstream of { outputFile };

    of << shaderCode << std::endl;

    std::cout << "Success!" << std::endl;

    return 0;
}