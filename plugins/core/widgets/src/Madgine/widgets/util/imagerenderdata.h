#pragma once

#include "Meta/serialize/hierarchy/serializabledataunit.h"

#include "Madgine/imageloader/imageloader.h"

#include "texturesettings.h"
#include "vertex.h"

#include "Meta/math/vector4.h"

#include "Meta/math/atlas2.h"

namespace Engine {
namespace Widgets {

    struct ImageRenderData : Serialize::SerializableDataUnit {

        void setImageName(std::string_view name);
        void setImage(Resources::ImageLoader::Resource *image);

        std::string_view imageName() const;
        Resources::ImageLoader::Resource *image() const;

        std::vector<Vertex> renderImage(Vector3 pos, Vector3 size, const Atlas2::Entry &entry, Vector4 color = Vector4 { 1.0f, 1.0f, 1.0f, 1.0f });
        static void renderQuad(std::vector<Vertex> &out, Vector3 pos, Vector2 size, Vector4 color = Vector4 { 1.0f, 1.0f, 1.0f, 1.0f }, Vector2 topLeftUV = { 0.0f, 0.0f }, Vector2 bottomRightUV = { 1.0f, 1.0f });

    protected:
        Resources::ImageLoader::Resource *mImage = nullptr;
    };

}
}