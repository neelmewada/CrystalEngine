#pragma once

namespace CE
{

	class CORE_API TextBlob
	{
	public:
		
		TextBlob();

		~TextBlob();
		

	private:

		b8 isAllocated = false;

		char* data = nullptr;
		
		// String length excluding null terminator
		u32 textLength = 0;

	};
    
} // namespace CE
