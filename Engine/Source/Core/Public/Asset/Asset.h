#pragma once

namespace CE
{
    
	class CORE_API Asset : public Object
	{
		CE_CLASS(Asset, CE::Object)
	public:

		bool IsAsset() override { return true; }

		INLINE const IO::Path& GetSourceAssetRelativePath() const
		{
			return sourceAssetRelativePath;
		}

	private:
		
		IO::Path sourceAssetRelativePath{};
	};

}

CE_RTTI_CLASS(CORE_API, CE, Asset,
	CE_SUPER(CE::Object),
	CE_ABSTRACT,
	CE_ATTRIBS(Abstract),
	CE_FIELD_LIST(
		CE_FIELD(sourceAssetRelativePath)
	),
	CE_FUNCTION_LIST()
)
