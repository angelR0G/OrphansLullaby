#pragma once

#include <cstdint>
#include <vector>

struct Waypoint_Connection {
	uint16_t 	to{},
				cost{};
	bool 		active{true};
};

struct Waypoint_t {
	float		x{},
				y{},
				z{};
	uint16_t    id{};
	uint8_t		clusterId{};
	std::vector<Waypoint_Connection>	connections;

	// Tactical properties
	uint8_t		visibility{};

	Waypoint_t(float _x, float _y, float _z, uint16_t _id, uint8_t cId, std::vector<Waypoint_Connection> conn):
		x{_x}, y{_y}, z{_z}, id{_id}, clusterId{cId}, connections{conn} {};

	void addConnection(Waypoint_Connection newConnection) {
		connections.emplace_back(newConnection);
	}
};
