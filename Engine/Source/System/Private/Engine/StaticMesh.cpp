#include "System.h"

namespace CE
{

    StaticMesh::StaticMesh()
    {
		modelData = CreateDefaultSubobject<ModelData>("ModelData");
    }

} // namespace CE
