#pragma once

namespace CE
{
	class NodeComponent;

    CLASS()
	class SYSTEM_API Node : public Object
	{
		CE_CLASS(Node, Object)
	public:

		Node();
		virtual ~Node();

		inline u32 GetChildrenCount() const { return children.GetSize(); }
		Node* GetChildAt(u32 index);

		inline u32 GetComponentCount() const { return components.GetSize(); }
		NodeComponent* GetComponentAt(u32 index);

		inline auto begin() { return children.begin(); }
		inline auto end() { return children.end(); }

	protected:

		void OnSubobjectAttached(Object* object) override;
		void OnSubobjectDetached(Object* object) override;

	system_internal:

		FIELD()
		Array<Node*> children{};

		FIELD()
		Array<NodeComponent*> components{};
	};

} // namespace CE

#include "Node.rtti.h"
