#pragma once

#include "Madgine/nodegraph/nodebase.h"
#include "Madgine/nodegraph/nodecollector.h"

#include "Python3/python3fileloader.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        struct MADGINE_PYTHON3_NODES_EXPORT Python3FunctionNode : NodeGraph::Node<Python3FunctionNode> {

            Python3FunctionNode(NodeGraph::NodeGraph &graph);
            Python3FunctionNode(const Python3FunctionNode &other, NodeGraph::NodeGraph &graph);

            virtual std::string_view name() const override;

            virtual size_t flowInCount() const override;
            virtual std::string_view flowInName(uint32_t index) const override;

            virtual size_t flowOutCount() const override;
            virtual std::string_view flowOutName(uint32_t index) const override;

            virtual size_t dataOutCount() const override;
            virtual std::string_view dataOutName(uint32_t index) const override;
            virtual ExtendedValueTypeDesc dataOutType(uint32_t index, bool bidir = true) const override;

            virtual size_t dataInCount() const override;
            virtual std::string_view dataInName(uint32_t index) const override;
            virtual ExtendedValueTypeDesc dataInType(uint32_t index, bool bidir = true) const override;

            virtual void interpret(NodeGraph::NodeInterpreter &interpreter, IndexType<uint32_t> &flowInOut, std::unique_ptr<NodeGraph::NodeInterpreterData> &data) const override;

            Python3FileLoader::ResourceType *getFile() const;
            void setFile(Python3FileLoader::ResourceType *file);

            std::string_view getName() const;
            void setName(std::string_view name);

            Python3FileLoader::HandleType mFile;

        protected:
            void updatePins();
            std::vector<std::pair<std::string, ExtendedValueTypeDesc>> mArguments;
            ExtendedValueTypeDesc mReturnType;

            std::string mName;
        };

    }
}
}

RegisterType(Engine::Scripting::Python3::Python3FunctionNode);
