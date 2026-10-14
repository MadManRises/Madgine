#include "../widgetslib.h"

#include "widgetptr.h"

#include "Meta/serialize/operations.h"
#include "Meta/serialize/streams/streamresult.h"

#include "widget.h"

namespace Engine {
namespace Widgets {

    std::strong_ordering WidgetPtr::operator<=>(const WidgetPtr &other) const
    {
        return ptr() <=> other.ptr();
    }

    bool WidgetPtr::operator==(const WidgetPtr &other) const
    {
        return ptr() == other.ptr();
    }

    WidgetPtr::operator bool() const
    {
        return static_cast<bool>(ptr());
    }

    void WidgetPtr::fromWidget(WidgetBase &w)
    {
        Base::operator=(w.lifetime().bind(Execution::ConstantBinding{w}));
    }

    Serialize::StreamResult tag_invoke(Serialize::apply_map_t, WidgetPtr &ptr, Serialize::FormattedSerializeStream &in, bool success, Serialize::ContextPtr context)
    {
        WidgetBase *w = std::exchange(reinterpret_cast<WidgetBase *&>(ptr), nullptr);
        STREAM_PROPAGATE_ERROR(Serialize::apply_map(w, in, success, context));
        if (w) {
            ptr = w->pointer();
        }
        return {};
    }

}
namespace Serialize {

    StreamResult Operations<Widgets::WidgetPtr>::read(FormattedSerializeStream &in, Widgets::WidgetPtr &w, const char *name, ContextPtr context)
    {
        w = {};
        return Serialize::read(in, reinterpret_cast<Engine::Widgets::WidgetBase *&>(w), name, context);
    }

    void Operations<Widgets::WidgetPtr>::write(FormattedSerializeStream &out, const Widgets::WidgetPtr &w, const char *name, ContextPtr context)
    {
        if (!Execution::access_binding(w, [&](Widgets::WidgetBase &w) {
                Serialize::write(out, &w, name);
            })) {
            Serialize::write(out, static_cast<Widgets::WidgetBase *>(nullptr), name, context);
        }
    }

    StreamResult Operations<Widgets::WidgetPtr>::visitStream(FormattedSerializeStream &in, const char *name, const StreamVisitor &visitor, size_t depth)
    {
        throw 0;
    }

}
}
