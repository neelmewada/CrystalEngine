#pragma once

namespace CE
{
    
	struct CORE_API AssetPath
	{
	public:

		AssetPath() = default;

		AssetPath(const Name& packageName, const Name& assetName);

		inline const Name& GetPackageName() const
		{
			return packageName;
		}

		inline const Name& GetAssetName() const
		{
			return assetName;
		}

		inline Name GetFullName() const
		{
			return packageName.GetString() + "." + assetName.GetString();
		}

	private:

		// Path of package
		Name packageName{};

		// Path of asset within the package
		Name assetName{};
	};

} // namespace CE
