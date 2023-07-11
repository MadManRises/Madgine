#include "directx11lib.h"

#include "directx11shadercodegen.h"

#include "Madgine/codegen/codegen_shader.h"

#include "Meta/math/vector4.h"

namespace Engine {
namespace Render {
    namespace DirectX11ShaderCodeGen {

        static std::map<ValueTypeDesc, std::string_view> sTypeMap {
            { toValueTypeDesc<Vector4>(), "float4" },
            { toValueTypeDesc<Vector3>(), "float3" },
            { toValueTypeDesc<Vector2>(), "float2" },
            { toValueTypeDesc<float>(), "float" },
            { toValueTypeDesc<Vector4i>(), "int4" },
            { toValueTypeDesc<Vector3i>(), "int3" },
            { toValueTypeDesc<Vector2i>(), "int2" },
            { toValueTypeDesc<Matrix4>(), "float4x4" }
        };

        void generateType(std::ostream &stream, const CodeGen::Type &type)
        {
            std::visit(overloaded {
                           [&](const ValueTypeDesc &type) {
                               stream << sTypeMap.at(type);
                           },
                           [&](CodeGen::Struct *structInfo) {
                               stream << structInfo->mName;
                           } },
                type.mBaseType);
        }

        void generate(std::ostream &stream, const CodeGen::Statement &statement);

        void generate(std::ostream &stream, const Vector4 &v)
        {
            stream << "float4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
        }

        void generate(std::ostream &stream, const Vector3 &v)
        {
            stream << "float3(" << v.x << ", " << v.y << ", " << v.z << ")";
        }
                
        void generate(std::ostream &stream, const Vector2 &v)
        {
            stream << "float2(" << v.x << ", " << v.y << ")";
        }

        void generate(std::ostream &stream, const Vector2i &v)
        {
            stream << "int2(" << v.x << ", " << v.y << ")";
        }

        void generate(std::ostream &stream, const Vector4i &v)
        {
            stream << "int4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
        }
                
        void generate(std::ostream &stream, const Quaternion &q)
        {
            throw 0;
        }

        void generate(std::ostream &stream, const CoW<Matrix3> &m)
        {
            throw 0;
        }

        void generate(std::ostream &stream, const CoW<Matrix4> &m)
        {
            throw 0;
        }

        void generate(std::ostream &stream, const Void &v)
        {
            throw 0;
        }

        void generate(std::ostream &stream, const std::string_view &v)
        {
            stream << "\"" << v << "\"";
        }

        void generate(std::ostream &stream, const float &f)
        {
            stream << f;
        }

        void generate(std::ostream &stream, const EnumHolder &e)
        {
            stream << e.toString();
        }

        void generate(std::ostream &stream, const ObjectPtr &o)
        {
            throw 0;
        }

        void generate(std::ostream &stream, const OwnedScopePtr &o)
        {
            throw 0;
        }

        void generate(std::ostream &stream, const TypedScopePtr &t)
        {
            throw 0;
        }

        void generate(std::ostream &stream, const KeyValueFunction &f)
        {
            throw 0;
        }

        void generate(std::ostream &stream, const BoundApiFunction &f)
        {
            throw 0;
        }

        void generate(std::ostream &stream, const ApiFunction &f)
        {
            throw 0;
        }

        void generate(std::ostream &stream, const KeyValueVirtualSequenceRange &c)
        {
            throw 0;
        }

        void generate(std::ostream &stream, const KeyValueVirtualAssociativeRange &c)
        {
            throw 0;
        }

        void generate(std::ostream &stream, const CodeGen::Assignment &statement)
        {
            generate(stream, statement.mTarget);
            stream << " = ";
            generate(stream, statement.mStatement);
        }

        void generate(std::ostream &stream, const CodeGen::Return &statement)
        {
            stream << "return ";
            generate(stream, statement.mStatement);
        }

        void generate(std::ostream &stream, const CodeGen::VariableAccess &statement)
        {
            stream << statement.mVariableName;
        }

        void generate(std::ostream &stream, const CodeGen::VariableDefinition &def)
        {
            generateType(stream, def.mVariable.mType);
            stream << " " << def.mVariable.mName;
            if (def.mInitializer) {
                stream << " = ";
                generate(stream, *def.mInitializer);
            }
        }

        void generate(std::ostream &stream, const CodeGen::MemberAccess &access)
        {
            generate(stream, access.mStatement);
            stream << "." << access.mMemberName;
        }

        void generate(std::ostream &stream, const CodeGen::Namespace &ns)
        {
            throw 0;
        }

        void generate(std::ostream &stream, const CodeGen::CustomCodeBlock &block)
        {
            throw 0;
        }

        void generate(std::ostream &stream, const CodeGen::ArithOperation &op)
        {
            const char *opCode;
            switch (op.mType) {
            case CodeGen::ArithOperation::ADD:
                opCode = " + ";
                break;
            case CodeGen::ArithOperation::MUL:
                stream << "mul(";
                opCode = ", ";
                break;
            case CodeGen::ArithOperation::DIV:
                opCode = " / ";
                break;
            default:
                throw 0;
            }

            bool first = true;
            for (const CodeGen::Statement &statement : op.mOperands) {
                if (first)
                    first = false;
                else
                    stream << opCode;
                generate(stream, statement);
            }

            switch (op.mType) {
            case CodeGen::ArithOperation::MUL:
                stream << ")";
                break;
            default:
                break;
            }
        }

        void generate(std::ostream &stream, const CodeGen::Constructor &ctor)
        {
            generateType(stream, ctor.mType);
            stream << "(";
            bool first = true;
            for (const CodeGen::Statement &statement : ctor.mArguments) {
                if (first)
                    first = false;
                else
                    stream << ",";
                generate(stream, statement);
            }
            stream << ")";
        }

        void generate(std::ostream &stream, const CodeGen::Constant &c)
        {
            c.mValue.visit([&](const auto &part) { generate(stream, part); });
        }

        void generate(std::ostream &stream, const CodeGen::Statement &statement)
        {
            std::visit([&](const auto &part) { generate(stream, part); }, statement);
        }

        const char *guessSemantic(std::string_view name, bool rasterized)
        {
            if (name.find("pos2") != std::string_view::npos)
                return "POSITION2";
            else if (name == "w")
                return "POSITION1";
            else if (name.find("pos") != std::string_view::npos)
                return rasterized ? "SV_Position" : "POSITION";
            else if (name.find("col") != std::string_view::npos)
                return "COLOR";
            else if (StringUtil::toLower(name).find("uv") != std::string::npos)
                return "TEXCOORD";
            else if (StringUtil::toLower(name).find("normal") != std::string::npos)
                return "NORMAL";
            else if (StringUtil::toLower(name).find("bone") != std::string::npos)
                return "BONEINDICES";
            else if (StringUtil::toLower(name).find("weights") != std::string::npos)
                return "WEIGHTS";
            else
                throw 0;
        }

        void generate(std::ostream &stream, const CodeGen::ShaderFile &file, uint32_t index)
        {
            for (const std::pair<const std::string, CodeGen::Struct> &structInfo : file.mStructs) {

                if (structInfo.first == "VertexData") {
                    if (index == 0) {
                        stream << "struct VertexDataIn {\n";
                        for (const CodeGen::VariableDefinition &arg : structInfo.second.mVariables) {
                            stream << "\t";
                            generateType(stream, arg.mVariable.mType);
                            stream << " " << arg.mVariable.mName << " : " << guessSemantic(arg.mVariable.mName, false) << ";\n";                            
                        }
                        stream << "};\n";
                    } else {
                        continue;
                    }
                }

                bool isBuffer = !structInfo.second.mAnnotations.empty() && StringUtil::startsWith(structInfo.second.mAnnotations.front(), "buffer");
                if (isBuffer) {
                    std::string_view number = std::string_view { structInfo.second.mAnnotations.front() }.substr(strlen("buffer"));
                    uint32_t bufferIndex = std::atoi(number.data());
                    stream << "cbuffer " << structInfo.second.mName << " : register(b" << bufferIndex << ")";
                } else
                    stream << "struct " << structInfo.second.mName;
                stream << "{\n";

                for (const CodeGen::VariableDefinition &arg : structInfo.second.mVariables) {
                    stream << "\t";
                    for (const std::string &annotation : arg.mVariable.mAnnotations) {

                        stream << annotation << " ";
                    }
                    generateType(stream, arg.mVariable.mType);
                    stream << " " << arg.mVariable.mName;
                    if (structInfo.second.mName == "RasterizerData" && !structInfo.second.mAnnotations.empty() && structInfo.second.mAnnotations.front() == "semantic")
                        stream << " : " << guessSemantic(arg.mVariable.mName, true);
                    stream << ";\n";
                }
                stream << "};\n\n";
            }

            const std::vector<CodeGen::Function> &functions = *std::next(file.mInstances.begin(), index);
            for (const CodeGen::Function &function : functions) {
                std::string_view name = function.mName == "main" ? "mainImpl" : std::string_view { function.mName };
                generateType(stream, function.mReturnType);
                stream << " " << name << "(";
                bool first = true;
                for (const CodeGen::Variable &arg : function.mArguments) {
                    generateType(stream, arg.mType);
                    stream << " " << arg.mName;
                    if (first)
                        first = false;
                    else
                        stream << ",";
                }
                stream << ")";

                stream << "{\n";

                for (const CodeGen::Statement &statement : function.mStatements) {
                    stream << "\t";
                    generate(stream, statement);
                    stream << ";\n";
                }

                stream << "}\n\n";
            }

            if (index == 0) {
                stream << "RasterizerData main(VertexDataIn dataIn){\n";
                stream << "\tVertexData IN;\n";
                for (const CodeGen::VariableDefinition &inVar : file.mStructs.at("VertexData").mVariables) {
                    stream << "\tIN." << inVar.mVariable.mName << " = dataIn." << inVar.mVariable.mName << ";\n";
                }
                stream << "\treturn mainImpl(IN);\n";
                stream << "}\n\n";
            } else {
                stream << "float4 main(RasterizerData IN) : SV_TARGET {\n";
                stream << "\treturn mainImpl(IN);\n";
                stream << "}\n\n";
            }

        }

    }

}
}