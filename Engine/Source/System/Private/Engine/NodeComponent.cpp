#include "System.h"

namespace CE
{
	NodeComponent::NodeComponent()
	{

	}

	NodeComponent::~NodeComponent()
	{

	}

	NodeComponent* NodeComponent::GetChildAt(u32 index)
	{
		if (index >= children.GetSize())
			return nullptr;
		return children[index];
	}

	void NodeComponent::OnSubobjectAttached(Object* object)
	{
		Super::OnSubobjectAttached(object);

		if (object == nullptr)
			return;
		
		if (object->IsOfType<NodeComponent>())
		{
			children.Add((NodeComponent*)object);
		}
	}

	void NodeComponent::OnSubobjectDetached(Object* object)
	{
		Super::OnSubobjectAttached(object);

		if (object == nullptr)
			return;
		
		if (object->IsOfType<NodeComponent>())
		{
			children.Remove((NodeComponent*)object);
		}
	}

} // namespace CE
