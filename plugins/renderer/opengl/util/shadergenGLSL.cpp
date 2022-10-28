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
    std::cerr << "Usage: Shadergen <input-file> <output-folder>\n";
    return -1;
}

int transpileGLSL(std::string &out, const std::vector<unsigned char> &code)
{
    spirv_cross::CompilerGLSL glsl { (uint32_t *)code.data(), code.size() / 4 };    

    spirv_cross::ShaderResources resources = glsl.get_shader_resources();

    std::map<spirv_cross::ID, std::pair<std::string, uint32_t>> imageData;

    for (auto& resource : resources.separate_images) {
        imageData[resource.id] = { glsl.get_name(resource.id), glsl.get_decoration(resource.id, spv::DecorationBinding) };
    }

    glsl.build_dummy_sampler_for_combined_images();

    glsl.build_combined_image_samplers();

    auto &mappings = glsl.get_combined_image_samplers();
    std::map<spirv_cross::ID, spirv_cross::ID> map;
    for (auto& mapping : mappings) {
        map[mapping.combined_id] = mapping.image_id;
    }

    resources = glsl.get_shader_resources();

    for (auto &resource : resources.sampled_images) {
        auto &data = imageData[map[resource.id]];
        glsl.set_name(resource.id, data.first);
        glsl.set_decoration(resource.id, spv::DecorationBinding, data.second);
    }

    out = glsl.compile();

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

    std::cout << "Transpiling " << inputFile.filename() << " to GLSL..." << std::endl;

    std::ifstream ifs { inputFile, std::ios_base::binary };
    ifs >> std::noskipws;

    std::vector<unsigned char> code {
        std::istream_iterator<char> { ifs }, std::istream_iterator<char> {}
    };

    std::string shaderCode;

    try {
        int result = transpileGLSL(shaderCode, code);
        if (result != 0)
            return result;
    } catch (spirv_cross::CompilerError &error) {
        std::cerr << inputFile.string() << "(1,1): error: " << error.what()
                  << "\n";
        return -1;
    }

    std::string extension = "_" + inputFile.extension().string().substr(1, 2) + ".glsl";

    std::filesystem::path outputFile = outputFolder / (inputFile.stem().string() + extension);

    std::ofstream of { outputFile };

    of << shaderCode << std::endl;

    std::cout << "Success!" << std::endl;

    return 0;
}