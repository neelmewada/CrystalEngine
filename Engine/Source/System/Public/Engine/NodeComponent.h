#pragma once

namespace CE
{
	class Node;

	CLASS()
	class SYSTEM_API NodeComponent : public Object
	{
		CE_CLASS(NodeComponent, Object)
	public:

		NodeComponent();
		virtual ~NodeComponent();

		inline u32 GetChildCount() const { return children.GetSize(); }
		NodeComponent* GetChildAt(u32 index);

		inline auto begin() { return children.begin(); }
		inline auto end() { return children.end(); }

	protected:

		void OnSubobjectAttached(Object* object) override;
		void OnSubobjectDetached(Object* object) override;

	system_internal:

		FIELD()
		Node* owner = nullptr;

		FIELD()
		Array<NodeComponent*> children{};
	};

} // namespace CE

#include "NodeComponent.rtti.h"
