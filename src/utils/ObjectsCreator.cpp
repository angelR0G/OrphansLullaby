#include "ObjectsCreator.hpp"

#include "../manager/gamemanager.hpp"
#include "../engine/entityFactory.hpp"
#include "../engine/graphic/engine.hpp"
#include "../engine/graphic/sceneTree/sceneLight.hpp"
#include "../utils/vectorMath.hpp"
#include "WeaponTemplates.hpp"
#include <fstream>
#include <cassert>

// Program mode
#define NONE                            0
#define READ_AMMO_MACHINE               1
#define READ_INJECTION_MACHINE          2
#define READ_SOUND_BARREL               3
#define READ_EXPLOSIVE_BARREL           4
#define READ_WEAPON_MACHINE             5
#define READ_DOOR                       6
#define READ_PLAYER_SPAWN               7
#define READ_POWERUP_MACHINE            8
#define READ_SOUND_GENERATOR            9

// Names for changing state
#define MODE_1      "ammo"
#define MODE_2      "injection"
#define MODE_3      "soundbarrel"
#define MODE_4      "explosive"
#define MODE_5      "weapon"
#define MODE_6      "door"
#define MODE_7      "player"
#define MODE_8      "powerup"
#define MODE_9      "pointlight"
#define MODE_10     "dirlight"
#define MODE_11     "spotlight"
#define MODE_12     "mapsound"

// Other macros
//#define ANGLE_CONVERSION    -3.1415927/180
#define ANGLE_CONVERSION    -1

const std::vector<std::string> files {
    "media/maps/map1/map_data/objects.obj",
    "media/maps/map2/map_data/objects.obj",
    "media/maps/menuscene/map_data/objects.obj"
};

struct MapObject {
    uint8_t type{NONE};
    float   x{},
            y{},
            z{},
            yaw{};
    uint8_t extraData1{};
    uint8_t extraData2{};
    uint8_t extraData3{};
    float   extraData4{};
    float   extraData5{};

    GameManager* gameMan{GameManager::Instance()};
    EntityFactory* factory{EntityFactory::Instance()};

    WeaponData searchWeapon(uint8_t weaponId) {
        WeaponData weapon{};

        if(weaponId < WeaponTemplates::weapons.size()) {
            weapon = WeaponTemplates::weapons[weaponId];
        }

        return weapon;
    }

    void generateObject(Transform* playerSpawn) {
        // Generate an object
        if(type == READ_AMMO_MACHINE) {
            // Ammo machine
            factory->createBulletMachine(   Transform{{x, y, z, 0, yaw, 0, 6.f, 10.f, 5.f}}, 
                                            Transform{{0, 10, 8, 0, 0, 0, 20, 10, 12}});
        }
        else if(type == READ_INJECTION_MACHINE) {
            // Injections machine
            factory->createHealthMachine(   Transform{{x, y, z, 0, yaw, 0, 6.f, 10.f, 5.f}}, 
                                            Transform{{0, 10, 8, 0, 0, 0, 20, 10, 12}});
        }
        else if(type == READ_SOUND_BARREL) {
            // Distraction barrel
            factory->createBarrel(TARGET_BARREL_ID, Transform{{x, y, z, 0, yaw, 0, 3, 4.8, 3}});
        }
        else if(type == READ_EXPLOSIVE_BARREL) {
            // Explosive barrel
            factory->createBarrel(EXPLOSIVE_BARREL_ID, Transform{{x, y, z, 0, yaw, 0, 3, 4.8, 3}});
        }
        else if(type == READ_WEAPON_MACHINE) {
            // Weapon machine
            WeaponData w = searchWeapon(extraData1);

            if(w.weaponID != 0) {
                factory->createWeaponMachine(w, Transform{{x, y, z, 0, yaw, 0, 6.f, 4.2f, 5.f}}, 
                                                Transform{{10, 0, 0, 0, 0, 0, 10, 10, 20}});
            }
        }
        else if(type == READ_DOOR) {
            // Door
            Entity door = factory->createWallDoor(  Transform{{x, y, z, 0, yaw, 0, 1, 1, 1}}, 
                                                    Transform{{0, 0, 0, 0, 0, 0, extraData5 * SCALE_FACTOR_MAP, 20, extraData4 * SCALE_FACTOR_MAP}}, extraData3);
            gameMan->addDoor(door.getId(), {extraData1,extraData2});
        }
        else if(type == READ_PLAYER_SPAWN) {
            // Player spawn position
            playerSpawn->x = x;
            playerSpawn->y = y;
            playerSpawn->z = z;
            playerSpawn->ry = yaw;
        }
        else if(type == READ_POWERUP_MACHINE) {
            // Power up machine
            factory->createEffectMachine(   Transform{{x, y, z, 0, yaw, 0, 6.f, 10.f, 5.f}}, 
                                            Transform{{0, 10, 8, 0, 0, 0, 20, 10, 12}});
        }
        else if(type == READ_SOUND_GENERATOR) {
            // Sound emitter
            factory->createSoundEmitter(    Transform{{x, y, z, 0, 0, 0, 0, 0, 0}}, extraData1);
        }

        // Reset variables
        type = NONE;
        x = y = z = yaw = extraData4 = 0.f;
        extraData1 = extraData2 = extraData3 = 0;
    }
};

struct MapLight {
    GraphicEngine* engine {GraphicEngine::Instance()};

    bool    posSet{false};
    float   x{}, y{}, z{};
    float   r{}, g{}, b{};
    float   dirX{}, dirY{}, dirZ{};
    uint8_t type{LIGHT_TYPE_OMNI};
    float   intensity{},
            radius{},
            innerAngle{},
            outerAngle{};
    bool    castShadows{true};
    
    void calculateLightDirection() {
        // Calculate vector from pos to dir
        dirX -= x;
        dirY -= y;
        dirZ -= z;

        // Normalize to get light direction
        vectorMath::normalizeVector3D(&dirX, &dirY, &dirZ);
    }

    void generateLight() {
        if(type == LIGHT_TYPE_DIRECT) {
            // Modify sun light
            calculateLightDirection();

            engine->setSunlight({r, g, b}, {-dirX, -dirY, -dirZ}, intensity);
        }
        else {
            // Create new light
            auto l {engine->addSceneLight()};
            l->setType(type);
            l->setTranslation({x, y, z});
            l->setColor({r, g, b});
            l->setIntensity(intensity);
            l->setRadius(radius);
            l->setCastShadows(castShadows);

            // Add specific light type properties
            if(type == LIGHT_TYPE_SPOT) {
                calculateLightDirection();
                l->setDirection({dirX, dirY, dirZ});
                l->setInnerAngle(innerAngle);
                l->setOuterAngle(outerAngle);
            }
        }

        // Reset variables
        posSet      = false;
        castShadows = true;
    }
};

/*
Given a file path as argument, the program reads the file trying to get the information about 
map objects and lights in it.
*/
Transform generateMapObjects(uint8_t fileIndex) {
    Transform playerSpawn;

    // Open given file
    std::ifstream               readFile;
    readFile.open(files[fileIndex]);

    try {
        // If finds the file, reads it
        if (!readFile.fail() && readFile.is_open()) {
            std::string line;

            // Initialize values
            MapObject       mapObj  {};
            MapLight        mapLight{};
            size_t          nextPos {};
            bool            nextData{};
            bool            readLight{false};
            std::vector<std::string>    data{};

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
                        // Generate previous objects or light
                        if(readLight)   mapLight.generateLight();
                        else            mapObj.generateObject(&playerSpawn);

                        // Read new object or light
                        bool newObject{true};
                        readLight = false;
                        
                        if(data[1].find(MODE_1) != data[1].npos)
                            mapObj.type = READ_AMMO_MACHINE;

                        else if(data[1].find(MODE_2) != data[1].npos)
                            mapObj.type = READ_INJECTION_MACHINE;

                        else if(data[1].find(MODE_3) != data[1].npos)
                            mapObj.type = READ_SOUND_BARREL;

                        else if(data[1].find(MODE_4) != data[1].npos) 
                            mapObj.type = READ_EXPLOSIVE_BARREL;

                        else if(data[1].find(MODE_5) != data[1].npos) 
                            mapObj.type = READ_WEAPON_MACHINE;

                        else if(data[1].find(MODE_6) != data[1].npos) 
                            mapObj.type = READ_DOOR;

                        else if(data[1].find(MODE_7) != data[1].npos) 
                            mapObj.type = READ_PLAYER_SPAWN;

                        else if(data[1].find(MODE_8) != data[1].npos) 
                            mapObj.type = READ_POWERUP_MACHINE;

                        else if(data[1].find(MODE_9) != data[1].npos) {
                            mapLight.type   = LIGHT_TYPE_OMNI;
                            readLight       = true;
                        }
                        else if(data[1].find(MODE_10) != data[1].npos) {
                            mapLight.type   = LIGHT_TYPE_DIRECT;
                            readLight       = true;
                        }
                        else if(data[1].find(MODE_11) != data[1].npos) {
                            mapLight.type   = LIGHT_TYPE_SPOT;
                            readLight       = true;
                        }
                        else if(data[1].find(MODE_12) != data[1].npos)
                            mapObj.type = READ_SOUND_GENERATOR;
                        else
                            newObject = false;
                    
                        if(newObject) {
                            // Split object name in parts
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

                            if(readLight) {
                                // Get lights information
                                if( (mapLight.type == LIGHT_TYPE_DIRECT  && data.size() < 5) ||
                                    (mapLight.type == LIGHT_TYPE_OMNI    && data.size() < 6) ||
                                    (mapLight.type == LIGHT_TYPE_SPOT    && data.size() < 8)    ) 
                                    assert("Missing data loading lights.");

                                // Save light color
                                mapLight.r  = std::stof(data[1]);
                                mapLight.g  = std::stof(data[2]);
                                mapLight.b  = std::stof(data[3]);
                                mapLight.intensity  = std::stof(data[4]);

                                if(mapLight.type == LIGHT_TYPE_OMNI) {
                                    mapLight.radius     = std::stof(data[5]);
                                    if(data.size() > 6) mapLight.castShadows = false;
                                }
                                else if(mapLight.type == LIGHT_TYPE_SPOT) {
                                    mapLight.radius     = std::stof(data[5]);
                                    mapLight.innerAngle = std::stof(data[6]);
                                    mapLight.outerAngle = std::stof(data[7]);
                                    if(data.size() > 8) mapLight.castShadows = false;
                                }
                            }
                            else if (mapObj.type == READ_SOUND_GENERATOR){
                                // Get sound data
                                if(data.size() >= 2)
                                    mapObj.extraData1 = std::stoi(data[1]);
                            }
                            else {
                                // Get map objects information
                                // Get object rotation
                                mapObj.yaw = std::stof(data[data.size()-1]) * ANGLE_CONVERSION;

                                if(mapObj.type == READ_WEAPON_MACHINE) {
                                    // Get weapon ID
                                    mapObj.extraData1 = std::stoi(data[1]);
                                }
                                else if(mapObj.type == READ_DOOR) {
                                    // Get areas connected by doors
                                    mapObj.extraData1 = std::stoi(data[1]);
                                    mapObj.extraData2 = std::stoi(data[2]);

                                    // Get door model index
                                    mapObj.extraData3 = std::stoi(data[3]);

                                    // Get door size
                                    mapObj.extraData4 = std::stof(data[4]);
                                    mapObj.extraData5 = std::stof(data[5]);
                                }
                            }
                        }
                    }
                    else if(data[0].compare("v") == 0) {
                        ///////////////////////////////////////////////////////
                        // New vertex
                        ///////////////////////////////////////////////////////
                        float vx    = std::round(std::stof(data[1]) * -SCALE_FACTOR_MAP * 100) / 100;
                        float vy    = std::round(std::stof(data[2]) * SCALE_FACTOR_MAP * 100) / 100;
                        float vz    = std::round(std::stof(data[3]) * SCALE_FACTOR_MAP * 100) / 100;

                        if(readLight) {
                            // New light
                            if(!mapLight.posSet) {
                                mapLight.x      = vx;
                                mapLight.y      = vy;
                                mapLight.z      = vz; 

                                mapLight.posSet = true;
                                vy -= 1.f;
                            }

                            mapLight.dirX   = vx;
                            mapLight.dirY   = vy;
                            mapLight.dirZ   = vz;
                        }
                        else {
                            // New map object
                            mapObj.x    = vx;
                            mapObj.y    = vy;
                            mapObj.z    = vz;
                        }                      
                    }
                } 
            }

            // Generate last object or light
            if(readLight)   mapLight.generateLight();
            else            mapObj.generateObject(&playerSpawn);

            readFile.close();
        }
    }catch (std::exception const& e){
        if(readFile.is_open())
            readFile.close();
    }

    return playerSpawn;
}