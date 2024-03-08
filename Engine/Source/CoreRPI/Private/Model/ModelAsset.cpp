#include "CoreRPI.h"

namespace CE::RPI
{

    Model* ModelAsset::CreateModel()
    {
        Model* model = new Model();
        for (int i = 0; i < lods.GetSize(); i++)
        {
            ModelLod* lod = lods[i]->CreateModelLod();
            model->lods.Add(lod);
        }
        return model;
    }

} // namespace CE
