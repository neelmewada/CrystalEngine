#pragma once

namespace CE::RHI
{

    class CORERHI_API FrameGraph final
    {
    public:
        FrameGraph();
        virtual ~FrameGraph();
        
    private:
        
        //! A database of all attachments used in this frame graph.
        FrameAttachmentDatabase attachmentDatabase{};

		//! List of scopes in the correct chronological order from start to end.
        Array<Scope*> scopes{};
        
        friend class FrameAttachmentDatabase;
    };

} // namespace CE::RHI
