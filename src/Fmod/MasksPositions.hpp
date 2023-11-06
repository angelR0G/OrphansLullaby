#pragma once

//Sound factory path positions
#define PISTOL_SHOOT            0
#define AMMO_MACHINE_GREETINGS  1
#define ZOMBIE_PAIN             2
#define ZOMBIE_ATTACK           3
#define PISTOL_RELOAD           4
#define AMMO_MACHINE_EXIT       5
#define HITMARKER               6
#define BULLET_MEATIMPACT       7
#define E_ZOMBIE_STEPS          8
#define ZOMBIE_EXPLOSION        9
#define PLAYER_PAIN             10
#define PLAYER_STEPS            11
#define ZOMBIE_STEPS            12
#define BOOST_MACHINE           13
#define AMBIENT                 14
#define DIALOGUES               15
#define CSHOTGUN_SHOOT          16
#define CSHOTGUN_RELOAD         17
#define REVOLVER_SHOOT          18
#define REVOLVER_RELOAD         19
#define APISTOL_SHOOT           20
#define M16_SHOOT               21
#define M16_RELOAD              22
#define SHOTGUN_SHOOT           23
#define SHOTGUN_RELOAD          24
#define BAZOOKA_SHOOT           25
#define BAZOOKA_RELOAD          26
#define BAZOOKA_EXPLOSION       27
#define LMACHINEGUN_SHOOT       28
#define LMACHINEGUN_RELOAD      29
#define HMACHINEGUN_SHOOT       30
#define HMACHINEGUN_RELOAD      31
#define M1_SHOOT                32
#define M1_RELOAD               33
#define HEAL_MACHINE_GREETINGS  34
#define HEAL_MACHINE_EXIT       35
#define EXPLOSION               36
#define BUY                     37
#define LOW_HEALTH              38
#define ACTION_MUSIC            39
#define PAUSE_STATE             40
#define LASER_SHOOT             41
#define LASER_RELOAD            42
#define GET_OBJECT              43
#define HEAL_MACHINE_MUSIC      44
#define AMMO_MACHINE_MUSIC      45
#define ELECTRIC_PANEL          46
#define TARGET_BARREL           47
#define NEW_ROUND               48
#define AGONIC_BREATH           49
#define M1K3_FLY                50
#define T_ZOMBIE_STEPS          51
#define T_ZOMBIE_ATTACK         52
#define T_ZOMBIE_SPECIAL        53
#define ZOMBIE_SCREAM_ATTACK    54
#define ZOMBIE_SPIT             55
#define GENERATOR_TV            56
#define GENERATOR_CHRISTMAS     57
#define GENERATOR_ARCADE        58
#define GENERATOR_TURRON        59
#define WEAPON_SWITCH           60
#define WEAPON_NOAMMO           61

//Zombie defines, below there are some of them commented because they use the same name but we want to still know their position.
#define M_ZOMBIE_PAIN               0
#define M_ZOMBIE_ATTACK             1
#define M_ZOMBIE_STEPS              2
#define M_ZOMBIE_SCREAM_ATTACK      3


//Explosive zombie defines
// #define M_ZOMBIE_PAIN            0
#define M_ZOMBIE_EXPLOSION          0
// #define M_ZOMBIE_ATTACK          1
//#define M_ZOMBIE_STEPS            2

//Tank zombie defines
// #define M_ZOMBIE_PAIN            0
// #define M_ZOMBIE_ATTACK          1
// #define M_ZOMBIE_STEPS           2
// #define M_ZOMBIE_SCREAM_ATTACK   3
// #define M_ZOMBIE_SPECIAL         4

//Spitter zombie defines
// #define M_ZOMBIE_PAIN            0
// #define M_ZOMBIE_ATTACK          1
// #define M_ZOMBIE_STEPS           2
#define M_ZOMBIE_SCREAM_ATTACK      3
#define M_ZOMBIE_SPECIAL            4


//Player defines
#define M_PISTOL_SHOOT              0
#define M_PISTOL_RELOAD             1
#define M_HITMARKER                 2
#define M_BULLET_MEATIMPACT         3
#define M_PLAYER_PAIN               4
#define M_PLAYER_STEPS              5
#define M_AMBIENT                   6   
#define M_DIALOGUES                 7   
#define M_CSHOTGUN_SHOOT            8
#define M_CSHOTGUN_RELOAD           9
#define M_REVOLVER_SHOOT            10
#define M_REVOLVER_RELOAD           11
#define M_APISTOL_SHOOT             12
#define M_M16_SHOOT                 13
#define M_M16_RELOAD                14
#define M_SHOTGUN_SHOOT             15
#define M_SHOTGUN_RELOAD            16
#define M_BAZOOKA_SHOOT             17
#define M_BAZOOKA_RELOAD            18
#define M_WEAPON_SWITCH             19
#define M_LMACHINEGUN_SHOOT         20
#define M_LMACHINEGUN_RELOAD        21
#define M_HMACHINEGUN_SHOOT         22
#define M_HMACHINEGUN_RELOAD        23
#define M_M1_SHOOT                  24
#define M_M1_RELOAD                 25
#define M_BUY                       26
#define M_LOW_HEALTH                27
#define M_ACTION_MUSIC              28
#define M_PAUSE_STATE               29
#define M_LASER_SHOOT               30
#define M_LASER_RELOAD              31
#define M_GET_OBJECT                32
#define M_NEW_ROUND                 33
#define M_AGONIC_BREATH             34
#define M_WEAPON_NOAMMO             35
#define M_BOOST_MACHINE             36

//Define Generator mask
#define M_GENERATOR                 0

//Bullet vendin machine defines
#define M_AMMO_MACHINE_GREETINGS    0
#define M_AMMO_MACHINE_EXIT         1
#define M_AMMO_MACHINE_MUSIC        2

//Injection vendin machine defines
#define M_HEAL_MACHINE_GREETINGS    0
#define M_HEAL_MACHINE_EXIT         1
#define M_HEAL_MACHINE_MUSIC        2

//Bazooka bullet and Explosive barrel defines
#define M_EXPLOSION                 0

//Target barrel defines
#define M_LULLABY                   0

//Electric panels defines
#define M_ELECTRIC_PANEL            0

//M1K3 defines
#define M_M1K3_FLY                  0

//Dialogue status
#define PLAYER_DIALOGUE_STOPPED   0
#define RADIO_DIALOGUE_STOPPED    0
#define PLAYER_DIALOGUE_PLAYING   1
#define RADIO_DIALOGUE_PLAYING    1

//Player dialogue selector
#define END_ROUND_SOUND           0
#define KILL_SOUND                1
#define NO_POINTS_SOUND           2
#define NEW_ZONE_SOUND            3
#define BUY_WEAPON_SOUND          4
#define NO_AMMO_SOUND             5

//Radio dialogue selector
#define RADIO_WELCOME             0
#define RADIO_FIXMACHINE          1
#define RADIO_ISSUE               2
#define RADIO_CONNECTION          3
#define RADIO_SCORT               4
#define RADIO_LOCKED              5
#define RADIO_OPEN                6
#define RADIO_DEFEND              7
#define RADIO_COMPLEAT            8
#define RADIO_STAY                9