#pragma once


namespace CE::RPI
{

	/// @brief The ParentPass class allows adding passes as children. ParentPass don't do any work on their own.
	CLASS()
	class CORERPI_API ParentPass : public Pass
	{
		CE_CLASS(ParentPass, Pass)
	public:
		friend class Pass;
		friend class PassRegistry;
		friend class RenderPipeline;
		friend class PassSystem;
		friend class PassTree;

		~ParentPass();

		void AddChild(Pass* childPass);
		void RemoveChild(Pass* childPass);

		void Clear();

		virtual bool IsParentPass() const override final { return true; }
        
        Pass* GetPass(const Name& passName);

	corerpi_internal:

		Array<Pass*> passes{};

	};
    
} // namespace CE::RPI

#include "ParentPass.rtti.h"
