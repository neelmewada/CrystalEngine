#pragma once

namespace CE
{
    
	struct CORE_API AssetPath
	{
	public:

		AssetPath() = default;

		AssetPath(const Name& bundlePath, const Name& assetName);

		inline const Name& GetBundlePath() const
		{
			return bundlePath;
		}

		inline const Name& GetAssetName() const
		{
			return assetName;
		}

		inline Name GetFullName() const
		{
			return bundlePath.GetString() + "." + assetName.GetString();
		}
        
		// Path of bundle
		Name bundlePath{};

		// Path of asset within the bundle
		Name assetName{};
	};

	template<>
	inline SIZE_T GetHash<AssetPath>(const AssetPath& value)
	{
		if (value.GetBundlePath().GetHashValue() == 0 && value.GetAssetName().GetHashValue() == 0)
			return 0;
		return GetCombinedHash(value.GetBundlePath().GetHashValue(), value.GetAssetName().GetHashValue());
	}

} // namespace CE
