#include "opengllib.h"

#include "openglshaderloader.h"

#include "Generic/stringutil.h"

#include "util/openglshader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "openglshadercodegen.h"

#include "Interfaces/filesystem/api.h"

#include "Madgine/render/shadinglanguage/slloader.h"

#include "codegen/resolveincludes.h"

UNIQUECOMPONENT(Engine::Render::OpenGLShaderLoader);

METATABLE_BEGIN(Engine::Render::OpenGLShaderLoader)
METATABLE_END(Engine::Render::OpenGLShaderLoader)

METATABLE_BEGIN_BASE(Engine::Render::OpenGLShaderLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::OpenGLShaderLoader::ResourceType)

namespace Engine {
namespace Render {

    void OpenGLShaderLoader::HandleType::create(std::string name, const CodeGen::ShaderFile &file, ShaderType type, OpenGLShaderLoader *loader)
    {
        if (name != Resources::ResourceBase::sUnnamed) {
            switch (type) {
            case ShaderType::PixelShader:
                name += "_PS";
                break;
            case ShaderType::VertexShader:
                name += "_VS";
                break;
            default:
                throw 0;
            }
        }

        *this = OpenGLShaderLoader::loadManual(
            name, {}, [=, &file](OpenGLShaderLoader *loader, OpenGLShader &shader, OpenGLShaderLoader::ResourceDataInfo &info) mutable { return loader->create(shader, info.resource(), file, type); }, {}, loader);
    }

    void OpenGLShaderLoader::HandleType::load(std::string name, ShaderType type, OpenGLShaderLoader *loader)
    {
        switch (type) {
        case ShaderType::PixelShader:
            name += "_PS";
            break;
        case ShaderType::VertexShader:
            name += "_VS";
            break;
        case ShaderType::GeometryShader:
            name += "_GS";
            break;
        default:
            throw 0;
        }

        *this = OpenGLShaderLoader::load(name, loader);
    }

    OpenGLShaderLoader::OpenGLShaderLoader()
        : ResourceLoader({
#if !OPENGL_ES
            ".glsl"
#else
            ".glsl_es"
#endif
        })
    {
    }

    bool OpenGLShaderLoader::loadImpl(OpenGLShader &shader, ResourceDataInfo &info)
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

        std::string source = info.resource()->readAsText();

        return loadFromSource(shader, filename, source, type);
    }

    void OpenGLShaderLoader::unloadImpl(OpenGLShader &shader, ResourceDataInfo &info)
    {
        shader.reset();
    }

    bool OpenGLShaderLoader::create(OpenGLShader &shader, ResourceType *res, const CodeGen::ShaderFile &file, ShaderType type)
    {
        if (res->path().empty()) {
            Filesystem::Path dir = Filesystem::appDataPath() / "generated/shader/opengl";

            Filesystem::createDirectories(dir);

            res->setPath(dir / (std::string { res->name() } + (type == VertexShader ? "_VS" : "_PS") + ".glsl"));
        }

        std::stringstream ss;
        OpenGLShaderCodeGen::generate(ss, file, type);

        {
            std::ofstream f { res->path() };
            f << ss.str();
        }

        return loadFromSource(shader, res->name(), ss.str(), type);
    }

    bool OpenGLShaderLoader::loadFromSource(OpenGLShader &shader, std::string_view name, std::string source, ShaderType type)
    {
        OpenGLShader tempShader { type };

        GLuint handle = tempShader.mHandle;

        std::set<std::string> files;

        CodeGen::resolveIncludes(source, [this, &files](const Filesystem::Path &path, size_t line, std::string_view filename) {

            Resources::ResourceBase *res;

            if (path.extension() == ".sl") {
                res = SlLoader::get(path.stem());
            } else {
                res = get(path.stem(), this);
            }

            return "#line 1 " + std::to_string(files.size()) + "\n" + res->readAsText() + "\n#line " + std::to_string(line + 1) + " " + std::to_string(files.size());
        }, name, files);

        
        const char *cSource
            = source.data();

        glShaderSource(handle, 1, &cSource, NULL);
        const GLchar *root = "/";
        glCompileShaderIncludeARB(handle, 1, &root, nullptr);
        // check for shader compile errors
        GLint success;
        char infoLog[512];
        glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(handle, 512, NULL, infoLog);
            LOG_ERROR("Loading of " << (type == VertexShader ? "VS" : "PS") << " Shader '" << name << "' failed:");
            LOG_ERROR(infoLog);
            return false;
        }

        shader = std::move(tempShader);

        return true;
    }

}
}
