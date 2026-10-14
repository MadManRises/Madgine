#pragma once

#include "Generic/execution/binding.h"
#include "Generic/execution/lifetime.h"

#include "Madgine/behavior/context.h"

namespace Engine {
namespace Widgets {

    struct MADGINE_WIDGETS_EXPORT WidgetPtr : Execution::Lifetime<Reflect::get_reflect_contextual>::BindingPoint<Execution::ConstantBinding<WidgetBase &>> {

        using Base = Execution::Lifetime<Reflect::get_reflect_contextual>::BindingPoint<Execution::ConstantBinding<WidgetBase &>>;

        WidgetPtr() = default;
        WidgetPtr(const WidgetPtr &) = default;
        WidgetPtr(WidgetPtr &&) = default;

        using Base::Base;

        template <Execution::Binding<WidgetBase &> Binding>
            requires(!std::same_as<std::remove_cvref_t<Binding>, WidgetPtr>)
        WidgetPtr(Binding &&binding)
        {
            Execution::access_binding(std::forward<Binding>(binding), [this](WidgetBase &w) {
                fromWidget(w);
            });
        }

        WidgetPtr &operator=(const WidgetPtr &other) = default;

        WidgetPtr &operator=(WidgetPtr &&other) = default;

        std::strong_ordering operator<=>(const WidgetPtr &other) const;
        bool operator==(const WidgetPtr &other) const;

        explicit operator bool() const;

        template <Execution::Binding<WidgetBase &> Binding>
            requires(!std::same_as<std::remove_cvref_t<Binding>, WidgetPtr>)
        WidgetPtr &operator=(Binding &&binding)
        {
            if (!Execution::access_binding(std::forward<Binding>(binding), [this](WidgetBase &w) {
                    fromWidget(w);
                })) {
                *this = {};
            }
            return *this;
        }

    private:
        void fromWidget(WidgetBase &w);

        friend Serialize::StreamResult tag_invoke(Serialize::apply_map_t, WidgetPtr &, Serialize::FormattedSerializeStream &, bool, Serialize::ContextPtr);

        template <typename... Configs, typename Context>
        friend void tag_invoke(Serialize::set_active_t<Configs...>, WidgetPtr &, bool, bool, Context &&)
        {
        }

        template <Concepts::DecayedOneOf<WidgetPtr> T, typename Callable, typename Context>
        friend Reflect::Result tag_invoke(Reflect::call_t<T> call, Callable &&callable, const Reflect::Value &arg, Context &&context)
        {
            if (Reflect::Value_isNull(arg)) {
                return callable(WidgetPtr {}, context);
            }
            return tag_invoke(Reflect::call_t<Reflect::ScopeBinding> {}, [&](const Reflect::ScopeBinding &binding, Context &context) { return callable(WidgetPtr { binding.typed<WidgetBase &>() }, context); }, arg, context);
        }
    };

}

namespace Serialize {
    template <>
    struct Operations<Widgets::WidgetPtr> {
        static StreamResult read(FormattedSerializeStream &in, Widgets::WidgetPtr &e, const char *name, ContextPtr context);
        static void write(FormattedSerializeStream &out, const Widgets::WidgetPtr &e, const char *name, ContextPtr context);
        static StreamResult visitStream(FormattedSerializeStream &in, const char *name, const StreamVisitor &visitor, size_t depth);
    };
}

}