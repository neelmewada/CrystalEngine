#include "System.h"

namespace CE
{
    void Mesh::Clear()
    {
		materialSlotIndex = 0;
		vertices.Clear();
		uvCoords.Clear();
		normals.Clear();
		tangents.Clear();
		indices.Clear();
		vertexColors.Clear();
    }

    void Mesh::Release()
    {
		Clear();
    }

} // namespace CE
