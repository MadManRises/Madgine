#include "../../clientnodeslib.h"

#include "shadercodegenerator.h"

#include "Madgine/nodegraph/nodebase.h"
#include "Madgine/nodegraph/nodegraph.h"

#include "gpubuffernode.h"

namespace Engine {
namespace Render {

    CodeGen::Statement ShaderCodeGenerator::read(NodeGraph::Pin pin)
    {       

        int instanceIndex = mRasterizerData ? 1 : 0;
        
        if (mData[pin.mNode - 1]) {
            ShaderCodeGeneratorData *instanceData = dynamic_cast<ShaderCodeGeneratorData *>(mData[pin.mNode - 1].get());
            if (instanceData) {
                assert(instanceData->mInstanceIndex);                
                instanceIndex = instanceData->mInstanceIndex;
            }
        }

        if (instanceIndex == 2) {
            return CodeGenerator::read(pin);
        } else {

            if (mRasterizerData) {

                if (instanceIndex == 1) {
                    return CodeGenerator::read(pin);
                } else {

                    std::string_view name = mGraph.node(pin.mNode)->dataProviderName(pin.mIndex);

                    mRasterizerData->mVariables.push_back({ { { mFile.mConditionalsBitMask } }, { std::string { name }, toValueTypeDesc<Vector4>(), false, { "nointerpolation" } } });

                    mAdditionalRasterizerStatements.push_back(CodeGen::Assignment { {},CodeGen::VariableAccess { "OUT." + std::string { name } }, CodeGenerator::read(pin) });

                    return CodeGen::VariableAccess { "IN." + std::string { name } };
                }
            } else {
                assert(instanceIndex == 0);
                return CodeGenerator::read(pin);
            }
        }
    }

    CodeGen::File &ShaderCodeGenerator::file()
    {
        return mFile;
    }

    void ShaderCodeGenerator::registerBuffer(GPUBufferCodeGeneratorData &data)
    {
        data.mIndex = mBuffers.size();
        mBuffers.push_back(&data);
    }


}
}