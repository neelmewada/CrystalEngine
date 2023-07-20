#include "System.h"

namespace CE
{
	PathTreeNode::~PathTreeNode()
	{
		if (parent != nullptr)
			parent->children.Remove(this);

		RemoveAll();
	}

	bool PathTreeNode::ChildExists(const String& name)
	{
		for (auto child : children)
		{
			if (child != nullptr && child->name == name)
				return true;
		}
		return false;
	}

	PathTreeNode* PathTreeNode::GetOrAddChild(const String& name, PathTreeNodeType type)
	{
		for (auto child : children)
		{
			if (child != nullptr && child->name == name)
				return child;
		}
		children.Add(new PathTreeNode());
		children.Top()->name = name;
		children.Top()->nodeType = type;
		children.Top()->parent = this;
		return children.Top();
	}

	PathTreeNode* PathTreeNode::GetChild(const String& name)
	{
		for (auto child : children)
		{
			if (child != nullptr && child->name == name)
				return child;
		}
		return nullptr;
	}

	void PathTreeNode::RemoveAll()
	{
		for (int i = children.GetSize() - 1; i >= 0; i--)
		{
			delete children[i];
		}
		children.Clear();
	}

	PathTree::PathTree()
	{
		rootNode = new PathTreeNode();
		rootNode->name = "/";
		rootNode->nodeType = PathTreeNodeType::Directory;
	}

	PathTree::~PathTree()
	{
		delete rootNode;
		rootNode = nullptr;
	}

	bool PathTree::AddPath(const Name& path)
	{
		if (!path.IsValid())
			return false;

		String pathStr = path.GetString();
		if (!pathStr.StartsWith("/"))
			return false;

		Array<String> components = {};
		pathStr.Split(Array<String>{ "/" , "\\"}, components);

		PathTreeNode* curNode = rootNode;

		for (int i = 0; i < components.GetSize(); i++)
		{
			const String& component = components[i];
			bool isLast = i == components.GetSize() - 1;
			
			if (!isLast || !component.Contains('.'))
			{
				curNode = curNode->GetOrAddChild(component, PathTreeNodeType::Directory);
			}
			else
			{
				curNode = curNode->GetOrAddChild(component, PathTreeNodeType::Asset);
			}
		}

		return true;
	}

	bool PathTree::RemovePath(const Name& path)
	{
		if (!path.IsValid())
			return false;

		String pathStr = path.GetString();
		if (!pathStr.StartsWith("/") || pathStr == "/")
			return false;

		Array<String> components = pathStr.Split('/');

		PathTreeNode* curNode = rootNode;

		for (int i = 0; i < components.GetSize(); i++)
		{
			if (curNode == nullptr)
				return false;

			const String& component = components[i];

			curNode = curNode->GetChild(component);
		}

		if (curNode == rootNode)
			return false;

		delete curNode;
		return true;
	}

	void PathTree::RemoveAll()
	{
		rootNode->RemoveAll();
	}

	PathTreeNode* PathTree::GetNode(const Name& path)
	{
		if (!path.IsValid())
			return nullptr;

		String pathStr = path.GetString();
		if (!pathStr.StartsWith("/"))
			return nullptr;
		if (pathStr == "/")
			return rootNode;

		Array<String> components = pathStr.Split('/');

		PathTreeNode* curNode = rootNode;

		for (int i = 0; i < components.GetSize(); i++)
		{
			if (curNode == nullptr)
				return nullptr;

			const String& component = components[i];

			curNode = curNode->GetChild(component);
		}

		if (curNode == rootNode)
			return nullptr;

		return curNode;
	}
	
} // namespace CE
