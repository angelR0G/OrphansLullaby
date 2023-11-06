#include "damageMark.hpp"

#include "../IrrlichtFacade/image.hpp"
#include "vectorMath.hpp"
#include <math.h>

DamageMark::DamageMark(ImageNode* t, float x, float z, float dur) :
texture{t}, damagePosX{x}, damagePosZ{z}, duration{dur} {}

float DamageMark::getDrawAngle(const float& x, const float& z, const float& orientation) const noexcept {
    // Calculate orientation of hit from reference position
    // First, get vector from reference position to damage point
    float   dirX {damagePosX - x},
            dirZ {damagePosZ - z};

    // Then normalize vector to get direction
    vectorMath::normalizeVector2D(&dirX, &dirZ);
    float angle {std::atan2(dirX, dirZ)};

    // Finally, take into account reference orientation
    angle -= orientation;

    return angle;
}