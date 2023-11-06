#pragma once

struct ImageNode;

struct DamageMark {
    DamageMark(ImageNode* t, float x, float z, float dur);

    [[nodiscard]] float getDrawAngle(const float& x, const float& z, const float& orientation) const noexcept;

    float   damagePosX  {},
            damagePosZ  {},
            duration    {};
    ImageNode* texture  {};
};