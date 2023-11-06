#pragma once

#include <vector>

#include "../engine/types.hpp"

struct IAParameters{
    std::size_t entity_id;
    SB steering_bv;
    float *arrival_radius;
    bool alive;
    std::string enenmy_type;
    bool modified;
    float *basic_dmg;
    float *basic_cd;
    float *special_dmg;
    float *special_cd;
    float *infection;
    std::string pathIDs;
};

struct EditorIA{
    EditorIA();
    static void helpMarker(const char*, float);
    static size_t selectable(std::vector<std::string>, size_t, float, const char*);
    void checkForEnemiesAlive(EntityMan& EM, std::vector<IAParameters>*, int);
    void updateEditorIA(EntityMan& EM, int, std::vector<IAParameters>*, bool);
    void checkModifiedValue(EntityMan&EM, bool*, Entity&);
    void showAttackStats(IAParameters*);

    std::string getNavPathIds(NavigationComponent*);

    private:
        float   windowWidth{},
                windowHeight{};
        GraphicEngine* engine3d;

        int     filter_enemy {0}; //0 -> All | 1 -> Normal | 2 -> Explosive | 3 -> Thrower | 4-> Rugby
        std::vector<std::string> sb_types {"Arrive", "Seek", "Flee", "Pursue", "Attack"};
        std::vector<std::string> enemy_types {"All", "Normal", "Explosive", "Thrower", "Rugby"};
};