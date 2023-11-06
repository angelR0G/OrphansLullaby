#include "CollisionCreator.hpp"
#include "../engine/physics/physicsEngine.hpp"
#include <fstream>
#include <math.h>

// Program mode
#define NONE                            0
#define READ_AABB                       1
#define READ_VERTICAL_ORIENTED_BOX      2
#define READ_ROTATED_BOX                4
#define READ_NO_SIGHT_BOX               8

// Names for changing state
#define MODE_1      "acollision"
#define MODE_2      "yawcollision"
#define MODE_3      "rotcollision"
#define MODE_4      "sightcollision"

// Other macros
#define SCALE_FACTOR        10
#define ANGLE_CONVERSION    -3.1415927/180

struct CollisionProcesor {
    float   minX{FLT_MAX},
            minY{FLT_MAX},
            minZ{FLT_MAX},
            maxX{-FLT_MAX},
            maxY{-FLT_MAX},
            maxZ{-FLT_MAX};
    float   yaw{0.0f},
            pitch{0.0f};
    uint16_t    mask{SOLID_COLLISION};

    void reset() {
        minX    = FLT_MAX;
        minY    = FLT_MAX;
        minZ    = FLT_MAX;
        maxX    = -FLT_MAX;
        maxY    = -FLT_MAX;
        maxZ    = -FLT_MAX;
        yaw     = 0.0f;
        pitch   = 0.0f;
        mask    = SOLID_COLLISION;
    }

    void processPoint(float x, float y, float z) {
        if(x < minX)
            minX = x;
        if(x > maxX)
            maxX = x;
        if(y < minY)
            minY = y;
        if(y > maxY)
            maxY = y;
        if(z < minZ)
            minZ = z;
        if(z > maxZ)
            maxZ = z;
    }

    bool isInside(float x, float y, float z) {
        return x >= minX && x <= maxX && y >= minY && y <= maxY && z >= minZ && z <= maxZ;
    }

    void generateCollision(std::vector<CollisionObject>& collisionVector, PhysicsEngine& engine) {
        Entity*             newCol = nullptr;
        CollisionComponent* colCmp = nullptr;

        float midX  = (maxX + minX) / 2.0;
        float midY  = (maxY + minY) / 2.0;
        float midZ  = (maxZ + minZ) / 2.0;
        float sizeX = maxX - midX;
        float sizeY = maxY - midY;
        float sizeZ = maxZ - midZ;

        collisionVector.emplace_back(engine.addCollisionObject(mask, midX, midY, midZ, pitch, yaw));
        engine.addBoxColliderToObject(&collisionVector[collisionVector.size()-1], 0, 0, 0, sizeX, sizeY, sizeZ);
    }
};

/*
Given a file path as argument, the program reads the file trying to get the information about 
collisions in it. The file must be .obj. 
To find AABB collisions objects have to be called as MODE_1 macro.
Objects named as MODE_2 macro will be rotated around vertical axis with the given angle: "{MODE_2}_{angle}"
Objects named as MODE_3 macro will be rotated around two axis with the given angles: "{MODE_3}_{vert_angle}_{horiz_angle}"
*/ 
void generateMapCollisions(std::string file, PhysicsEngine& engine, std::vector<CollisionObject>& collisionVector) {
    // Open given file
    std::ifstream               readFile;
    readFile.open(file);

    try {
        // If finds the file, reads it
        if (!readFile.fail() && readFile.is_open()) {
            std::string line;

            // Initialize values
            uint8_t         mode                = NONE;
            unsigned int    numCollisions       = 0;
            bool            collisionProcesed   = false;
            bool            nextData;
            size_t          nextPos;
            std::vector<std::string>    data;
            CollisionProcesor           col;

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
                        if(collisionProcesed) {
                            // Creates a collision and reset the procesor
                            col.generateCollision(collisionVector, engine);
                            
                            col.reset();
                            collisionProcesed = false;
                        }

                        // Change mode for the next collision
                        if(data[1].find(MODE_1) != data[1].npos)
                            mode = READ_AABB;
                        else if(data[1].find(MODE_2) != data[1].npos ||
                                data[1].find(MODE_4) != data[1].npos) {
                            // Check found mode
                            if(data[1].find(MODE_2) != data[1].npos)        
                                mode = READ_VERTICAL_ORIENTED_BOX;
                            else {
                                mode = READ_NO_SIGHT_BOX;

                                // Set collision mask
                                col.mask = NO_SIGHT_COLLISION;
                            }

                            // Search yaw rotation
                            size_t      pos = data[1].find('_');
                            std::string aux = data[1].substr(pos+1);
                            pos = aux.find('_');
                            if(pos != aux.npos)
                                aux = aux.substr(0, pos);

                            // Convert to radians and save it
                            col.yaw = std::stof(aux) * ANGLE_CONVERSION;
                        }
                        else if(data[1].find(MODE_3) != data[1].npos) {
                            mode = READ_ROTATED_BOX;

                            // Search yaw rotation
                            size_t      pos = data[1].find('_');
                            std::string aux = data[1].substr(pos+1);
                            std::string aux2 = aux;
                            pos = aux.find('_');
                            if(pos != aux.npos)
                                aux = aux.substr(0, pos);

                            // Convert to radians and save it
                            col.yaw = std::stof(aux) * ANGLE_CONVERSION;

                            // Search pitch rotation
                            aux = aux2.substr(pos+1);
                            pos = aux.find('_');
                            if(pos != aux.npos)
                                aux = aux.substr(0, pos);

                            // Convert to radians and save it
                            col.pitch = std::stof(aux) * ANGLE_CONVERSION;
                        }
                    }
                    else if(data[0].compare("v") == 0) {
                        ///////////////////////////////////////////////////////
                        // New vertex
                        ///////////////////////////////////////////////////////
                        float x = std::round(std::stof(data[1]) * -SCALE_FACTOR * 1000) / 1000;
                        float y = std::round(std::stof(data[2]) * SCALE_FACTOR * 1000) / 1000;
                        float z = std::round(std::stof(data[3]) * SCALE_FACTOR * 1000) / 1000;

                        if(mode & (READ_AABB | READ_VERTICAL_ORIENTED_BOX | READ_ROTATED_BOX | READ_NO_SIGHT_BOX)) {
                            // Update the collision procesor
                            col.processPoint(x, y, z);
                            collisionProcesed = true;
                        }
                    }
                } 
            }

            // Creates the last collision if it is has not been processed
            if(collisionProcesed) {
                col.generateCollision(collisionVector, engine);
                col.reset();
            }

            readFile.close();
        }
    }catch (std::exception const& e){
        if(readFile.is_open())
            readFile.close();
    }

    return;
}