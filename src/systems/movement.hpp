#pragma once

#include "../engine/types.hpp"

struct MovementSystem {
    void update(EntityMan& EM, const double, std::array<float, 3>*);
    void deleteData(MovementComponent&);
};