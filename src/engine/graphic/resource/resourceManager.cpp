#include <stdexcept>

#include "resourceManager.hpp"
#include "meshresource.hpp"
#include "materialresource.hpp"
#include "textureresource.hpp"
#include "shaderresource.hpp"
#include "programresource.hpp"
#include "animationresource.hpp"

ResourceManager* ResourceManager::Instance(){
    if(pResourceManager.get() == nullptr){
        pResourceManager = std::unique_ptr<ResourceManager>(new ResourceManager());
    }

    return pResourceManager.get();
}

ResourceManager::ResourceManager(){}

ResourceManager::~ResourceManager(){

}

Resource* ResourceManager::getResource(std::string path){
    auto found = resources.find(path);
    Resource* resource{nullptr};

    if(found != resources.end()){
        resource = found->second.get();
    }
    return resource;
}


MeshResource* ResourceManager::getMesh(std::string path){
    Resource* resource = getResource(path);
    MeshResource* meshResource{nullptr};
    //std::shared_ptr<MeshResource> cResource = std::shared_ptr<MeshResource>(resource.get());
    if(resource == nullptr){
        //Insert the new mesh in the resource manager
        auto mesh = resources.insert({path, std::unique_ptr<MeshResource>(new MeshResource())});
        meshResource = static_cast<MeshResource*>(mesh.first->second.get()); 
        meshResource->createMesh(path);
    }else{
        meshResource = static_cast<MeshResource*>(resource);
    }

    return meshResource;
}

TextureResource* ResourceManager::getTexture(std::string path, std::string type){
    Resource* resource = getResource(path);
    TextureResource* textureResource{nullptr};
    
    if(resource == nullptr){
        //Insert the new texture in the resource manager
        auto mesh = resources.insert({path, std::unique_ptr<TextureResource>(new TextureResource())});
        textureResource = static_cast<TextureResource*>(mesh.first->second.get()); 
        textureResource->loadResource(path, type);
    }else{
        textureResource = static_cast<TextureResource*>(resource);
    }

    return textureResource;
}

MaterialResource* ResourceManager::getMaterial(std::string path){
    Resource* resource = getResource(path);
    MaterialResource* materialResource{nullptr};
    //std::shared_ptr<MeshResource> cResource = std::shared_ptr<MeshResource>(resource.get());
    if(resource == nullptr){
        //Insert the new mesh in the resource manager
        auto mesh = resources.insert({path, std::unique_ptr<MaterialResource>(new MaterialResource())});
        materialResource = static_cast<MaterialResource*>(mesh.first->second.get()); 
        materialResource->loadResource(path);
    }else{
        materialResource = static_cast<MaterialResource*>(resource);
    }

    return materialResource;
}

// TextureResource* ResourceManager::getTexture(std::string path, aiMaterial* mat, aiTextureType type, std::string typeS){
//     Resource* resource = getResource(path);
//     TextureResource* textureResource{nullptr};
    
//     if(resource == nullptr){
//         //Insert the new texture in the resource manager
//         auto mesh = resources.insert({path, std::unique_ptr<TextureResource>(new TextureResource())});
//         textureResource = static_cast<TextureResource*>(mesh.first->second.get()); 
//         textureResource->loadResource(path, mat, type, typeS);
//     }else{
//         textureResource = static_cast<TextureResource*>(resource);
//     }

//     return textureResource;
// }

ShaderResource* ResourceManager::getShader(std::string path, unsigned int type){
    Resource* resource = getResource(path);
    ShaderResource* shaderResource{nullptr};

    if(resource == nullptr){
        //Insert the new mesh in the resource manager
        auto shader = resources.insert({path, std::unique_ptr<ShaderResource>(new ShaderResource())});
        shaderResource = static_cast<ShaderResource*>(shader.first->second.get()); 
        shaderResource->loadShader(path, type);
    }else{
        shaderResource = static_cast<ShaderResource*>(resource);
    }

    return shaderResource;
}

ProgramResource* ResourceManager::getProgram(uint8_t p){
    ProgramResource* programResource{programs[p].get()};

    if(programs[p] == nullptr){
        programs[p] = std::unique_ptr<ProgramResource>(new ProgramResource());
        programResource = programs[p].get();
    }

    return programResource;
}

void ResourceManager::freeAllResources() {
    resources.clear();
    
    for(auto& p : programs)
        p = nullptr;
}

AnimationResource* ResourceManager::getAnimation(std::string path, MeshResource* mesh){
    Resource* resource = getResource(path);
    AnimationResource* animationResource{nullptr};
    //std::shared_ptr<MeshResource> cResource = std::shared_ptr<MeshResource>(resource.get());
    if(resource == nullptr){
        //Insert the new mesh in the resource manager
        auto anim = resources.insert({path, std::unique_ptr<AnimationResource>(new AnimationResource())});
        animationResource = static_cast<AnimationResource*>(anim.first->second.get()); 
        animationResource->loadResource(path, mesh);
    }else{
        animationResource = static_cast<AnimationResource*>(resource);
    }

    return animationResource;
}