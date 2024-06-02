#pragma once

namespace CE
{
	class FWidget;

	class FUSIONCORE_API FusionException : public CE::Exception
	{
	public:

		FusionException(const String& message, FWidget* sender = nullptr) : Exception(message), sender(sender)
		{}

		FWidget* GetSender() const { return sender; }


	private:

		FWidget* sender = nullptr;

	};

} // namespace CE
