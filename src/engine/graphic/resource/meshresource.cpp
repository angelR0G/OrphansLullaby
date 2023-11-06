#include "meshresource.hpp"
#include <memory>

#include "materialresource.hpp"
#include "resourceManager.hpp"
#include "programresource.hpp"
#include "../engine.hpp"

#include "../utils/assimp_helper.hpp"
#include "animations/bone.hpp"


MeshResource::MeshResource() : boundingBox{0, 0} {}

MeshResource::~MeshResource(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    material = nullptr;
}

void MeshResource::draw(){

    // bind appropriate textures
    size_t numTextures {material->getTexturesNumber()};
    glActiveTexture(GL_TEXTURE0);
    for(unsigned int i = 0; i < numTextures; ++i) {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding

        //Get texture map type
        std::string name = material->getTexture(i).type;

        //If exist a map for a type of texture load in to the shader the map
        if(name.compare("noTexture") != 0){
            glUniform1i(uTextures[i].texture, i);
            glUniform1f(uTextures[i].coefficient, 0.f);
            glUniform1i(uTextures[i].useTexture, true);
            // now set the sampler to the correct texture unit
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, material->getTexture(i).id);
        }else{
            //Load a coefficient for the type of material in case of no map for texture
            glUniform1i(uTextures[i].texture, i);
            glUniform1f(uTextures[i].coefficient, material->getTextureCoeficient(i));
            glUniform1i(uTextures[i].useTexture, false);
        }
    }
    
    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);

   /*glBindTexture(GL_TEXTURE_2D, material->getTexture(0).id);
    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);*/
}

void MeshResource::setupMesh(){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    //glBindVertexArray(0);

    // ids
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

    // weights
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    glBindVertexArray(0);

    if(uModel == -1){
        ProgramResource* program = ResourceManager::Instance()->getProgram(PROGRAM_DEFAULT);
        glUseProgram(program->getProgramId());
        uModel      = glGetUniformLocation(program->getProgramId(), "model");
        uModelInv   = glGetUniformLocation(program->getProgramId(), "modelInverse");

        //Locate all uniforms for the textures
        uTextures[0].texture        = glGetUniformLocation(program->getProgramId(), "tDiffuse.sTexture");
        uTextures[0].coefficient    = glGetUniformLocation(program->getProgramId(), "tDiffuse.coefficient");
        uTextures[0].useTexture     = glGetUniformLocation(program->getProgramId(), "tDiffuse.useTexture");

        uTextures[1].texture        = glGetUniformLocation(program->getProgramId(), "tRoughness.sTexture");
        uTextures[1].coefficient    = glGetUniformLocation(program->getProgramId(), "tRoughness.coefficient");
        uTextures[1].useTexture     = glGetUniformLocation(program->getProgramId(), "tRoughness.useTexture");

        uTextures[2].texture        = glGetUniformLocation(program->getProgramId(), "tMetallic.sTexture");
        uTextures[2].coefficient    = glGetUniformLocation(program->getProgramId(), "tMetallic.coefficient");
        uTextures[2].useTexture     = glGetUniformLocation(program->getProgramId(), "tMetallic.useTexture");

        uTextures[3].texture        = glGetUniformLocation(program->getProgramId(), "tNormal.sTexture");
        uTextures[3].coefficient    = glGetUniformLocation(program->getProgramId(), "tNormal.coefficient");
        uTextures[3].useTexture     = glGetUniformLocation(program->getProgramId(), "tNormal.useTexture");

        uTextures[4].texture        = glGetUniformLocation(program->getProgramId(), "tEmissive.sTexture");
        uTextures[4].coefficient    = glGetUniformLocation(program->getProgramId(), "tEmissive.coefficient");
        uTextures[4].useTexture     = glGetUniformLocation(program->getProgramId(), "tEmissive.useTexture");
    }
    
    ProgramResource* program = ResourceManager::Instance()->getProgram(PROGRAM_DEFAULT);
    uModel      = glGetUniformLocation(program->getProgramId(), "model");
    uModelInv   = glGetUniformLocation(program->getProgramId(), "modelInverse");

    program     = ResourceManager::Instance()->getProgram(PROGRAM_SHADOWS);
    uModelShadow    = glGetUniformLocation(program->getProgramId(), "model");

    program     = ResourceManager::Instance()->getProgram(PROGRAM_POINT_SHADOWS);
    uModelPointShadow   = glGetUniformLocation(program->getProgramId(), "model");
}

void MeshResource::createMesh(std::string path){
    //Create new mesh
    name = path;
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenBoundingBoxes);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        throw std::runtime_error("ERROR::ASSIMP\n");
    }
    //aiMesh* assimpMesh = scene->mMeshes[scene->mRootNode->mMeshes[0]];
    aiMesh* assimpMesh = scene->mMeshes[0];

    //Get the mesh bounding box
    aiAABB bbox = assimpMesh->mAABB;

    boundingBox = {bbox.mMin.x, bbox.mMin.y, bbox.mMin.z, bbox.mMax.x, bbox.mMax.y, bbox.mMax.z};
    loadMesh(assimpMesh, scene);
}

const AABBBoundingBox MeshResource::getBoundingBox() const noexcept{
    return boundingBox;
}

void MeshResource::loadMesh(aiMesh* assimpMesh, const aiScene* assimpScene){

    for(unsigned int i = 0; i < assimpMesh->mNumVertices; i++)
    {
        Vertex vertex;
        setVertexBoneDataToDefault(vertex);
        glm::vec3 vector;
        
        // positions
        vector.x = assimpMesh->mVertices[i].x;
        vector.y = assimpMesh->mVertices[i].y;
        vector.z = assimpMesh->mVertices[i].z;
        vertex.Position = vector;

        // normals
        if (assimpMesh->HasNormals()){
            vector.x = assimpMesh->mNormals[i].x;
            vector.y = assimpMesh->mNormals[i].y;
            vector.z = assimpMesh->mNormals[i].z;
            vertex.Normal = vector;
        }

        // texture coordinates
        if(assimpMesh->mTextureCoords[0]){
            glm::vec2 vec;
            vec.x = assimpMesh->mTextureCoords[0][i].x; 
            vec.y = assimpMesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < assimpMesh->mNumFaces; i++){
        aiFace face = assimpMesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);        
    }
    
    //load bones data
    extractBoneWeightForVertices(vertices, assimpMesh, assimpScene);
    setupMesh();

    // mat.setTranslation(m_Positions[0].position);
    // mat.setRotation(glm::eulerAngles(m_Rotations[0].orientation));
    // mat.setScale(m_Scales[0].scale);


}

// aiMaterial* MeshResource::getMaterial(){
//     return mat;
// }

void MeshResource::setMaterial(MaterialResource* mat){
    material = mat;
}

GLint MeshResource::getModelUniform() noexcept{
    return uModel;
}

GLint MeshResource::getModelInvUniform() noexcept{
    return uModelInv;
}

void MeshResource::setVertexBoneDataToDefault(Vertex& vertex)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        vertex.m_BoneIDs[i] = -1;
        vertex.m_Weights[i] = 0.0f;
    }
}
void MeshResource::setVertexBoneData(Vertex& vertex, int boneID, float weight) {
    for (unsigned int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
        if (vertex.m_BoneIDs[i] < 0) {
            vertex.m_Weights[i] = weight;
            vertex.m_BoneIDs[i] = boneID;
            break;
        }
    }
}

void MeshResource::extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene) {
    BoneInfoMap& boneInfoMap = m_BoneInfoMap;
    int& boneCount = m_BoneCounter;

    //Store all bones in the mesh
    for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCount;
            newBoneInfo.offset = AssimpGLMHelpers::convertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            boneInfoMap[boneName] = newBoneInfo;
            boneID = boneCount;
            boneCount++;
        }
        else {
            boneID = boneInfoMap[boneName].id;
        }

        assert(boneID != -1);
        
        aiVertexWeight* weights = mesh->mBones[boneIndex]->mWeights;
        unsigned int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (unsigned int weightIndex{0}; weightIndex < numWeights; ++weightIndex) {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;

            assert((unsigned int)vertexId <= vertices.size());
            setVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}

using BoneInfoMap = std::unordered_map<std::string, BoneInfo>;
BoneInfoMap MeshResource::getBoneInfoMap() { 
    return m_BoneInfoMap; 
}

int MeshResource::getBoneCount() { 
    return m_BoneCounter;
}
GLint MeshResource::getModelShadowUniform() noexcept{
    return uModelShadow;
}

GLint MeshResource::getModelPointShadowUniform() noexcept {
    return uModelPointShadow;
}