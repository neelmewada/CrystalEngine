#pragma once

namespace CE
{
	namespace Editor
	{
		class AssetManager;
	}

	struct CORE_API AssetDatabaseEntry
	{
		enum Type
		{
			None = 0,
			Directory,
			Asset
		};

		~AssetDatabaseEntry()
		{
			for (auto child : children)
			{
				delete child;
			}
			children.Clear();
		}

		IO::Path relativePath{};
		String displayName{};
		Type type = None;

		Array<AssetDatabaseEntry*> children{};
		AssetDatabaseEntry* parent = nullptr;
	};
    
	class CORE_API AssetDatabase : public Object
	{
		CE_CLASS(AssetDatabase, Object)
	private:

		AssetDatabase();

	public:

		virtual ~AssetDatabase();

		void ReloadDatabase();

	private:

		friend class CE::Editor::AssetManager;

		AssetDatabaseEntry* rootEntry = nullptr;
	};

} // namespace CE


CE_RTTI_CLASS(CORE_API, CE, AssetDatabase,
	CE_SUPER(CE::Object),
	CE_NOT_ABSTRACT,
	CE_ATTRIBS(),
	CE_FIELD_LIST(),
	CE_FUNCTION_LIST()
)
