#include "sceneMesh.hpp"
#include "../resource/resourceManager.hpp"
#include "../resource/meshresource.hpp"
#include "../resource/shaderresource.hpp"
#include "../resource/animationresource.hpp"
#include "../frustrumPlane.hpp"

GE::SceneMesh::SceneMesh() : SceneEntity(){}

void GE::SceneMesh::loadMesh(std::string path){
    ResourceManager* resourceMan = ResourceManager::Instance();
    mesh = resourceMan->getMesh(path);

    findLODData(path);

    if(normalShader == nullptr) {
        normalShader    = ResourceManager::Instance()->getShader("media/shaders/vertex.glsl", GL_VERTEX_SHADER);
        shadowShader    = ResourceManager::Instance()->getShader("media/shaders/vertexShadow.glsl", GL_VERTEX_SHADER);
    }
}

void GE::SceneMesh::resetSceneMesh() noexcept{
    normalShader = nullptr;
    shadowShader = nullptr;
}

void GE::SceneMesh::draw(const glm::mat4& modelMatrix, ShaderResource* shader){
    // If mesh is visible, draw it
    if(visible) {
        // Get mesh to be drawn
        MeshResource* drawMesh {getLODMesh()};
    
        if(shader == normalShader) {
            // Normal shader
            shader->setUniformMat4(drawMesh->getModelUniform(), modelMatrix);

            glm::mat4 modelInverse {glm::inverse(modelMatrix)};
            shader->setUniformMat4(drawMesh->getModelInvUniform(), modelInverse);

            sendBoneMatrices(0, shader);
        }
        else if (shader == shadowShader){
            if(!castShadows) return;

            // Shadow shader
            shader->setUniformMat4(drawMesh->getModelShadowUniform(), modelMatrix);
            sendBoneMatrices(1, shader);
            
        }
        else {
            if(!castShadows) return;
            
            // Point shadows shader
            shader->setUniformMat4(drawMesh->getModelPointShadowUniform(), modelMatrix);
            sendBoneMatrices(2, shader);
        }
        
        drawMesh->draw();
    }
}

AABBBoundingBox GE::SceneMesh::getBoundingBox() const noexcept {
    return mesh->getBoundingBox();
}

AABBBoundingBox GE::SceneMesh::getTransformedBoundingBox() noexcept {
    glm::mat4       meshTransform   {transform.getTransformMatrix()};
    AABBBoundingBox bbox            {mesh->getBoundingBox()};
    const glm::vec3 globalCenter    {meshTransform * glm::vec4(bbox.center.x, bbox.center.y, bbox.center.z, 1.f) };

    // Scaled orientation
    const glm::vec3 right   = meshTransform[0] * bbox.extent.x;
    const glm::vec3 up      = meshTransform[1];
    const glm::vec3 forward = -meshTransform[2] * bbox.extent.z;

    const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

    const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

    const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

    bbox.center    = {globalCenter.x, globalCenter.y, globalCenter.z};
    bbox.extent    = {newIi, newIj, newIk};

    bbox.calculatePoints();

    return bbox;
}

MeshResource* GE::SceneMesh::getLODMesh() const noexcept {
    MeshResource* m {mesh};

    // Check if exist LOD mesh data
    if(meshLOD.size() > 0) {
        // Calculate distance from reference position to object
        auto objectPosition {getWorldLocation()};

        float dist {distance(objectPosition, lodReferencePosition)};

        // Select appropriate LOD mesh due to distance
        size_t index{0};
        while(index < meshLOD.size() && meshLOD[index].first < dist) {
            m = meshLOD[index].second;

            ++index;
        }
    }

    return m;
}

void GE::SceneMesh::setLODReferencePosition(float x, float y, float z) noexcept {
    lodReferencePosition = {x, y, z};
}

void GE::SceneMesh::asignMaterial(MaterialResource* mat){
    mesh->setMaterial(mat);

    for(auto m : meshLOD) 
        m.second->setMaterial(mat);
}

void GE::SceneMesh::asignAnimation(bool active, AnimationResource* anim){
    animations.push_back({active, anim});
}

void GE::SceneMesh::setVisible(bool v) {
    visible = v;
}

void GE::SceneMesh::setCastShadows(bool cast) {
    castShadows = cast;
}

bool GE::SceneMesh::getVisible() {
    return visible;
}

MeshResource* GE::SceneMesh::getMesh(){
    return mesh;
}

std::vector<AnimationResource*> GE::SceneMesh::getAnimation(){
    std::vector<AnimationResource*> anims{};
    for(size_t i{}; i < animations.size(); ++i){
        anims.push_back(animations[i].animation);
    }
    return anims;
}

void GE::SceneMesh::findLODData(std::string path) noexcept {
    // Search LOD file
    auto pos {path.find_last_of('.')};

    if(pos == std::string::npos) return;

    // Ej. from wall.obj search wall.txt
    path = path.substr(0, pos);
    path.append(".txt");

    // Open file
    std::ifstream readFile;
    try {
        readFile.open(path);

        if (!readFile.fail() && readFile.is_open()) {
            ResourceManager*    resourceMan {ResourceManager::Instance()};
            std::string         line        {};
            size_t              nextPos     {};
            bool                nextData    {};
            std::vector<std::string> data   {};
    

            while (readFile.good()) {
                // While there are lines to read, continues
                std::getline(readFile, line);

                // Split line for spaces
                data.clear();
                do {
                    nextPos     = line.find(' ');
                    nextData    = nextPos != line.npos;

                    // If there is a space, split the two parts
                    if (nextData) {
                        data.emplace_back(line.substr(0, nextPos));
                        line = line.substr(nextPos+1);
                    }
                } while(nextData);
                data.emplace_back(line);

                // Check there are two strings (file LODDistance)
                if(data.size() < 2) continue;

                std::pair<float, MeshResource*> newLODData {};

                // Search mesh file
                newLODData.first    = std::stof(data[1]);
                newLODData.second   = resourceMan->getMesh(data[0]);

                // Save LOD data, checking distance order is correct
                auto it {meshLOD.begin()};
                for(it; it < meshLOD.end(); ++it) {
                    if(newLODData.first < it->first) {
                        // If distance is lower than another LOD, insert it there
                        meshLOD.insert(it, newLODData);
                        break;
                    }
                }

                if(it >= meshLOD.end()) {
                    // Distance is greater than other LODs, save it at the end
                    meshLOD.emplace_back(newLODData);
                }
            }

            // Close file
            readFile.close();
        }

    }catch(std::exception const& e) {
        if(readFile.is_open()) readFile.close();
    }
}


void GE::SceneMesh::sendBoneMatrices(uint8_t sLoc, ShaderResource* shader){
    if(animations.size() != 0){
        bool appliedTransform{false};
        for(unsigned int i{0}; i < animations.size() && !appliedTransform; ++i){
            if(animations[i].animation != nullptr && animations[i].active){
                auto mBones = animations[i].m_FinalBoneMatrices;
                auto mLoc   = animations[i].animation->getUniformBones(sLoc);
                for (unsigned int i = 0; i < mBones.size(); ++i){
                    shader->setUniformMat4(mLoc[i], mBones[i]);
                }
                appliedTransform = true;
            }
        }
    }
}

void GE::SceneMesh::setUseAnimation(bool act, int animPos){
    animations[animPos].active = act;
}

void GE::SceneMesh::setFinalBoneMatrices(std::array<glm::mat4, MAX_BONES> mat, int animPos){
    animations[animPos].m_FinalBoneMatrices = mat;
}

std::array<glm::mat4, MAX_BONES> GE::SceneMesh::getFinalBoneMatrices(int animPos){
    return animations[animPos].m_FinalBoneMatrices;
}