#pragma once

namespace CE
{

	class CORE_API BundleSerializer final
	{
	public:

		BundleSerializer();

		virtual ~BundleSerializer();

	private:

		friend class Bundle;
	};

} // namespace CE
