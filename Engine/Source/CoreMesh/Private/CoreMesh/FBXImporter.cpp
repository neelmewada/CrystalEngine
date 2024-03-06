#include "CoreMesh.h"

#include "ofbx.h"

namespace CE
{

    
    FbxImporter::FbxImporter()
    {

    }

    FbxImporter::~FbxImporter()
    {

    }

    inline Vec4 ToVec4(ofbx::Vec4 v)
    {
        return Vec4(v.x, v.y, v.z);
    }

    inline Vec3 ToVec3(ofbx::Vec3 v)
    {
        return Vec3(v.x, v.y, v.z);
    }

    inline Vec3 ToVec3(ofbx::DVec3 v)
    {
        return Vec3(v.x, v.y, v.z);
    }

    inline Vec2 ToVec2(ofbx::Vec2 v)
    {
        return Vec2(v.x, v.y);
    }

    inline Matrix4x4 ToMatrix(const ofbx::DMatrix& mat)
    {
        Matrix4x4 v{};
        
        for (int x = 0; x < 4; x++)
        {
            v.rows[x] = Vec4(mat.m[x * 4 + 0], mat.m[x * 4 + 1], mat.m[x * 4 + 2], mat.m[x * 4 + 3]);
        }

        return v;
    }

    CMScene* FbxImporter::LoadScene(const u8* fbxData, int dataSize, const MeshImportConfig& config)
    {
        CMScene* scene = nullptr;
        success = false;
        errorMessage = "";

        defer(
            if (!success)
            {
                CE_LOG(Error, All, "Failed to load FBX file. {}", errorMessage);
            }
        );

        ofbx::IScene* fbxScene = ofbx::load(fbxData, dataSize, (u16)config.loadFlags);
        if (fbxScene == nullptr)
        {
            errorMessage = "FBX Loader failed!";
            return nullptr;
        }

        scene = new CMScene();

        int totalLights = fbxScene->getLightCount();

        for (int i = 0; i < totalLights; i++)
        {
            auto fLight = fbxScene->getLight(i);

            CMLight light{};
            light.color = Color(fLight->getColor().r, fLight->getColor().g, fLight->getColor().b, 1.0);
            light.castLight = fLight->doesCastLight();
            light.intensity = fLight->getIntensity();

            light.fog = fLight->getFog();

            light.innerAngle = fLight->getInnerAngle();
            light.outerAngle = fLight->getOuterAngle();

            light.castShadows = fLight->doesCastShadows();
            light.decayStart = fLight->getDecayStart();
            light.decayType = (CMDecayType)fLight->getDecayType();

            light.enableNearAttenuation = fLight->doesEnableNearAttenuation();
            light.nearAttenuationStart = fLight->getNearAttenuationStart();
            light.nearAttenuationEnd = fLight->getNearAttenuationEnd();

            light.enableFarAttenuation = fLight->doesEnableFarAttenuation();
            light.farAttenuationStart = fLight->getFarAttenuationStart();
            light.farAttenuationEnd = fLight->getFarAttenuationEnd();

            light.castShadows = fLight->doesCastShadows();
            light.shadowColor = Color(fLight->getShadowColor().r, fLight->getShadowColor().g, fLight->getShadowColor().b, 1.0f);

            light.localTransform = ToMatrix(fLight->getLocalTransform());
            light.globalTransform = ToMatrix(fLight->getGlobalTransform());
            light.localTranslation = ToVec3(fLight->getLocalTranslation());
            light.localRotation = ToVec3(fLight->getLocalRotation());
            light.localScaling = ToVec3(fLight->getLocalScaling());

            scene->lights.Add(light);
        }

        int meshCount = fbxScene->getMeshCount();

        for (int i = 0; i < meshCount; i++)
        {
            scene->meshes.Add({});

            CMMesh& outMesh = scene->meshes.Top();

            const ofbx::Mesh* mesh = fbxScene->getMesh(i);
            outMesh.localTransform = ToMatrix(mesh->getLocalTransform());
            outMesh.globalTransform = ToMatrix(mesh->getGlobalTransform());
            outMesh.localTranslation = ToVec3(mesh->getLocalTranslation());
            outMesh.localRotation = ToVec3(mesh->getLocalRotation());
            outMesh.localScaling = ToVec3(mesh->getLocalScaling());

            const ofbx::GeometryData& geom = mesh->getGeometryData();
            const ofbx::Vec3Attributes positions = geom.getPositions();
            const ofbx::Vec3Attributes normals = geom.getNormals();
            const ofbx::Vec3Attributes tangents = geom.getTangents();
            const ofbx::Vec4Attributes colors = geom.getColors();
            const ofbx::Vec2Attributes uvs = geom.getUVs();
            const ofbx::Vec2Attributes uvs1 = geom.getUVs(1);
            u32 indicesOffset = 0;

            // each ofbx::Mesh can have several materials == partitions == submeshes
            for (int partitionIdx = 0; partitionIdx < geom.getPartitionCount(); partitionIdx++)
            {
                const ofbx::GeometryPartition& partition = geom.getPartition(partitionIdx);

                outMesh.submeshes.Add({});
                CMMeshPartition& submesh = outMesh.submeshes.Top();

                // partitions most likely have several polygons, they are not triangles necessarily, use ofbx::triangulate if you want triangles
                for (int polygonIdx = 0; polygonIdx < partition.polygon_count; ++polygonIdx)
                {
                    const ofbx::GeometryPartition::Polygon& polygon = partition.polygons[polygonIdx];

                    int* triIndices = new int[polygon.vertex_count * 3];

                    u32 triCount = ofbx::triangulate(geom, polygon, triIndices);

                    for (int i = 0; i < triCount; i++)
                    {
                        submesh.indices.Add(triIndices[i] + indicesOffset);

                        Vec3 pos = ToVec3(positions.get(triIndices[i]));
                        outMesh.positions.Add(pos);

                        if (tangents.values)
                        {
                            Vec3 tangent = ToVec3(tangents.get(triIndices[i]));
                            outMesh.tangents.Add(tangent);
                        }

                        if (normals.values)
                        {
                            Vec3 normal = ToVec3(normals.get(triIndices[i]));
                            outMesh.normals.Add(normal);
                        }

                        if (colors.values)
                        {
                            Vec4 color = ToVec4(colors.get(triIndices[i]));
                            outMesh.colors.Add(color);
                        }

                        if (uvs.values)
                        {
                            Vec2 uv = ToVec2(uvs.get(triIndices[i]));
                            outMesh.uvs[0].Add(uv);
                        }

                        if (uvs1.values)
                        {
                            Vec2 uv = ToVec2(uvs1.get(triIndices[i]));
                            outMesh.uvs[1].Add(uv);
                        }
                    }

                    delete[] triIndices;
                }

                indicesOffset += partition.triangles_count;
            }
        }

        fbxScene->destroy();

        success = true;
        return scene;
    }

} // namespace CE
