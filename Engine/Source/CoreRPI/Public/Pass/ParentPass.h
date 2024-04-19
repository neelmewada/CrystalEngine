#pragma once


namespace CE::RPI
{

	/// @brief The ParentPass class allows adding passes as children. ParentPass don't do any work on their own.
	CLASS()
	class CORERPI_API ParentPass : public Pass
	{
		CE_CLASS(ParentPass, Pass)
	public:

		~ParentPass();

		void AddChild(Pass* childPass);
		void RemoveChild(Pass* childPass);

		void Clear();

		virtual bool IsParentPass() const override final { return true; }
        
        Pass* GetPass(const Name& passName);

	protected:

		Array<Pass*> passes{};

		RPI_PASS(ParentPass);
	};
    
} // namespace CE::RPI

#include "ParentPass.rtti.h"
