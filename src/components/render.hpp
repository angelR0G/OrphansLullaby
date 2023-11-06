#pragma once

#include <memory>
#include "../IrrlichtFacade/mesh.hpp"
#include "../IrrlichtFacade/emitter.hpp"

struct RenderComponent{
    ~RenderComponent(){}
    MeshNode node;
    particleEmitter emitter;
};