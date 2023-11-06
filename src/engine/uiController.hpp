#pragma once

#include <string>
#include <memory>
#include "../manager/playerStats.hpp"
#include "../manager/weaponStats.hpp"
#include "../manager/roundStats.hpp"
#include "../IrrlichtFacade/text.hpp"
#include "../IrrlichtFacade/image.hpp"
#include "../utils/damageMark.hpp"

#include <IMGUI/imgui.h>
#include <IMGUI/imgui_impl_glfw.h>
#include <IMGUI/imgui_impl_opengl3.h>

#define INFECTION_DISPLAY_TIME 4.5

struct GraphicEngine;
struct Goals;

struct PlayerBasicValues {
    float   posX,
            posY,
            posZ,
            orientation;
};

struct UiController{
    static UiController* Instance();
    ~UiController();
    void createImages();
    void drawMenuBk(int menuid, bool ingame);
    void drawImageFull(int, bool);
    void drawText(const char* string, float x, float y, float r, float g, float b, float a, bool centered, float fontScale = 8);
    void drawTextInWindow(const char* string, float x, float y, float r, float g, float b, float a, bool centered, float fontScale = 8);
    bool drawButtonInWindow(const char* string, float x, float y, float r, float g, float b, float a, bool centered, float fontScale = 8);
    bool drawButtonInWindowAction(const char* string, float x, float y, int action, bool centered, float fontScale = 8);
    //void drawMenu(GraphicEngine& dev, bool ingame);
    void drawBackground(const char* string, float r, float g, float b, float a);
    void drawLogo();
    void drawLoadingScreen();
    void drawPowerUp(SpecialEffect effect, size_t offset, float effectHealth, float dispTime);
    void drawMainGoal(Goals* goals);
    void drawDamageMarks(const PlayerBasicValues& playerBasic);
    void drawInfectionWarning(float x, float y);
	std::string wrapString(const std::string&, size_t, size_t& lines);
    void drawRectangle(float posX, float posY, float width, float height, float r, float g, float b, float a);

    void drawCinematicProgressBar(float value);

    void startWindow(std::string);
    void endWindow();
    void drawTextCenterIMGUI();
    void ImGui_UI(int points, playerStats ps, weaponStats ws, roundStats, int fps, Goals* goals, const double dt, const PlayerBasicValues& pb);
    void setWindowSize(float width, float height);
    
    void addDamageMark(float x, float z);
    
    void reset();
    void update(float dt);

    private:
        UiController();
        inline static std::unique_ptr<UiController> uiController{nullptr};

        float   windowWidth{},
                windowHeight{};
        std::vector<ImageNode> uiImages;

        ImFont* font1;

        float   infectionValue{-1},
                infectionValueOld{-1};

        float   infectionValueTime{0};

        //Splatter
        int splatterAmount{};
        
        std::vector<float> splatterspec_w;
        std::vector<float> splatterspec_h;

        std::vector<float> splatterspec_x;
        std::vector<float> splatterspec_y;

        // Damage mark
        std::vector<DamageMark> damageMarks;

	   void updateDamageMarks(float dt);
};