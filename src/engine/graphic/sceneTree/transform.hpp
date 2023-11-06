#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace GE
{
    using namespace glm;
    struct Transform{
        Transform();
        void setTranslation(vec3) noexcept;
        void translate(vec3) noexcept;
        void setRotation(vec3) noexcept;
        void setScale(vec3) noexcept;
        void setTransformMatrix(mat4) noexcept;
        mat4 updateTransformMatrix(mat4);
        mat4 getRotationMatrixFromRotation() const noexcept;

        [[nodiscard]] vec3 getTranslation() const noexcept;
        [[nodiscard]] vec3 getRotation() const noexcept;
        [[nodiscard]] vec3 getScale() const noexcept;
        [[nodiscard]] mat4 getTransformMatrix(mat4 = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1});

        void setUpdate() noexcept;
        [[nodiscard]] bool getUpdate() const noexcept;

        private:
            vec3 translation{0, 0, 0};
            vec3 rotation{0, 0, 0};
            vec3 scale{1, 1, 1};
            mat4 transformMatrix;
            bool update;
    };
};//end namespace GE
