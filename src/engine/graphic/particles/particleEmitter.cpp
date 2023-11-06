#include "particleEmitter.hpp"

#include "../resource/resourceManager.hpp"
#include "../resource/programresource.hpp"
#include "../resource/shaderresource.hpp"

#include "../../../utils/vectorMath.hpp"

#include <cassert>

const float particleQuad[] = {
    // Position         // UV
    0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
    1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
    0.0f, 0.0f, 0.0f,   0.0f, 0.0f,

    0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
    1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
    1.0f, 0.0f, 0.0f,   1.0f, 0.0f
}; 

ParticleEmitter::ParticleEmitter(ParticleEmitterType t) {
    if(t == ParticleEmitterType::ExplosionParticle)
        type = std::unique_ptr<ExplosionParticle>(new ExplosionParticle());
    else if(t == ParticleEmitterType::InfectionExplosionParticle)
        type = std::unique_ptr<InfectionExplosionParticle>(new InfectionExplosionParticle());
    else if(t == ParticleEmitterType::BloodParticle)
        type = std::unique_ptr<BloodParticle>(new BloodParticle());
    else if(t == ParticleEmitterType::DeathParticle)
        type = std::unique_ptr<DeathParticle>(new DeathParticle());
    else
        assert("Particle emitter with invalid type");

    if(resourceMan == nullptr) {
        // Init static variables
        resourceMan = ResourceManager::Instance();
        ProgramResource* program {resourceMan->getProgram(PROGRAM_PARTICLES)};
        uModel  = glGetUniformLocation(program->getProgramId(), "model");
        uColor  = glGetUniformLocation(program->getProgramId(), "color");
    }

    // Load particle material
    material = resourceMan->getMaterial(type->material);

    // Generate and bind buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particleQuad), particleQuad, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    // Vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // Vertex UV
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)12);
    glBindVertexArray(0);
};

ParticleEmitter::~ParticleEmitter() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    material = nullptr;
}

void ParticleEmitter::update(float dt) {
    // Update alive particles
    for(auto p {particles.begin()}; p<particles.end(); ++p) {
        if(p->dead) break;

        if(p->lifetime < 0) {
            killParticle(p);
            --p;
        }
        else {
            p->lifetime -= dt;

            // Update values
            type->updateParticle(*p, dt);

            // Update orientation
            updateParticleOrientation(*p);
        }
    }

    // Check if it is active during a period of time
    if(activeForTime) {
        activeRemainingTime -= dt;

        if(activeRemainingTime < 0) setActive(false);
    }
    
    // If is inactive, does not generate new particles
    if (!active) return;

    // Update time and generate corresponding particles
    lastGeneration += dt;
    float generationTime {type->generationTime * generationFactor};
    
    size_t generatedParticles {0};
    while(lastGeneration > generationTime && generatedParticles < MAX_GENERATION_UPDATE) {
        // Resize particles container if it was necessary
        if(particlesCount < particles.size()) 
            particles[particlesCount] = type->newParticle(emitterTransform, playerPos);
        else
            particles.emplace_back(type->newParticle(emitterTransform, playerPos));

        Particle& newParticle   = particles[particlesCount];
        newParticle.dead        = false;

        // Update generated particle with the time from creation
        lastGeneration -= generationTime;
        type->updateParticle(newParticle, lastGeneration);
        updateParticleOrientation(newParticle);

        // Increment number of particles
        ++particlesCount;
        ++generatedParticles;
    }

    // Make sure lastGeneratio time remains under particle generation time
    lastGeneration = fmod(lastGeneration, generationTime);
}

void ParticleEmitter::updateParticleOrientation(Particle& p) noexcept {
    auto partPos {p.particleTransform.getTranslation()};
    // Get vector from emitter to player
    float x {playerPos[0] - partPos.x};
    float y {playerPos[1] - partPos.y};
    float z {playerPos[2] - partPos.z};

    vectorMath::normalizeVector3D(&x, &y, &z);

    // Calculate orientation
    auto orien { std::atan2(x, z) };
    auto vorien {std::atan2(y, sqrt(x*x + z*z)) };

    p.particleTransform.setRotation({-vorien, orien, 0});
}

void ParticleEmitter::draw(glm::mat4 modelMatrix, ShaderResource* shader) {
    ShaderResource* vShader = resourceMan->getShader("media/shaders/vertex.glsl", GL_VERTEX_SHADER);

    if(vShader != shader) return;
    
    // Change blend mode and culling
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glCullFace(GL_FRONT);

    // Activate particles program and get vertex shader
    glUseProgram(resourceMan->getProgram(PROGRAM_PARTICLES)->getProgramId());
    vShader = resourceMan->getShader("media/shaders/vertexParticles.glsl", GL_VERTEX_SHADER);

    // Bind texture
    glBindTexture(GL_TEXTURE_2D, material->getTexture(0).id);

    // Draw alive particles
    for(auto p {particles.begin()}; p<particles.end(); ++p) {
        if(p->dead) break;

        if(interpolateParticles)
            vShader->setUniformMat4(uModel, getInterpolatedTransform(*p));
        else
            vShader->setUniformMat4(uModel, p->particleTransform.getTransformMatrix(glm::mat4(1.0)));
            
        vShader->setUniformVec3(uColor, p->color);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }

    // Return to default blend mode and culling
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);

    glUseProgram(resourceMan->getProgram(PROGRAM_DEFAULT)->getProgramId());
}

void ParticleEmitter::setPlayerReferencePosition(entityPos const& p) noexcept {
    playerPos = p;
}

void ParticleEmitter::setEmitterPosition(entityPos const& ePos) noexcept {
    emitterTransform.setTranslation({ePos[0], ePos[1], ePos[2]});
}

void ParticleEmitter::setActive(bool a) noexcept {
    active          = a;
    lastGeneration  = 0.f;
    activeForTime   = a;
}

void ParticleEmitter::setActiveForTime(float time) noexcept {
    setActive(true);

    activeForTime       = true;
    activeRemainingTime = time;
}

bool ParticleEmitter::getActive() const noexcept {
    return active;
}

void ParticleEmitter::killParticle(std::vector<Particle>::iterator p) noexcept {
    // Move last particle to killed particle position
    *p = particles[particlesCount - 1];

    // Kill last particle
    particles[particlesCount - 1].dead = true;

    // Update particles count
    --particlesCount;
}

glm::mat4 ParticleEmitter::getInterpolatedTransform(Particle& p) const noexcept {
    auto transform {p.particleTransform};

    auto currentPos {p.particleTransform.getTranslation()};
    transform.setTranslation({
        p.previousPosition.x + (currentPos.x - p.previousPosition.x) * interpolationValue,
        p.previousPosition.y + (currentPos.y - p.previousPosition.y) * interpolationValue,
        p.previousPosition.z + (currentPos.z - p.previousPosition.z) * interpolationValue
    });
    
    return transform.getTransformMatrix(glm::mat4(1.0));
}

void ParticleEmitter::setGenerationFactor(float factor) noexcept {
    generationFactor = std::max(factor, 1.0f);
}

void ParticleEmitter::setInterpolation(bool interpolate) noexcept {
    interpolateParticles = interpolate;
}