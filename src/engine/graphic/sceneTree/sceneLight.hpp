#pragma once

#include "sceneEntity.hpp"
#define LIGHT_TYPE_OMNI     1
#define LIGHT_TYPE_DIRECT   2
#define LIGHT_TYPE_SPOT     3

namespace GE
{
    struct SceneLight : public SceneEntity{
        SceneLight();
        ~SceneLight() = default;
        void draw(const glm::mat4&, ShaderResource*);

        void setActive(     bool);
        void setType(       uint8_t);
        void setIntensity(  float);
        void setRadius(     float);
        void setColor(      glm::vec3);
        void setDirection(  glm::vec3);
        void setInnerAngle( float);
        void setOuterAngle( float);
        void setCastShadows(bool);

        [[nodiscard]] bool      getActive() const;
        [[nodiscard]] uint8_t   getType() const;
        [[nodiscard]] float     getIntensity() const;
        [[nodiscard]] float     getRadius() const;
        [[nodiscard]] glm::vec3 getColor() const;
        [[nodiscard]] glm::vec3 getDirection() const;
        [[nodiscard]] float     getInnerAngle() const;
        [[nodiscard]] float     getOuterAngle() const;
        [[nodiscard]] bool      getCastShadows() const;

        private:
            bool        active{true};
            uint8_t     type{1};                    // 1: Point light. 2: Directional light. 3: Spotlight.
            float       intensity{1};
            float       radius{1};
            glm::vec3   color{1.0f, 1.0f, 1.0f};
            glm::vec3   direction{0.0, -1.0, 0.0};
            float       innerAngle{90};
            float       outerAngle{10};
            bool        castShadows{true};
    };
} //end namespace GE