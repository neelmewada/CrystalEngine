#pragma once

namespace CE
{
    
	struct CORE_API AssetPath
	{
	public:

		AssetPath() = default;

		AssetPath(const Name& packagePath, const Name& assetName);

		inline const Name& GetPackagePath() const
		{
			return packagePath;
		}

		inline const Name& GetAssetName() const
		{
			return assetName;
		}

		inline Name GetFullName() const
		{
			return packagePath.GetString() + "." + assetName.GetString();
		}

	private:

		// Path of package
		Name packagePath{};

		// Path of asset within the package
		Name assetName{};
	};

	template<>
	inline SIZE_T GetHash<AssetPath>(const AssetPath& value)
	{
		if (value.GetPackagePath().GetHashValue() == 0 && value.GetAssetName().GetHashValue() == 0)
			return 0;
		return GetCombinedHash(value.GetPackagePath().GetHashValue(), value.GetAssetName().GetHashValue());
	}

} // namespace CE
