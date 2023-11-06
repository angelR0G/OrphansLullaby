#pragma once

#include <vector>
#include <unordered_map>
#include <array>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../sceneTree/transform.hpp"
#include "resource.hpp"
#include "vertex.hpp"
#include "texture.hpp"
#include "../../../utils/bbox.hpp"

#include <glad/glad.h>

#include "animations/bone.hpp"

struct MaterialResource;
/*
struct BBVec3D{
    float x{}, y{}, z{};
    glm::vec3 convertBBVecToVec3(){
        glm::vec3{x, y, z};
    }

    BBVec3D operator+(const BBVec3D bsum){
        return BBVec3D{x+bsum.x, y+bsum.y, z+bsum.z};
    }
    BBVec3D operator-(const BBVec3D bsum){
        return BBVec3D{x-bsum.x, y-bsum.y, z-bsum.z};
    } 
    BBVec3D operator*(const float scalar){
        return BBVec3D{x*scalar, y*scalar, z*scalar};
    }
};

struct BoundingBox{
    BBVec3D min{}, max{};
    glm::vec3 center{}, extends{};
    std::array<glm::vec3, 8> verticesBB{};
};
*/
struct TextureUniform{
    GLint   texture{-1},
            coefficient{-1},
            useTexture{-1};
};

struct MeshResource : public Resource{
    friend struct ResourceManager;
    public:
    using BoneInfoMap = std::unordered_map<std::string, BoneInfo>;
        MeshResource();
        ~MeshResource();
        void draw();
        aiMaterial* getMaterial();
        [[nodiscard]] static GLint getModelUniform() noexcept;
        [[nodiscard]] static GLint getModelInvUniform() noexcept;
        [[nodiscard]] static GLint getModelShadowUniform() noexcept;
        [[nodiscard]] static GLint getModelPointShadowUniform() noexcept;
        void setMaterial(MaterialResource*);

        //Load bones data from the model for animation
        void extractBoneWeightForVertices(std::vector<Vertex>&, aiMesh*, const aiScene*);
        void setVertexBoneData(Vertex&, int, float);
        void setVertexBoneDataToDefault(Vertex&);

        BoneInfoMap getBoneInfoMap();
	    int getBoneCount();
        [[nodiscard]] const AABBBoundingBox getBoundingBox() const noexcept;
    private:
        // mesh data
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        //Mesh material
        MaterialResource* material;
        AABBBoundingBox boundingBox;

        //Bones data
        BoneInfoMap m_BoneInfoMap;
	    int m_BoneCounter = 0;
        std::vector<KeyPosition> m_Positions;
        std::vector<KeyRotation> m_Rotations;
        std::vector<KeyScale> m_Scales;
        // GE::Transform mat;
        

        // render data
        unsigned int VAO, VBO, EBO;
        void setupMesh();
        void createMesh(std::string);
        void loadMesh(aiMesh* assimpMesh, const aiScene* assimpScene);
        inline static GLint     uModel      {-1},
                                uModelInv   {-1},
                                uModelShadow{-1},
                                uModelPointShadow{-1};
        inline static std::array<TextureUniform, 5> uTextures{-1, -1, -1, -1, -1};      
};