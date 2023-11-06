#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <array>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//Forward declarations
struct Resource;
struct MeshResource;
struct TextureResource;
struct ShaderResource;
struct ProgramResource;
struct MaterialResource;
struct AnimationResource;

//Defines for different programs
#define PROGRAM_DEFAULT             0
#define PROGRAM_SKYBOX              1
#define PROGRAM_PARTICLES           2
#define PROGRAM_SHADOWS             3
#define PROGRAM_POINT_SHADOWS       4
#define PROGRAM_DEBUG_LINES         5
#define PROGRAM_VIDEO_PLAYER        6

struct ResourceManager{
    using resourcesMap      = std::unordered_map<std::string, std::unique_ptr<Resource>>;
    using programContainer  = std::array<std::unique_ptr<ProgramResource>, 7>;

    static ResourceManager* Instance();
    ~ResourceManager();
    ResourceManager(ResourceManager&) = delete;
    ResourceManager(const ResourceManager&) = delete;

    MeshResource*       getMesh     (std::string);
    MaterialResource*   getMaterial (std::string);
    TextureResource*    getTexture  (std::string, std::string type = "tDiffuse");
    TextureResource*    getTexture  (std::string, aiMaterial*, aiTextureType, std::string);
    ProgramResource*    getProgram  (uint8_t);
    ShaderResource*     getShader   (std::string, unsigned int type);
    AnimationResource*  getAnimation(std::string, MeshResource*);

    void freeAllResources();

    protected:
        ResourceManager();
    private:
        inline static std::unique_ptr<ResourceManager> pResourceManager{nullptr};
        programContainer programs{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
        resourcesMap resources;
        
        Resource* getResource(std::string);
};
        