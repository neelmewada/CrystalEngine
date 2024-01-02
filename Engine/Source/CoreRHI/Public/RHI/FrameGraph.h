#pragma once

namespace CE::RHI
{

    class CORERHI_API FrameGraph final
    {
    public:
        FrameGraph();
        virtual ~FrameGraph();
        
    private:
        
        FrameAttachmentDatabase attachmentDatabase{};
        Array<Scope*> scopes{};
        
        friend class FrameAttachmentDatabase;
    };

} // namespace CE::RHI
