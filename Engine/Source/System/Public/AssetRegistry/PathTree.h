#pragma once

namespace CE
{
	enum class PathTreeNodeType
	{
		Directory = 0,
		Asset
	};

	struct SYSTEM_API PathTreeNode
	{
		~PathTreeNode();

		bool ChildExists(const String& name);

		PathTreeNode* GetOrAddChild(const String& name, PathTreeNodeType type = PathTreeNodeType::Directory);

		PathTreeNode* GetChild(const String& name);

		void RemoveAll();

		inline bool IsTerminal() const
		{
			return children.IsEmpty();
		}

		Name name{};
		PathTreeNodeType nodeType{};

		Array<PathTreeNode*> children{};
		PathTreeNode* parent = nullptr;
	};
    
	class SYSTEM_API PathTree
	{
	public:

		PathTree();
		~PathTree();

		bool AddPath(const Name& path);

		bool RemovePath(const Name& path);

		void RemoveAll();

		PathTreeNode* GetNode(const Name& path);

		inline PathTreeNode* GetRootNode() const { return rootNode; }

	private:

		PathTreeNode* rootNode = nullptr;
	};

} // namespace CE
