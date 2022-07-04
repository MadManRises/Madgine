#include "opengllib.h"

#include "openglshaderloader.h"

#include "util/openglshader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "openglshadercodegen.h"

#include "Interfaces/filesystem/api.h"

#include "Madgine/render/shadinglanguage/slloader.h"

#include "codegen/resolveincludes.h"

#include "openglrendercontext.h"

UNIQUECOMPONENT(Engine::Render::OpenGLShaderLoader);

METATABLE_BEGIN(Engine::Render::OpenGLShaderLoader)
METATABLE_END(Engine::Render::OpenGLShaderLoader)

METATABLE_BEGIN_BASE(Engine::Render::OpenGLShaderLoader::Resource, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::OpenGLShaderLoader::Resource)

namespace Engine {
namespace Render {

    Threading::TaskFuture<bool> OpenGLShaderLoader::Handle::create(std::string name, const CodeGen::ShaderFile &file, ShaderType type, OpenGLShaderLoader *loader)
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

        return Base::Handle::create(
            name, {}, [=, &file](OpenGLShaderLoader *loader, OpenGLShader &shader, OpenGLShaderLoader::ResourceDataInfo &info) { return loader->create(shader, info.resource(), file, type); }, loader);
    }

    Threading::TaskFuture<bool> OpenGLShaderLoader::Handle::load(std::string_view name, ShaderType type, OpenGLShaderLoader *loader)
    {
        std::string actualName { name };
        switch (type) {
        case ShaderType::PixelShader:
            actualName += "_PS";
            break;
        case ShaderType::VertexShader:
            actualName += "_VS";
            break;
        case ShaderType::GeometryShader:
            actualName += "_GS";
            break;
        default:
            throw 0;
        }

        return Base::Handle::load(actualName, loader);
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

        return loadFromSource(shader, filename, source, type, info.resource()->path());
    }

    void OpenGLShaderLoader::unloadImpl(OpenGLShader &shader)
    {
        shader.reset();
    }

    bool OpenGLShaderLoader::create(OpenGLShader &shader, Resource *res, const CodeGen::ShaderFile &file, ShaderType type)
    {
        if (res->path().empty()) {
            Filesystem::Path dir = Filesystem::appDataPath() / "generated/shader/opengl";

            Filesystem::createDirectories(dir);

            res->setPath(dir / (std::string { res->name() } + (type == VertexShader ? "_VS" : "_PS") + ".glsl"));
        }

        std::ostringstream ss;
        OpenGLShaderCodeGen::generate(ss, file, type);

        {
            std::ofstream f { res->path() };
            f << ss.str();
        }

        return loadFromSource(shader, res->name(), ss.str(), type, res->path());
    }

    bool OpenGLShaderLoader::loadFromSource(OpenGLShader &shader, std::string_view name, std::string source, ShaderType type, const Filesystem::Path &path)
    {
        OpenGLShader tempShader { type };

        GLuint handle = tempShader.mHandle;

        std::map<std::string, size_t> files {
            { std::string { path.filename() }, 0 }
        };

        CodeGen::resolveIncludes(
            source, [this, &files](const Filesystem::Path &path, size_t line, std::string_view filename) {
                Resources::ResourceBase *res;

                if (path.extension() == ".sl") {
                    res = SlLoader::get(path.stem());
                } else {
                    res = get(path.stem(), this);
                }

                return "#line 1 " + std::to_string(files.at(path)) + "\n" + res->readAsText() + "\n#line " + std::to_string(line + 1) + " " + std::to_string(files.at(std::string { filename }));
            },
            path.filename().str(), files);

        const char *cSource = source.data();

        glShaderSource(handle, 1, &cSource, NULL);
        glCompileShader(handle);
        // check for shader compile errors
        GLint success;
        char infoLog[512];
        glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(handle, 512, NULL, infoLog);
            Engine::Util::LogDummy { Engine::Util::MessageType::ERROR_TYPE, path.c_str() }
                << "Compilation of " << (type == VertexShader ? "VS" : "PS")
                << " Shader '" << name << "' failed:\n"
                << infoLog;
            return false;
        }

        shader = std::move(tempShader);

        return true;
    }

    Threading::TaskQueue *OpenGLShaderLoader::loadingTaskQueue() const
    {
        return OpenGLRenderContext::renderQueue();
    }

}
}
