#include "../nodegraphtoolslib.h"

#include "debugvisualizer.h"

#include "imgui/imgui.h"
#include "imgui/imguiaddons.h"
#include "imguihelpers.h"

#include "Madgine/nodegraph/pins.h"

#include "Madgine/nodegraph/nodeinterpreter.h"

#include "Madgine_Tools/debugger/debuggerview.h"

namespace Engine {
namespace Tools {

    struct ConnectorInfo {
        ImVec2 mPos;
        NodeGraph::PinType mType;
        uint32_t mIndex;
    };

    void visualizeDebugLocation(DebuggerView *view, const Debug::ContextInfo *context, const NodeGraph::NodeDebugLocation *location)
    {
        const NodeGraph::NodeBase *node = location->mNode;
        const NodeGraph::NodeGraph &graph = *location->mInterpreter->graph();

        if (ImGui::BeginTable("NodeContext", 3)) {

            std::vector<std::tuple<ImVec2, ImVec2, ImColor>> connections;

            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            ImGui::TableNextColumn();

            struct NodeInfo {
                const NodeGraph::NodeBase *mNode;
                std::vector<ConnectorInfo> mConnectors;
            };
            std::vector<NodeInfo> predecessors;
            std::vector<NodeInfo> successors;

            auto insert_unique = [](std::vector<NodeInfo> &vector, const NodeGraph::NodeBase *item, ImVec2 pos, NodeGraph::PinType type, uint32_t index) {
                if (item) {
                    NodeInfo *info;
                    auto it = std::ranges::find(vector, item, &NodeInfo::mNode);
                    if (it == vector.end())
                        info = &vector.emplace_back(NodeInfo { item });
                    else
                        info = &*it;
                    info->mConnectors.push_back({ pos, type, index });
                }
            };

            if (node) {
                ImGui::BeginGroupPanel();
                if (BeginNode(node)) {
                    uint32_t maxGroupCount = max(max(max(node->flowInGroupCount(), node->flowOutGroupCount()), max(node->dataInGroupCount(), node->dataOutGroupCount())), max(node->dataReceiverGroupCount(), node->dataProviderGroupCount()));
                    for (uint32_t group = 0; group < maxGroupCount; ++group) {

                        uint32_t inFlowCount = group < node->flowInGroupCount() ? node->flowInCount(group) : 0;
                        uint32_t outFlowCount = group < node->flowOutGroupCount() ? node->flowOutCount(group) : 0;
                        for (uint32_t flowIndex = 0; flowIndex < max(inFlowCount, outFlowCount); ++flowIndex) {
                            ImGui::TableNextColumn();
                            if (flowIndex < inFlowCount)
                                FlowInPin(node->flowInName(flowIndex, group).data(), node->flowInMask(flowIndex, group), !node->flowInSources(flowIndex, group).empty());

                            ImGui::TableNextColumn();
                            if (flowIndex < outFlowCount) {
                                ImVec2 pos = FlowOutPin(node->flowOutName(flowIndex, group).data(), node->flowOutMask(flowIndex, group), static_cast<bool>(node->flowOutTarget(flowIndex, group)));
                                NodeGraph::Pin target = node->flowOutTarget(flowIndex, group);
                                insert_unique(successors, graph.node(target.mNode), pos, NodeGraph::PinType::Flow, target.mIndex);
                            }
                        }

                        uint32_t inDataCount = group < node->dataInGroupCount() ? node->dataInCount(group) : 0;
                        uint32_t outDataCount = group < node->dataOutGroupCount() ? node->dataOutCount(group) : 0;
                        for (uint32_t dataIndex = 0; dataIndex < max(inDataCount, outDataCount); ++dataIndex) {
                            ImGui::TableNextColumn();
                            if (dataIndex < inDataCount) {
                                NodeGraph::Pin source = node->dataInSource(dataIndex, group);

                                ExtendedValueTypeDesc type = node->dataInType(dataIndex, group);

                                ImVec2 pos = DataInPin(node->dataInName(dataIndex, group).data(), type, node->dataInMask(dataIndex, group), static_cast<bool>(source));

                                insert_unique(predecessors, graph.node(source.mNode), pos, NodeGraph::PinType::Data, source.mIndex);
                            }

                            ImGui::TableNextColumn();
                            if (dataIndex < outDataCount) {
                                NodeGraph::Pin target = node->dataOutTarget(dataIndex, group);

                                ExtendedValueTypeDesc type = node->dataOutType(dataIndex, group);

                                ImVec2 pos = DataOutPin(node->dataOutName(dataIndex, group).data(), type, node->dataOutMask(dataIndex, group), static_cast<bool>(target));

                                insert_unique(successors, graph.node(target.mNode), pos, NodeGraph::PinType::Data, target.mIndex);
                            }
                        }

                        uint32_t dataReceiverCount = group < node->dataReceiverGroupCount() ? node->dataReceiverCount(group) : 0;
                        uint32_t dataProviderCount = group < node->dataProviderGroupCount() ? node->dataProviderCount(group) : 0;
                        for (uint32_t dataInstanceIndex = 0; dataInstanceIndex < max(dataReceiverCount, dataProviderCount); ++dataInstanceIndex) {
                            ImGui::TableNextColumn();
                            if (dataInstanceIndex < dataReceiverCount) {
                                ExtendedValueTypeDesc type = node->dataReceiverType(dataInstanceIndex, group);

                                DataReceiverPin(node->dataReceiverName(dataInstanceIndex, group).data(), type, node->dataReceiverMask(dataInstanceIndex, group), !node->dataReceiverSources(dataInstanceIndex, group).empty());
                            }

                            ImGui::TableNextColumn();
                            if (dataInstanceIndex < dataProviderCount) {
                                ExtendedValueTypeDesc type = node->dataProviderType(dataInstanceIndex, group);

                                DataProviderPin(node->dataProviderName(dataInstanceIndex, group).data(), type, node->dataProviderMask(dataInstanceIndex, group), !node->dataProviderTargets(dataInstanceIndex, group).empty());
                            }
                        }
                    }
                    EndNode();
                }
                ImGui::EndGroupPanel();
            } else {
                ImGui::Text("Return");
            }

            ImGui::TableNextColumn();

            auto renderDependencyNode = [&](const NodeInfo &info) {
                const NodeGraph::NodeBase *node = info.mNode;
                ImGui::PushID(node);
                ImGui::BeginGroupPanel();
                if (BeginNode(node)) {
                    uint32_t maxGroupCount = max(max(max(node->flowInGroupCount(), node->flowOutGroupCount()), max(node->dataInGroupCount(), node->dataOutGroupCount())), max(node->dataReceiverGroupCount(), node->dataProviderGroupCount()));
                    for (uint32_t group = 0; group < maxGroupCount; ++group) {

                        uint32_t inFlowCount = group < node->flowInGroupCount() ? node->flowInCount(group) : 0;
                        uint32_t outFlowCount = group < node->flowOutGroupCount() ? node->flowOutCount(group) : 0;
                        for (uint32_t flowIndex = 0; flowIndex < max(inFlowCount, outFlowCount); ++flowIndex) {
                            ImGui::TableNextColumn();
                            if (flowIndex < inFlowCount) {
                                uint32_t mask = node->flowInMask(flowIndex, group);
                                ImVec2 pos = FlowInPin(node->flowInName(flowIndex, group).data(), mask, !node->flowInSources(flowIndex, group).empty());
                                auto it = std::ranges::find_if(info.mConnectors, [=](const ConnectorInfo &con) { return con.mType == NodeGraph::PinType::Flow && con.mIndex == flowIndex; });
                                if (it != info.mConnectors.end())
                                    connections.emplace_back(pos, it->mPos, FlowColor(mask));
                            }

                            ImGui::TableNextColumn();
                            if (flowIndex < outFlowCount)
                                FlowOutPin(node->flowOutName(flowIndex, group).data(), node->flowOutMask(flowIndex, group), static_cast<bool>(node->flowOutTarget(flowIndex, group)));
                        }

                        uint32_t inDataCount = group < node->dataInGroupCount() ? node->dataInCount(group) : 0;
                        uint32_t outDataCount = group < node->dataOutGroupCount() ? node->dataOutCount(group) : 0;
                        for (uint32_t dataIndex = 0; dataIndex < max(inDataCount, outDataCount); ++dataIndex) {
                            ImGui::TableNextColumn();
                            if (dataIndex < inDataCount) {
                                NodeGraph::Pin source = node->dataInSource(dataIndex, group);

                                ExtendedValueTypeDesc type = node->dataInType(dataIndex, group);

                                DataInPin(node->dataInName(dataIndex, group).data(), type, node->dataInMask(dataIndex, group), static_cast<bool>(source));
                            }

                            ImGui::TableNextColumn();
                            if (dataIndex < outDataCount) {
                                NodeGraph::Pin target = node->dataOutTarget(dataIndex, group);

                                ExtendedValueTypeDesc type = node->dataOutType(dataIndex, group);

                                DataOutPin(node->dataOutName(dataIndex, group).data(), type, node->dataOutMask(dataIndex, group), static_cast<bool>(target));
                            }
                        }

                        uint32_t dataReceiverCount = group < node->dataReceiverGroupCount() ? node->dataReceiverCount(group) : 0;
                        uint32_t dataProviderCount = group < node->dataProviderGroupCount() ? node->dataProviderCount(group) : 0;
                        for (uint32_t dataInstanceIndex = 0; dataInstanceIndex < max(dataReceiverCount, dataProviderCount); ++dataInstanceIndex) {
                            ImGui::TableNextColumn();
                            if (dataInstanceIndex < dataReceiverCount) {
                                ExtendedValueTypeDesc type = node->dataReceiverType(dataInstanceIndex, group);
                                uint32_t mask = node->dataReceiverMask(dataInstanceIndex, group);
                                ImVec2 pos = DataReceiverPin(node->dataReceiverName(dataInstanceIndex, group).data(), type, mask, !node->dataReceiverSources(dataInstanceIndex, group).empty());

                                auto it = std::ranges::find_if(info.mConnectors, [=](const ConnectorInfo &con) { return con.mType == NodeGraph::PinType::Data && con.mIndex == dataInstanceIndex; });
                                if (it != info.mConnectors.end())
                                    connections.emplace_back(pos, it->mPos, DataColor(mask));
                            }

                            ImGui::TableNextColumn();
                            if (dataInstanceIndex < dataProviderCount) {
                                ExtendedValueTypeDesc type = node->dataProviderType(dataInstanceIndex, group);
                                uint32_t mask = node->dataProviderMask(dataInstanceIndex, group);
                                ImVec2 pos = DataProviderPin(node->dataProviderName(dataInstanceIndex, group).data(), type, mask, !node->dataProviderTargets(dataInstanceIndex, group).empty());

                                auto it = std::ranges::find_if(info.mConnectors, [=](const ConnectorInfo &con) { return con.mType == NodeGraph::PinType::Data && con.mIndex == dataInstanceIndex; });
                                if (it != info.mConnectors.end())
                                    connections.emplace_back(pos, it->mPos, DataColor(mask));
                            }
                        }
                    }
                    EndNode();
                }
                ImGui::EndGroupPanel();
                ImGui::PopID();
            };

            ImGui::TableSetColumnIndex(0);

            for (const NodeInfo &pred : predecessors)
                renderDependencyNode(pred);

            ImGui::TableSetColumnIndex(2);

            for (const NodeInfo &succ : successors)
                renderDependencyNode(succ);

            ImGui::EndTable();

            for (const auto &p : connections) {
                ImVec2 from = std::get<0>(p);
                ImVec2 to = std::get<1>(p);
                ImVec2 p2 = {
                    0.5f * from.x + 0.5f * to.x,
                    from.y
                };
                ImVec2 p3 = {
                    0.5f * from.x + 0.5f * to.x,
                    to.y
                };
                ImGui::GetWindowDrawList()->AddBezierCubic(from, p2, p3, to, std::get<2>(p), 1.0f, 20);
            }
        }

        view->visualizeDebugLocation(context, location->mChild);
    }

}
}