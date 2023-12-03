#include "CoreRPI.h"

namespace CE::RPI
{

    Model::Model()
    {

    }

    Model::~Model()
    {
		for (auto lod : lods)
		{
			delete lod;
		}
		lods.Clear();
    }

} // namespace CE::RPI
