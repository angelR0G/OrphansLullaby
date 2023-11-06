#pragma once

struct AttackStatsComponent {
    float   basic_dmg   {},
            special_dmg {},
            basic_cd    {},
            special_cd  {};
    bool attacked {}; //true attacked, false no attacked
};