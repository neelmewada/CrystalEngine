#include "System.h"

namespace CE
{
    
	Node::Node()
	{

	}

	Node::~Node()
	{

	}

	Node* Node::GetChildAt(u32 index)
	{
		if (index >= children.GetSize())
			return nullptr;
		return children[index];
	}

	NodeComponent* Node::GetComponentAt(u32 index)
	{
		if (index >= components.GetSize())
			return nullptr;
		return components[index];
	}

    Node* Node::GetParent() const
    {
		if (GetOuter() != nullptr && GetOuter()->IsOfType<Node>())
		{
			return (Node*)GetOuter();
		}
		return nullptr;
    }

	bool Node::IsRoot() const
	{
		return GetOuter() == nullptr || GetOuter()->IsOfType<Scene>();
	}

    void Node::OnSubobjectAttached(Object* object)
	{
		Super::OnSubobjectAttached(object);

		if (object == nullptr)
			return;

		if (object->IsOfType<Node>())
		{
			children.Add((Node*)object);
		}
		else if (object->IsOfType<NodeComponent>())
		{
			components.Add((NodeComponent*)object);
		}
	}

	void Node::OnSubobjectDetached(Object* object)
	{
		Super::OnSubobjectDetached(object);

		if (object == nullptr)
			return;

		if (object->IsOfType<Node>())
		{
			children.Remove((Node*)object);
		}
		else if (object->IsOfType<NodeComponent>())
		{
			components.Remove((NodeComponent*)object);
		}
	}

} // namespace CE
