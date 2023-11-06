#include "particleTypes.hpp"
#include "../../../utils/vectorMath.hpp"

#define GEN_TIME_EXPLOSION      0.001f
#define LIFE_TIME_EXPLOSION     0.5f

#define GEN_TIME_INFECTION_EXPLOSION      0.001f
#define LIFE_TIME_INFECTION_EXPLOSION     1.0f

#define GEN_TIME_BLOOD      0.01f
#define LIFE_TIME_BLOOD     0.1f

#define GEN_TIME_DEATH      0.01f
#define LIFE_TIME_DEATH     1.0f

#define DEG_TO_RADIANS(deg)     deg * 3.1415f/180.0f

glm::vec3 interpolateColor(glm::vec3& firstColor, glm::vec3& secondColor, float factor);


ParticleType::ParticleType(float gT, float pL) : 
    generationTime{gT}, particlesLife{pL} {}


/*****************************************
 * EXPLOSION PARTICLE
*****************************************/
ExplosionParticle::ExplosionParticle() : ParticleType(GEN_TIME_EXPLOSION, LIFE_TIME_EXPLOSION) {}

Particle ExplosionParticle::newParticle(GE::Transform const& initTansform, entityPos playerPos) noexcept {
    Particle p;

    p.particleTransform = initTansform;
    p.particleTransform.setScale({3.0, 3.0, 3.0});

    const float velocity {100.f};
    float vertical  { (rand() % 1001) / 1000.f};        // Random vertical direction (between 0 and 1)
    float direction { DEG_TO_RADIANS(rand() % 360)};    // Random horizontal direction

    // Get a random movement direction (normalized) and multiply for a constant speed
    p.velocity  = {sin(direction) * vertical, 1 - vertical, cos(direction) * vertical};
    vectorMath::normalizeVector3D(&p.velocity.x, &p.velocity.y, &p.velocity.z);
    p.velocity *= velocity;

    p.lifetime  = particlesLife;
    
    return p;
}

void ExplosionParticle::updateParticle(Particle& particle, float dt) noexcept {
    // Save previous values
    particle.previousPosition   = particle.particleTransform.getTranslation();
    particle.previousColor      = particle.color;

    // Change color over life
    float changeColorTime {particlesLife/2.0f};
    if(particle.lifetime > changeColorTime) {
        // Interpolate between white and red
        glm::vec3 white {1.0, 1.0, 1.0};
        glm::vec3 red   {0.9, 0.0, 0.0};
        float factor    {1 - (particle.lifetime-changeColorTime)/(particlesLife-changeColorTime)};

        particle.color = interpolateColor(white, red, factor);
    }
    else {
        // Interpolate between red and black
        glm::vec3 red   {0.9, 0.0, 0.0};
        glm::vec3 black {0.2, 0.2, 0.2};
        float factor    {1 - std::max(particle.lifetime, 0.f)/changeColorTime};

        particle.color = interpolateColor(red, black, factor);
    }

    // Move particle
    particle.particleTransform.translate(particle.velocity * dt);
}


/*****************************************
 * INFECTION EXPLOSION PARTICLE
*****************************************/
InfectionExplosionParticle::InfectionExplosionParticle() : ParticleType(GEN_TIME_INFECTION_EXPLOSION, LIFE_TIME_INFECTION_EXPLOSION) {}

Particle InfectionExplosionParticle::newParticle(GE::Transform const& initTansform, entityPos playerPos) noexcept {
    Particle p;

    p.particleTransform = initTansform;
    p.particleTransform.setScale({5.0, 5.0, 5.0});
    glm::vec3 purple {0.48, 0.0, 0.8};
    p.color = purple;

    const float velocity {60.f};
    float vertical  { ((rand() % 801) / 1000.f) - 0.2f};    // Random vertical direction (between -0.2 and 0.6)
    float direction { DEG_TO_RADIANS(rand() % 360)};        // Random horizontal direction

    // Get a random movement direction (normalized) and multiply for a constant speed
    p.velocity  = {sin(direction) * vertical, 1 - vertical, cos(direction) * vertical};
    vectorMath::normalizeVector3D(&p.velocity.x, &p.velocity.y, &p.velocity.z);
    p.velocity *= velocity;

    p.lifetime  = particlesLife;

    return p;
}

void InfectionExplosionParticle::updateParticle(Particle& particle, float dt) noexcept {
    // Save previous values
    particle.previousPosition   = particle.particleTransform.getTranslation();
    particle.previousColor      = particle.color;

    // Change scale over life
    glm::vec3 initialScale {5.0, 5.0, 5.0};
    glm::vec3 finalScale   {0.2, 0.2, 0.2};
    float factor    {1 - particle.lifetime/particlesLife};

    float alive = particlesLife - particle.lifetime;

    particle.particleTransform.setScale(interpolateColor(initialScale, finalScale, factor));
    particle.particleTransform.translate({0.0, -5.5*alive, 0.0});

    // Move particle
    particle.particleTransform.translate(particle.velocity * dt);
}

/*****************************************
 * BLOOD PARTICLE
*****************************************/
BloodParticle::BloodParticle() : ParticleType(GEN_TIME_BLOOD, LIFE_TIME_BLOOD) {}

Particle BloodParticle::newParticle(GE::Transform const& initTansform, entityPos playerPos) noexcept {
    Particle p;

    p.particleTransform = initTansform;
    p.particleTransform.setScale({2.0, 2.0, 2.0});
    p.color = {0.54, 0.16, 0.62};

    //Calculate particle direction towards the player
    glm::vec3 partPos = p.particleTransform.getTranslation();
    glm::vec3 plaPos  = {playerPos[0], playerPos[1], playerPos[2]};
    glm::vec3 dist    = plaPos - partPos;
    glm::vec3 dir     = glm::normalize(dist);

    const float velocity {100.f};
    float direction { DEG_TO_RADIANS(rand() % 45)};    // Random horizontal direction

    // Get a random movement direction (normalized) and multiply for a constant speed
    p.velocity  = {dir.x*sin(direction), -dir.y, dir.z*cos(direction)};
    vectorMath::normalizeVector3D(&p.velocity.x, &p.velocity.y, &p.velocity.z);
    p.velocity *= velocity;

    p.lifetime  = particlesLife;

    return p;
}

void BloodParticle::updateParticle(Particle& particle, float dt) noexcept {
    // Save previous values
    particle.previousPosition   = particle.particleTransform.getTranslation();
    particle.previousColor      = particle.color;

    // Change scale over life
    glm::vec3 initialScale {1.0, 1.0, 1.0};
    glm::vec3 finalScale   {0.2, 0.2, 0.2};
    float factor    {1 - particle.lifetime/particlesLife};

    float alive = particlesLife - particle.lifetime;

    particle.particleTransform.setScale(interpolateColor(initialScale, finalScale, factor));
    particle.particleTransform.translate({0.0, -7.5*alive, 0.0});

    // Move particle
    particle.particleTransform.translate(particle.velocity * dt);
}

/*****************************************
 * DEATH PARTICLE
*****************************************/
DeathParticle::DeathParticle() : ParticleType(GEN_TIME_DEATH, LIFE_TIME_DEATH) {}

Particle DeathParticle::newParticle(GE::Transform const& initTansform, entityPos playerPos) noexcept {
    Particle p;

    p.particleTransform = initTansform;
    glm::vec3 position {p.particleTransform.getTranslation()};
    position.x += rand() % 10 - 5;
    position.z += rand() % 10 - 5;
    position.y += rand() % 5  - 8;
    p.particleTransform.setTranslation({position});
    p.particleTransform.setScale({0.7, 0.7, 0.7});
    glm::vec3 purple {0.48, 0.0, 0.8};
    p.color = purple;

    const float velocity {17.f};
    float vertical  { ((rand() % 801) / 1000.f)};
    //float direction { DEG_TO_RADIANS(rand() % 360)};        // Random horizontal direction

    // Get a random movement direction (normalized) and multiply for a constant speed
    p.velocity  = {0, vertical, 0};
    vectorMath::normalizeVector3D(&p.velocity.x, &p.velocity.y, &p.velocity.z);
    p.velocity *= velocity;

    p.lifetime  = particlesLife;

    return p;
}

void DeathParticle::updateParticle(Particle& particle, float dt) noexcept {
    // Save previous values
    particle.previousPosition   = particle.particleTransform.getTranslation();
    particle.previousColor      = particle.color;

    // Change scale over life
    glm::vec3 initialScale {0.7, 0.7, 0.7};
    glm::vec3 finalScale   {0.01, 0.01, 0.01};
    float factor    {1 - particle.lifetime/particlesLife};

    float alive = particlesLife - particle.lifetime;

    particle.particleTransform.setScale(interpolateColor(initialScale, finalScale, factor));

    // Move particle
    particle.particleTransform.translate(particle.velocity * dt);
}



// OTHER FUNCTIONS
glm::vec3 interpolateColor(glm::vec3& firstColor, glm::vec3& secondColor, float factor) {
    return {firstColor + (secondColor - firstColor) * factor};
}