#pragma once

namespace CE::RHI
{

    class CORERHI_API FrameGraph final
    {
    public:
        FrameGraph();
        virtual ~FrameGraph();
        
    private:
        
		struct GraphEdge
		{
			u32 producerIndex = 0;
			u32 consumerIndex = 0;
		};

		struct GraphNode
		{
			Scope* scope = nullptr;
			Array<Scope*> producers{};
			Array<Scope*> consumers{};
		};

        //! A database of all attachments used in this frame graph.
        FrameAttachmentDatabase attachmentDatabase{};

        Array<Scope*> scopes{};
		Array<GraphNode> nodes{};
		Array<GraphEdge> edges{};
		HashMap<Name, Scope*> scopesById{};
		
		Array<Scope*> startProducers{};
		Array<Scope*> endConsumers{};
		Scope* currentScope = nullptr;

        friend class FrameAttachmentDatabase;
		friend class FrameGraphCompiler;
		friend class FrameGraphBuilder;
		friend class RHI_FrameGraphBuilder_Test;
    };

} // namespace CE::RHI
