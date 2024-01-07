#include "CoreRHI.h"

namespace CE::RHI
{

    FrameGraph::FrameGraph()
    {
        
    }

    FrameGraph::~FrameGraph()
    {
		Clear();
    }

    void FrameGraph::Clear()
    {
		for (auto scope : scopes)
		{
			delete scope;
		}
		scopes.Clear();
		scopesById.Clear();
		producers.Clear();

		attachmentDatabase.Clear();
		lastWrittenAttachmentToScope.Clear();
		attachmentReadSchedule.Clear();
		nodeDependencies.Clear();
    }

	bool FrameGraph::Build()
	{
		lastWrittenAttachmentToScope = {};
		attachmentReadSchedule = {};

		for (Scope* scope : scopes)
		{
			// Process read dependencies
			for (auto attachment : scope->readAttachments) 
			{
				if (lastWrittenAttachmentToScope.KeyExists(attachment->id))
				{
					AddScopeDependency(lastWrittenAttachmentToScope[attachment->id], scope);
				}
				attachmentReadSchedule[attachment->id].Add(scope);
			}

			// Process write dependencies
			for (const auto& attachment : scope->writeAttachments)
			{
				if (attachmentReadSchedule.KeyExists(attachment->id))
				{
					for (auto readPass : attachmentReadSchedule[attachment->id])
					{
						AddScopeDependency(readPass, scope);
					}
					attachmentReadSchedule[attachment->id].Clear();
				}

				if (lastWrittenAttachmentToScope.KeyExists(attachment->id))
				{
					AddScopeDependency(lastWrittenAttachmentToScope[attachment->id], scope);
				}

				lastWrittenAttachmentToScope[attachment->id] = scope;
			}
		}

		FinalizeGraph();

		return true;
	}

	void FrameGraph::AddScopeDependency(Scope* from, Scope* to)
	{
		if (from != to)
		{
			nodeDependencies[to].Add(from);
		}
	}

	bool FrameGraph::ScopeHasDependency(Scope* source, Scope* dependentOn) 
	{
		if (!nodeDependencies.KeyExists(source))
			return false;

		for (Scope* scope : nodeDependencies[source])
		{
			if (scope == dependentOn)
				return true;
			else if (ScopeHasDependency(scope, dependentOn))
				return true;
		}
		return false;
	}

	void FrameGraph::FinalizeGraph()
	{
		HashMap<Scope*, HashSet<Scope*>> producersForEachScope{};

		for (auto& [scope, dependencies] : nodeDependencies)
		{
			HashSet<Scope*> producerList = dependencies;

			for (Scope* prod1 : dependencies)
			{
				for (Scope* prod2 : dependencies)
				{
					if (prod1 == prod2)
						continue;

					if (ScopeHasDependency(prod2, prod1))
					{
						producerList.Remove(prod1);
					}
					else if (ScopeHasDependency(prod1, prod2))
					{
						producerList.Remove(prod2);
					}
				}
			}

			producersForEachScope[scope] = producerList;
		}

		nodeDependencies = producersForEachScope;
	}

} // namespace CE::RHI
