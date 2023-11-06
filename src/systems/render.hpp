#pragma once

#include "../engine/types.hpp"
#include "../manager/playerStats.hpp"
#include "../manager/weaponStats.hpp"
#include "../IrrlichtFacade/text.hpp"
#include "../engine/uiController.hpp"
#include "../manager/roundStats.hpp"

struct UiEngine;
struct GraphicEngine;

struct RenderSystem {
    RenderSystem();
    ~RenderSystem();
    void render(EntityMan& EM, GraphicEngine& irr, float, const double dt, roundStats& rs);
    void update(EntityMan& EM, const float dt, GraphicEngine& gE);
    void drawMenu(GraphicEngine& dev, bool ingame);
    void drawOptionValues(float menu_x, float menu_y, float menu_gap, float fonscale, int ingame);
    int drawSideMenus(GraphicEngine& gE, float y, float r, float g, float b, float a, float fontScale, int ingame);
    void drawCredits(GraphicEngine& gE, float x, float y, float gap, float r, float g, float b, float a, float fontScale);
    void drawLoadingScreen(GraphicEngine& gE);
    void drawStats(float x, float y, float fontScale, float gap);
    //float valueAtTime(float currentTime, float maxTime, float changeTime, float maxValue, float minValue, float changeDur);

    void reset();
    [[nodiscard]] UiController* getUiController() noexcept;
    
    private:
        UiController* uiController{nullptr};

        float windowWidth, windowHeight;

        std::vector<const char*> particleAmountLevels {"Minimo","Medio","Maximo"};
        std::vector<const char*> shadowQualityLevels {"Bajo","Alto","Ultra"};
};
