//
// Created by Echo on 24-12-20.
//

#include "Mesh.h"

#include "MeshMeta.h"
#include "Resource/AssetDataBase.h"
void resource::Mesh::PerformLoad()
{
    auto op_meta = AssetDataBase::QueryMeta<MeshMeta>(GetHandle());
    if (op_meta)
    {

    }
}