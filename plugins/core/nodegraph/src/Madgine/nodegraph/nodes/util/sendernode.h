#pragma once

#include "Generic/execution/concepts.h"
#include "Generic/execution/state.h"

#include "../variablenode.h"
#include "../../nodecollector.h"

namespace Engine {
namespace NodeGraph {

    template <template <typename...> Sender>
    struct SenderNode : Node<SenderNode<Sender>> {

    };

    struct NodeGraph_Context {

    };

    struct nodegraph_receiver : Engine::Execution::execution_receiver<NodeGraph_Context> {

        NodeGraph_Context mContext;

        friend NodeGraph_Context &tag_invoke(Engine::Execution::get_context_t, nodegraph_receiver &self)
        {
            return self.mContext;
        }
    };

    template <typename _Rec>
    struct nodegraph_base_state {
        using Rec = _Rec;

        Rec mRec;

        template <typename CPO>
        friend decltype(auto) tag_invoke(CPO f, nodegraph_base_state &state)
        {
            return f(state.mRec);
        }
    };

    struct nodegraph_connect_t;

    template <typename Sender, typename Rec>
    using nodegraph_connect_result_t = tag_invoke_result_t<nodegraph_connect_t, Sender, Rec>;

    struct SenderConnection {
    };

    template <typename T>
    SenderConnection operator+(SenderConnection, T&& t) {
        return {};
    }

    template <typename State>
    struct nodegraph_algorithm_state_helper {

        using InnerRec = typename State::Rec;
        using Rec = InnerRec;

        template <typename Sender, typename... Args>
        nodegraph_algorithm_state_helper(Sender &&sender, Rec &&rec, Args &&...args)
            : mState { nodegraph_connect(std::forward<Sender>(sender), InnerRec { std::forward<Rec>(rec), std::forward<Args>(args)... }) }
        {
        }

        ~nodegraph_algorithm_state_helper() { }

        State mState;

        template <typename CPO>
        friend decltype(auto) tag_invoke(CPO f, nodegraph_algorithm_state_helper &state)
        {
            return f(state.mState);
        }
    };

    template <typename Sender, typename InnerRec>
    using nodegraph_algorithm_state = nodegraph_algorithm_state_helper<nodegraph_connect_result_t<Sender, InnerRec>>;

    extern const nodegraph_connect_t nodegraph_connect;

    struct nodegraph_connect_t {

        template <Engine::fixed_string Name, typename Sender, typename T, typename Rec>
        friend auto tag_invoke(nodegraph_connect_t, Engine::Execution::Variable_t::sender<Name, Sender, T> &&sender, Rec &&rec)
        {
            struct state : nodegraph_algorithm_state<Sender, Rec> {
                auto build(NodeGraph &graph)
                {
                    std::unique_ptr<VariableNode> node = std::make_unique<VariableNode>(graph);
                    node->mDefaultValue = mInitialValue;
                    graph.addNode(std::move(node));

                    mState.build(graph);
                }
                T mInitialValue;
            };
            return state { { std::move(sender.mSender), std::forward<Rec>(rec) }, std::forward<T>(sender.mInitialValue) };
        }

        template <typename T, Engine::fixed_string Name, typename Rec>
        friend auto tag_invoke(nodegraph_connect_t, Engine::Execution::read_var_t::sender<T, Name> &&sender, Rec &&rec)
        {
            struct state : nodegraph_base_state<Rec> {
                auto build(NodeGraph &graph)
                {
                    return std::make_tuple(SenderConnection {});                
                }
            };
            return state { std::forward<Rec>(rec) };
        }

        template <typename Sender, Engine::fixed_string Name, typename Rec>
        friend auto tag_invoke(nodegraph_connect_t, Engine::Execution::write_var_t::sender<Sender, Name> &&sender, Rec &&rec)
        {
            struct state : nodegraph_algorithm_state<Sender, Rec> {
                auto generate()
                {
                    
                }
            };
            return state { { std::move(sender.mSender), std::forward<Rec>(rec) } };
        }

        struct nodegraph_let_value_t {
            
            template <typename Sender, typename F, typename Rec>
            struct state : nodegraph_base_state<Rec> {
                auto build(NodeGraph &graph)
                {
                    auto connections = nodegraph_connect(std::move(mSender), mRec).build(graph);
                    nodegraph_connect(TupleUnpacker::invokeExpand(mF, connections), mRec).build(graph);
                }

                Sender mSender;
                F mF;
            };
        };

        template <typename Sender, typename F, typename Rec>
        friend auto tag_invoke(nodegraph_connect_t, Engine::Execution::let_value_t::sender<Sender, F> &&sender, Rec &&rec)
        {
            return nodegraph_let_value_t::state<Sender, F, Rec> { std::forward<Rec>(rec), std::move(sender.mSender), std::forward<F>(sender.mF) };
        }

        template <typename Sender, typename T, typename Rec>
        friend auto tag_invoke(nodegraph_connect_t, Engine::Execution::assign_t::sender<Sender, T> &&sender, Rec &&rec)
        {
            struct state : nodegraph_algorithm_state<Sender, Rec> {
                auto generate()
                {

                }
                T mVar;
            };
            return state { { std::move(sender.mSender), std::forward<Rec>(rec) }, std::forward<T>(sender.mVar) };
        }

        template <typename Sender1, typename Sender2, typename Rec>
        friend auto tag_invoke(nodegraph_connect_t, Engine::Execution::sequence_t::sender<Sender1, Sender2> &&sender, Rec &&rec)
        {
            struct state : nodegraph_base_state<Rec> {

                auto generate()
                {
                    
                }
                Sender1 mSender1;
                Sender2 mSender2;
            };
            return state { std::forward<Rec>(rec), std::forward<Sender1>(sender.mSender1), std::forward<Sender2>(sender.mSender2) };
        }

        template <typename... Args, typename Rec>
        friend auto tag_invoke(nodegraph_connect_t, Engine::Execution::just_t::sender<Args...> &&sender, Rec &&rec)
        {
            struct state : nodegraph_base_state<Rec> {
                auto generate()
                {
                    
                }
                std::tuple<Args...> mArgs;
            };
            return state { std::forward<Rec>(rec), std::move(sender.mArgs) };
        }

        template <typename Sender, typename T, typename Rec>
        friend auto tag_invoke(nodegraph_connect_t, Engine::Execution::then_t::sender<Sender, T> &&sender, Rec &&rec)
        {
            struct state : nodegraph_algorithm_state<Sender, Rec> {
                auto generate()
                {
                    return std::make_tuple(
                        Engine::TupleUnpacker::invokeExpand(mTransform, mState.generate()));
                }
                T mTransform;
            };
            return state { { std::move(sender.mSender), std::forward<Rec>(rec) }, std::forward<T>(sender.mTransform) };
        }

        template <typename Sender, typename Rec>
        requires tag_invocable<nodegraph_connect_t, Sender, Rec>
        auto operator()(Sender &&sender, Rec &&rec) const
            noexcept(is_nothrow_tag_invocable_v<nodegraph_connect_t, Sender, Rec>)
                -> tag_invoke_result_t<nodegraph_connect_t, Sender, Rec>
        {
            return tag_invoke(*this, std::forward<Sender>(sender), std::forward<Rec>(rec));
        }
    };

    inline constexpr nodegraph_connect_t nodegraph_connect;

    struct nodegraph_for_each_t {

        template <typename C, typename F>
        struct sender {
            //using value_type = typename std::remove_reference_t<C>::value_type &;
            //using inner_sender_t = std::invoke_result_t<F, value_type>;
            using result_type = void;
            template <template <typename...> typename Tuple>
            using value_types = Tuple<>;

            template <typename Rec>
            friend auto tag_invoke(nodegraph_connect_t, sender &&sender, Rec &&rec)
            {
                struct state : nodegraph_base_state<Rec> {
                    auto build(NodeGraph &graph)
                    {
                        std::unique_ptr<SenderNode<Execution::for_each_t::sender>> node = std::make_unique<SenderNode<Execution::for_each_t::sender>>(graph, std::forward<C>(mContainer), std::forward<F>(mF));
                        node->mDefaultValue = mInitialValue;
                        graph.addNode(std::move(node));

                        mState.build(graph);
                    }
                    C mContainer;
                    F mF;
                };
                return state { std::forward<Rec>(rec), std::forward<C>(sender.mContainer), std::forward<F>(sender.mF) };
            }
            C mContainer;
            F mF;
        };
    };

    template <typename C, typename F>
    requires std::same_as<std::remove_reference_t<C>, SenderConnection>
    auto tag_invoke(Engine::Execution::for_each_t, C &&c, F &&f)
    {
        return nodegraph_for_each_t::sender<C, F> { std::forward<C>(c), std::forward<F>(f) };
    }

    template <typename Sender>
    auto graphBuilderFromSender(Sender &&sender)
    {
        return nodegraph_connect(std::move(sender), nodegraph_receiver {});
    }

}
}