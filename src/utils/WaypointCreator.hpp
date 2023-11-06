#pragma once

#include <vector>
#include <array>
#include <string>
#include <memory> 
#include "waypoint.hpp"
#include "cluster.hpp"

void generateWaypoints( std::string, 
                        std::vector<MapCluster>&, 
                        std::vector<uint16_t>&, 
                        std::vector<std::shared_ptr<Waypoint_t>>&, 
                        std::vector<std::vector<std::shared_ptr<Waypoint_t>>>&,
                        std::vector<std::pair<uint16_t, uint16_t>>&,
                        std::vector<std::vector<uint8_t>>&);