#pragma once

namespace CE::RPI
{

	/// @brief The ParentPass class allows adding passes as children. ParentPass don't do any work on their own.
	class CORERPI_API ParentPass : public Pass
	{
		friend class Pass;
		friend class PassFactory;
		friend class PassLibrary;
		friend class PassTemplate;
		friend class PassSystem;
	public:

		ParentPass();

		~ParentPass();

		virtual Name GetClassName() const override { return "ParentPass"; }

		void AddChild(Pass* childPass);
		void RemoveChild(Pass* childPass);

		void Clear();

	private:

		Array<PassPtr> passes{};

	};
    
} // namespace CE::RPI
