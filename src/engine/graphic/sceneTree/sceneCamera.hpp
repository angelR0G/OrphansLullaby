#pragma once

#include "sceneEntity.hpp"
#include "../frustrumPlane.hpp"

namespace GE
{
    struct SceneCamera : public SceneEntity{
        SceneCamera(bool active = true);
        ~SceneCamera() = default;

        void draw(const glm::mat4&, ShaderResource*);
        glm::mat4 getViewMatrix();
        glm::mat4 getProjectionMatrix();
        void activateCamera();

        void setProjectionNear(float);
        void setProjectionFar(float);
        void setProjectionViewportSize(float, float);
        void setProjectionCameraAngle(float);
        void setUpdateProjection() noexcept;
        static void setRenderDistance(float) noexcept;

        [[nodiscard]] std::pair<float, float> getNearFarPlanes() const noexcept;
        [[nodiscard]] float     getCamAngle() const noexcept;
        [[nodiscard]] float     getWindowAspect() const noexcept;
        [[nodiscard]] glm::vec3 getUpVector() const noexcept;

        private:
            glm::vec3 upVector{0, -1, 0};
            glm::mat4 projectionMatrix{};

            // Projection matrix construction values
            bool updateProjection{true};
            float near{0.1f}, far{2000.f};
            float vpWidth{}, vpHeight{};
            float camAngle{70.f};

            inline static float renderDistance{1.f}; 
    };
} //end namespace GE