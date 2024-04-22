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

    Model* ModelAsset::GetModel()
    {
        if (model == nullptr)
            model = CreateModel();
        return model;
    }

    void ModelAsset::AddModelLod(ModelLodAsset* lod)
    {
        lods.Add(lod);
    }

} // namespace CE
