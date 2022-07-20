#pragma once

#include "Madgine/nodegraph/codegenerator.h"

#include "Madgine/codegen/codegen_shader.h"

namespace Engine {
namespace Render {

    struct GPUBufferCodeGeneratorData;

    struct ShaderCodeGeneratorData : NodeGraph::CodeGeneratorData {
        IndexType<uint32_t> mInstanceIndex;
    };

	struct ShaderCodeGenerator : NodeGraph::CodeGenerator {

        using CodeGenerator::CodeGenerator;

        virtual CodeGen::Statement read(uint32_t dataInIndex) override;

        virtual CodeGen::File &file() override;

        void registerBuffer(GPUBufferCodeGeneratorData &data);

        NodeGraph::NodeInterpreter *mInterpreter;

        CodeGen::ShaderFile mFile;

        std::vector<CodeGen::Statement> mAdditionalRasterizerStatements;
        CodeGen::Struct *mRasterizerData = nullptr;

        std::vector<GPUBufferCodeGeneratorData *> mBuffers;
    };

}
}