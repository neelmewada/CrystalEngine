#include "System.h"

namespace CE
{
    CE::TextureCube::TextureCube()
    {
        
    }

    CE::TextureCube::~TextureCube()
    {

    }

    RPI::Texture* CE::TextureCube::GetRpiTexture()
    {
        if (!rpiTexture)
        {
            
        }
        return rpiTexture;
    }
} // namespace CE
