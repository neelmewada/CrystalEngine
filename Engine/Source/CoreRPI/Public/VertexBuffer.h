#pragma once

namespace CE::RPI
{
    
    class CORERPI_API VertexBuffer
    {
    public:
        VertexBuffer(const RHI::VertexBufferView& vertexBufferView) : vertexBufferView(vertexBufferView)
        {

        }


    private:
        RHI::VertexBufferView vertexBufferView{};

        // Used for modifying vertices on CPU side.
        u8* data = nullptr;
    };

} // namespace CE::RPI
