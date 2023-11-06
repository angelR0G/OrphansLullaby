#pragma once

#include <vector>

struct menuConfig{
    menuConfig();
    ~menuConfig();

    //Sound
    float   soundTotal   {100},
            soundMusic   {100},
            soundVoices  {100},
            soundAmbient {100},
            soundEffects {100};
    //Control
    float   sensibilityX {60},
            sensibilityY {60};
    float sprintAlternate{0}; //bool
    //Graphic
    float fov{70};
    float particles{1}; //bool
    float particlesQuantity{2};
    float shadows{1}, shadowsSmooth{1}; //bool
    float shadowsQuality{1};
    float renderDistance{100};

    std::vector<const char*> optionsName {"VOLUMEN TOTAL","VOLUMEN MÚSICA","VOLUMEN VOCES","VOLUMEN AMBIENTE","VOLUMEN EFECTOS",
                                "SENSIBILIDAD X","SENSIBILIDAD Y", "ALTERNAR CORRER",
                                "FOV","INTERPOLACIÓN DE PARTÍCULAS","CANTIDAD DE PARTÍCULAS","ACTIVAR SOMBRAS","SUAVIZADO DE SOMBRAS","CALIDAD DE SOMBRAS","DISTANCIA DE RENDERIZADO"};

    std::vector<float*> optionsValue {&soundTotal,&soundMusic,&soundVoices,&soundAmbient,&soundEffects,
                                &sensibilityX,&sensibilityY,&sprintAlternate,
                                &fov,&particles,&particlesQuantity,&shadows,&shadowsSmooth,&shadowsQuality,&renderDistance};

    std::vector<float> opValueDefault   {100,100,100,100,100,
                                        60,60,0,
                                        70,1,2,1,1,1,100};

    std::vector<float> opValueMax       {100,100,100,100,100,
                                        100,100,1,
                                        90,1,2,1,1,2,100}; 

    std::vector<float> opValueMin       {0,0,0,0,0,
                                        0,0,0,
                                        45,0,0,0,0,0,30}; 
};