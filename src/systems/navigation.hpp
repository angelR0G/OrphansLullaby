#pragma once

#include "../engine/types.hpp"
#include "../utils/cluster.hpp"
#include <vector>

struct GraphicEngine;

struct NavigationSystem {
    void loadMapNavigation(uint8_t);
    void clearMapNavigation();
    void debugNavigation();
    void configureDebug(bool, bool, bool) noexcept;
    Waypoint_t* getWaypoint(size_t);
    std::vector<size_t> getSizeSpawn();
    Waypoint_t* getSpawnPoint(size_t, size_t);
    Waypoint_t* getSpawnPoint(float, float, float);
    void findPathBasic(float, float, float, float, float, float, NavigationComponent&);
    void findPathBasic(const Waypoint_t* const, float, float, float, NavigationComponent&);
    void findPathBasic(const Waypoint_t* const, const Waypoint_t* const, NavigationComponent&);
    unsigned int estimateCost(const Waypoint_t* const, const Waypoint_t* const);
    void unlockDoorConnection(std::pair<uint8_t, uint8_t>);

    void calculateNewPath(std::vector<float>, EntityMan*, size_t);

    void update(EntityMan& EM, float deltaTime, std::vector<Entity*> targetEntities);

    [[nodiscard]] MapCluster getPlayerCluster(float, float, float) noexcept;

    ~NavigationSystem();

    private:
        // Clusters of waypoints reference position
        static std::vector<MapCluster> clusters;
        // Matrix of conection between waypoint clusters
        static std::vector<std::vector<uint16_t>> clustersConnection;
        // Waypoint array
        static std::vector<std::shared_ptr<Waypoint_t>> waypoints;
        // Clusters index for search optimization
        static std::vector<uint16_t> clustersIndex;
        // Spawn points arrays
        static std::vector<std::vector<std::shared_ptr<Waypoint_t>>> spawnpoints;
        // Matrix of connections between clusters used for spawn
        static std::vector<std::vector<uint8_t>> spawnConnection;
        // Vector with waypoint connections blocked by doors
        static std::vector<std::pair<uint16_t, uint16_t>> blockedWpConnections;
        // Last player area
        static uint8_t playerArea;
        
        // Debug
        bool debugWaypoints{false};
        bool debugConnections{false};
        bool debugSpawns{false};
        GraphicEngine* device{nullptr};

        int calculateTacticalCost(NavigationComponent&, const Waypoint_t*);
        void updatePlayerArea(float, float, float) noexcept;
};