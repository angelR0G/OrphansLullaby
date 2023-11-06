#pragma once

#include "sceneEntity.hpp"
#include "../frustrumPlane.hpp"
#include <string>
#include <vector>

#include "../resource/meshresource.hpp"
struct ShaderResource;
struct MaterialResource;
struct AnimationResource;

namespace GE
{
    struct SceneAnimation{
        bool active{false};
        AnimationResource* animation{nullptr};
        std::array<glm::mat4, MAX_BONES> m_FinalBoneMatrices{};
    };

    struct SceneMesh : public SceneEntity{
        SceneMesh();
        ~SceneMesh() = default;
        void loadMesh(std::string);
        void draw(const glm::mat4&, ShaderResource*);
        void asignMaterial(MaterialResource*);
        void asignAnimation(bool, AnimationResource*);
        bool getUseAnimation();
        void setUseAnimation(bool, int);
        void setVisible(bool);
        bool getVisible();
        void setCastShadows(bool);
        MeshResource* getMesh();
        std::vector<AnimationResource*> getAnimation();
        AABBBoundingBox getBoundingBox() const noexcept;
        AABBBoundingBox getTransformedBoundingBox() noexcept;
        static void resetSceneMesh() noexcept;
        static void setLODReferencePosition(float, float, float) noexcept;
        void setFinalBoneMatrices(std::array<glm::mat4, MAX_BONES>, int);
        std::array<glm::mat4, MAX_BONES> getFinalBoneMatrices(int);

        private:            
            MeshResource* mesh;
            std::vector<SceneAnimation> animations{};
            std::vector<std::pair<float, MeshResource*>> meshLOD;
            bool visible{true};
            bool castShadows{true};

            inline static ShaderResource*   normalShader        {nullptr};
            inline static ShaderResource*   shadowShader        {nullptr};
            inline static glm::vec3         lodReferencePosition{};

            [[nodiscard]] MeshResource* getLODMesh() const noexcept;
            void findLODData(std::string) noexcept;
            void sendBoneMatrices(uint8_t, ShaderResource*);
    };
} //end namespace GE