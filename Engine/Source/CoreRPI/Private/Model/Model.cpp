#include "CoreRPI.h"

namespace CE::RPI
{

    Model::Model()
    {

    }

    Model::~Model()
    {
		for (RPI::ModelLod* lod : lods)
		{
			delete lod;
		}
		lods.Clear();
    }

} // namespace CE::RPI
