#include "CoreMinimal.h"

namespace CE
{

	Variant::~Variant()
	{
		
	}

	void Variant::CopyFrom(const Variant& copy)
	{
		memcpy(this, &copy, sizeof(Variant));
	}

	void Variant::Free()
	{
		
	}
    
} // namespace CE
