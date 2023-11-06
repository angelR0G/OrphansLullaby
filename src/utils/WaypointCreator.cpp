#include "WaypointCreator.hpp"
#include <fstream>
#include <math.h>
#include <cfloat>
#include <memory>

// Program mode
#define NONE                            0
#define READ_WAYPOINTS                  1
#define READ_DIRECCTIONAL_WAYPOINTS     2
#define READ_CLUSTER                    3
#define READ_SPAWNPOINTS                4
#define READ_WP_PROPERTY_VISIBILITY     5

// Names for changing state
#define MODE_1      "waypoints"
#define MODE_2      "direccional"
#define MODE_3      "cluster"
#define MODE_4      "spawn"
#define MODE_5      "wpVisibility"

// Other macros
#define SCALE_FACTOR            10
#define DEFAULT_VISIBLE_VALUE   100

/*
Given a file path as argument, the program reads the file trying to get the information about 
waypoints and spawn points in it.
The file must be .obj. Should the waypoints be clasified in clusters they have to be specified 
first in objects that contain MODE_3 macro in their name.
To find waypoints in an object it has to be called as MODE_1 macro.
To specify one direction connections between waypoints use an object called as MODE_2 macro, 
but always after waypoints have been previously found.
Waypoint properties should always be found after waypoints.
*/
void generateWaypoints( std::string                                             file, 
                        std::vector<MapCluster>&                                clusters, 
                        std::vector<uint16_t>&                                  clustersIndex, 
                        std::vector<std::shared_ptr<Waypoint_t>>&               waypointsVector, 
                        std::vector<std::vector<std::shared_ptr<Waypoint_t>>>&  spawnMatrix, 
                        std::vector<std::pair<uint16_t, uint16_t>>&             blockedConnections,
                        std::vector<std::vector<uint8_t>>&                      spawnConnection) {
    // Open given file
    std::ifstream               readFile;
    readFile.open(file);

    try {
        // If finds the file, reads it
        if (!readFile.fail() && readFile.is_open()) {
            std::string line;

            // Initialize values
            uint8_t         mode                = NONE;
            unsigned int    numWaypoints        = 0;
            unsigned int    numSpawnpoints      = 0;
            unsigned int    numClusters         = 0;
            size_t          nextPos;
            bool            nextData;
            std::vector<std::string>                    data;
            std::vector<std::shared_ptr<Waypoint_t>>    spawnVector;
            std::vector<uint16_t>                       direcctionalId;
            std::vector<uint16_t>                       finalId;
            clusters.clear();

            while (readFile.good()) {
                // While there are lines to read, continues
                std::getline(readFile, line);

                // Split line for spaces
                data.clear();
                do {
                    nextPos     = line.find(' ');
                    nextData    = nextPos != line.npos;

                    // If there is a space, split the two parts
                    if (nextData) {
                        data.emplace_back(line.substr(0, nextPos));
                        line = line.substr(nextPos+1);
                    }
                } while(nextData);
                data.emplace_back(line);

                if(data.size() > 0) {
                    if(data[0].compare("o") == 0) {
                        ///////////////////////////////////////////////////////
                        // Change mode
                        ///////////////////////////////////////////////////////
                        if(data[1].compare(MODE_1) == 0)
                            mode = READ_WAYPOINTS;

                        else if(data[1].compare(MODE_2) == 0)
                            mode = READ_DIRECCTIONAL_WAYPOINTS;

                        else if(data[1].compare(MODE_4) == 0)
                            mode = READ_SPAWNPOINTS;

                        else if(data[1].compare(MODE_5) == 0)
                            mode = READ_WP_PROPERTY_VISIBILITY;

                        else if(data[1].find(MODE_3) != data[1].npos) {
                            mode = READ_CLUSTER;

                            clusters.emplace_back(Position3D{FLT_MAX, FLT_MAX, FLT_MAX}, Position3D{-FLT_MAX, -FLT_MAX, -FLT_MAX}, numClusters);
                            numClusters++;

                            // Search cluster values
                            line = data[1];
                            data.clear();
                            do {
                                nextPos     = line.find('_');
                                nextData    = nextPos != line.npos;

                                // If there is a space, split the two parts
                                if (nextData) {
                                    data.emplace_back(line.substr(0, nextPos));
                                    line = line.substr(nextPos+1);
                                }
                            } while(nextData);
                            data.emplace_back(line);

                            if(data.size() > 1) {
                                // Save reverb value
                                clusters[numClusters-1].reverb = std::stof(data[1]);
                            }
                        }
                    }
                    else if(data[0].compare("v") == 0) {
                        ///////////////////////////////////////////////////////
                        // New vertex
                        ///////////////////////////////////////////////////////
                        float x = std::round(std::stof(data[1]) * -SCALE_FACTOR * 100) / 100;
                        float y = std::round(std::stof(data[2]) * SCALE_FACTOR * 100) / 100;
                        float z = std::round(std::stof(data[3]) * SCALE_FACTOR * 100) / 100;

                        if(mode == READ_WAYPOINTS) {
                            // Create a new Waypoint
                            std::shared_ptr<Waypoint_t> wp (new Waypoint_t(x, y, z, numWaypoints, 0, {}));
                            numWaypoints++;

                            waypointsVector.emplace_back(std::move(wp));
                        }
                        else if(mode == READ_SPAWNPOINTS) {
                            // Create a new Waypoint
                            std::shared_ptr<Waypoint_t> sp (new Waypoint_t(x, y, z, numWaypoints, 0, {}));
                            numSpawnpoints++;

                            spawnVector.emplace_back(std::move(sp));
                        }
                        else if(mode == READ_DIRECCTIONAL_WAYPOINTS || 
                                mode == READ_WP_PROPERTY_VISIBILITY) {
                            // Identify actual waypoint comparing positions
                            float distX;
                            float distY;
                            float distZ;

                            float   minDist     = FLT_MAX;
                            unsigned int    waypointNum = UINT32_MAX;
                            // Look for the closest point
                            for (size_t i = 0; i<waypointsVector.size(); i++) {
                                distX = waypointsVector[i]->x - x;
                                distY = waypointsVector[i]->y - y;
                                distZ = waypointsVector[i]->z - z;

                                float dist = sqrt(distX*distX + distY*distY + distZ*distZ);

                                if(dist < minDist) {
                                    minDist     = dist;
                                    waypointNum = i;
                                }
                            }
                            
                            if(mode == READ_DIRECCTIONAL_WAYPOINTS) {
                                // Adds to the vector the actual position of the waypoint
                                if(waypointNum != UINT32_MAX)
                                    direcctionalId.emplace_back(waypointNum);
                            }
                            else if(mode == READ_WP_PROPERTY_VISIBILITY) {
                                // Set a default values for visible waypoints
                                waypointsVector[waypointNum].get()->visibility = DEFAULT_VISIBLE_VALUE;
                            }
                        }
                        else if(mode == READ_CLUSTER) {
                            // Modify cluster range
                            MapCluster* c = &clusters[numClusters-1];

                            if(x < c->minPos.x) c->minPos.x = x;
                            if(y < c->minPos.y) c->minPos.y = y;
                            if(z < c->minPos.z) c->minPos.z = z;
                            if(x > c->maxPos.x) c->maxPos.x = x;
                            if(y > c->maxPos.y) c->maxPos.y = y;
                            if(z > c->maxPos.z) c->maxPos.z = z;
                        }
                    }
                    else if(data[0].compare("l") == 0) {
                        ///////////////////////////////////////////////////////
                        // New vertex connection
                        ///////////////////////////////////////////////////////
                        unsigned int firstWp    = std::stoi(data[1]) - 1;
                        unsigned int secondWp   = std::stoi(data[2]) - 1;

                        // Get the waypoints
                        Waypoint_t* wp1 = nullptr;
                        Waypoint_t* wp2 = nullptr;

                        if(mode == READ_WAYPOINTS || mode == READ_DIRECCTIONAL_WAYPOINTS) {
                            if(mode == READ_WAYPOINTS) {
                                wp1 = waypointsVector[firstWp].get();
                                wp2 = waypointsVector[secondWp].get();
                            }
                            else if(mode == READ_DIRECCTIONAL_WAYPOINTS) {
                                unsigned int    idx1 = direcctionalId[firstWp   - waypointsVector.size()];
                                unsigned int    idx2 = direcctionalId[secondWp  - waypointsVector.size()];

                                wp1 = waypointsVector[idx1].get();
                                wp2 = waypointsVector[idx2].get();
                            }

                            // Calculate distance between waypoints (connection cost)
                            float distX = wp1->x - wp2->x;
                            float distY = wp1->y - wp2->y;
                            float distZ = wp1->z - wp2->z;
                            uint16_t dist = std::round(std::sqrt(distX*distX + distY*distY + distZ*distZ) * 10);

                            // Add connection to waypoints
                            if(mode == READ_WAYPOINTS) {
                                wp1->addConnection({wp2->id, dist});
                                wp2->addConnection({wp1->id, dist});
                            }
                            else if(mode == READ_DIRECCTIONAL_WAYPOINTS) {
                                wp1->addConnection({wp2->id, dist});
                            }
                        }
                    }
                } 
            }

            // Assign waypoints to clusters
            for(size_t i=0; i<waypointsVector.size(); ++i) {
                Waypoint_t* wp = waypointsVector[i].get();
                
                // For every waypoint check every cluster
                if(wp->clusterId == 0) {
                    for(size_t j=0; j<numClusters && wp->clusterId == 0; ++j) {
                        MapCluster* c = &clusters[j];

                        // Check if the waypoint is inside the cluster 
                        if(c->isPointInside({wp->x, wp->y, wp->z}))
                            wp->clusterId = j;
                    }
                }

                // Check connected waypoints
                for(auto wpConn{wp->connections.begin()}; wpConn != wp->connections.end(); ++wpConn) {
                    Waypoint_t* connectedWp = waypointsVector[wpConn->to].get();
                    
                    // For every waypoint check every cluster
                    if(connectedWp->clusterId == 0) {
                        for(size_t j=0; j<numClusters && connectedWp->clusterId == 0; ++j) {
                            MapCluster* c = &clusters[j];

                            // Check if the waypoint is inside the cluster 
                            if(c->isPointInside({connectedWp->x, connectedWp->y, connectedWp->z}))
                                connectedWp->clusterId = j;
                        }
                    }

                    // If waypoints are in different clusters check if a door block the connection
                    if(wp->clusterId != connectedWp->clusterId) {
                        if(spawnConnection[wp->clusterId][connectedWp->clusterId] == 2) {
                            // Deactivate connection
                            wpConn->active = false;
                        }
                    }
                }
            }

            // Modify waypoints visibility property values
            for(size_t i=0; i<waypointsVector.size(); ++i) {
                Waypoint_t* wp = waypointsVector[i].get();

                // Count how many connected waypoints are visible
                uint8_t visibleConnections{0};
                for(auto wpConn{wp->connections.begin()}; wpConn != wp->connections.end(); ++wpConn) {
                    if(waypointsVector[wpConn->to].get()->visibility >= DEFAULT_VISIBLE_VALUE)
                        ++visibleConnections;
                }

                // Increase visibility value depending on the "visible" connected waypoints
                wp->visibility += DEFAULT_VISIBLE_VALUE * (visibleConnections/(float)wp->connections.size());
            }

            // For optimizing waypoint search, order the vector by cluster ID
            if(numClusters > 1) {
                finalId.reserve(waypointsVector.size());
                clustersIndex.resize(numClusters, UINT16_MAX);

                for(size_t i=0; i<waypointsVector.size(); ++i) {
                    std::shared_ptr<Waypoint_t> wp = waypointsVector[i];
                    bool reorder {false};
                    size_t reorderPos{0};
                    for(size_t j=i+1; j<waypointsVector.size(); ++j) {
                        if(waypointsVector[j]->clusterId < wp->clusterId) {
                            // Found a waypoint of previous cluster
                            wp          = waypointsVector[j];
                            reorder     = true;
                            reorderPos  = j;
                        }
                    }
                    if(reorder) {
                        // Reorder the two waypoints
                        waypointsVector[reorderPos] = waypointsVector[i];
                        waypointsVector[i]          = wp;
                    }
                    finalId[wp->id] = i;
                }
                
                // After ordering the waypoints, update their connections and ID and indexes clusters
                for(size_t i=0; i<waypointsVector.size(); ++i) {
                    waypointsVector[i]->id = i;

                    std::vector<Waypoint_Connection>* conn = &waypointsVector[i]->connections;
                    for(size_t j=0; j<conn->size(); ++j) {
                        (*conn)[j].to = finalId[(*conn)[j].to];

                        // Check if the connection is not active to save it
                        if(!(*conn)[j].active) {
                            std::pair<uint16_t, uint16_t> aux {i, (*conn)[j].to};
                            blockedConnections.emplace_back(aux);
                        }
                    }

                    if(clustersIndex[waypointsVector[i]->clusterId] > i)
                        clustersIndex[waypointsVector[i]->clusterId] = i;
                }
            }
            else {
                clustersIndex = {0};
            }

            // Initialize a spawnpoint vector for every cluster
            spawnMatrix.resize(std::max(numClusters, (unsigned int)1));

            // Save spawnpoints in their cluster
            for(size_t i=0; i<spawnVector.size(); i++) {
                std::shared_ptr<Waypoint_t> sp = spawnVector[i];

                // For every spawnpoint check every cluster
                for(size_t j=0; j<numClusters && sp->clusterId == 0; j++) {
                    MapCluster* c = &clusters[j];

                    // Check if the spawnpoint is inside the cluster 
                    if(c->isPointInside({sp->x, sp->y, sp->z}))
                        sp->clusterId = j;
                }

                spawnMatrix[sp->clusterId].emplace_back(sp);
            }

            readFile.close();
        }
    }catch (std::exception const& e){
        if(readFile.is_open())
            readFile.close();
    }

    return;
}