#pragma once

#include "serializestreambuf.h"

namespace Engine {
namespace Serialize {

    struct META_EXPORT WrappingSerializeStreambuf : SerializeStreambuf {

        WrappingSerializeStreambuf(std::unique_ptr<std::streambuf> buffer, std::unique_ptr<Formatter> format);

        WrappingSerializeStreambuf(std::unique_ptr<std::streambuf> buffer, std::unique_ptr<Formatter> format, SerializeManager &mgr, ParticipantId id);

        void imbue(const std::locale &loc) override;

        pos_type seekoff(off_type off, std::ios_base::seekdir dir,
            std::ios_base::openmode mode = std::ios_base::in) override;
        pos_type seekpos(pos_type pos,
            std::ios_base::openmode mode = std::ios_base::in) override;

        std::streamsize showmanyc() override;

        std::streamsize xsgetn(char *s, std::streamsize n) override;

        int pbackfail(int c = EOF) override;

        int_type overflow(int c = EOF) override;

        std::streamsize xsputn(const char *s, std::streamsize n) override;

        int_type underflow() override;

        int uflow() override;

        int sync() override;

    private:
        std::unique_ptr<std::streambuf> mBuffer;
    };

}
}