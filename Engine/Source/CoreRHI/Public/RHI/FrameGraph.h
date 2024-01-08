#pragma once

#if PAL_TRAIT_BUILD_TESTS
class RHI_FrameGraphBuilder_Test;
#endif

namespace CE::RHI
{

    class CORERHI_API FrameGraph final
    {
    public:
        FrameGraph();
        virtual ~FrameGraph();
        
    private:

		void Clear();
        
		bool Build();

		struct GraphNode
		{
			Scope* scope = nullptr;
			Array<Scope*> producers{};

			inline bool operator==(const GraphNode& rhs) const
			{
				return scope == rhs.scope;
			}
		};

		bool ScopeHasDependency(Scope* source, Scope* dependentOn);

		void AddScopeDependency(Scope* from, Scope* to);

		void FinalizeGraph();

		HashMap<AttachmentID, Scope*> lastWrittenAttachmentToScope{};
		HashMap<AttachmentID, HashSet<Scope*>> attachmentReadSchedule{};
		HashMap<Scope*, HashSet<Scope*>> nodeDependencies{};

        //! A database of all attachments used in this frame graph.
        FrameAttachmentDatabase attachmentDatabase{};

        Array<Scope*> scopes{};
		HashMap<ScopeID, Scope*> scopesById{};
		
		Array<Scope*> producers{};
		Scope* currentScope = nullptr;

        friend class FrameAttachmentDatabase;
		friend class FrameGraphCompiler;
		friend class FrameGraphBuilder;
		friend class FrameScheduler;
#if PAL_TRAIT_BUILD_TESTS
		friend class RHI_FrameGraphBuilder_Test;
#endif
    };

} // namespace CE::RHI
