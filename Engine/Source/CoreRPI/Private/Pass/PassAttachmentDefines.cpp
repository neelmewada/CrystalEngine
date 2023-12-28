#include "CoreRPI.h"

namespace CE::RPI
{

	void PassDefinition::OnAfterDeserialize()
	{
		slotNamesToIndex.Clear();
		imageAttachmentNameToIndex.Clear();
		bufferAttachmentNameToIndex.Clear();

		for (int i = 0; i < slots.GetSize(); i++)
		{
			slotNamesToIndex[slots[i].name] = i;
		}

		for (int i = 0; i < imageAttachments.GetSize(); i++)
		{
			imageAttachmentNameToIndex[slots[i].name] = i;
		}

		for (int i = 0; i < bufferAttachments.GetSize(); i++)
		{
			bufferAttachmentNameToIndex[slots[i].name] = i;
		}
	}

} // namespace CE::RPI
