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
		friend class PassFactory;
		friend class PassLibrary;
		friend class PassTemplate;
		friend class PassSystem;

		~ParentPass();

		void AddChild(Pass* childPass);
		void RemoveChild(Pass* childPass);

		void Clear();

	private:

		Array<Pass*> passes{};

	};
    
} // namespace CE::RPI

#include "ParentPass.rtti.h"
