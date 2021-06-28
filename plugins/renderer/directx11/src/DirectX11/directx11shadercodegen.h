#pragma once

namespace Engine {
namespace Render {
    namespace DirectX11ShaderCodeGen {
    
        void generate(std::ostream &stream, const CodeGen::ShaderFile &file, uint32_t index);
    
    }
}
}