#include "ai.hpp"
#include "../utils/collisionTypes.hpp"

#include "../engine/physics/physicsEngine.hpp"
#include "../manager/eventmanager.hpp"

#include <numbers>
#include <cmath>

#define TIME_ALIVE          15
#define RESPAWN_DISTANCE    150

#define AVOID_DISTANCE      10

#define PLAYER_DIST_MIN     50
#define ENEMY_EYE           14
#define PLAYER_EYE          8

#define M1K3_ROUND              5
#define DEFEND_MACHINE_ROUND    1

#define MATCH_VELOCITY_DIST         75
#define TARGET_DIST_MIN             30
#define NEARBY_ENEMY_DISTANCE       13
#define AVOIDANCE_ENEMY_DISTANCE    10
#define COHESION_WEIGHTS            1
#define ALIGNMENT_WEIGHTS           4
#define AVOID_WEIGHTS               2

const double PI {std::numbers::pi};

double
calculateDistanceLineal(double posX, double posZ, double tposX, double tposZ){
    auto distx { tposX - posX };
    auto distz { tposZ - posZ };

    auto distlin { std::sqrt(distx*distx + distz*distz) };

    return distlin;
}

double
calculateOrientation(double posX, double posZ, double tposX, double tposZ){
    auto distx { tposX - posX };
    auto distz { tposZ - posZ };

    auto torien { std::atan2(distx, distz) };    // Calcula la arcotangente que da el angulo que necesita para llegar al objetivo
    if (torien < 0)    torien += 2*PI;           // Convertimos el angulo a positivo si es negativo

    return torien;
}

void
AISystem::arrive(AIComponent& ai, MovementComponent& phy, BasicComponent& pos) {
    if(ai.tactive){
        auto distlin { calculateDistanceLineal(pos.x, pos.z, ai.tx, ai.tz) };
        
        // Si la distancia actual es menor que el radio de llegada, ya se llega
        if(distlin < phy.vLinear/2 && std::abs(ai.ty - pos.y) < 25){
            ai.tactive = false;
            //phy.moving = false;
            return;
        }
        else {
            phy.moving = true;
        }
    
        pos.orientation = calculateOrientation(pos.x, pos.z, ai.tx, ai.tz);
    }
}

void
AISystem::flee(AIComponent& ai, MovementComponent& phy, BasicComponent& pos, BasicComponent& posPlayer) {
    phy.moving = true;

    pos.orientation = calculateOrientation(pos.x, pos.z, ai.tx, ai.tz);
    pos.orientation += PI;
    //if (pos.orientation < 0)    pos.orientation += 2*PI;           // Convertimos el angulo a positivo si es negativo
    //phy.movOrientation = 0.5;   // Se mueve en direccion contraria al jugador
}

void
AISystem::seek(AIComponent& ai, MovementComponent& phy, BasicComponent& pos, BasicComponent& posPlayer) {
    auto distlin { calculateDistanceLineal(pos.x, pos.z, ai.tx, ai.tz) };

    // Si la distancia actual es menor que el radio de llegada, ya se llega
    if(distlin < ai.arrivalRadius){
        phy.moving = false;
        phy.vX = 0;
        phy.vY = 0;
        phy.vZ = 0;
        return;
    }
    else {
        phy.moving = true;
    }

    pos.orientation = calculateOrientation(pos.x, pos.z, posPlayer.x, posPlayer.z);
}

void
AISystem::afk(MovementComponent& phy) {
    phy.moving = false;
    phy.vX = 0;
    phy.vY = 0;
    phy.vZ = 0;

    return;
}

void
AISystem::pursue(AIComponent& ai, MovementComponent& phy, BasicComponent& pos, BasicComponent& posPlayer, MovementComponent* phyPlayer) {
    // check if player is going towards the enemy
    auto minOrientation = pos.orientation - PI/4;
    auto maxOrientation = pos.orientation + PI/4;
    auto playerOrientation = posPlayer.orientation + PI;

    float vX {0}, vZ {0};
    if(phyPlayer != nullptr) {
        vX = phyPlayer->vX;
        vZ = phyPlayer->vZ;
    }
    
    if(playerOrientation>=minOrientation && playerOrientation<=maxOrientation){
        pos.orientation = calculateOrientation(pos.x, pos.z, ai.tx, ai.tz);
        return;
    }

    auto distlin { calculateDistanceLineal(pos.x, pos.z, ai.tx, ai.tz) };

    auto predTime { distlin / phy.vLinear };
    if (phy.vLinear <= distlin / 100) predTime = 100;

    auto predX { ai.tx + vX * predTime };
    auto predZ { ai.tz + vZ * predTime };

    auto distlinPred { calculateDistanceLineal(pos.x, pos.z, predX, predZ) };

    // Si la distancia actual es menor que el radio de llegada, ya se llega
    if(distlinPred < ai.arrivalRadius){
        phy.moving = false;

        return;
    }
    else {
        phy.moving = true;
    }

    pos.orientation = calculateOrientation(pos.x, pos.z, predX, predZ);
}

void AISystem::update(EntityMan& EM, PhysicsEngine& physics, float deltaTime, bool goalEnemyTarget, size_t currentGoal){
    // vector de entidades que sean posibles objetivos y se la paso al update por el landa raro ese
    // en el update de la ia se decide a que objetivo va cada uno y tal
    std::vector<Entity*> targetEntities = getIdTargets(EM);

    using CList = MetaP::Typelist<>;
    using TList = MetaP::Typelist<EnemyTag>;
    EM.foreach<CList, TList>([&, targetEntities](Entity& e){
        if(!e.hasTag<MarkToDestroyTag>() && !e.hasTag<MarkToDestroyDelayTag>()){
            auto* phy = &EM.getComponent<MovementComponent>(e);
            auto* pos = &EM.getComponent<BasicComponent>(e);
            auto* ai  = &EM.getComponent<AIComponent>(e);
            auto* nav = &EM.getComponent<NavigationComponent>(e);
            auto* hp  = &EM.getComponent<HealthComponent>(e);
            SoundComponent* sc {nullptr};

            AnimationComponent* animC = nullptr;
            if(e.hasComponent<AnimationComponent>())
                animC = &EM.getComponent<AnimationComponent>(e);

            // get current orientation before updating ai
            pos->prevorientation = pos->orientation;
            if(e.hasComponent<SoundComponent>())
                sc  = &EM.getComponent<SoundComponent>(e);

            // get current orientation before updating ai
            pos->prevorientation = pos->orientation;

            BasicComponent*     basicObj    {nullptr};
            MovementComponent*  movObj      {nullptr};
            if(targetEntities.size()>1 && (ai->targetMask == 1)){
                basicObj = &EM.getComponent<BasicComponent>(*targetEntities[1]);
                if(targetEntities[1]->hasComponent<MovementComponent>()) {
                    movObj = &EM.getComponent<MovementComponent>(*targetEntities[1]);
                }

                // Check if player is near enemy to change target to him
                BasicComponent* basicPly = &EM.getComponent<BasicComponent>(*targetEntities[0]);
                auto distx { basicPly->x - pos->x};
                auto distz { basicPly->z - pos->z};
                auto distlin { std::sqrt(distx*distx + distz*distz) };
                if(distlin<=PLAYER_DIST_MIN){
                    RayCast ray;
                    ray.filterMask = SOLID_COLLISION | PLAYER_COLLISION | DESTROYABLE_COLLISION | BARREL_COLLISION;
                    physics.raycast(ray, pos->x, pos->y+ENEMY_EYE, pos->z, basicPly->x, basicPly->y+PLAYER_EYE, basicPly->z);

                    if(ray.collision && (ray.bodyflags & PLAYER_COLLISION)){
                        ai->targetMask = 0;     // Enemy has player near so starts targeting him
                    }
                }
            }
            else{
                basicObj = &EM.getComponent<BasicComponent>(*targetEntities[0]);
                if(targetEntities[0]->hasComponent<MovementComponent>()) {
                    movObj = &EM.getComponent<MovementComponent>(*targetEntities[0]);
                }
            }

            if(!goalEnemyTarget && ai->targetMask & 1){
                ai->targetMask = 0;
            }

            if (ai->behaviourTree && !e.hasTag<EnemyExecuteBT>()){
                AttackStatsComponent* atckstatus = &EM.getComponent<AttackStatsComponent>(e);
                uint8_t enemyType = getEnemyType(&e);
                EntityContext ectx {pos, basicObj, phy, ai, &physics, nav, hp, sc, animC, deltaTime, e.getId(), atckstatus, enemyType};
                ai->behaviourTree->run(&ectx);
            }

            //Raise timer of attack duration
            ai->attackCD += deltaTime;

            // Raise timer of time alive
            ai->time_alive += deltaTime;

            if(ai->behaviour == SB::Seek || ai->behaviour == SB::Flee || ai->behaviour == SB::Pursue){
                ai->tx = basicObj->x;
                ai->tz = basicObj->z;

                ai->time_alive = 0;
            }

            switch (ai->behaviour){
                case SB::Arrive:    arrive(*ai, *phy, *pos);                            break;
		        case SB::Seek:      seek(*ai, *phy, *pos, *basicObj);                   break;
		        case SB::Flee:      flee(*ai, *phy, *pos, *basicObj);                   break;
                case SB::Pursue:    pursue(*ai, *phy, *pos, *basicObj, movObj);         break;
                case SB::AFK:       afk(*phy);                                          break;
            }

            if(e.template hasTag<EnemyNormalTag>()){
                float distToTarget = calculateDistanceLineal(pos->x, pos->z, basicObj->x, basicObj->z);

                if(distToTarget>TARGET_DIST_MIN)
                    updateFlock(EM, e);
            }

            avoidCollision(*pos, physics, *phy);
            updateOrientationAvoid(*pos);

            if(ai->targetMask < targetEntities.size()){
                Entity *objEnt = targetEntities[ai->targetMask];
                if(objEnt!=nullptr){
                    BasicComponent* objRespawn = &EM.getComponent<BasicComponent>(*objEnt);
                    respawnEnemy(&e, *pos, objRespawn, *ai, physics);
                }
            }
        }
    });

    Entity* player {nullptr};
    using CPList = MetaP::Typelist<>;
    using TPList = MetaP::Typelist<PlayerTag>;
    EM.foreach<CPList, TPList>([&](Entity& e){
        player = &e;
    });

    // ENTITIES WITH AI THAT ARE NOT ENEMIES
    using C2List = MetaP::Typelist<AIComponent>;
    using T2List = MetaP::Typelist<ReparableTag, ExplosiveBarrelTag, TargetBarrelTag>;
    EM.foreachOR<C2List, T2List>([&](Entity& e){
        if(!e.hasTag<EnemyTag>()){
            auto* pos = &EM.getComponent<BasicComponent>(e);
            auto* ai  = &EM.getComponent<AIComponent>(e);
            auto* hp  = &EM.getComponent<HealthComponent>(e);
            SoundComponent*      sc {nullptr};
            NavigationComponent* nav {nullptr};
            MovementComponent*   mov {nullptr};

            AnimationComponent* animC{nullptr};
            if(e.hasComponent<AnimationComponent>())
                animC = &EM.getComponent<AnimationComponent>(e);

            if(e.hasComponent<NavigationComponent>())
                nav = &EM.getComponent<NavigationComponent>(e);
            if(e.hasComponent<MovementComponent>()){
                mov = &EM.getComponent<MovementComponent>(e);
            }
            if(e.hasTag<TargetBarrelTag>()){
                sc = &EM.getComponent<SoundComponent>(e);
            }

            if (ai->behaviourTree){
                BasicComponent* playerBasic {nullptr};
                if(player != nullptr) playerBasic = &EM.getComponent<BasicComponent>(*player);
                
                EntityContext ectx {pos, playerBasic, mov, ai, &physics, nav, hp, sc, animC, deltaTime, e.getId(), nullptr, 0};

                if(!e.hasTag<MovableObjectiveTag>()){
                    ai->behaviourTree->run(&ectx);
                }
                else{
                    TriggerComponent* tgg = &EM.getComponent<TriggerComponent>(e);
                    if(tgg->bActive) {
                        ai->behaviourTree->run(&ectx);
                    }
                }
            }

            if(e.hasTag<MovableObjectiveTag>() && mov!=nullptr){
                if(ai->behaviour==SB::Arrive){
                    arrive(*ai, *mov, *pos);
                }
                else{
                    afk(*mov);
                }

                if(ai->targetMask == ai->routeToFollow.size()){
                    ++ai->targetMask;
                    //complete goal
                    EventManager* eventMan = EventManager::Instance();
                    std::shared_ptr<Event> event ( new Event(EVENT_M1KE_COMPLETE) );
                    
                    eventMan->scheduleEvent(event);
                }
            }

            if( (e.template hasTag<ReparableTag>() && !e.template hasTag<MovableObjectiveTag>() && goalEnemyTarget && currentGoal==DEFEND_MACHINE_ROUND) ||
                (e.template hasTag<MovableObjectiveTag>() && goalEnemyTarget && currentGoal==M1K3_ROUND) ){
                if(hp->health<=0 && targetEntities.size()>1){
                    removeTarget(targetEntities[1]->getId());
                }
                else if(hp->health>0 && targetEntities.size()<2){
                    addNewTarget(e.getId());
                }
            }
        }
    });
}

// method to calculate behaviours of flocking ONLY for normal enemies
void AISystem::updateFlock(EntityMan& EM, Entity& e){
    BasicComponent* basiccmp_e = &EM.getComponent<BasicComponent>(e);
    MovementComponent* movcmp_e = &EM.getComponent<MovementComponent>(e);

    int nearbyEnemies {}, closestEnemies {}, fasterNearbyEnemies{};
    float totalPosX {}, totalPosZ {};
    double totalEnemiesOrient {basiccmp_e->orientation};
    double totalAvoidOrient {}, choesionOrient {}, alignmentOrient {}, avoidOrient {}, resultOrientation {};

    using CListEnemyNormal = MetaP::Typelist<>;
    using TListEnemyNormal = MetaP::Typelist<EnemyNormalTag>;
    EM.foreach<CListEnemyNormal, TListEnemyNormal>([&](Entity& ent_enemyNormal){
        // check if is not the same entity
        if(ent_enemyNormal.getId()!=e.getId() && !ent_enemyNormal.hasTag<MarkToDestroyTag>() && !ent_enemyNormal.hasTag<MarkToDestroyDelayTag>()){
            // check if enemy is near
            BasicComponent* basiccmp_enemyNormal = &EM.getComponent<BasicComponent>(ent_enemyNormal);
            float distlin = calculateDistanceLineal(basiccmp_e->x, basiccmp_e->z, basiccmp_enemyNormal->x, basiccmp_enemyNormal->z);
            if(distlin<=MATCH_VELOCITY_DIST){
                // check if enemy is behind
                double torien = calculateOrientation(basiccmp_e->x, basiccmp_e->z, basiccmp_enemyNormal->x, basiccmp_enemyNormal->z);
                auto angleDiff = std::abs(basiccmp_e->orientation - torien);
                if(angleDiff<=PI/2){
                    ++fasterNearbyEnemies;

                    if(distlin<=NEARBY_ENEMY_DISTANCE){
                        // cohesion calculations
                        totalPosX += basiccmp_enemyNormal->x;
                        totalPosZ += basiccmp_enemyNormal->z;
                        // alignment calculations
                        totalEnemiesOrient += basiccmp_enemyNormal->orientation;

                        ++nearbyEnemies;

                        // avoidance calculations
                        if(distlin<=AVOIDANCE_ENEMY_DISTANCE){
                            avoidOrient = calculateOrientation(basiccmp_e->x, basiccmp_e->z, basiccmp_enemyNormal->x, basiccmp_enemyNormal->z);
                            if(std::rand()%1==1){
                                avoidOrient += PI/2;
                            }
                            else{
                                avoidOrient -= PI/2;
                            }
                            totalAvoidOrient += avoidOrient;
                            ++closestEnemies;
                        }
                    }
                }
            }
        }
    });

    // if there are enemies nearby do flocking calculations
    if(nearbyEnemies>0){
        if(movcmp_e->run){
            movcmp_e->run = false;
        }

        // cohesion behaviour
        totalPosX /= nearbyEnemies;
        totalPosZ /= nearbyEnemies;
        choesionOrient = calculateOrientation(basiccmp_e->x, basiccmp_e->z, totalPosX, totalPosZ) * COHESION_WEIGHTS;
        if (choesionOrient < 0)    choesionOrient += 2*PI;

        // alignment behaviour
        alignmentOrient = (totalEnemiesOrient / nearbyEnemies + 1) * ALIGNMENT_WEIGHTS;
        if (alignmentOrient < 0)    alignmentOrient += 2*PI;

        resultOrientation = choesionOrient + alignmentOrient;
        int numOrientations = 2;

        if(closestEnemies>0){
            // avoid behaviour
            avoidOrient = (totalAvoidOrient / closestEnemies) * AVOID_WEIGHTS;
            if (avoidOrient < 0)    avoidOrient += 2*PI;

            resultOrientation += avoidOrient;
            ++numOrientations;
        }

        //resultOrientation /= numOrientations;
        if (resultOrientation < 0)    resultOrientation += 2*PI;

        // make results smoother
        double currentOrient = basiccmp_e->orientation;
        if(resultOrientation-currentOrient>PI/6){
            basiccmp_e->orientation += PI/6;
        }
        else{
            basiccmp_e->orientation = resultOrientation;
        }
    }
    else{
        if(fasterNearbyEnemies>0){
            movcmp_e->run = true;
        }
        else{
            movcmp_e->run = false;
        }
    }
}

void AISystem::respawnEnemy(Entity* e, BasicComponent& pos, BasicComponent* target_pos, AIComponent& ai, PhysicsEngine& physics){
    bool respawnEntity{false};

    if(pos.y<=-500){
        respawnEntity = true;
    }
    else if(ai.time_alive>=TIME_ALIVE){
        // check distance of enemy to target
        double distlin = calculateDistanceLineal(pos.x, pos.z, target_pos->x, target_pos->z);

        if(distlin>=RESPAWN_DISTANCE){
            // check if enemy is visible for target
            RayCast ray;
            ray.filterMask = SOLID_COLLISION | PLAYER_COLLISION;
            physics.raycast(ray, pos.x, pos.y+ENEMY_EYE, pos.z, target_pos->x, target_pos->y+PLAYER_EYE, target_pos->z);

            if( ray.collision && (ray.bodyflags & SOLID_COLLISION) ){
                respawnEntity = true;
                
            }
        }
    }

    if(respawnEntity) {
        // Destroy enemy
        e->addTag<MarkToDestroyTag>();

        // Check enemy type
        int enemyType {static_cast<int>(getEnemyType(e))};

        // Respawn a new enemy of the same type
        eventRespawnEnemy(enemyType);
    }
}

void AISystem::eventRespawnEnemy(int enemyType){
    EventManager* eventMan = EventManager::Instance();
    std::shared_ptr<Event> event ( new Event(EVENT_RESPAWN_ENEMY) );
    event->dataNum = enemyType;
            
    eventMan->scheduleEvent(event);
}

void AISystem::avoidCollision(BasicComponent& pos, PhysicsEngine& physics, MovementComponent& phy){
    //Angle to avoid to a side
    double avoidR = 0, avoidL = 0;

    //Distance to collision
    double distColR = 0, distColL = 0;

    //Separation between raycasts
    float rayGap = 3;

    // Calculate distances of raycasts
    float   posY   = pos.y + rayGap * sin(pos.verticalOrientation);
    
    float   posXR  = pos.x + rayGap * cos(pos.orientation) * cos(pos.verticalOrientation),
            posZR  = pos.z + rayGap * -sin(pos.orientation) * cos(pos.verticalOrientation),
            posXL  = pos.x - rayGap * cos(pos.orientation) * cos(pos.verticalOrientation),
            posZL  = pos.z - rayGap * -sin(pos.orientation) * cos(pos.verticalOrientation);

    // Calculate entity direction and make raycasts
    float   targetY   = posY + 9.4  + AVOID_DISTANCE * sin(pos.verticalOrientation);
    
    float   targetXR  = posXR + AVOID_DISTANCE * sin(pos.orientation) * cos(pos.verticalOrientation),
            targetZR  = posZR + AVOID_DISTANCE * cos(pos.orientation) * cos(pos.verticalOrientation),
            targetXL  = posXL + AVOID_DISTANCE * sin(pos.orientation) * cos(pos.verticalOrientation),
            targetZL  = posZL + AVOID_DISTANCE * cos(pos.orientation) * cos(pos.verticalOrientation);


    RayCast rayR, rayL;
    physics.raycast(rayR, posXR, posY, posZR, targetXR, targetY, targetZR);
    physics.raycast(rayL, posXL, posY, posZL, targetXL, targetY, targetZL);

    
    double avoidAngle  = PI/24;
    float totalMov     = abs(phy.vX) + abs(phy.vY) + abs(phy.vZ);
    
    //Weight value of movement and distance to collision to calculate angles
    float movValue     = 0.67,
          distColValue = 0.44;

    //Calculate angles to move orientation in order to avoid collision

    //Collision left
    if(rayL.collision && (rayL.bodyflags & (NO_TRASPASABLE_COLLISION))){
        distColL = calculateDistanceLineal(pos.x, pos.z, rayL.hitX, rayL.hitZ);
        if(distColL == 0) distColL += 1;
        //Angle to apply depends on distance to collision and velocity
        avoidL += avoidAngle * ((AVOID_DISTANCE/distColL)*distColValue) * (totalMov*movValue);
    }

    //Collision right
    if(rayR.collision && (rayR.bodyflags & (NO_TRASPASABLE_COLLISION))){
        distColR = calculateDistanceLineal(pos.x, pos.z, rayR.hitX, rayR.hitZ);
        if(distColR == 0) distColR += 1;
        avoidR -= avoidAngle * ((AVOID_DISTANCE/distColR)*distColValue) * (totalMov*movValue);
    }

    //Both collisions
    if(avoidL != 0 && avoidR != 0){
        avoidR -= avoidL;

        /*
        //Right collision nearer
        if(avoidR > (-avoidL)){
            avoidL -= avoidR;
        //Left collision nearer
        }else{
            avoidR -= avoidL;
        }
        */
    }

    pos.orientationAvoid += avoidR + avoidL;

    //std::cout<< pos.orientationAvoid <<std::endl;
}

void AISystem::updateOrientationAvoid(BasicComponent& pos){
    pos.orientation += pos.orientationAvoid;

    if (pos.orientation < 0)       pos.orientation += 2*PI;
    if (pos.orientation > 2*PI)    pos.orientation -= 2*PI;

    pos.orientationAvoid *= 999999/1000000;

    const double orStep {PI/240};

    if(pos.orientationAvoid < orStep && pos.orientationAvoid > -orStep)
        pos.orientationAvoid = 0;
}

void AISystem::addNewTarget(size_t idTarget){
    idTargets.emplace_back(idTarget);
}

void AISystem::removeTarget(size_t idTarget){
    for(size_t i=0; i<idTargets.size(); i++){
        if(idTargets[i]==idTarget)
            idTargets.erase(idTargets.begin()+i);
    }
}

void AISystem::clearTargets() {
    idTargets.clear();
}

std::vector<Entity*> AISystem::getIdTargets(EntityMan& EM){

    std::vector<Entity*> ent;

    for(size_t i=0; i<idTargets.size(); i++){
        auto* e     = EM.getEntityById(idTargets[i]);
        ent.emplace_back(e);
    }

    return ent;
}

// replace the given target in the position of the vector given by argument
void AISystem::changeTarget(uint8_t vecPos, size_t idTarget){
    idTargets[vecPos] = idTarget;
}


// return -> 0: normal | 1: explosive | 2: thrower | 3: rugby
uint8_t AISystem::getEnemyType(Entity* e){
    uint8_t type = 0;

    if(e->hasTag<EnemyExplosiveTag>()){
        type = 1;
    }
    if(e->hasTag<EnemyThrowerTag>()){
        type = 2;
    }
    if(e->hasTag<EnemyRugbyTag>()){
        type = 3;
    }

    return type;
}