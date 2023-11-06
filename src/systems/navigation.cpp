#include "navigation.hpp"
#include "../utils/scheduler.hpp"
#include "../utils/WaypointCreator.hpp"
#include "../engine/graphic/engine.hpp"
#include <cassert>
#include <queue>
#include <map>

#define DIST_COST_ESTIMATION    20

// points where movable objective has to go
std::vector <std::vector<float>> movableObjectivePoints {};

const std::vector<std::string> navFiles {
    "media/maps/map1/map_data/waypoints.obj",
    "media/maps/map2/map_data/waypoints.obj"
};

// Constant matrix data
const std::vector<std::vector<uint16_t>> CLUSTERS_CONNECTION[]= {
    {   {0  ,700,300,900}, 
        {400,0  ,400,200},
        {300,400,0  ,600},
        {200,200,500,0  }   
    },
    {
        {0,   220, 400, 560, 300, 440, 540, 820, 720, 0, 560, 760, 320, 400},
        {220, 0,   180, 340, 80,  220, 320, 600, 500, 0, 340, 540, 100, 180},
        {400, 180, 0,   180, 260, 400, 480, 760, 660, 0, 500, 700, 280, 340},
        {560, 340, 180, 0,   260, 490, 300, 580, 480, 0, 320, 520, 440, 160},
        {300, 80,  260, 260, 0,   300, 240, 520, 420, 0, 260, 460, 180, 100},
        {440, 220, 400, 490, 300, 0,   190, 470, 370, 0, 490, 690, 120, 330},
        {540, 320, 480, 300, 240, 190, 0,   280, 180, 0, 300, 500, 310, 140},
        {820, 600, 760, 580, 520, 470, 280,  0,  100, 0, 580, 780, 590, 420},
        {720, 500, 660, 480, 420, 370, 180, 100, 0,   0, 480, 680, 490, 320},
        {500, 280, 440, 260, 200, 430, 240, 520, 420, 0, 260, 460, 380, 100},
        {560, 340, 500, 320, 260, 490, 300, 580, 480, 0, 0,   200, 440, 160},
        {760, 540, 700, 520, 460, 690, 500, 780, 680, 0, 200, 0,   640, 360},
        {320, 100, 280, 440, 180, 120, 310, 590, 490, 0, 440, 640, 0,   280},
        {400, 180, 340, 160, 100, 330, 140, 420, 320, 0, 160, 360, 280, 0  }
    }
};

const std::vector<std::vector<uint8_t>> SPAWN_CONNECTION[]= {
    {   {1  ,0  ,2  ,0}, 
        {0  ,1  ,2  ,1},
        {2  ,2  ,1  ,0},
        {1  ,1  ,0  ,1}
    },
    {   {1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {2, 1, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
        {0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 2, 0},
        {0, 0, 0, 0, 0, 2, 1, 0, 1, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 2},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
        {0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 2, 1, 0, 1, 0, 0, 0, 2, 0, 0, 1}
    }
};

// Definition of navigation graph
std::vector<std::vector<uint16_t>>                      NavigationSystem::clustersConnection {};
std::vector<MapCluster>                                 NavigationSystem::clusters {};
std::vector<std::shared_ptr<Waypoint_t>>                NavigationSystem::waypoints {};
std::vector<uint16_t>                                   NavigationSystem::clustersIndex {};
std::vector<std::vector<std::shared_ptr<Waypoint_t>>>   NavigationSystem::spawnpoints {};
std::vector<std::vector<uint8_t>>                       NavigationSystem::spawnConnection {};
std::vector<std::pair<uint16_t, uint16_t>>              NavigationSystem::blockedWpConnections {};
uint8_t                                                 NavigationSystem::playerArea{};

// Structs for the pathfinding algorithm
struct PathfindingNode {
    uint16_t        nodeId{};
    uint16_t        previousNodeId{};
    unsigned int    cost{};
    unsigned int    estimatedCost{};
    bool            open{true};
    int             tacticalCost{};
};

using OpenList      = std::multimap<unsigned int, PathfindingNode>;
using ClosedList    = std::map<uint16_t, PathfindingNode>;

NavigationSystem::~NavigationSystem() {}

void NavigationSystem::loadMapNavigation(uint8_t mapIndex) {
    // Clears navigation information
    clearMapNavigation();

    // Read information and save it
    clustersConnection  = CLUSTERS_CONNECTION[mapIndex];
    spawnConnection     = SPAWN_CONNECTION[mapIndex];
    playerArea = 0;
    
    generateWaypoints(navFiles[mapIndex], clusters, clustersIndex, waypoints, spawnpoints, blockedWpConnections, spawnConnection);

    // Check if waypoints have been loaded
    assert(waypoints.size() != 0);
}

void NavigationSystem::unlockDoorConnection(std::pair<uint8_t, uint8_t> areas) {
    bool notFound{true};
    if(spawnConnection[areas.first][areas.second] == 2) {
        spawnConnection[areas.first][areas.second] = 1;

        // Unlock waypoint connections blocked by door
        for(auto pair{blockedWpConnections.begin()}; pair != blockedWpConnections.end() && notFound; ++pair) {
            if(waypoints[pair->first]->clusterId == areas.first && waypoints[pair->second]->clusterId == areas.second) {

                std::vector<Waypoint_Connection>* conn {&(waypoints[pair->first]->connections)};
                for(auto c{conn->begin()}; c != conn->end(); ++c) {
                    if(c->to == pair->second) {
                        c->active = true;
                    }
                }

                blockedWpConnections.erase(pair);
                notFound = false;
            }
        }
    }

    // Change areas to check in the other direction
    uint8_t aux {areas.first};
    areas.first = areas.second;
    areas.second = aux;

    notFound = true;
    if(spawnConnection[areas.first][areas.second] == 2) {
        spawnConnection[areas.first][areas.second] = 1;

        // Unlock waypoint connections blocked by door
        for(auto pair{blockedWpConnections.begin()}; pair != blockedWpConnections.end() && notFound; ++pair) {
            if(waypoints[pair->first]->clusterId == areas.first && waypoints[pair->second]->clusterId == areas.second) {

                std::vector<Waypoint_Connection>* conn {&(waypoints[pair->first]->connections)};
                for(auto c{conn->begin()}; c != conn->end(); ++c) {
                    if(c->to == pair->second) {
                        c->active = true;
                    }
                }

                blockedWpConnections.erase(pair);
                notFound = false;
            }
        }
    }
}

void NavigationSystem::clearMapNavigation() {
    // Clear all navigation information
    clusters.clear();
    clustersConnection.clear();
    spawnConnection.clear();
    blockedWpConnections.clear();
    waypoints.clear();
    clustersIndex.clear();
    spawnpoints.clear();
}

Waypoint_t* NavigationSystem::getWaypoint(size_t index) {
    Waypoint_t* wp = nullptr;
    if(index < waypoints.size())
        wp = waypoints[index].get();

    return wp;
}

void NavigationSystem::debugNavigation() {
    float wpX, wpY, wpZ;

    if (device != nullptr) {
        for(auto i = waypoints.begin(); i != waypoints.end(); ++i) {
            wpX = (*i)->x;
            wpY = (*i)->y;
            wpZ = (*i)->z;

            if(debugWaypoints) {
                device->draw3DLine(wpX-1, wpY, wpZ-1, wpX+1, wpY, wpZ+1);
                device->draw3DLine(wpX-1, wpY, wpZ+1, wpX+1, wpY, wpZ-1);
                device->draw3DLine(wpX, wpY+1, wpZ, wpX, wpY-1, wpZ);
            }

            if(debugConnections) {
                std::vector<Waypoint_Connection>* conn = &(*i)->connections;
                for(size_t j = 0; j<conn->size(); j++) {
                    if((*conn)[j].active) {
                        Waypoint_t* wpObj = waypoints[(*conn)[j].to].get();
                        device->draw3DLine(  wpX, wpY, wpZ, 
                                            wpX + (wpObj->x - wpX)/2, wpY + (wpObj->y - wpY)/2, wpZ + (wpObj->z - wpZ)/2);
                    }
                }
            }
        }

        if(debugSpawns) {
            for(size_t i=0; i< spawnpoints.size(); ++i) {
                for(auto j=spawnpoints[i].begin(); j != spawnpoints[i].end(); ++j) {
                    wpX = (*j)->x;
                    wpY = (*j)->y;
                    wpZ = (*j)->z;

                    device->draw3DLine(wpX-1, wpY, wpZ-1, wpX+1, wpY, wpZ+1);
                    device->draw3DLine(wpX-1, wpY, wpZ+1, wpX+1, wpY, wpZ-1);
                    device->draw3DLine(wpX, wpY+1, wpZ, wpX, wpY-1, wpZ);
                }
            }
        }
    }
}

/* 
Configure what you want to debug.
First prameter: waypoints
Second parameter: connections
Third parameter: spawn points
*/
void NavigationSystem::configureDebug(bool waypoints, bool connections, bool spawns) noexcept {
    debugWaypoints      = waypoints;
    debugConnections    = connections;
    debugSpawns         = spawns;

    // Check if something has to be debuged
    if(waypoints || connections || spawns)  device = GraphicEngine::Instance();
    else                                    device = nullptr;
}

std::vector<size_t> NavigationSystem::getSizeSpawn(){
    std::vector<size_t> vec{};
    vec.emplace_back(spawnpoints.size());
    vec.emplace_back(spawnpoints[0].size());
    return vec;
}

Waypoint_t* NavigationSystem::getSpawnPoint(size_t zone, size_t point){
    return spawnpoints[zone][point].get();
}

Waypoint_t* NavigationSystem::getSpawnPoint(float x, float y, float z){
    updatePlayerArea(x, y, z);
    
    // Gets a random number
    size_t random {(size_t)std::rand()%20};
    size_t index {0};

    // Get a random spawn area
    do {
        // Search an area connected to player area
        do{
            ++index;
            if(index >= spawnConnection.size()) index = 0;
        }while(spawnConnection[index][playerArea] != 1);

        --random;
    } while(random <= 20);

    // Return a random spawn point of the area
    random = std::rand()%spawnpoints[index].size();
    return spawnpoints[index][random].get();
}

void NavigationSystem::findPathBasic(float fromX, float fromY, float fromZ, float toX, float toY, float toZ, NavigationComponent& navCmp) {
    uint8_t fromCluster{255}, toCluster{255};
    Waypoint_t* fromWaypoint{nullptr};
    Waypoint_t* toWaypoint{nullptr};

    // Find the initial and final clusters
    for(size_t i=0; i<clusters.size(); ++i) {
        // Check initial position
        if(fromCluster == 255)
            if(clusters[i].isPointInside({fromX, fromY, fromZ}))
                fromCluster = i;

        // Check final position
        if(toCluster == 255)
            if(clusters[i].isPointInside({toX, toY, toZ}))
                toCluster   = i;
    }

    // Initial search position due to from cluster
    size_t  initPos         {0};
    bool    insideCluster   {true};
    if(fromCluster == 255) {
        insideCluster   = false;
        fromCluster     = 0;
    }
    else 
        initPos         = clustersIndex[fromCluster];

    // Find the closest waypoint to initial and final location
    float   distFrom    = FLT_MAX,
            distTo      = FLT_MAX;
    if(fromCluster == toCluster) {
        // Search the waypoints in the same cluster
        for(size_t i = initPos; i < waypoints.size(); ++i) {
            Waypoint_t* wp = waypoints[i].get();

            if(wp->clusterId == fromCluster) {
                // Calculate the distance to check the shortest
                // Initial point
                float dist = std::sqrt((wp->x-fromX)*(wp->x-fromX) + (wp->y-fromY)*(wp->y-fromY) + (wp->z-fromZ)*(wp->z-fromZ));

                if(dist < distFrom) {
                    distFrom        = dist;
                    fromWaypoint    = wp;
                }

                // Final point
                dist = std::sqrt((wp->x-toX)*(wp->x-toX) + (wp->y-toY)*(wp->y-toY) + (wp->z-toZ)*(wp->z-toZ));

                if(dist < distTo) {
                    distTo      = dist;
                    toWaypoint  = wp;
                }
            }
            else {
                if(insideCluster) 
                    // If the waypoint is in another cluster, stop looking
                    i = waypoints.size();
                else {
                    // If the position is not in a cluster, continue throw all clusters
                    ++fromCluster;
                    --i;
                }
            }
        }
    }
    else {
        // Search each waypoint in the proper cluster
        for(size_t i = initPos; i < waypoints.size(); ++i) {
            Waypoint_t* wp = waypoints[i].get();

            if(wp->clusterId == fromCluster) {
                // Calculate the distance to check the shortest
                float dist = std::sqrt((wp->x-fromX)*(wp->x-fromX) + (wp->y-fromY)*(wp->y-fromY) + (wp->z-fromZ)*(wp->z-fromZ));

                if(dist < distFrom) {
                    distFrom        = dist;
                    fromWaypoint    = wp;
                }
            }
            else {
                if(insideCluster) 
                    // If the waypoint is in another cluster, stop looking
                    i = waypoints.size();
                else {
                    // If the position is not in a cluster, continue throw all clusters
                    ++fromCluster;
                    --i;
                }
            }
        }

        // Initial search position due to to cluster
        if(toCluster == 255) {
            toCluster       = 0;
            insideCluster   = false;
            initPos         = 0;
        }
        else 
            insideCluster   = true;
            initPos         = clustersIndex[toCluster];

        for(size_t i = initPos; i < waypoints.size(); ++i) {
            Waypoint_t* wp = waypoints[i].get();

            if(wp->clusterId == toCluster) {
                // Calculate the distance to check the shortest
                float dist = std::sqrt((wp->x-toX)*(wp->x-toX) + (wp->y-toY)*(wp->y-toY) + (wp->z-toZ)*(wp->z-toZ));

                if(dist < distTo) {
                    distTo      = dist;
                    toWaypoint  = wp;
                }
            }
            else {
                if(insideCluster) 
                    // If the waypoint is in another cluster, stop looking
                    i = waypoints.size();
                else {
                    // If the position is not in a cluster, continue throw all clusters
                    ++toCluster;
                    --i;
                }
            }
        }
    }

    if(fromWaypoint != nullptr && toWaypoint != nullptr) 
        findPathBasic(fromWaypoint, toWaypoint, navCmp);
}

void NavigationSystem::findPathBasic(const Waypoint_t* const fromWp, float toX, float toY, float toZ, NavigationComponent& navCmp) {
    uint8_t toCluster{255};
    Waypoint_t* toWaypoint{nullptr};

    // Find the final cluster
    for(size_t i=0; i<clusters.size(); ++i) {
        // Check final position
        if(toCluster == 255)
            if(clusters[i].isPointInside({toX, toY, toZ}))
                toCluster   = i;
    }

    // Initial search position due to cluster
    size_t  initPos         {0};
    bool    insideCluster   {true};
    if(toCluster == 255) {
        insideCluster   = false;
        toCluster       = 0;
    }
    else
        initPos         = clustersIndex[toCluster];

    // Find the closest waypoint to final location
    float distTo = FLT_MAX;
    
    for(size_t i = initPos; i < waypoints.size(); ++i) {
        Waypoint_t* wp = waypoints[i].get();

        if(wp->clusterId == toCluster) {
            // Calculate the distance to check the shortest
            float dist = std::sqrt((wp->x-toX)*(wp->x-toX) + (wp->y-toY)*(wp->y-toY) + (wp->z-toZ)*(wp->z-toZ));
            if(dist < distTo) {
                distTo      = dist;
                toWaypoint  = wp;
            }
        }
        else {
            if(insideCluster) 
                // If the waypoint is in another cluster, stop looking
                i = waypoints.size();
            else {
                // If the position is not in a cluster, continue throw all clusters
                ++toCluster;
                --i;
            }
        }
    }

    if(fromWp != nullptr && toWaypoint != nullptr)
        findPathBasic(fromWp, toWaypoint, navCmp);
}

void NavigationSystem::findPathBasic(const Waypoint_t* const fromWp, const Waypoint_t* const toWp, NavigationComponent& navCmp) {

    // The lists of waypoints for the algorithm
    OpenList    openList{};
    ClosedList  nodeList{};

    // Check if tactical properties should be checked
    bool checkTactical{false};
    for(auto &value : navCmp.pathWeights) {
        if(value != 0) checkTactical = true;
    }

    // Create first waypoint of pathfinding
    PathfindingNode from {fromWp->id, UINT16_MAX, 0, estimateCost(fromWp, toWp), true, 0};

    // Calculate tactical properties of waypoint
    if(checkTactical) {
        from.tacticalCost = calculateTacticalCost(navCmp, fromWp);

        if(from.tacticalCost != 0) {
            // Save current value
            unsigned int previousCost = from.cost;

            // Modify cost
            from.cost += from.tacticalCost;

            // Prevent bad values
            if(from.tacticalCost < 0 && previousCost < from.cost)
                from.cost = 1;
            else if(from.tacticalCost > 0 && previousCost > from.cost)
                from.cost = UINT_MAX - 1;

            // Modify estimated cost
            previousCost = from.estimatedCost;
            from.estimatedCost += from.tacticalCost;

            // Prevent bad values
            if(from.tacticalCost < 0 && previousCost < from.estimatedCost)
                from.estimatedCost = 1;
            else if(from.tacticalCost > 0 && previousCost > from.estimatedCost)
                from.estimatedCost = UINT_MAX - 1;
        }
    }

    // Adds the initial waypoint to the open list
    openList.insert({from.estimatedCost, from});
    nodeList.insert({from.nodeId, from});

    // Pathfinding loop
    PathfindingNode node{};
    Waypoint_t*     wp{nullptr};
    while(!openList.empty()) {
        // Get the estimated closest waypoint to the goal
        auto topIterator = openList.begin();
        node    = topIterator->second;
        wp      = waypoints[node.nodeId].get();

        // If it is the goal, stop the algorithm
        if(wp->id == toWp->id) break;

        // Remove waypoint from open list and declare it closed
        auto nodeIterator = nodeList.find(node.nodeId);
        nodeIterator->second.open = false;
        openList.erase(topIterator);

        // Get waypoint connections
        auto conn = wp->connections;

        for(auto c = conn.begin(); c != conn.end(); ++c) {
            // Check if the connection is active
            if(c->active) {
                // Calculate current cost
                unsigned int eCost {node.cost + c->cost};

                // Create the node of connected waypoint
                PathfindingNode connectionNode {c->to, node.nodeId, eCost, eCost, true};

                // Search node in the node list
                auto connNodeIterator {nodeList.find(c->to)};

                if (connNodeIterator != nodeList.end()) {
                    // The waypoint exists, so check if this path is shorter
                    if(eCost < connNodeIterator->second.cost) {
                        // Calculate estimated time with the previous estimated time
                        connectionNode.estimatedCost = connNodeIterator->second.estimatedCost - (connNodeIterator->second.cost - eCost);

                        if(connNodeIterator->second.open) {
                            // Node is in open list, so look for it and remove it
                            auto openIterators = openList.equal_range(connNodeIterator->second.estimatedCost);

                            for(auto i=openIterators.first; i != openIterators.second; ++i) {
                                if(i->second.nodeId == connectionNode.nodeId) {
                                    openList.erase(i);
                                    break;
                                }
                            }
                        }
                        
                        // Update node and add it to open list
                        connNodeIterator->second = connectionNode;
                        openList.insert({connectionNode.estimatedCost, connectionNode});
                    }
                }
                else {
                    // It is a new waypoint, calculate estimated cost
                    connectionNode.estimatedCost += estimateCost(waypoints[connectionNode.nodeId].get(), toWp);

                    // For strategic pathfinding, check additional weights
                    if(checkTactical) {
                        connectionNode.tacticalCost = calculateTacticalCost(navCmp, wp);

                        if(connectionNode.tacticalCost != 0) {
                            // Save current value
                            unsigned int previousCost = connectionNode.cost;

                            // Modify cost
                            connectionNode.cost += connectionNode.tacticalCost;

                            // Prevent bad values
                            if(connectionNode.tacticalCost < 0 && previousCost < connectionNode.cost)
                                connectionNode.cost = 1;
                            else if(connectionNode.tacticalCost > 0 && previousCost > connectionNode.cost)
                                connectionNode.cost = UINT_MAX - 1;

                            // Modify estimated cost
                            previousCost = connectionNode.estimatedCost;
                            connectionNode.estimatedCost += connectionNode.tacticalCost;

                            // Prevent bad values
                            if(connectionNode.tacticalCost < 0 && previousCost < connectionNode.estimatedCost)
                                connectionNode.estimatedCost = 1;
                            else if(connectionNode.tacticalCost > 0 && previousCost > connectionNode.estimatedCost)
                                connectionNode.estimatedCost = UINT_MAX - 1;
                        }
                    }

                    // Add the waypoint to open list
                    nodeList.insert({connectionNode.nodeId, connectionNode});
                    openList.insert({connectionNode.estimatedCost, connectionNode});
                }
            }
        }
    }
    
    // Clear current path
    navCmp.path.clear();

    // If a path has been found, save it
    if(wp->id == toWp->id) {
        // Make the path revisiting waypoint nodes
        std::vector<uint16_t> pathFound;
        pathFound.reserve(10);
        
        while(node.nodeId != fromWp->id) {
            pathFound.emplace_back(node.nodeId);
            node = nodeList.find(node.previousNodeId)->second;
        }
        pathFound.emplace_back(node.nodeId);

        // Reverse path and save it
        for(size_t i=pathFound.size()-1; i<pathFound.size(); --i) {
            navCmp.path.emplace_back(waypoints[pathFound[i]].get());
        }
    }


}

unsigned int NavigationSystem::estimateCost(const Waypoint_t* const fromWp, const Waypoint_t* const toWp) {
    // Get an estimation depending on the clusters waypoints belong to
    unsigned int estimation = clustersConnection[fromWp->clusterId][toWp->clusterId];
    
    // Adds an extra cost that depends on the distance of the nodes
    unsigned int dist = std::sqrt((toWp->x-fromWp->x)*(toWp->x-fromWp->x) + (toWp->y-fromWp->y)*(toWp->y-fromWp->y) + (toWp->z-fromWp->z)*(toWp->z-fromWp->z))/DIST_COST_ESTIMATION;

    return estimation + dist;
}

void NavigationSystem::calculateNewPath(std::vector<float> f, EntityMan* EM, size_t entId){

    Entity* e = EM->getEntityById(entId);
    if(e!=nullptr && e->hasComponent<AIComponent>() && !(e->hasTag<MarkToDestroyTag>() || e->hasTag<MarkToDestroyDelayTag>())){
        AIComponent&         aic = EM->getComponent<AIComponent>(*e);
        NavigationComponent&  nc = EM->getComponent<NavigationComponent>(*e);
        BasicComponent&      bsc = EM->getComponent<BasicComponent>(*e);
    
        Waypoint_t* wp = nullptr;
        if(aic.tactive){
            //guarda el primer wp al que esta yendo
            wp = nc.path.front();
            findPathBasic(wp, f[3], f[4], f[5], nc);
        }
        else{
            nc.path.clear();
            findPathBasic(f[0], f[1], f[2], f[3], f[4], f[5], nc);
        }

        if(nc.path.size()>1){
            // Check if the first waypoint to go is behind of the enemy orientation to remove it
            constexpr auto PI {std::numbers::pi};

            auto distx { nc.path.front()->x - bsc.x };
            auto distz { nc.path.front()->z - bsc.z };

            auto torien { std::atan2(distx, distz) };    // Calcula la arcotangente que da el angulo que necesita para llegar al objetivo
            if (torien < 0)    torien += 2*PI;           // Convertimos el angulo a positivo si es negativo

            auto angleDiff = std::abs(bsc.orientation - torien);
            if(angleDiff>=PI/3){
                nc.path.erase(nc.path.begin());
            }
        }

        if(e->hasTag<MovableObjectiveTag>() && nc.path.size()>30){
            nc.path.clear();
        }

        nc.needToUpdate = NEED_TO_UPDATE_FALSE;
        nc.updateTime = NAVIGATION_UPDATE_TIME;
        aic.tactive = false;
    }
}

int NavigationSystem::calculateTacticalCost(NavigationComponent& navCmp, const Waypoint_t* wp) {
    int tCost{0};

    if(navCmp.pathWeights[0] != 0) {
        // Check waypoint visibility
        tCost += (navCmp.pathWeights[0]/(float) MAX_WEIGHT_VALUE) * (wp->visibility * 3);
    }

    return tCost;
}

void NavigationSystem::updatePlayerArea(float x, float y, float z) noexcept {
    // Find the cluster of the position (player position)
    if(clusters[playerArea].isPointOutside({x, y, z})) {
        for(size_t i=0; i<clusters.size(); ++i) {
            if(clusters[i].isPointInside({x, y, z})) {
                playerArea = i;
                break;
            }
        }
    }
}

MapCluster NavigationSystem::getPlayerCluster(float x, float y, float z) noexcept {
    updatePlayerArea(x, y, z);

    return clusters[playerArea];
}

void NavigationSystem::update(EntityMan& EM, float deltaTime, std::vector<Entity*> targetEntities){
    using CList = MetaP::Typelist<NavigationComponent, AIComponent>;
    using TList = MetaP::Typelist<>;
    EM.foreach<CList, TList>([&](Entity& e){
        if(!e.hasTag<MarkToDestroyTag>() && !e.hasTag<MarkToDestroyDelayTag>()){
            auto& pos = EM.getComponent<BasicComponent>(e);
            auto& nav = EM.getComponent<NavigationComponent>(e);
            auto& ai  = EM.getComponent<AIComponent>(e);

            if(e.hasTag<EnemyTag>()){
                using CList = MetaP::Typelist<>;
                using TList = MetaP::Typelist<PlayerTag>;
                std::vector<Entity*> player = EM.template search<CList, TList>();

                BasicComponent* basicObj;
                if(targetEntities.size()>1 && (ai.targetMask == 1)){
                    basicObj = &EM.getComponent<BasicComponent>(*targetEntities[1]);
                }
                else{
                    basicObj = &EM.getComponent<BasicComponent>(*targetEntities[0]);
                }

                if(nav.updateTime<=0 && (nav.needToUpdate == NEED_TO_UPDATE_FALSE)){
                    nav.needToUpdate = NEED_TO_UPDATE_TRUE;
                }
                else if(nav.needToUpdate == NEED_TO_UPDATE_FALSE){
                    nav.updateTime -= deltaTime;
                }

                if(nav.needToUpdate == NEED_TO_UPDATE_TRUE){
                    std::vector<float> floatParams;
                    floatParams.push_back(pos.x); floatParams.push_back(pos.y); floatParams.push_back(pos.z);
                    floatParams.push_back(basicObj->x); floatParams.push_back(basicObj->y); floatParams.push_back(basicObj->z);

                    std::unique_ptr<BehaviourData> bdata(new BehaviourData(&NavigationSystem::calculateNewPath, 3, 1, floatParams, &EM, e.getId(), this));
                    Scheduler::Instance()->addBehaviour(*bdata.get());

                    nav.needToUpdate = NEED_TO_UPDATE_RUNNING;
                }
            }
            else{
                // Entity is a Movable Objective
                if(nav.updateTime<=0 && (nav.needToUpdate == NEED_TO_UPDATE_FALSE)){
                    nav.needToUpdate = NEED_TO_UPDATE_TRUE;
                }
                else if(nav.needToUpdate == NEED_TO_UPDATE_FALSE){
                    nav.updateTime -= deltaTime;
                }
                if(nav.needToUpdate == NEED_TO_UPDATE_TRUE && ai.targetMask < ai.routeToFollow.size()){
                    std::vector<float> floatParams {};
                    floatParams.push_back(pos.x); floatParams.push_back(pos.y); floatParams.push_back(pos.z);
                    floatParams.push_back(ai.routeToFollow[ai.targetMask][0]);
                    floatParams.push_back(ai.routeToFollow[ai.targetMask][1]);
                    floatParams.push_back(ai.routeToFollow[ai.targetMask][2]);

                    std::unique_ptr<BehaviourData> bdata(new BehaviourData(&NavigationSystem::calculateNewPath, 3, 1, floatParams, &EM, e.getId(), this));
                    Scheduler::Instance()->addBehaviour(*bdata.get());

                    nav.needToUpdate = NEED_TO_UPDATE_RUNNING;
                }
            }
        }
    });
}