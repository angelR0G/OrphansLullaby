#pragma once

struct Vec3f {
    float x {}, y{}, z{};

    Vec3f() : 
    x{0.f}, y{0.f}, z{0.f} {};

    Vec3f(float xi, float yi, float zi) :
    x{xi}, y{yi}, z{zi} {};

    Vec3f(float v) :
    x{v}, y{v}, z{v} {};

    Vec3f operator+(const Vec3f& v) {
        return {x+v.x, y+v.y, z+v.z};
    }
};

struct AABBBoundingBox {
    Vec3f center{}, extent{};
    Vec3f points[8];

    AABBBoundingBox(const Vec3f& c, const Vec3f& e) : 
    center{c}, extent{e} 
    {
        points[0] = {center + Vec3f{-extent.x,  -extent.y,  -extent.z}};
        points[1] = {center + Vec3f{extent.x,   -extent.y,  -extent.z}};
        points[2] = {center + Vec3f{-extent.x,  extent.y,   -extent.z}};
        points[3] = {center + Vec3f{extent.x,   extent.y,   -extent.z}};
        points[4] = {center + Vec3f{-extent.x,  -extent.y,  extent.z}};
        points[5] = {center + Vec3f{extent.x,   -extent.y,  extent.z}};
        points[6] = {center + Vec3f{-extent.x,  extent.y,   extent.z}};
        points[7] = {center + Vec3f{extent.x,   extent.y,   extent.z}};
    }

    AABBBoundingBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) {
        extent = {(maxX - minX)/2.f, (maxY - minY)/2.f, (maxZ - minZ)/2.f};
        center = {minX + extent.x, minY + extent.y, minZ + extent.z};
        
        points[0] = {minX, minY, minZ};
        points[1] = {maxX, minY, minZ};
        points[2] = {minX, maxY, minZ};
        points[3] = {maxX, maxY, minZ};
        points[4] = {minX, minY, maxZ};
        points[5] = {maxX, minY, maxZ};
        points[6] = {minX, maxY, maxZ};
        points[7] = {maxX, maxY, maxZ};
    }

    void calculatePoints() noexcept{
        points[0] = {center + Vec3f{-extent.x,  -extent.y,  -extent.z}};
        points[1] = {center + Vec3f{extent.x,   -extent.y,  -extent.z}};
        points[2] = {center + Vec3f{-extent.x,  extent.y,   -extent.z}};
        points[3] = {center + Vec3f{extent.x,   extent.y,   -extent.z}};
        points[4] = {center + Vec3f{-extent.x,  -extent.y,  extent.z}};
        points[5] = {center + Vec3f{extent.x,   -extent.y,  extent.z}};
        points[6] = {center + Vec3f{-extent.x,  extent.y,   extent.z}};
        points[7] = {center + Vec3f{extent.x,   extent.y,   extent.z}};
    }

    bool intersects(const AABBBoundingBox& bbox) const noexcept {
        Vec3f   aMin {points[0]},       aMax{points[7]},
                bMin {bbox.points[0]},  bMax{bbox.points[7]};
                
        return !(   aMin.x > bMax.x || aMax.x < bMin.x ||
                    aMin.y > bMax.y || aMax.y < bMin.y ||
                    aMin.z > bMax.z || aMax.z < bMin.z  );
    }

    bool contains(const AABBBoundingBox& bbox) const noexcept {
        Vec3f   aMin {points[0]},       aMax{points[7]},
                bMin {bbox.points[0]},  bMax{bbox.points[7]};

        return (aMin.x <= bMin.x && aMax.x >= bMax.x && 
                aMin.y <= bMin.y && aMax.y >= bMax.y &&
                aMin.z <= bMin.z && aMax.z >= bMax.z    );
    }
};